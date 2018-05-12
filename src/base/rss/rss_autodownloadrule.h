#pragma once

#include <QDateTime>
#include <QSharedDataPointer>
#include <QVariant>

class QJsonObject;
class QRegularExpression;
class TriStateBool;

namespace RSS
{
    struct AutoDownloadRuleData;

    class AutoDownloadRule
    {
    public:
        explicit AutoDownloadRule(const QString &name = "");
        AutoDownloadRule(const AutoDownloadRule &other);
        ~AutoDownloadRule();

        QString name() const;
        void setName(const QString &name);

        bool isEnabled() const;
        void setEnabled(bool enable);

        QString mustContain() const;
        void setMustContain(const QString &tokens);
        QString mustNotContain() const;
        void setMustNotContain(const QString &tokens);
        QStringList feedURLs() const;
        void setFeedURLs(const QStringList &urls);
        int ignoreDays() const;
        void setIgnoreDays(int d);
        QDateTime lastMatch() const;
        void setLastMatch(const QDateTime &lastMatch);
        bool useRegex() const;
        void setUseRegex(bool enabled);
        bool useSmartFilter() const;
        void setUseSmartFilter(bool enabled);
        QString episodeFilter() const;
        void setEpisodeFilter(const QString &e);

        void appendLastComputedEpisode();
        QStringList previouslyMatchedEpisodes() const;
        void setPreviouslyMatchedEpisodes(const QStringList &previouslyMatchedEpisodes);

        QString savePath() const;
        void setSavePath(const QString &savePath);
        TriStateBool addPaused() const;
        void setAddPaused(const TriStateBool &addPaused);
        QString assignedCategory() const;
        void setCategory(const QString &category);

        bool matches(const QString &articleTitle) const;

        AutoDownloadRule &operator=(const AutoDownloadRule &other);
        bool operator==(const AutoDownloadRule &other) const;
        bool operator!=(const AutoDownloadRule &other) const;

        QJsonObject toJsonObject() const;
        static AutoDownloadRule fromJsonObject(const QJsonObject &jsonObj, const QString &name = "");

        QVariantHash toLegacyDict() const;
        static AutoDownloadRule fromLegacyDict(const QVariantHash &dict);

    private:
        bool matches(const QString &articleTitle, const QString &expression) const;
        QRegularExpression cachedRegex(const QString &expression, bool isRegex = true) const;

        QSharedDataPointer<AutoDownloadRuleData> m_dataPtr;
    };
}
