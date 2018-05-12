#include "rss_autodownloadrule.h"

#include <QDebug>
#include <QDir>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSharedData>
#include <QString>
#include <QStringList>

#include "../preferences.h"
#include "../tristatebool.h"
#include "../utils/fs.h"
#include "../utils/string.h"
#include "rss_feed.h"
#include "rss_article.h"
#include "rss_autodownloader.h"

namespace
{
    TriStateBool jsonValueToTriStateBool(const QJsonValue &jsonVal)
    {
        if (jsonVal.isBool())
            return TriStateBool(jsonVal.toBool());

        if (!jsonVal.isNull())
            qDebug() << Q_FUNC_INFO << "Incorrect value" << jsonVal.toVariant();

        return TriStateBool::Undefined;
    }

    QJsonValue triStateBoolToJsonValue(const TriStateBool &triStateBool)
    {
        switch (static_cast<int>(triStateBool)) {
        case 0:  return false;
        case 1:  return true;
        default: return QJsonValue();
        }
    }

    TriStateBool addPausedLegacyToTriStateBool(int val)
    {
        switch (val) {
        case 1:  return TriStateBool::True; // always
        case 2:  return TriStateBool::False; // never
        default: return TriStateBool::Undefined; // default
        }
    }

    int triStateBoolToAddPausedLegacy(const TriStateBool &triStateBool)
    {
        switch (static_cast<int>(triStateBool)) {
        case 0:  return 2; // never
        case 1:  return 1; // always
        default: return 0; // default
        }
    }
}

const QString Str_Name(QStringLiteral("name"));
const QString Str_Enabled(QStringLiteral("enabled"));
const QString Str_UseRegex(QStringLiteral("useRegex"));
const QString Str_MustContain(QStringLiteral("mustContain"));
const QString Str_MustNotContain(QStringLiteral("mustNotContain"));
const QString Str_EpisodeFilter(QStringLiteral("episodeFilter"));
const QString Str_AffectedFeeds(QStringLiteral("affectedFeeds"));
const QString Str_SavePath(QStringLiteral("savePath"));
const QString Str_AssignedCategory(QStringLiteral("assignedCategory"));
const QString Str_LastMatch(QStringLiteral("lastMatch"));
const QString Str_IgnoreDays(QStringLiteral("ignoreDays"));
const QString Str_AddPaused(QStringLiteral("addPaused"));
const QString Str_SmartFilter(QStringLiteral("smartFilter"));
const QString Str_PreviouslyMatched(QStringLiteral("previouslyMatchedEpisodes"));

namespace RSS
{
    struct AutoDownloadRuleData: public QSharedData
    {
        QString name;
        bool enabled = true;

        QStringList mustContain;
        QStringList mustNotContain;
        QString episodeFilter;
        QStringList feedURLs;
        bool useRegex = false;
        int ignoreDays = 0;
        QDateTime lastMatch;

        QString savePath;
        QString category;
        TriStateBool addPaused = TriStateBool::Undefined;

        bool smartFilter = false;
        QStringList previouslyMatchedEpisodes;

        mutable QString lastComputedEpisode;
        mutable QHash<QString, QRegularExpression> cachedRegexes;

        bool operator==(const AutoDownloadRuleData &other) const
        {
            return (name == other.name)
                    && (enabled == other.enabled)
                    && (mustContain == other.mustContain)
                    && (mustNotContain == other.mustNotContain)
                    && (episodeFilter == other.episodeFilter)
                    && (feedURLs == other.feedURLs)
                    && (useRegex == other.useRegex)
                    && (ignoreDays == other.ignoreDays)
                    && (lastMatch == other.lastMatch)
                    && (savePath == other.savePath)
                    && (category == other.category)
                    && (addPaused == other.addPaused)
                    && (smartFilter == other.smartFilter);
        }
    };
}

using namespace RSS;

QString computeEpisodeName(const QString &article)
{
    const QRegularExpression episodeRegex = AutoDownloader::instance()->smartEpisodeRegex();
    const QRegularExpressionMatch match = episodeRegex.match(article);

    // See if we can extract an season/episode number or date from the title
    if (!match.hasMatch())
        return QString();

    QStringList ret;
    for (int i = 1; i <= match.lastCapturedIndex(); ++i) {
        QString cap = match.captured(i);

        if (cap.isEmpty())
            continue;

        bool isInt = false;
        int x = cap.toInt(&isInt);

        ret.append(isInt ? QString::number(x) : cap);
    }
    return ret.join('x');
}

