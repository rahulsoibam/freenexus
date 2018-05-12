#include "rss_autodownloader.h"

#include <QDataStream>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSaveFile>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <QVector>

#include "../bittorrent/magneturi.h"
#include "../bittorrent/session.h"
#include "../asyncfilestorage.h"
#include "../global.h"
#include "../logger.h"
#include "../profile.h"
#include "../settingsstorage.h"
#include "../tristatebool.h"
#include "../utils/fs.h"
#include "rss_article.h"
#include "rss_autodownloadrule.h"
#include "rss_feed.h"
#include "rss_folder.h"
#include "rss_session.h"

struct ProcessingJob
{
    QString feedURL;
    QVariantHash articleData;
};

const QString ConfFolderName(QStringLiteral("rss"));
const QString RulesFileName(QStringLiteral("download_rules.json"));

const QString SettingsKey_ProcessingEnabled(QStringLiteral("RSS/AutoDownloader/EnableProcessing"));
const QString SettingsKey_SmartEpisodeFilter(QStringLiteral("RSS/AutoDownloader/SmartEpisodeFilter"));

namespace
{
    QVector<RSS::AutoDownloadRule> rulesFromJSON(const QByteArray &jsonData)
    {
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
        if (jsonError.error != QJsonParseError::NoError)
            throw RSS::ParsingError(jsonError.errorString());

        if (!jsonDoc.isObject())
            throw RSS::ParsingError(RSS::AutoDownloader::tr("Invalid data format."));

        const QJsonObject jsonObj {jsonDoc.object()};
        QVector<RSS::AutoDownloadRule> rules;
        for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
            const QJsonValue jsonVal {it.value()};
            if (!jsonVal.isObject())
                throw RSS::ParsingError(RSS::AutoDownloader::tr("Invalid data format."));

            rules.append(RSS::AutoDownloadRule::fromJsonObject(jsonVal.toObject(), it.key()));
        }

        return rules;
    }
}

using namespace RSS;

QPointer<AutoDownloader> AutoDownloader::m_instance = nullptr;

QString computeSmartFilterRegex(const QStringList &filters)
{
    return QString("(?:_|\\b)(?:%1)(?:_|\\b)").arg(filters.join(QString(")|(?:")));
}

AutoDownloader::AutoDownloader()
    : m_processingEnabled(SettingsStorage::instance()->loadValue(SettingsKey_ProcessingEnabled, false).toBool())
    , m_processingTimer(new QTimer(this))
    , m_ioThread(new QThread(this))
{
    Q_ASSERT(!m_instance); // only one instance is allowed
    m_instance = this;

    m_fileStorage = new AsyncFileStorage(
                Utils::Fs::expandPathAbs(specialFolderLocation(SpecialFolder::Config) + ConfFolderName));
    if (!m_fileStorage)
        throw std::runtime_error("Directory for RSS AutoDownloader data is unavailable.");

    m_fileStorage->moveToThread(m_ioThread);
    connect(m_ioThread, &QThread::finished, m_fileStorage, &AsyncFileStorage::deleteLater);
    connect(m_fileStorage, &AsyncFileStorage::failed, [](const QString &fileName, const QString &errorString)
    {
        LogMsg(tr("Couldn't save RSS AutoDownloader data in %1. Error: %2")
               .arg(fileName, errorString), Log::CRITICAL);
    });

    m_ioThread->start();

    connect(BitTorrent::Session::instance(), &BitTorrent::Session::downloadFromUrlFinished
            , this, &AutoDownloader::handleTorrentDownloadFinished);
    connect(BitTorrent::Session::instance(), &BitTorrent::Session::downloadFromUrlFailed
            , this, &AutoDownloader::handleTorrentDownloadFailed);

    // initialise the smart episode regex
    const QString regex = computeSmartFilterRegex(smartEpisodeFilters());
    m_smartEpisodeRegex = QRegularExpression(regex,
                                             QRegularExpression::CaseInsensitiveOption
                                             | QRegularExpression::ExtendedPatternSyntaxOption
                                             | QRegularExpression::UseUnicodePropertiesOption);

    load();

    m_processingTimer->setSingleShot(true);
    connect(m_processingTimer, &QTimer::timeout, this, &AutoDownloader::process);

    if (m_processingEnabled)
        startProcessing();
}

