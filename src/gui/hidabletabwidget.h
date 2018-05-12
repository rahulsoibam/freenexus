#ifndef HIDABLETABWIDGET_H
#define HIDABLETABWIDGET_H

#include <QTabWidget>
#include <QTabBar>

#ifdef Q_OS_MAC
#include <QStyle>
#endif

class HidableTabWidget : public QTabWidget
{
public:
    explicit HidableTabWidget(QWidget *parent = 0)
        : QTabWidget(parent)
    {
    }

protected:
#ifdef Q_OS_MAC
    void paintEvent(QPaintEvent* event) override
    {
        // Hide the pane for macintosh style
        if (!style()->inherits("QMacStyle"))
            QTabWidget::paintEvent(event);
    }
#endif

    void tabInserted(int index) override
    {
        QTabWidget::tabInserted(index);
        tabBar()->setVisible(count() != 1);
    }

    void tabRemoved(int index) override
    {
        //QTabWidget::tabInserted(index);
        QTabWidget::tabRemoved(index);
        tabBar()->setVisible(count() != 1);
    }
};

#endif // HIDABLETABWIDGET_H
