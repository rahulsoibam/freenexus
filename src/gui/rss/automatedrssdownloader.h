#ifndef AUTOMATEDRSSDOWNLOADER_H
#define AUTOMATEDRSSDOWNLOADER_H

#include <QDialog>
#include <QHash>
#include <QPair>
#include <QSet>

#include "base/rss/rss_autodownloadrule.h"

namespace Ui
{
    class AutomatedRssDownloader;
}

class QListWidgetItem;
class QRegularExpression;
class QShortcut;

namespace RSS
{
    class Feed;
}

class AutomatedRssDownloader: public QDialog
{
    Q_OBJECT

public:
    explicit AutomatedRssDownloader(QWidget *parent = nullptr);
    ~AutomatedRssDownloader() override;

private slots:
    void on_addRuleBtn_clicked();
    void on_removeRuleBtn_clicked();
    void on_browseSP_clicked();
    void on_exportBtn_clicked();
    void on_importBtn_clicked();

    void handleRuleCheckStateChange(QListWidgetItem *ruleItem);
    void handleFeedCheckStateChange(QListWidgetItem *feedItem);
    void displayRulesListMenu();
    void renameSelectedRule();
    void updateRuleDefinitionBox();
    void clearSelectedRuleDownloadedEpisodeList();
    void updateFieldsToolTips(bool regex);
    void updateMustLineValidity();
    void updateMustNotLineValidity();
    void updateEpisodeFilterValidity();
    void handleRuleDefinitionChanged();
    void handleRuleAdded(const QString &ruleName);
    void handleRuleRenamed(const QString &ruleName, const QString &oldRuleName);
    void handleRuleChanged(const QString &ruleName);
    void handleRuleAboutToBeRemoved(const QString &ruleName);

    void handleProcessingStateChanged(bool enabled);

private:
    void loadSettings();
    void saveSettings();
    void createRuleItem(const RSS::AutoDownloadRule &rule);
    void initCategoryCombobox();
    void clearRuleDefinitionBox();
    void updateEditedRule();
    void updateMatchingArticles();
    void saveEditedRule();
    void loadFeedList();
    void updateFeedList();
    void addFeedArticlesToTree(RSS::Feed *feed, const QStringList &articles);

    const QString m_formatFilterJSON;
    const QString m_formatFilterLegacy;

    Ui::AutomatedRssDownloader *m_ui;
    QListWidgetItem *m_currentRuleItem;
    QShortcut *m_editHotkey;
    QShortcut *m_deleteHotkey;
    QSet<QPair<QString, QString>> m_treeListEntries;
    RSS::AutoDownloadRule m_currentRule;
    QHash<QString, QListWidgetItem *> m_itemsByRuleName;
    QRegularExpression *m_episodeRegex;
};

#endif // AUTOMATEDRSSDOWNLOADER_H
