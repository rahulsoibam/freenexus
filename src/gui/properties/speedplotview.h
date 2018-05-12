#ifndef SPEEDPLOTVIEW_H
#define SPEEDPLOTVIEW_H

#ifndef Q_MOC_RUN
#include <boost/circular_buffer.hpp>
#endif

#include <QGraphicsView>
#include <QMap>

class QPen;

class SpeedPlotView : public QGraphicsView
{
    Q_OBJECT

public:
    enum GraphID
    {
        UP = 0,
        DOWN,
        PAYLOAD_UP,
        PAYLOAD_DOWN,
        OVERHEAD_UP,
        OVERHEAD_DOWN,
        DHT_UP,
        DHT_DOWN,
        TRACKER_UP,
        TRACKER_DOWN,

        NB_GRAPHS
    };

    enum TimePeriod
    {
        MIN1 = 0,
        MIN5,
        MIN30,
        HOUR6
    };

    struct PointData
    {
        qint64 x;
        int y[NB_GRAPHS];
    };

    explicit SpeedPlotView(QWidget *parent = nullptr);

    void setGraphEnable(GraphID id, bool enable);
    void setViewableLastPoints(TimePeriod period);

    void pushPoint(PointData point);

    void replot();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    enum PeriodInSeconds
    {
        MIN1_SEC = 60,
        MIN5_SEC = 5 * 60,
        MIN30_SEC = 30 * 60,
        HOUR6_SEC = 6 * 60 * 60
    };

    enum PointsToSave
    {
        MIN5_BUF_SIZE = 5 * 60,
        MIN30_BUF_SIZE = 10 * 60,
        HOUR6_BUF_SIZE = 20 * 60
    };

    struct GraphProperties
    {
        GraphProperties();
        GraphProperties(const QString &name, const QPen &pen, bool enable = false);

        QString name;
        QPen pen;
        bool enable;
    };

    int maxYValue();
    boost::circular_buffer<PointData> &getCurrentData();

    boost::circular_buffer<PointData> m_data5Min;
    boost::circular_buffer<PointData> m_data30Min;
    boost::circular_buffer<PointData> m_data6Hour;
    QMap<GraphID, GraphProperties> m_properties;

    TimePeriod m_period;
    int m_viewablePointsCount;

    int m_counter30Min;
    int m_counter6Hour;
};

#endif // SPEEDPLOTVIEW_H
