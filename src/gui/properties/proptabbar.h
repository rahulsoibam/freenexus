#ifndef PROPTABBAR_H
#define PROPTABBAR_H

#include <QHBoxLayout>

class QButtonGroup;

class PropTabBar : public QHBoxLayout
{
  Q_OBJECT
  Q_DISABLE_COPY(PropTabBar)

public:
    enum PropertyTab
    {
        MainTab,
        TrackersTab,
        PeersTab,
        URLSeedsTab,
        FilesTab,
        SpeedTab
    };

    explicit PropTabBar(QWidget *parent = nullptr);
    ~PropTabBar();
    int currentIndex() const;

signals:
    void tabChanged(int index);
    void visibilityToggled(bool visible);

public slots:
    void setCurrentIndex(int index);

private:
    QButtonGroup *m_btnGroup;
    int m_currentIndex;
};

#endif // PROPTABBAR_H