AutoDownloader::~AutoDownloader()
{
    store();

    m_ioThread->quit();
    m_ioThread->wait();
}

AutoDownloader *AutoDownloader::instance()
{
    return m_instance;
}

bool AutoDownloader::hasRule(const QString &ruleName) const
{
    return m_rules.contains(ruleName);
}

AutoDownloadRule AutoDownloader::ruleByName(const QString &ruleName) const
{
    return m_rules.value(ruleName, AutoDownloadRule("Unknown Rule"));
}

QList<AutoDownloadRule> AutoDownloader::rules() const
{
    return m_rules.values();
}

void AutoDownloader::insertRule(const AutoDownloadRule &rule)
{
    if (!hasRule(rule.name())) {
        // Insert new rule
        setRule_impl(rule);
        m_dirty = true;
        store();
        emit ruleAdded(rule.name());
        resetProcessingQueue();
    }
    else if (ruleByName(rule.name()) != rule) {
        // Update existing rule
        setRule_impl(rule);
        m_dirty = true;
        storeDeferred();
        emit ruleChanged(rule.name());
        resetProcessingQueue();
    }
}

bool AutoDownloader::renameRule(const QString &ruleName, const QString &newRuleName)
{
    if (!hasRule(ruleName)) return false;
    if (hasRule(newRuleName)) return false;

    m_rules.insert(newRuleName, m_rules.take(ruleName));
    m_dirty = true;
    store();
    emit ruleRenamed(newRuleName, ruleName);
    return true;
}

void AutoDownloader::removeRule(const QString &ruleName)
{
    if (m_rules.contains(ruleName)) {
        emit ruleAboutToBeRemoved(ruleName);
        m_rules.remove(ruleName);
        m_dirty = true;
        store();
    }
}

QByteArray AutoDownloader::exportRules(AutoDownloader::RulesFileFormat format) const
{
    switch (format) {
    case RulesFileFormat::Legacy:
        return exportRulesToLegacyFormat();
    default:
        return exportRulesToJSONFormat();
    }
}

void AutoDownloader::importRules(const QByteArray &data, AutoDownloader::RulesFileFormat format)
{
    switch (format) {
    case RulesFileFormat::Legacy:
        importRulesFromLegacyFormat(data);
        break;
    default:
        importRulesFromJSONFormat(data);
    }
}

QByteArray AutoDownloader::exportRulesToJSONFormat() const
{
    QJsonObject jsonObj;
    for (const auto &rule : copyAsConst(rules()))
        jsonObj.insert(rule.name(), rule.toJsonObject());

    return QJsonDocument(jsonObj).toJson();
}

void AutoDownloader::importRulesFromJSONFormat(const QByteArray &data)
{
    for (const auto &rule : copyAsConst(rulesFromJSON(data)))
        insertRule(rule);
}

QByteArray AutoDownloader::exportRulesToLegacyFormat() const
{
    QVariantHash dict;
    for (const auto &rule : copyAsConst(rules()))
        dict[rule.name()] = rule.toLegacyDict();

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << dict;

    return data;
}

void AutoDownloader::importRulesFromLegacyFormat(const QByteArray &data)
{
    QDataStream in(data);
    in.setVersion(QDataStream::Qt_4_5);
    QVariantHash dict;
    in >> dict;
    if (in.status() != QDataStream::Ok)
        throw ParsingError(tr("Invalid data format"));

    for (const QVariant &val : qAsConst(dict))
        insertRule(AutoDownloadRule::fromLegacyDict(val.toHash()));
}

