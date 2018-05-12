#ifndef BANDWIDTHSCHEDULER_H
#define BANDWIDTHSCHEDULER_H

#include <QObject>
#include <QTimer>

class BandwidthScheduler: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(BandwidthScheduler)

public:
    explicit BandwidthScheduler(QObject *parent = nullptr);
    void start();

signals:
    void bandwidthLimitRequested(bool alternative);

private:
    bool isTimeForAlternative() const;
    void onTimeout();

    QTimer m_timer;
    bool m_lastAlternative;
};

#endif // BANDWIDTHSCHEDULER_H
