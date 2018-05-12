#include <QtGlobal>

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC)) && defined(QT_DBUS_LIB)
#include "powermanagement_x11.h"
#endif
#include "powermanagement.h"

#ifdef Q_OS_MAC
#include <IOKit/pwr_mgt/IOPMLib.h>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#endif

PowerManagement::PowerManagement(QObject *parent) : QObject(parent), m_busy(false)
{
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC)) && defined(QT_DBUS_LIB)
    m_inhibitor = new PowerManagementInhibitor(this);
#endif
}

PowerManagement::~PowerManagement()
{
}

void PowerManagement::setActivityState(bool busy)
{
    if (busy) setBusy();
    else setIdle();
}

void PowerManagement::setBusy()
{
    if (m_busy) return;
    m_busy = true;

#ifdef Q_OS_WIN
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
#elif (defined(Q_OS_UNIX) && !defined(Q_OS_MAC)) && defined(QT_DBUS_LIB)
    m_inhibitor->RequestBusy();
#elif defined(Q_OS_MAC)
    IOReturn success = IOPMAssertionCreate(kIOPMAssertionTypeNoIdleSleep, kIOPMAssertionLevelOn, &m_assertionID);
    if (success != kIOReturnSuccess) m_busy = false;
#endif
}

void PowerManagement::setIdle()
{
    if (!m_busy) return;
    m_busy = false;

#ifdef Q_OS_WIN
    SetThreadExecutionState(ES_CONTINUOUS);
#elif (defined(Q_OS_UNIX) && !defined(Q_OS_MAC)) && defined(QT_DBUS_LIB)
    m_inhibitor->RequestIdle();
#elif defined(Q_OS_MAC)
    IOPMAssertionRelease(m_assertionID);
#endif
}
