#include "torrentcreatorthread.h"

#include <fstream>

#include <boost/bind.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/storage.hpp>
#include <libtorrent/torrent_info.hpp>

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QHash>

#include "base/global.h"
#include "base/utils/fs.h"
#include "base/utils/misc.h"
#include "base/utils/string.h"

namespace
{
    // do not include files and folders whose
    // name starts with a .
    bool fileFilter(const std::string &f)
    {
        return !Utils::Fs::fileName(QString::fromStdString(f)).startsWith('.');
    }
}

namespace libt = libtorrent;
using namespace BitTorrent;

TorrentCreatorThread::TorrentCreatorThread(QObject *parent)
    : QThread(parent)
{
}

TorrentCreatorThread::~TorrentCreatorThread()
{
    requestInterruption();
    wait();
}

void TorrentCreatorThread::create(const TorrentCreatorParams &params)
{
    m_params = params;
    start();
}

void TorrentCreatorThread::sendProgressSignal(int currentPieceIdx, int totalPieces)
{
    emit updateProgress(static_cast<int>((currentPieceIdx * 100.) / totalPieces));
}

void TorrentCreatorThread::run()
{
    const QString creatorStr("freeNexus " QBT_VERSION);

    emit updateProgress(0);

    try {
        const QString parentPath = Utils::Fs::branchPath(m_params.inputPath) + "/";

        // Adding files to the torrent
        libt::file_storage fs;
        if (QFileInfo(m_params.inputPath).isFile()) {
            libt::add_files(fs, Utils::Fs::toNativePath(m_params.inputPath).toStdString(), fileFilter);
        }
        else {
            // need to sort the file names by natural sort order
            QStringList dirs = {m_params.inputPath};

            QDirIterator dirIter(m_params.inputPath, (QDir::AllDirs | QDir::NoDotAndDotDot), QDirIterator::Subdirectories);
            while (dirIter.hasNext()) {
                dirIter.next();
                dirs += dirIter.filePath();
            }
            std::sort(dirs.begin(), dirs.end(), Utils::String::naturalLessThan<Qt::CaseInsensitive>);

            QStringList fileNames;
            QHash<QString, boost::int64_t> fileSizeMap;

            for (const auto &dir : qAsConst(dirs)) {
                QStringList tmpNames;  // natural sort files within each dir

                QDirIterator fileIter(dir, QDir::Files);
                while (fileIter.hasNext()) {
                    fileIter.next();

                    const QString relFilePath = fileIter.filePath().mid(parentPath.length());
                    tmpNames += relFilePath;
                    fileSizeMap[relFilePath] = fileIter.fileInfo().size();
                }

                std::sort(tmpNames.begin(), tmpNames.end(), Utils::String::naturalLessThan<Qt::CaseInsensitive>);
                fileNames += tmpNames;
            }

            for (const auto &fileName : qAsConst(fileNames))
                fs.add_file(fileName.toStdString(), fileSizeMap[fileName]);
        }

        if (isInterruptionRequested()) return;

        libt::create_torrent newTorrent(fs, m_params.pieceSize, -1
            , (m_params.isAlignmentOptimized ? libt::create_torrent::optimize_alignment : 0));

        // Add url seeds
        foreach (QString seed, m_params.urlSeeds) {
            seed = seed.trimmed();
            if (!seed.isEmpty())
                newTorrent.add_url_seed(seed.toStdString());
        }

        int tier = 0;
        foreach (const QString &tracker, m_params.trackers) {
            if (tracker.isEmpty())
                ++tier;
            else
                newTorrent.add_tracker(tracker.trimmed().toStdString(), tier);
        }

        if (isInterruptionRequested()) return;

        // calculate the hash for all pieces
        libt::set_piece_hashes(newTorrent, Utils::Fs::toNativePath(parentPath).toStdString()
            , [this, &newTorrent](const int n) { sendProgressSignal(n, newTorrent.num_pieces()); });
        // Set freeNexus as creator and add user comment to
        // torrent_info structure
        newTorrent.set_creator(creatorStr.toUtf8().constData());
        newTorrent.set_comment(m_params.comment.toUtf8().constData());
        // Is private ?
        newTorrent.set_priv(m_params.isPrivate);

        if (isInterruptionRequested()) return;

        libt::entry entry = newTorrent.generate();

        // add source field
        if (!m_params.source.isEmpty())
            entry["info"]["source"] = m_params.source.toStdString();

        if (isInterruptionRequested()) return;

        // create the torrent
        std::ofstream outfile(
#ifdef _MSC_VER
            Utils::Fs::toNativePath(m_params.savePath).toStdWString().c_str()
#else
            Utils::Fs::toNativePath(m_params.savePath).toUtf8().constData()
#endif
            , (std::ios_base::out | std::ios_base::binary | std::ios_base::trunc));
        if (outfile.fail())
            throw std::runtime_error(tr("create new torrent file failed").toStdString());

        if (isInterruptionRequested()) return;

        libt::bencode(std::ostream_iterator<char>(outfile), entry);
        outfile.close();

        emit updateProgress(100);
        emit creationSuccess(m_params.savePath, parentPath);
    }
    catch (const std::exception &e) {
        emit creationFailure(e.what());
    }
}

int TorrentCreatorThread::calculateTotalPieces(const QString &inputPath, const int pieceSize, const bool isAlignmentOptimized)
{
    if (inputPath.isEmpty())
        return 0;

    libt::file_storage fs;
    libt::add_files(fs, Utils::Fs::toNativePath(inputPath).toStdString(), fileFilter);
    return libt::create_torrent(fs, pieceSize, -1
        , (isAlignmentOptimized ? libt::create_torrent::optimize_alignment : 0)).num_pieces();
}
