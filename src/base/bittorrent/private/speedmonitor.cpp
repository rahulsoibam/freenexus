#include "speedmonitor.h"

SpeedMonitor::SpeedMonitor()
    : m_speedSamples(MAX_SAMPLES)
{
}

void SpeedMonitor::addSample(const SpeedSample &sample)
{
    if (m_speedSamples.size() >= MAX_SAMPLES) {
        m_sum -= m_speedSamples.front();
    }

    m_speedSamples.push_back(sample);
    m_sum += sample;
}

SpeedSampleAvg SpeedMonitor::average() const
{
    if (m_speedSamples.empty())
        return SpeedSampleAvg();

    qreal k = qreal(1.) / m_speedSamples.size();
    return SpeedSampleAvg(m_sum.download * k, m_sum.upload * k);
}

void SpeedMonitor::reset()
{
    m_sum = SpeedSample();
    m_speedSamples.clear();
}