QStringList AutoDownloader::smartEpisodeFilters() const
{
    const QVariant filtersSetting = SettingsStorage::instance()->loadValue(SettingsKey_SmartEpisodeFilter);

    if (filtersSetting.isNull()) {
        QStringList filters = {
            "s(\\d+)e(\\d+)",                       // Format 1: s01e01
            "(\\d+)x(\\d+)",                        // Format 2: 01x01
            "(\\d{4}[.\\-]\\d{1,2}[.\\-]\\d{1,2})", // Format 3: 2017.01.01
            "(\\d{1,2}[.\\-]\\d{1,2}[.\\-]\\d{4})"  // Format 4: 01.01.2017
        };

        return filters;
    }

    return filtersSetting.toStringList();
}

QRegularExpression AutoDownloader::smartEpisodeRegex() const
{
    return m_smartEpisodeRegex;
}

void AutoDownloader::setSmartEpisodeFilters(const QStringList &filters)
{
    SettingsStorage::instance()->storeValue(SettingsKey_SmartEpisodeFilter, filters);

    const QString regex = computeSmartFilterRegex(filters);
    m_smartEpisodeRegex.setPattern(regex);
}

void AutoDownloader::process()
{
    if (m_processingQueue.isEmpty()) return; // processing was disabled

    processJob(m_processingQueue.takeFirst());
    if (!m_processingQueue.isEmpty())
        // Schedule to process the next torrent (if any)
        m_processingTimer->start();
}

void AutoDownloader::handleTorrentDownloadFinished(const QString &url)
{
    auto job = m_waitingJobs.take(url);
    if (!job) return;

    if (Feed *feed = Session::instance()->feedByURL(job->feedURL))
        if (Article *article = feed->articleByGUID(job->articleData.value(Article::KeyId).toString()))
            article->markAsRead();
}

void AutoDownloader::handleTorrentDownloadFailed(const QString &url)
{
    m_waitingJobs.remove(url);
    // TODO: Re-schedule job here.
}

void AutoDownloader::handleNewArticle(Article *article)
{
    if (!article->isRead() && !article->torrentUrl().isEmpty())
        addJobForArticle(article);
}

void AutoDownloader::setRule_impl(const AutoDownloadRule &rule)
{
    m_rules.insert(rule.name(), rule);
}

void AutoDownloader::addJobForArticle(Article *article)
{
    const QString torrentURL = article->torrentUrl();
    if (m_waitingJobs.contains(torrentURL)) return;

    QSharedPointer<ProcessingJob> job(new ProcessingJob);
    job->feedURL = article->feed()->url();
    job->articleData = article->data();
    m_processingQueue.append(job);
    if (!m_processingTimer->isActive())
        m_processingTimer->start();
}

void AutoDownloader::processJob(const QSharedPointer<ProcessingJob> &job)
{
    for (AutoDownloadRule &rule: m_rules) {
        if (!rule.isEnabled()) continue;
        if (!rule.feedURLs().contains(job->feedURL)) continue;
        if (!rule.matches(job->articleData.value(Article::KeyTitle).toString())) continue;

        auto articleDate = job->articleData.value(Article::KeyDate).toDateTime();
        // if rule is in ignoring state do nothing with matched torrent
        if (rule.ignoreDays() > 0) {
            if (rule.lastMatch().isValid()) {
                if (articleDate < rule.lastMatch().addDays(rule.ignoreDays()))
                    return;
            }
        }

        rule.setLastMatch(articleDate);
        rule.appendLastComputedEpisode();

        m_dirty = true;
        storeDeferred();

        BitTorrent::AddTorrentParams params;
        params.savePath = rule.savePath();
        params.category = rule.assignedCategory();
        params.addPaused = rule.addPaused();
        if (!rule.savePath().isEmpty())
            params.useAutoTMM = TriStateBool::False;
        auto torrentURL = job->articleData.value(Article::KeyTorrentURL).toString();
        BitTorrent::Session::instance()->addTorrent(torrentURL, params);

        if (BitTorrent::MagnetUri(torrentURL).isValid()) {
            if (Feed *feed = Session::instance()->feedByURL(job->feedURL)) {
                if (Article *article = feed->articleByGUID(job->articleData.value(Article::KeyId).toString()))
                    article->markAsRead();
            }
        }
        else {
            // waiting for torrent file downloading
            // normalize URL string via QUrl since DownloadManager do it
            m_waitingJobs.insert(QUrl(torrentURL).toString(), job);
        }

        return;
    }
}