AutoDownloadRule::AutoDownloadRule(const QString &name)
    : m_dataPtr(new AutoDownloadRuleData)
{
    setName(name);
}

AutoDownloadRule::AutoDownloadRule(const AutoDownloadRule &other)
    : m_dataPtr(other.m_dataPtr)
{
}

AutoDownloadRule::~AutoDownloadRule() {}

QRegularExpression AutoDownloadRule::cachedRegex(const QString &expression, bool isRegex) const
{
    // Use a cache of regexes so we don't have to continually recompile - big performance increase.
    // The cache is cleared whenever the regex/wildcard, must or must not contain fields or
    // episode filter are modified.
    Q_ASSERT(!expression.isEmpty());
    QRegularExpression regex(m_dataPtr->cachedRegexes[expression]);

    if (!regex.pattern().isEmpty())
        return regex;

    return m_dataPtr->cachedRegexes[expression] = QRegularExpression(isRegex ? expression : Utils::String::wildcardToRegex(expression), QRegularExpression::CaseInsensitiveOption);
}

bool AutoDownloadRule::matches(const QString &articleTitle, const QString &expression) const
{
    static QRegularExpression whitespace("\\s+");

    if (expression.isEmpty()) {
        // A regex of the form "expr|" will always match, so do the same for wildcards
        return true;
    }
    else if (m_dataPtr->useRegex) {
        QRegularExpression reg(cachedRegex(expression));
        return reg.match(articleTitle).hasMatch();
    }
    else {
        // Only match if every wildcard token (separated by spaces) is present in the article name.
        // Order of wildcard tokens is unimportant (if order is important, they should have used *).
        foreach (const QString &wildcard, expression.split(whitespace, QString::SplitBehavior::SkipEmptyParts)) {
            QRegularExpression reg(cachedRegex(wildcard, false));

            if (!reg.match(articleTitle).hasMatch())
                return false;
        }
    }

    return true;
}

