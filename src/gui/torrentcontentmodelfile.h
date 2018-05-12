#ifndef TORRENTCONTENTMODELFILE_H
#define TORRENTCONTENTMODELFILE_H

#include "torrentcontentmodelitem.h"

class TorrentContentModelFile: public TorrentContentModelItem
{
public:
    TorrentContentModelFile(const QString &fileName, qulonglong fileSize,
                            TorrentContentModelFolder *parent, int fileIndex);

    int fileIndex() const;
    void setPriority(int newPriority, bool updateParent = true) override;
    void setProgress(qreal progress);
    void setAvailability(qreal availability);
    ItemType itemType() const override;

private:
    int m_fileIndex;
};

#endif // TORRENTCONTENTMODELFILE_H
