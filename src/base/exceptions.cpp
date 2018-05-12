#include "exceptions.h"

RuntimeError::RuntimeError(const QString &message)
    : std::runtime_error {message.toUtf8().data()}
    , m_message {message}
{
}

QString RuntimeError::message() const
{
    return m_message;
}
