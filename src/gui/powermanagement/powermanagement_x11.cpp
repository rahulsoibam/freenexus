#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>

#include "powermanagement_x11.h"

PowerManagementInhibitor::PowerManagementInhibitor(QObject *parent) : QObject(parent)
{
    if (!QDBusConnection::sessionBus().isConnected())
    {
        qDebug("D-Bus: Could not connect to session bus");
        m_state = error;
    }
    else
    {
        m_state = idle;
    }

    m_intended_state = idle;
    m_cookie = 0;
    m_use_gsm = false;
}

PowerManagementInhibitor::~PowerManagementInhibitor()
{
}

void PowerManagementInhibitor::RequestIdle()
{
    m_intended_state = idle;
    if (m_state == error || m_state == idle || m_state == request_idle || m_state == request_busy)
        return;

    qDebug("D-Bus: PowerManagementInhibitor: Requesting idle");

    QDBusMessage call;
    if (!m_use_gsm)
        call = QDBusMessage::createMethodCall(
                "org.freedesktop.PowerManagement",
                "/org/freedesktop/PowerManagement/Inhibit",
                "org.freedesktop.PowerManagement.Inhibit",
                "UnInhibit");
    else
        call = QDBusMessage::createMethodCall(
                "org.gnome.SessionManager",
                "/org/gnome/SessionManager",
                "org.gnome.SessionManager",
                "Uninhibit");

    m_state = request_idle;

    QList<QVariant> args;
    args << m_cookie;
    call.setArguments(args);

    QDBusPendingCall pcall = QDBusConnection::sessionBus().asyncCall(call, 1000);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(OnAsyncReply(QDBusPendingCallWatcher*)));
}


void PowerManagementInhibitor::RequestBusy()
{
    m_intended_state = busy;
    if (m_state == error || m_state == busy || m_state == request_busy || m_state == request_idle)
        return;

    qDebug("D-Bus: PowerManagementInhibitor: Requesting busy");

    QDBusMessage call;
    if (!m_use_gsm)
        call = QDBusMessage::createMethodCall(
                "org.freedesktop.PowerManagement",
                "/org/freedesktop/PowerManagement/Inhibit",
                "org.freedesktop.PowerManagement.Inhibit",
                "Inhibit");
    else
        call = QDBusMessage::createMethodCall(
                "org.gnome.SessionManager",
                "/org/gnome/SessionManager",
                "org.gnome.SessionManager",
                "Inhibit");

    m_state = request_busy;

    QList<QVariant> args;
    args << "freeNexus";
    if (m_use_gsm) args << 0u;
    args << "Active torrents are presented";
    if (m_use_gsm) args << 8u;
    call.setArguments(args);

    QDBusPendingCall pcall = QDBusConnection::sessionBus().asyncCall(call, 1000);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(OnAsyncReply(QDBusPendingCallWatcher*)));
}

void PowerManagementInhibitor::OnAsyncReply(QDBusPendingCallWatcher *call)
{
    if (m_state == request_idle)
    {
        QDBusPendingReply<> reply = *call;

        if (reply.isError())
        {
            qDebug("D-Bus: Reply: Error: %s", qUtf8Printable(reply.error().message()));
            m_state = error;
        }
        else
        {
            m_state = idle;
            qDebug("D-Bus: PowerManagementInhibitor: Request successful");
            if (m_intended_state == busy) RequestBusy();
        }
    }
    else if (m_state == request_busy)
    {
        QDBusPendingReply<uint> reply = *call;

        if (reply.isError())
        {
            qDebug("D-Bus: Reply: Error: %s", qUtf8Printable(reply.error().message()));

            if (!m_use_gsm)
            {
                qDebug("D-Bus: Falling back to org.gnome.SessionManager");
                m_use_gsm = true;
                m_state = idle;
                if (m_intended_state == busy)
                    RequestBusy();
            }
            else
            {
                m_state = error;
            }
        }
        else
        {
            m_state = busy;
            m_cookie = reply.value();
            qDebug("D-Bus: PowerManagementInhibitor: Request successful, cookie is %d", m_cookie);
            if (m_intended_state == idle) RequestIdle();
        }
    }
    else
    {
        qDebug("D-Bus: Unexpected reply in state %d", m_state);
        m_state = error;
    }

    call->deleteLater();
}
