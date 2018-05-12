#pragma once

#include <QSet>
#include <QString>
#include <QVector>

#include "../tristatebool.h"

namespace BitTorrent
{
    struct AddTorrentParams
    {
        QString name;
        QString category;
        QSet<QString> tags;
        QString savePath;
        bool disableTempPath = false; // e.g. for imported torrents
        bool sequential = false;
        bool firstLastPiecePriority = false;
        TriStateBool addForced;
        TriStateBool addPaused;
        QVector<int> filePriorities; // used if TorrentInfo is set
        bool ignoreShareLimits = false;
        bool skipChecking = false;
        TriStateBool createSubfolder;
        TriStateBool useAutoTMM;
        int uploadLimit = -1;
        int downloadLimit = -1;
    };
}
