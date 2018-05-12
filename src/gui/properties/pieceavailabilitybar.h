#ifndef PIECEAVAILABILITYBAR_H
#define PIECEAVAILABILITYBAR_H

#include "piecesbar.h"

class PieceAvailabilityBar : public PiecesBar
{
    using base = PiecesBar;
    Q_OBJECT
    Q_DISABLE_COPY(PieceAvailabilityBar)

public:
    PieceAvailabilityBar(QWidget *parent);

    void setAvailability(const QVector<int> &avail);

    // PiecesBar interface
    void clear() override;

private:
    bool updateImage(QImage &image) override;
    QString simpleToolTipText() const override;
    bool isFileNameCorrectionNeeded() const override;

    // last used int vector, uses to better resize redraw
    // TODO: make a diff pieces to new pieces and update only changed pixels, speedup when update > 20x faster
    QVector<int> m_pieces;

    // scale int vector to float vector
    QVector<float> intToFloatVector(const QVector<int> &vecin, int reqSize);
};

#endif // PIECEAVAILABILITYBAR_H
