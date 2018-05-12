#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QUrlQuery>

#include "base/logger.h"
#include "base/net/downloadhandler.h"
#include "base/net/downloadmanager.h"
#include "dnsupdater.h"

using namespace Net;

DNSUpdater::DNSUpdater(QObject *parent)
    : QObject(parent)
    , m_state(OK)
    , m_service(DNS::NONE)
{
    updateCredentials();

    // Load saved settings from previous session
    const Preferences *const pref = Preferences::instance();
    m_lastIPCheckTime = pref->getDNSLastUpd();
    m_lastIP = QHostAddress(pref->getDNSLastIP());

    // Start IP checking timer
    m_ipCheckTimer.setInterval(IP_CHECK_INTERVAL_MS);
    connect(&m_ipCheckTimer, SIGNAL(timeout()), SLOT(checkPublicIP()));
    m_ipCheckTimer.start();

    // Check lastUpdate to avoid flooding
    if (!m_lastIPCheckTime.isValid()
        || (m_lastIPCheckTime.secsTo(QDateTime::currentDateTime()) * 1000 > IP_CHECK_INTERVAL_MS)) {
        checkPublicIP();
    }
}

DNSUpdater::~DNSUpdater()
{
    // Save lastupdate time and last ip
    Preferences *const pref = Preferences::instance();
    pref->setDNSLastUpd(m_lastIPCheckTime);
    pref->setDNSLastIP(m_lastIP.toString());
}

void DNSUpdater::checkPublicIP()
{
    Q_ASSERT(m_state == OK);

    DownloadHandler *handler = DownloadManager::instance()->downloadUrl(
                "http://checkip.dyndns.org", false, 0, false,
                "freeNexus/" QBT_VERSION_2);
    connect(handler, SIGNAL(downloadFinished(QString, QByteArray)), SLOT(ipRequestFinished(QString, QByteArray)));
    connect(handler, SIGNAL(downloadFailed(QString, QString)), SLOT(ipRequestFailed(QString, QString)));

    m_lastIPCheckTime = QDateTime::currentDateTime();
}

void DNSUpdater::ipRequestFinished(const QString &url, const QByteArray &data)
{
    Q_UNUSED(url);

    // Parse response
    QRegExp ipregex("Current IP Address:\\s+([^<]+)</body>");
    if (ipregex.indexIn(data) >= 0) {
        QString ipStr = ipregex.cap(1);
        qDebug() << Q_FUNC_INFO << "Regular expression captured the following IP:" << ipStr;
        QHostAddress newIp(ipStr);
        if (!newIp.isNull()) {
            if (m_lastIP != newIp) {
                qDebug() << Q_FUNC_INFO << "The IP address changed, report the change to DynDNS...";
                qDebug() << m_lastIP.toString() << "->" << newIp.toString();
                m_lastIP = newIp;
                updateDNSService();
            }
        }
        else {
            qWarning() << Q_FUNC_INFO << "Failed to construct a QHostAddress from the IP string";
        }
    }
    else {
        qWarning() << Q_FUNC_INFO << "Regular expression failed to capture the IP address";
    }
}

void DNSUpdater::ipRequestFailed(const QString &url, const QString &error)
{
    Q_UNUSED(url);
    qWarning() << "IP request failed:" << error;
}

void DNSUpdater::updateDNSService()
{
    qDebug() << Q_FUNC_INFO;

    m_lastIPCheckTime = QDateTime::currentDateTime();
    DownloadHandler *handler = DownloadManager::instance()->downloadUrl(
                getUpdateUrl(), false, 0, false,
                "freeNexus/" QBT_VERSION_2);
    connect(handler, SIGNAL(downloadFinished(QString, QByteArray)), SLOT(ipUpdateFinished(QString, QByteArray)));
    connect(handler, SIGNAL(downloadFailed(QString, QString)), SLOT(ipUpdateFailed(QString, QString)));
}

QString DNSUpdater::getUpdateUrl() const
{
    QUrl url;
#ifdef QT_NO_OPENSSL
    url.setScheme("http");
#else
    url.setScheme("https");
#endif
    url.setUserName(m_username);
    url.setPassword(m_password);

    Q_ASSERT(!m_lastIP.isNull());
    // Service specific
    switch (m_service) {
    case DNS::DYNDNS:
        url.setHost("members.dyndns.org");
        break;
    case DNS::NOIP:
        url.setHost("dynupdate.no-ip.com");
        break;
    default:
        qWarning() << "Unrecognized Dynamic DNS service!";
        Q_ASSERT(0);
    }
    url.setPath("/nic/update");

    QUrlQuery urlQuery(url);
    urlQuery.addQueryItem("hostname", m_domain);
    urlQuery.addQueryItem("myip", m_lastIP.toString());
    url.setQuery(urlQuery);
    Q_ASSERT(url.isValid());

    qDebug() << Q_FUNC_INFO << url.toString();
    return url.toString();
}

