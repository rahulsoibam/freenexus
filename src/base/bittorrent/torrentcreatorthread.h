#ifndef BITTORRENT_TORRENTCREATORTHREAD_H
#define BITTORRENT_TORRENTCREATORTHREAD_H

#include <QStringList>
#include <QThread>

namespace BitTorrent
{
    struct TorrentCreatorParams
    {
        bool isPrivate;
        bool isAlignmentOptimized;
        int pieceSize;
        QString inputPath;
        QString savePath;
        QString comment;
        QString source;
        QStringList trackers;
        QStringList urlSeeds;
    };

    class TorrentCreatorThread : public QThread
    {
        Q_OBJECT

    public:
        TorrentCreatorThread(QObject *parent = nullptr);
        ~TorrentCreatorThread();

        void create(const TorrentCreatorParams &params);

        static int calculateTotalPieces(const QString &inputPath, const int pieceSize, const bool isAlignmentOptimized);

    protected:
        void run();

    signals:
        void creationFailure(const QString &msg);
        void creationSuccess(const QString &path, const QString &branchPath);
        void updateProgress(int progress);

    private:
        void sendProgressSignal(int currentPieceIdx, int totalPieces);

        TorrentCreatorParams m_params;
    };
}

#endif // BITTORRENT_TORRENTCREATORTHREAD_H