bool AutoDownloadRule::matches(const QString &articleTitle) const
{
    // Reset the lastComputedEpisode, we don't want to leak it between matches
    m_dataPtr->lastComputedEpisode.clear();

    if (!m_dataPtr->mustContain.empty()) {
        bool logged = false;
        bool foundMustContain = false;

        // Each expression is either a regex, or a set of wildcards separated by whitespace.
        // Accept if any complete expression matches.
        foreach (const QString &expression, m_dataPtr->mustContain) {
            if (!logged) {
//                qDebug() << "Checking matching" << (m_dataPtr->useRegex ? "regex:" : "wildcard expressions:") << m_dataPtr->mustContain.join("|");
                logged = true;
            }

            // A regex of the form "expr|" will always match, so do the same for wildcards
            foundMustContain = matches(articleTitle, expression);

            if (foundMustContain) {
//                qDebug() << "Found matching" << (m_dataPtr->useRegex ? "regex:" : "wildcard expression:") << expression;
                break;
            }
        }

        if (!foundMustContain)
            return false;
    }

    if (!m_dataPtr->mustNotContain.empty()) {
        bool logged = false;

        // Each expression is either a regex, or a set of wildcards separated by whitespace.
        // Reject if any complete expression matches.
        foreach (const QString &expression, m_dataPtr->mustNotContain) {
            if (!logged) {
//                qDebug() << "Checking not matching" << (m_dataPtr->useRegex ? "regex:" : "wildcard expressions:") << m_dataPtr->mustNotContain.join("|");
                logged = true;
            }

            // A regex of the form "expr|" will always match, so do the same for wildcards
            if (matches(articleTitle, expression)) {
//                qDebug() << "Found not matching" << (m_dataPtr->useRegex ? "regex:" : "wildcard expression:") << expression;
                return false;
            }
        }
    }

    if (!m_dataPtr->episodeFilter.isEmpty()) {
//        qDebug() << "Checking episode filter:" << m_dataPtr->episodeFilter;
        QRegularExpression f(cachedRegex("(^\\d{1,4})x(.*;$)"));
        QRegularExpressionMatch matcher = f.match(m_dataPtr->episodeFilter);
        bool matched = matcher.hasMatch();

        if (!matched)
            return false;

        QString s = matcher.captured(1);
        QStringList eps = matcher.captured(2).split(";");
        int sOurs = s.toInt();

        foreach (QString ep, eps) {
            if (ep.isEmpty())
                continue;

            // We need to trim leading zeroes, but if it's all zeros then we want episode zero.
            while (ep.size() > 1 && ep.startsWith("0"))
                ep = ep.right(ep.size() - 1);

            if (ep.indexOf('-') != -1) { // Range detected
                QString partialPattern1 = "\\bs0?(\\d{1,4})[ -_\\.]?e(0?\\d{1,4})(?:\\D|\\b)";
                QString partialPattern2 = "\\b(\\d{1,4})x(0?\\d{1,4})(?:\\D|\\b)";
                QRegularExpression reg(cachedRegex(partialPattern1));

                if (ep.endsWith('-')) { // Infinite range
                    int epOurs = ep.leftRef(ep.size() - 1).toInt();

                    // Extract partial match from article and compare as digits
                    matcher = reg.match(articleTitle);
                    matched = matcher.hasMatch();

                    if (!matched) {
                        reg = QRegularExpression(cachedRegex(partialPattern2));
                        matcher = reg.match(articleTitle);
                        matched = matcher.hasMatch();
                    }

                    if (matched) {
                        int sTheirs = matcher.captured(1).toInt();
                        int epTheirs = matcher.captured(2).toInt();
                        if (((sTheirs == sOurs) && (epTheirs >= epOurs)) || (sTheirs > sOurs)) {
//                            qDebug() << "Matched episode:" << ep;
//                            qDebug() << "Matched article:" << articleTitle;
                            return true;
                        }
                    }
                }
                else { // Normal range
                    QStringList range = ep.split('-');
                    Q_ASSERT(range.size() == 2);
                    if (range.first().toInt() > range.last().toInt())
                        continue; // Ignore this subrule completely

                    int epOursFirst = range.first().toInt();
                    int epOursLast = range.last().toInt();

                    // Extract partial match from article and compare as digits
                    matcher = reg.match(articleTitle);
                    matched = matcher.hasMatch();

                    if (!matched) {
                        reg = QRegularExpression(cachedRegex(partialPattern2));
                        matcher = reg.match(articleTitle);
                        matched = matcher.hasMatch();
                    }

                    if (matched) {
                        int sTheirs = matcher.captured(1).toInt();
                        int epTheirs = matcher.captured(2).toInt();
                        if ((sTheirs == sOurs) && ((epOursFirst <= epTheirs) && (epOursLast >= epTheirs))) {
//                            qDebug() << "Matched episode:" << ep;
//                            qDebug() << "Matched article:" << articleTitle;
                            return true;
                        }
                    }
                }
            }
            else { // Single number
                QString expStr("\\b(?:s0?" + s + "[ -_\\.]?" + "e0?" + ep + "|" + s + "x" + "0?" + ep + ")(?:\\D|\\b)");
                QRegularExpression reg(cachedRegex(expStr));
                if (reg.match(articleTitle).hasMatch()) {
//                    qDebug() << "Matched episode:" << ep;
//                    qDebug() << "Matched article:" << articleTitle;
                    return true;
                }
            }
        }

        return false;
    }

    if (useSmartFilter()) {
        // now see if this episode has been downloaded before
        const QString episodeStr = computeEpisodeName(articleTitle);

        if (!episodeStr.isEmpty()) {
            bool previouslyMatched = m_dataPtr->previouslyMatchedEpisodes.contains(episodeStr);
            bool isRepack = articleTitle.contains("REPACK", Qt::CaseInsensitive) || articleTitle.contains("PROPER", Qt::CaseInsensitive);
            if (previouslyMatched && !isRepack)
                return false;

            m_dataPtr->lastComputedEpisode = episodeStr;
        }
    }

//    qDebug() << "Matched article:" << articleTitle;
    return true;
}

AutoDownloadRule &AutoDownloadRule::operator=(const AutoDownloadRule &other)
{
    m_dataPtr = other.m_dataPtr;
    return *this;
}

bool AutoDownloadRule::operator==(const AutoDownloadRule &other) const
{
    return (m_dataPtr == other.m_dataPtr) // optimization
            || (*m_dataPtr == *other.m_dataPtr);
}

bool AutoDownloadRule::operator!=(const AutoDownloadRule &other) const
{
    return !operator==(other);
}

