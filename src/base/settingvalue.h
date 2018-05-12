#ifndef SETTINGVALUE_H
#define SETTINGVALUE_H

#include <functional>
#include <type_traits>
#include <QMetaEnum>
#include <QString>
#include <QVariant>

#include "settingsstorage.h"

template <typename T>
class CachedSettingValue
{
    using ProxyFunc = std::function<T (const T&)>;

public:
    explicit CachedSettingValue(const char *keyName, const T &defaultValue = T())
        : m_keyName(QLatin1String(keyName))
        , m_value(loadValue(defaultValue))
    {
    }

    explicit CachedSettingValue(const char *keyName, const T &defaultValue
            , const ProxyFunc &proxyFunc)
        : m_keyName(QLatin1String(keyName))
        , m_value(proxyFunc(loadValue(defaultValue)))
    {
    }

    T value() const
    {
        return m_value;
    }

    operator T() const
    {
        return value();
    }

    CachedSettingValue<T> &operator=(const T &newValue)
    {
        m_value = newValue;
        storeValue(m_value);
        return *this;
    }

private:
    // regular load/save pair
    template <typename U, typename std::enable_if<!std::is_enum<U>::value, int>::type = 0>
    U loadValue(const U &defaultValue)
    {
        return SettingsStorage::instance()->loadValue(m_keyName, defaultValue).template value<T>();
    }

    template <typename U, typename std::enable_if<!std::is_enum<U>::value, int>::type = 0>
    void storeValue(const U &value)
    {
        SettingsStorage::instance()->storeValue(m_keyName, value);
    }

    // load/save pair for an enum
    // saves literal value of the enum constant, obtained from QMetaEnum
    template <typename U, typename std::enable_if<std::is_enum<U>::value, int>::type = 0>
    U loadValue(const U &defaultValue)
    {
        static_assert(std::is_same<int, typename std::underlying_type<U>::type>::value,
                      "Enumeration underlying type has to be int");

        bool ok = false;
        const U res = static_cast<U>(QMetaEnum::fromType<U>().keyToValue(
            SettingsStorage::instance()->loadValue(m_keyName).toString().toLatin1().constData(), &ok));
        return ok ? res : defaultValue;
    }

    template <typename U, typename std::enable_if<std::is_enum<U>::value, int>::type = 0>
    void storeValue(const U &value)
    {
        SettingsStorage::instance()->storeValue(m_keyName,
            QString::fromLatin1(QMetaEnum::fromType<U>().valueToKey(static_cast<int>(value))));
    }

    const QString m_keyName;
    T m_value;
};

#endif // SETTINGVALUE_H
