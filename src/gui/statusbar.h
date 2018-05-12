#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

class QLabel;
class QPushButton;

namespace BitTorrent
{
    struct SessionStatus;
}

class StatusBar: public QStatusBar
{
    Q_OBJECT
    Q_DISABLE_COPY(StatusBar)

public:
    StatusBar(QWidget *parent = nullptr);
    ~StatusBar() override;

signals:
    void alternativeSpeedsButtonClicked();
    void connectionButtonClicked();

public slots:
    void showRestartRequired();

private slots:
    void refresh();
    void updateAltSpeedsBtn(bool alternative);
    void capDownloadSpeed();
    void capUploadSpeed();

private:
    void updateConnectionStatus();
    void updateDHTNodesNumber();
    void updateSpeedLabels();

    QPushButton *m_dlSpeedLbl;
    QPushButton *m_upSpeedLbl;
    QLabel *m_DHTLbl;
    QPushButton *m_connecStatusLblIcon;
    QPushButton *m_altSpeedsBtn;
};

#endif // STATUSBAR_H