void DNSUpdater::ipUpdateFinished(const QString &url, const QByteArray &data)
{
    Q_UNUSED(url);
    // Parse reply
    processIPUpdateReply(data);
}

void DNSUpdater::ipUpdateFailed(const QString &url, const QString &error)
{
    Q_UNUSED(url);
    qWarning() << "IP update failed:" << error;
}

void DNSUpdater::processIPUpdateReply(const QString &reply)
{
    Logger *const logger = Logger::instance();
    qDebug() << Q_FUNC_INFO << reply;
    QString code = reply.split(" ").first();
    qDebug() << Q_FUNC_INFO << "Code:" << code;

    if ((code == "good") || (code == "nochg")) {
        logger->addMessage(tr("Your dynamic DNS was successfully updated."), Log::INFO);
        return;
    }

    if ((code == "911") || (code == "dnserr")) {
        logger->addMessage(tr("Dynamic DNS error: The service is temporarily unavailable, it will be retried in 30 minutes."), Log::CRITICAL);
        m_lastIP.clear();
        // It will retry in 30 minutes because the timer was not stopped
        return;
    }

    // Everything below is an error, stop updating until the user updates something
    m_ipCheckTimer.stop();
    m_lastIP.clear();
    if (code == "nohost") {
        logger->addMessage(tr("Dynamic DNS error: hostname supplied does not exist under specified account."), Log::CRITICAL);
        m_state = INVALID_CREDS;
        return;
    }

    if (code == "badauth") {
        logger->addMessage(tr("Dynamic DNS error: Invalid username/password."), Log::CRITICAL);
        m_state = INVALID_CREDS;
        return;
    }

    if (code == "badagent") {
        logger->addMessage(tr("Dynamic DNS error: freeNexus was blacklisted by the service, please report a bug at http://bugs.freenexus.org."),
                           Log::CRITICAL);
        m_state = FATAL;
        return;
    }

    if (code == "!donator") {
        logger->addMessage(tr("Dynamic DNS error: %1 was returned by the service, please report a bug at http://bugs.freenexus.org.").arg("!donator"),
                           Log::CRITICAL);
        m_state = FATAL;
        return;
    }

    if (code == "abuse") {
        logger->addMessage(tr("Dynamic DNS error: Your username was blocked due to abuse."), Log::CRITICAL);
        m_state = FATAL;
    }
}

void DNSUpdater::updateCredentials()
{
    if (m_state == FATAL) return;
    Preferences *const pref = Preferences::instance();
    Logger *const logger = Logger::instance();
    bool change = false;
    // Get DNS service information
    if (m_service != pref->getDynDNSService()) {
        m_service = pref->getDynDNSService();
        change = true;
    }
    if (m_domain != pref->getDynDomainName()) {
        m_domain = pref->getDynDomainName();
        QRegExp domain_regex("^(?:(?!\\d|-)[a-zA-Z0-9\\-]{1,63}\\.)+[a-zA-Z]{2,}$");
        if (domain_regex.indexIn(m_domain) < 0) {
            logger->addMessage(tr("Dynamic DNS error: supplied domain name is invalid."), Log::CRITICAL);
            m_lastIP.clear();
            m_ipCheckTimer.stop();
            m_state = INVALID_CREDS;
            return;
        }
        change = true;
    }
    if (m_username != pref->getDynDNSUsername()) {
        m_username = pref->getDynDNSUsername();
        if (m_username.length() < 4) {
            logger->addMessage(tr("Dynamic DNS error: supplied username is too short."), Log::CRITICAL);
            m_lastIP.clear();
            m_ipCheckTimer.stop();
            m_state = INVALID_CREDS;
            return;
        }
        change = true;
    }
    if (m_password != pref->getDynDNSPassword()) {
        m_password = pref->getDynDNSPassword();
        if (m_password.length() < 4) {
            logger->addMessage(tr("Dynamic DNS error: supplied password is too short."), Log::CRITICAL);
            m_lastIP.clear();
            m_ipCheckTimer.stop();
            m_state = INVALID_CREDS;
            return;
        }
        change = true;
    }

    if ((m_state == INVALID_CREDS) && change) {
        m_state = OK; // Try again
        m_ipCheckTimer.start();
        checkPublicIP();
    }
}

QUrl DNSUpdater::getRegistrationUrl(int service)
{
    switch (service) {
    case DNS::DYNDNS:
        return QUrl("https://www.dyndns.com/account/services/hosts/add.html");
    case DNS::NOIP:
        return QUrl("https://www.noip.com/remote-access");
    default:
        Q_ASSERT(0);
    }
    return QUrl();
}
