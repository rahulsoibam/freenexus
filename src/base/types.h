#ifndef TYPES_H
#define TYPES_H

#include <QMap>

const qlonglong MAX_ETA = 8640000;

enum class ShutdownDialogAction
{
    Exit,
    Shutdown,
    Suspend,
    Hibernate
};

typedef QMap<QString, QString> QStringMap;

#endif // TYPES_H
