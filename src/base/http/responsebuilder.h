#ifndef HTTP_RESPONSEBUILDER_H
#define HTTP_RESPONSEBUILDER_H

#include "types.h"

namespace Http
{
    class ResponseBuilder
    {
    public:
        void status(uint code = 200, const QString &text = QLatin1String("OK"));
        void header(const QString &name, const QString &value);
        void print(const QString &text, const QString &type = CONTENT_TYPE_HTML);
        void print(const QByteArray &data, const QString &type = CONTENT_TYPE_HTML);
        void clear();

        Response response() const;

    private:
        void print_impl(const QByteArray &data, const QString &type);

        Response m_response;
    };
}

#endif // HTTP_RESPONSEBUILDER_H
