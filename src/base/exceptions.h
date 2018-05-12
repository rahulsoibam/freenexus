#pragma once

#include <stdexcept>
#include <QString>

class RuntimeError : public std::runtime_error
{
public:
    explicit RuntimeError(const QString &message = "");
    QString message() const;

private:
    const QString m_message;
};
