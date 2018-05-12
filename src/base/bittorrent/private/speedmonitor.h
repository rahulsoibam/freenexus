#ifndef SPEEDMONITOR_H
#define SPEEDMONITOR_H

#ifndef Q_MOC_RUN
#include <boost/circular_buffer.hpp>
#endif

#include <QtGlobal>

template<typename T>
struct Sample
{
    Sample()
        : download()
        , upload()
    {
    }

    Sample(T dl, T ul)
        : download(dl)
        , upload(ul)
    {
    }

    Sample<T> &operator+=(const Sample<T> &other)
    {
        download += other.download;
        upload   += other.upload;
        return *this;
    }

    Sample<T> &operator-=(const Sample<T> &other)
    {
        download -= other.download;
        upload   -= other.upload;
        return *this;
    }

    T download;
    T upload;
};

typedef Sample<qlonglong> SpeedSample;
typedef Sample<qreal> SpeedSampleAvg;

class SpeedMonitor
{
public:
    SpeedMonitor();

    void addSample(const SpeedSample &sample);
    SpeedSampleAvg average() const;
    void reset();

private:
    static const int MAX_SAMPLES = 30;
    boost::circular_buffer<SpeedSample> m_speedSamples;
    SpeedSample m_sum;
};

#endif // SPEEDMONITOR_H
