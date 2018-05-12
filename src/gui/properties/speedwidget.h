#ifndef SPEEDWIDGET_H
#define SPEEDWIDGET_H

#include <QComboBox>
#include <QWidget>

#include "speedplotview.h"

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QMenu;
class QSignalMapper;
class PropertiesWidget;

class ComboBoxMenuButton : public QComboBox
{
    Q_OBJECT

public:
    ComboBoxMenuButton(QWidget *parent, QMenu *menu);
    virtual void showPopup();

private:
    QMenu *m_menu;
};


class SpeedWidget : public QWidget
{
    Q_OBJECT

public:
    SpeedWidget(PropertiesWidget *parent);
    ~SpeedWidget();

private slots:
    void onPeriodChange(int period);
    void onGraphChange(int id);
    void update();

private:
    void loadSettings();
    void saveSettings() const;

    QVBoxLayout *m_layout;
    QHBoxLayout *m_hlayout;
    QLabel *m_periodLabel;
    QComboBox *m_periodCombobox;
    SpeedPlotView *m_plot;

    ComboBoxMenuButton *m_graphsButton;
    QMenu *m_graphsMenu;
    QList<QAction *> m_graphsMenuActions;
    QSignalMapper *m_graphsSignalMapper;
};

#endif // SPEEDWIDGET_H
