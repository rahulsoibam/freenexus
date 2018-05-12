#include "bandwidthscheduler.h"

#include <utility>

#include <QDate>
#include <QTime>

#include "base/preferences.h"

BandwidthScheduler::BandwidthScheduler(QObject *parent)
    : QObject(parent)
    , m_lastAlternative(false)
{
    connect(&m_timer, &QTimer::timeout, this, &BandwidthScheduler::onTimeout);
}

void BandwidthScheduler::start()
{
    m_lastAlternative = isTimeForAlternative();
    emit bandwidthLimitRequested(m_lastAlternative);

    // Timeout regularly to accommodate for external system clock changes
    // eg from the user or from a timesync utility
    m_timer.start(30000);
}

bool BandwidthScheduler::isTimeForAlternative() const
{
    const Preferences* const pref = Preferences::instance();

    QTime start = pref->getSchedulerStartTime();
    QTime end = pref->getSchedulerEndTime();
    const QTime now = QTime::currentTime();
    const int schedulerDays = pref->getSchedulerDays();
    const int day = QDate::currentDate().dayOfWeek();
    bool alternative = false;

    if (start > end) {
        std::swap(start, end);
        alternative = true;
    }

    if ((start <= now) && (end >= now)) {
        switch (schedulerDays) {
        case EVERY_DAY:
            alternative = !alternative;
            break;
        case WEEK_ENDS:
            if ((day == 6) || (day == 7))
                alternative = !alternative;
            break;
        case WEEK_DAYS:
            if ((day != 6) && (day != 7))
                alternative = !alternative;
            break;
        default:
            if (day == (schedulerDays - 2))
                alternative = !alternative;
        }
    }

    return alternative;
}

void BandwidthScheduler::onTimeout()
{
    bool alternative = isTimeForAlternative();

    if (alternative != m_lastAlternative) {
        m_lastAlternative = alternative;
        emit bandwidthLimitRequested(alternative);
    }
}
