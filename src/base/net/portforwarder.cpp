#include "portforwarder.h"

#include <QDebug>

#include <libtorrent/session.hpp>
#include <libtorrent/version.hpp>

#include "base/logger.h"
#include "base/settingsstorage.h"

static const QString KEY_ENABLED = QLatin1String("Network/PortForwardingEnabled");

namespace libt = libtorrent;
using namespace Net;

PortForwarder::PortForwarder(libtorrent::session *provider, QObject *parent)
    : QObject(parent)
    , m_active(false)
    , m_provider(provider)
{
    if (SettingsStorage::instance()->loadValue(KEY_ENABLED, true).toBool())
        start();
}

PortForwarder::~PortForwarder()
{
    stop();
}

void PortForwarder::initInstance(libtorrent::session *const provider)
{
    if (!m_instance)
        m_instance = new PortForwarder(provider);
}

void PortForwarder::freeInstance()
{
    if (m_instance) {
        delete m_instance;
        m_instance = nullptr;
    }
}

PortForwarder *PortForwarder::instance()
{
    return m_instance;
}

bool PortForwarder::isEnabled() const
{
    return m_active;
}

void PortForwarder::setEnabled(bool enabled)
{
    if (m_active != enabled) {
        if (enabled)
            start();
        else
            stop();

        SettingsStorage::instance()->storeValue(KEY_ENABLED, enabled);
    }
}

void PortForwarder::addPort(quint16 port)
{
    if (!m_mappedPorts.contains(port)) {
        m_mappedPorts.insert(port, 0);
        if (m_active)
            m_mappedPorts[port] = m_provider->add_port_mapping(libt::session::tcp, port, port);
    }
}

void PortForwarder::deletePort(quint16 port)
{
    if (m_mappedPorts.contains(port)) {
        if (m_active)
            m_provider->delete_port_mapping(m_mappedPorts[port]);
        m_mappedPorts.remove(port);
    }
}

void PortForwarder::start()
{
    qDebug("Enabling UPnP / NAT-PMP");
#if LIBTORRENT_VERSION_NUM < 10100
    m_provider->start_upnp();
    m_provider->start_natpmp();
#else
    libt::settings_pack settingsPack = m_provider->get_settings();
    settingsPack.set_bool(libt::settings_pack::enable_upnp, true);
    settingsPack.set_bool(libt::settings_pack::enable_natpmp, true);
    m_provider->apply_settings(settingsPack);
#endif
    for (auto i = m_mappedPorts.begin(); i != m_mappedPorts.end(); ++i) {
        // quint16 port = i.key();
        i.value() = m_provider->add_port_mapping(libt::session::tcp, i.key(), i.key());
    }
    m_active = true;
    Logger::instance()->addMessage(tr("UPnP / NAT-PMP support [ON]"), Log::INFO);
}

void PortForwarder::stop()
{
    qDebug("Disabling UPnP / NAT-PMP");
#if LIBTORRENT_VERSION_NUM < 10100
    m_provider->stop_upnp();
    m_provider->stop_natpmp();
#else
    libt::settings_pack settingsPack = m_provider->get_settings();
    settingsPack.set_bool(libt::settings_pack::enable_upnp, false);
    settingsPack.set_bool(libt::settings_pack::enable_natpmp, false);
    m_provider->apply_settings(settingsPack);
#endif
    m_active = false;
    Logger::instance()->addMessage(tr("UPnP / NAT-PMP support [OFF]"), Log::INFO);
}

PortForwarder *PortForwarder::m_instance = nullptr;
