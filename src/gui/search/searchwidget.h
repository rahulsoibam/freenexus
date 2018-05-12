#pragma once

#include <QList>
#include <QPointer>
#include <QWidget>

class QSignalMapper;
class QTabWidget;

class MainWindow;
class SearchTab;

namespace Ui
{
    class SearchWidget;
}

class SearchWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SearchWidget)

public:
    explicit SearchWidget(MainWindow *mainWindow);
    ~SearchWidget() override;

    void giveFocusToSearchInput();

private slots:
    void on_searchButton_clicked();
    void on_downloadButton_clicked();
    void on_pluginsButton_clicked();

private:
    void tabChanged(int index);
    void closeTab(int index);
    void resultsCountUpdated();
    void tabStatusChanged(QWidget *tab);
    void selectMultipleBox(int index);

    void fillCatCombobox();
    void fillPluginComboBox();
    void selectActivePage();
    void searchTextEdited(QString);
    void updateButtons();

    QString selectedCategory() const;
    QString selectedPlugin() const;

    Ui::SearchWidget *m_ui;
    QSignalMapper *m_tabStatusChangedMapper;
    QPointer<SearchTab> m_currentSearchTab; // Selected tab
    QPointer<SearchTab> m_activeSearchTab; // Tab with running search
    QList<SearchTab *> m_allTabs; // To store all tabs
    MainWindow *m_mainWindow;
    bool m_isNewQueryString;
};