QJsonObject AutoDownloadRule::toJsonObject() const
{
    return {{Str_Enabled, isEnabled()}
        , {Str_UseRegex, useRegex()}
        , {Str_MustContain, mustContain()}
        , {Str_MustNotContain, mustNotContain()}
        , {Str_EpisodeFilter, episodeFilter()}
        , {Str_AffectedFeeds, QJsonArray::fromStringList(feedURLs())}
        , {Str_SavePath, savePath()}
        , {Str_AssignedCategory, assignedCategory()}
        , {Str_LastMatch, lastMatch().toString(Qt::RFC2822Date)}
        , {Str_IgnoreDays, ignoreDays()}
        , {Str_AddPaused, triStateBoolToJsonValue(addPaused())}
        , {Str_SmartFilter, useSmartFilter()}
        , {Str_PreviouslyMatched, QJsonArray::fromStringList(previouslyMatchedEpisodes())}};
}

AutoDownloadRule AutoDownloadRule::fromJsonObject(const QJsonObject &jsonObj, const QString &name)
{
    AutoDownloadRule rule(name.isEmpty() ? jsonObj.value(Str_Name).toString() : name);

    rule.setUseRegex(jsonObj.value(Str_UseRegex).toBool(false));
    rule.setMustContain(jsonObj.value(Str_MustContain).toString());
    rule.setMustNotContain(jsonObj.value(Str_MustNotContain).toString());
    rule.setEpisodeFilter(jsonObj.value(Str_EpisodeFilter).toString());
    rule.setEnabled(jsonObj.value(Str_Enabled).toBool(true));
    rule.setSavePath(jsonObj.value(Str_SavePath).toString());
    rule.setCategory(jsonObj.value(Str_AssignedCategory).toString());
    rule.setAddPaused(jsonValueToTriStateBool(jsonObj.value(Str_AddPaused)));
    rule.setLastMatch(QDateTime::fromString(jsonObj.value(Str_LastMatch).toString(), Qt::RFC2822Date));
    rule.setIgnoreDays(jsonObj.value(Str_IgnoreDays).toInt());
    rule.setUseSmartFilter(jsonObj.value(Str_SmartFilter).toBool(false));

    const QJsonValue feedsVal = jsonObj.value(Str_AffectedFeeds);
    QStringList feedURLs;
    if (feedsVal.isString())
        feedURLs << feedsVal.toString();
    else foreach (const QJsonValue &urlVal, feedsVal.toArray())
        feedURLs << urlVal.toString();
    rule.setFeedURLs(feedURLs);

    const QJsonValue previouslyMatchedVal = jsonObj.value(Str_PreviouslyMatched);
    QStringList previouslyMatched;
    if (previouslyMatchedVal.isString()) {
        previouslyMatched << previouslyMatchedVal.toString();
    }
    else {
        foreach (const QJsonValue &val, previouslyMatchedVal.toArray())
            previouslyMatched << val.toString();
    }
    rule.setPreviouslyMatchedEpisodes(previouslyMatched);

    return rule;
}

QVariantHash AutoDownloadRule::toLegacyDict() const
{
    return {{"name", name()},
        {"must_contain", mustContain()},
        {"must_not_contain", mustNotContain()},
        {"save_path", savePath()},
        {"affected_feeds", feedURLs()},
        {"enabled", isEnabled()},
        {"category_assigned", assignedCategory()},
        {"use_regex", useRegex()},
        {"add_paused", triStateBoolToAddPausedLegacy(addPaused())},
        {"episode_filter", episodeFilter()},
        {"last_match", lastMatch()},
        {"ignore_days", ignoreDays()}};
}

AutoDownloadRule AutoDownloadRule::fromLegacyDict(const QVariantHash &dict)
{
    AutoDownloadRule rule(dict.value("name").toString());

    rule.setUseRegex(dict.value("use_regex", false).toBool());
    rule.setMustContain(dict.value("must_contain").toString());
    rule.setMustNotContain(dict.value("must_not_contain").toString());
    rule.setEpisodeFilter(dict.value("episode_filter").toString());
    rule.setFeedURLs(dict.value("affected_feeds").toStringList());
    rule.setEnabled(dict.value("enabled", false).toBool());
    rule.setSavePath(dict.value("save_path").toString());
    rule.setCategory(dict.value("category_assigned").toString());
    rule.setAddPaused(addPausedLegacyToTriStateBool(dict.value("add_paused").toInt()));
    rule.setLastMatch(dict.value("last_match").toDateTime());
    rule.setIgnoreDays(dict.value("ignore_days").toInt());

    return rule;
}

