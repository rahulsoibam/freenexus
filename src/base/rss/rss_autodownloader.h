#pragma once

#include <stdexcept>

#include <QBasicTimer>
#include <QHash>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QRegularExpression>
#include <QSharedPointer>

class QThread;
class QTimer;
class Application;
class AsyncFileStorage;
struct ProcessingJob;

namespace RSS
{
    class Article;
    class Feed;
    class Item;

    class AutoDownloadRule;

    class ParsingError : public std::runtime_error
    {
    public:
        explicit ParsingError(const QString &message);
        QString message() const;
    };

    class AutoDownloader final: public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(AutoDownloader)

        friend class ::Application;

        AutoDownloader();
        ~AutoDownloader() override;

    public:
        enum class RulesFileFormat
        {
            Legacy,
            JSON
        };

        static AutoDownloader *instance();

        bool isProcessingEnabled() const;
        void setProcessingEnabled(bool enabled);

        QStringList smartEpisodeFilters() const;
        void setSmartEpisodeFilters(const QStringList &filters);
        QRegularExpression smartEpisodeRegex() const;

        bool hasRule(const QString &ruleName) const;
        AutoDownloadRule ruleByName(const QString &ruleName) const;
        QList<AutoDownloadRule> rules() const;

        void insertRule(const AutoDownloadRule &rule);
        bool renameRule(const QString &ruleName, const QString &newRuleName);
        void removeRule(const QString &ruleName);

        QByteArray exportRules(RulesFileFormat format = RulesFileFormat::JSON) const;
        void importRules(const QByteArray &data, RulesFileFormat format = RulesFileFormat::JSON);

    signals:
        void processingStateChanged(bool enabled);
        void ruleAdded(const QString &ruleName);
        void ruleChanged(const QString &ruleName);
        void ruleRenamed(const QString &ruleName, const QString &oldRuleName);
        void ruleAboutToBeRemoved(const QString &ruleName);

    private slots:
        void process();
        void handleTorrentDownloadFinished(const QString &url);
        void handleTorrentDownloadFailed(const QString &url);
        void handleNewArticle(Article *article);

    private:
        void timerEvent(QTimerEvent *event) override;
        void setRule_impl(const AutoDownloadRule &rule);
        void resetProcessingQueue();
        void startProcessing();
        void addJobForArticle(Article *article);
        void processJob(const QSharedPointer<ProcessingJob> &job);
        void load();
        void loadRules(const QByteArray &data);
        void loadRulesLegacy();
        void store();
        void storeDeferred();
        QByteArray exportRulesToJSONFormat() const;
        void importRulesFromJSONFormat(const QByteArray &data);
        QByteArray exportRulesToLegacyFormat() const;
        void importRulesFromLegacyFormat(const QByteArray &data);

        static QPointer<AutoDownloader> m_instance;

        bool m_processingEnabled;
        QTimer *m_processingTimer;
        QThread *m_ioThread;
        AsyncFileStorage *m_fileStorage;
        QHash<QString, AutoDownloadRule> m_rules;
        QList<QSharedPointer<ProcessingJob>> m_processingQueue;
        QHash<QString, QSharedPointer<ProcessingJob>> m_waitingJobs;
        bool m_dirty = false;
        QBasicTimer m_savingTimer;
        QRegularExpression m_smartEpisodeRegex;
    };
}
