#include <QByteArray>
#include <QRegExp>
#include <QStringList>

#include <libtorrent/bencode.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/magnet_uri.hpp>

#include "base/utils/string.h"
#include "magneturi.h"

namespace
{
    QString bcLinkToMagnet(QString bcLink)
    {
        QByteArray rawBc = bcLink.toUtf8();
        rawBc = rawBc.mid(8); // skip bc://bt/
        rawBc = QByteArray::fromBase64(rawBc); // Decode base64
        // Format is now AA/url_encoded_filename/size_bytes/info_hash/ZZ
        QStringList parts = QString(rawBc).split("/");
        if (parts.size() != 5) return QString();

        QString filename = parts.at(1);
        QString hash = parts.at(3);
        QString magnet = "magnet:?xt=urn:btih:" + hash;
        magnet += "&dn=" + filename;
        return magnet;
    }
}

namespace libt = libtorrent;
using namespace BitTorrent;

MagnetUri::MagnetUri(const QString &source)
    : m_valid(false)
    , m_url(source)
{
    if (source.isEmpty()) return;

    if (source.startsWith("bc://bt/", Qt::CaseInsensitive)) {
        qDebug("Creating magnet link from bc link");
        m_url = bcLinkToMagnet(source);
    }
    else if (((source.size() == 40) && !source.contains(QRegExp("[^0-9A-Fa-f]")))
             || ((source.size() == 32) && !source.contains(QRegExp("[^2-7A-Za-z]")))) {
        m_url = "magnet:?xt=urn:btih:" + source;
    }

    libt::error_code ec;
    libt::parse_magnet_uri(m_url.toUtf8().constData(), m_addTorrentParams, ec);
    if (ec) return;

    m_valid = true;
    m_hash = m_addTorrentParams.info_hash;
    m_name = QString::fromStdString(m_addTorrentParams.name);

    foreach (const std::string &tracker, m_addTorrentParams.trackers)
        m_trackers.append(QString::fromStdString(tracker));

    foreach (const std::string &urlSeed, m_addTorrentParams.url_seeds)
        m_urlSeeds.append(QUrl(urlSeed.c_str()));
}

bool MagnetUri::isValid() const
{
    return m_valid;
}

InfoHash MagnetUri::hash() const
{
    return m_hash;
}

QString MagnetUri::name() const
{
    return m_name;
}

QList<TrackerEntry> MagnetUri::trackers() const
{
    return m_trackers;
}

QList<QUrl> MagnetUri::urlSeeds() const
{
    return m_urlSeeds;
}

QString MagnetUri::url() const
{
    return m_url;
}

libtorrent::add_torrent_params MagnetUri::addTorrentParams() const
{
    return m_addTorrentParams;
}