void AutoDownloadRule::setMustContain(const QString &tokens)
{
    m_dataPtr->cachedRegexes.clear();

    if (m_dataPtr->useRegex)
        m_dataPtr->mustContain = QStringList() << tokens;
    else
        m_dataPtr->mustContain = tokens.split("|");

    // Check for single empty string - if so, no condition
    if ((m_dataPtr->mustContain.size() == 1) && m_dataPtr->mustContain[0].isEmpty())
        m_dataPtr->mustContain.clear();
}

void AutoDownloadRule::setMustNotContain(const QString &tokens)
{
    m_dataPtr->cachedRegexes.clear();

    if (m_dataPtr->useRegex)
        m_dataPtr->mustNotContain = QStringList() << tokens;
    else
        m_dataPtr->mustNotContain = tokens.split("|");

    // Check for single empty string - if so, no condition
    if ((m_dataPtr->mustNotContain.size() == 1) && m_dataPtr->mustNotContain[0].isEmpty())
        m_dataPtr->mustNotContain.clear();
}

QStringList AutoDownloadRule::feedURLs() const
{
    return m_dataPtr->feedURLs;
}

void AutoDownloadRule::setFeedURLs(const QStringList &urls)
{
    m_dataPtr->feedURLs = urls;
}

QString AutoDownloadRule::name() const
{
    return m_dataPtr->name;
}

void AutoDownloadRule::setName(const QString &name)
{
    m_dataPtr->name = name;
}

QString AutoDownloadRule::savePath() const
{
    return m_dataPtr->savePath;
}

void AutoDownloadRule::setSavePath(const QString &savePath)
{
    m_dataPtr->savePath = Utils::Fs::fromNativePath(savePath);
}

TriStateBool AutoDownloadRule::addPaused() const
{
    return m_dataPtr->addPaused;
}

void AutoDownloadRule::setAddPaused(const TriStateBool &addPaused)
{
    m_dataPtr->addPaused = addPaused;
}

QString AutoDownloadRule::assignedCategory() const
{
    return m_dataPtr->category;
}

void AutoDownloadRule::setCategory(const QString &category)
{
    m_dataPtr->category = category;
}

bool AutoDownloadRule::isEnabled() const
{
    return m_dataPtr->enabled;
}

void AutoDownloadRule::setEnabled(bool enable)
{
    m_dataPtr->enabled = enable;
}

QDateTime AutoDownloadRule::lastMatch() const
{
    return m_dataPtr->lastMatch;
}

void AutoDownloadRule::setLastMatch(const QDateTime &lastMatch)
{
    m_dataPtr->lastMatch = lastMatch;
}

void AutoDownloadRule::setIgnoreDays(int d)
{
    m_dataPtr->ignoreDays = d;
}

int AutoDownloadRule::ignoreDays() const
{
    return m_dataPtr->ignoreDays;
}

QString AutoDownloadRule::mustContain() const
{
    return m_dataPtr->mustContain.join("|");
}

QString AutoDownloadRule::mustNotContain() const
{
    return m_dataPtr->mustNotContain.join("|");
}

bool AutoDownloadRule::useSmartFilter() const
{
    return m_dataPtr->smartFilter;
}

void AutoDownloadRule::setUseSmartFilter(bool enabled)
{
    m_dataPtr->smartFilter = enabled;
}

bool AutoDownloadRule::useRegex() const
{
    return m_dataPtr->useRegex;
}

void AutoDownloadRule::setUseRegex(bool enabled)
{
    m_dataPtr->useRegex = enabled;
    m_dataPtr->cachedRegexes.clear();
}

QStringList AutoDownloadRule::previouslyMatchedEpisodes() const
{
    return m_dataPtr->previouslyMatchedEpisodes;
}

void AutoDownloadRule::setPreviouslyMatchedEpisodes(const QStringList &previouslyMatchedEpisodes)
{
    m_dataPtr->previouslyMatchedEpisodes = previouslyMatchedEpisodes;
}

void AutoDownloadRule::appendLastComputedEpisode()
{
    if (!m_dataPtr->lastComputedEpisode.isEmpty()) {
        // TODO: probably need to add a marker for PROPER/REPACK to avoid duplicate downloads
        m_dataPtr->previouslyMatchedEpisodes.append(m_dataPtr->lastComputedEpisode);
        m_dataPtr->lastComputedEpisode.clear();
    }
}

QString AutoDownloadRule::episodeFilter() const
{
    return m_dataPtr->episodeFilter;
}

void AutoDownloadRule::setEpisodeFilter(const QString &e)
{
    m_dataPtr->episodeFilter = e;
    m_dataPtr->cachedRegexes.clear();
}
