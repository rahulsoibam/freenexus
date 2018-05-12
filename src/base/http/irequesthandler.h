#ifndef HTTP_IREQUESTHANDLER_H
#define HTTP_IREQUESTHANDLER_H

#include "types.h"

namespace Http
{
    class IRequestHandler
    {
    public:
        virtual ~IRequestHandler() {}
        virtual Response processRequest(const Request &request, const Environment &env) = 0;
    };
}

#endif // HTTP_IREQUESTHANDLER_H

