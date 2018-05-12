#ifndef SCANFOLDERSMODEL_H
#define SCANFOLDERSMODEL_H

#include <QAbstractListModel>
#include <QList>

class QStringList;
class FileSystemWatcher;

class ScanFoldersModel: public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(ScanFoldersModel)

public:
    enum PathStatus
    {
        Ok,
        DoesNotExist,
        CannotRead,
        CannotWrite,
        AlreadyInList
    };

    enum Column
    {
        WATCH,
        DOWNLOAD,
        NB_COLUMNS
    };

    enum PathType
    {
        DOWNLOAD_IN_WATCH_FOLDER,
        DEFAULT_LOCATION,
        CUSTOM_LOCATION
    };

    static bool initInstance(QObject *parent = 0);
    static void freeInstance();
    static ScanFoldersModel* instance();

    static QString pathTypeDisplayName(const PathType type);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // TODO: removePaths(); singular version becomes private helper functions;
    // also: remove functions should take modelindexes
    PathStatus addPath(const QString &watchPath, const PathType &downloadType, const QString &downloadPath, bool addToFSWatcher = true);
    PathStatus updatePath(const QString &watchPath, const PathType &downloadType, const QString &downloadPath);
    // PRECONDITION: The paths must have been added with addPath() first.
    void addToFSWatcher(const QStringList &watchPaths);
    void removePath(int row, bool removeFromFSWatcher = true);
    bool removePath(const QString &path, bool removeFromFSWatcher = true);
    void removeFromFSWatcher(const QStringList &watchPaths);

    void makePersistent();

public slots:
    void configure();

private slots:
    void addTorrentsToSession(const QStringList &pathList);

private:
    explicit ScanFoldersModel(QObject *parent = 0);
    ~ScanFoldersModel();

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool downloadInWatchFolder(const QString &filePath) const;
    bool downloadInDefaultFolder(const QString &filePath) const;
    QString downloadPathTorrentFolder(const QString &filePath) const;
    int findPathData(const QString &path) const;

private:
    static ScanFoldersModel *m_instance;
    struct PathData;

    QList<PathData*> m_pathList;
    FileSystemWatcher *m_fsWatcher;
};

#endif // SCANFOLDERSMODEL_H
