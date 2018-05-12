#ifndef DOWNLOADEDPIECESBAR_H
#define DOWNLOADEDPIECESBAR_H

#include <QBitArray>
#include <QVector>
#include <QWidget>

#include "piecesbar.h"

class DownloadedPiecesBar : public PiecesBar
{
    using base = PiecesBar;
    Q_OBJECT
    Q_DISABLE_COPY(DownloadedPiecesBar)

public:
    DownloadedPiecesBar(QWidget *parent);

    void setProgress(const QBitArray &pieces, const QBitArray &downloadedPieces);

    void setColors(const QColor &background, const QColor &border, const QColor &complete, const QColor &incomplete);

    // PiecesBar interface
    void clear() override;

private:
    // scale bitfield vector to float vector
    QVector<float> bitfieldToFloatVector(const QBitArray &vecin, int reqSize);
    virtual bool updateImage(QImage &image) override;
    QString simpleToolTipText() const override;

    // incomplete piece color
    QColor m_dlPieceColor;
    // last used bitfields, uses to better resize redraw
    // TODO: make a diff pieces to new pieces and update only changed pixels, speedup when update > 20x faster
    QBitArray m_pieces;
    QBitArray m_downloadedPieces;
};

#endif // DOWNLOADEDPIECESBAR_H
