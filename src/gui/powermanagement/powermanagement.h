#ifndef POWERMANAGEMENT_H
#define POWERMANAGEMENT_H

#include <QObject>

#ifdef Q_OS_MAC
// Require Mac OS X >= 10.5
#include <IOKit/pwr_mgt/IOPMLib.h>
#endif

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC)) && defined(QT_DBUS_LIB)
// Require DBus
class PowerManagementInhibitor;
#endif

class PowerManagement : public QObject
{
  Q_OBJECT

public:
  PowerManagement(QObject *parent = 0);
  virtual ~PowerManagement();

  void setActivityState(bool busy);

private:
  bool m_busy;

  void setBusy();
  void setIdle();

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC)) && defined(QT_DBUS_LIB)
  PowerManagementInhibitor *m_inhibitor;
#endif
#ifdef Q_OS_MAC
  IOPMAssertionID m_assertionID;
#endif
};

#endif // POWERMANAGEMENT_H
