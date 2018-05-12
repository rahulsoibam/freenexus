#include "responsebuilder.h"

using namespace Http;

void ResponseBuilder::status(uint code, const QString &text)
{
    m_response.status = ResponseStatus(code, text);
}

void ResponseBuilder::header(const QString &name, const QString &value)
{
    m_response.headers[name] = value;
}

void ResponseBuilder::print(const QString &text, const QString &type)
{
    print_impl(text.toUtf8(), type);
}

void ResponseBuilder::print(const QByteArray &data, const QString &type)
{
    print_impl(data, type);
}

void ResponseBuilder::clear()
{
    m_response = Response();
}

Response ResponseBuilder::response() const
{
    return m_response;
}

void ResponseBuilder::print_impl(const QByteArray &data, const QString &type)
{
    if (!m_response.headers.contains(HEADER_CONTENT_TYPE))
        m_response.headers[HEADER_CONTENT_TYPE] = type;

    m_response.content += data;
}
