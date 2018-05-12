#ifndef PIECESBAR_H
#define PIECESBAR_H

#include <QColor>
#include <QImage>
#include <QWidget>

class QHelpEvent;

namespace BitTorrent
{
    class TorrentHandle;
}

class PiecesBar : public QWidget
{
    using base = QWidget;
    Q_OBJECT
    Q_DISABLE_COPY(PiecesBar)

public:
    explicit PiecesBar(QWidget *parent = nullptr);

    void setTorrent(BitTorrent::TorrentHandle *torrent);
    void setColors(const QColor &background, const QColor &border, const QColor &complete);

    virtual void clear();

    // QObject interface
    virtual bool event(QEvent*) override;

protected:
    // QWidget interface
    void enterEvent(QEvent*) override;
    void leaveEvent(QEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;

    void paintEvent(QPaintEvent*) override;
    void requestImageUpdate();

    QColor backgroundColor() const;
    QColor borderColor() const;
    QColor pieceColor() const;
    const QVector<QRgb> &pieceColors() const;

    // mix two colors by light model, ratio <0, 1>
    static QRgb mixTwoColors(QRgb rgb1, QRgb rgb2, float ratio);

    static constexpr int borderWidth = 1;

private:
    void showToolTip(const QHelpEvent*);
    void highlightFile(int imagePos);

    virtual QString simpleToolTipText() const = 0;

    /// whether to perform removing of ".unwanted" directory from paths
    virtual bool isFileNameCorrectionNeeded() const;

    // draw new image to replace the actual image
    // returns true if image was successfully updated
    virtual bool updateImage(QImage &image) = 0;
    void updatePieceColors();

    const BitTorrent::TorrentHandle *m_torrent;
    QImage m_image;
    // I used values, because it should be possible to change colors at run time
    // border color
    QColor m_borderColor;
    // background color
    QColor m_bgColor;
    // complete piece color
    QColor m_pieceColor;
    // buffered 256 levels gradient from bg_color to piece_color
    QVector<QRgb> m_pieceColors;
    bool m_hovered;
    QRect m_highlitedRegion; //!< part of the bar can be highlighted; this rectangle is in the same frame as m_image
};

#endif // PIECESBAR_H