void AutoDownloader::load()
{
    QFile rulesFile(m_fileStorage->storageDir().absoluteFilePath(RulesFileName));

    if (!rulesFile.exists())
        loadRulesLegacy();
    else if (rulesFile.open(QFile::ReadOnly))
        loadRules(rulesFile.readAll());
    else
        LogMsg(tr("Couldn't read RSS AutoDownloader rules from %1. Error: %2")
               .arg(rulesFile.fileName(), rulesFile.errorString()), Log::CRITICAL);
}

void AutoDownloader::loadRules(const QByteArray &data)
{
    try {
        const auto rules = rulesFromJSON(data);
        for (const auto &rule : rules)
            setRule_impl(rule);
    }
    catch (const ParsingError &error) {
        LogMsg(tr("Couldn't load RSS AutoDownloader rules. Reason: %1")
               .arg(error.message()), Log::CRITICAL);
    }
}

void AutoDownloader::loadRulesLegacy()
{
    SettingsPtr settings = Profile::instance().applicationSettings(QStringLiteral("freeNexus-rss"));
    QVariantHash rules = settings->value(QStringLiteral("download_rules")).toHash();
    foreach (const QVariant &ruleVar, rules) {
        auto rule = AutoDownloadRule::fromLegacyDict(ruleVar.toHash());
        if (!rule.name().isEmpty())
            insertRule(rule);
    }
}

void AutoDownloader::store()
{
    if (!m_dirty) return;

    m_dirty = false;
    m_savingTimer.stop();

    QJsonObject jsonObj;
    foreach (auto rule, m_rules)
        jsonObj.insert(rule.name(), rule.toJsonObject());

    m_fileStorage->store(RulesFileName, QJsonDocument(jsonObj).toJson());
}

void AutoDownloader::storeDeferred()
{
    if (!m_savingTimer.isActive())
        m_savingTimer.start(5 * 1000, this);
}

bool AutoDownloader::isProcessingEnabled() const
{
    return m_processingEnabled;
}

void AutoDownloader::resetProcessingQueue()
{
    m_processingQueue.clear();
    if (!m_processingEnabled) return;

    foreach (Article *article, Session::instance()->rootFolder()->articles()) {
        if (!article->isRead() && !article->torrentUrl().isEmpty())
            addJobForArticle(article);
    }
}

void AutoDownloader::startProcessing()
{
    resetProcessingQueue();
    connect(Session::instance()->rootFolder(), &Folder::newArticle, this, &AutoDownloader::handleNewArticle);
}

void AutoDownloader::setProcessingEnabled(bool enabled)
{
    if (m_processingEnabled != enabled) {
        m_processingEnabled = enabled;
        SettingsStorage::instance()->storeValue(SettingsKey_ProcessingEnabled, m_processingEnabled);
        if (m_processingEnabled) {
            startProcessing();
        }
        else {
            m_processingQueue.clear();
            disconnect(Session::instance()->rootFolder(), &Folder::newArticle, this, &AutoDownloader::handleNewArticle);
        }

        emit processingStateChanged(m_processingEnabled);
    }
}

void AutoDownloader::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    store();
}

ParsingError::ParsingError(const QString &message)
    : std::runtime_error(message.toUtf8().data())
{
}

QString ParsingError::message() const
{
    return what();
}
