#ifndef HTTP_RESPONSEGENERATOR_H
#define HTTP_RESPONSEGENERATOR_H

#include "types.h"

namespace Http
{
    QByteArray toByteArray(Response response);
    QString httpDate();
    void compressContent(Response &response);
}

#endif // HTTP_RESPONSEGENERATOR_H
