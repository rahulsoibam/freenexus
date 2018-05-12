#ifndef NET_PORTFORWARDER_H
#define NET_PORTFORWARDER_H

#include <QHash>
#include <QObject>

namespace libtorrent
{
    class session;
}

namespace Net
{
    class PortForwarder : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(PortForwarder)

    public:
        static void initInstance(libtorrent::session *const provider);
        static void freeInstance();
        static PortForwarder *instance();

        bool isEnabled() const;
        void setEnabled(bool enabled);

        void addPort(quint16 port);
        void deletePort(quint16 port);

    private:
        explicit PortForwarder(libtorrent::session *const provider, QObject *parent = nullptr);
        ~PortForwarder();

        void start();
        void stop();

        bool m_active;
        libtorrent::session *m_provider;
        QHash<quint16, int> m_mappedPorts;

        static PortForwarder *m_instance;
    };
}

#endif // NET_PORTFORWARDER_H
