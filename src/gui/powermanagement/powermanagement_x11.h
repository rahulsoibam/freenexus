#ifndef POWERMANAGEMENTINHIBITOR_H
#define POWERMANAGEMENTINHIBITOR_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QDBusPendingCallWatcher;
QT_END_NAMESPACE

class PowerManagementInhibitor : public QObject
{
    Q_OBJECT

public:
    PowerManagementInhibitor(QObject *parent = 0);
    virtual ~PowerManagementInhibitor();

    void RequestIdle();
    void RequestBusy();

private slots:
    void OnAsyncReply(QDBusPendingCallWatcher *call);

private:
    enum _state
    {
        error,
        idle,
        request_busy,
        busy,
        request_idle
    };

    enum _state m_state;
    enum _state m_intended_state;
    unsigned int m_cookie;

    bool m_use_gsm;
};

#endif // POWERMANAGEMENTINHIBITOR_H
