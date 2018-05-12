#ifndef BITTORRENT_TORRENTINFO_H
#define BITTORRENT_TORRENTINFO_H

#include <QCoreApplication>
#include <QtGlobal>

#include <libtorrent/torrent_info.hpp>
#include <libtorrent/version.hpp>

#include "base/indexrange.h"

class QString;
class QUrl;
class QDateTime;
class QStringList;
class QByteArray;
template<typename T> class QList;
template<typename T> class QVector;

namespace BitTorrent
{
    class InfoHash;
    class TrackerEntry;

    class TorrentInfo
    {
        Q_DECLARE_TR_FUNCTIONS("TorrentInfo")

    public:
#if LIBTORRENT_VERSION_NUM < 10100
        typedef boost::intrusive_ptr<const libtorrent::torrent_info> NativeConstPtr;
        typedef boost::intrusive_ptr<libtorrent::torrent_info> NativePtr;
#else
        typedef boost::shared_ptr<const libtorrent::torrent_info> NativeConstPtr;
        typedef boost::shared_ptr<libtorrent::torrent_info> NativePtr;
#endif

        explicit TorrentInfo(NativeConstPtr nativeInfo = NativeConstPtr());
        TorrentInfo(const TorrentInfo &other);

        static TorrentInfo load(const QByteArray &data, QString *error = nullptr) noexcept;
        static TorrentInfo loadFromFile(const QString &path, QString *error = nullptr) noexcept;

        TorrentInfo &operator=(const TorrentInfo &other);

        bool isValid() const;
        InfoHash hash() const;
        QString name() const;
        QDateTime creationDate() const;
        QString creator() const;
        QString comment() const;
        bool isPrivate() const;
        qlonglong totalSize() const;
        int filesCount() const;
        int pieceLength() const;
        int pieceLength(int index) const;
        int piecesCount() const;
        QString filePath(int index) const;
        QStringList filePaths() const;
        QString fileName(int index) const;
        QString origFilePath(int index) const;
        qlonglong fileSize(int index) const;
        qlonglong fileOffset(int index) const;
        QList<TrackerEntry> trackers() const;
        QList<QUrl> urlSeeds() const;
        QByteArray metadata() const;
        QStringList filesForPiece(int pieceIndex) const;
        QVector<int> fileIndicesForPiece(int pieceIndex) const;
        QVector<QByteArray> pieceHashes() const;

        using PieceRange = IndexRange<int>;
        // returns pair of the first and the last pieces into which
        // the given file extends (maybe partially).
        PieceRange filePieces(const QString &file) const;
        PieceRange filePieces(int fileIndex) const;

        void renameFile(uint index, const QString &newPath);

        bool hasRootFolder() const;
        void stripRootFolder();

        NativePtr nativeInfo() const;

    private:
        // returns file index or -1 if fileName is not found
        int fileIndex(const QString &fileName) const;
        NativePtr m_nativeInfo;
    };
}

#endif // BITTORRENT_TORRENTINFO_H
