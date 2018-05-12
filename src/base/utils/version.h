#ifndef FREENEXUS_UTILS_VERSION_H
#define FREENEXUS_UTILS_VERSION_H

#include <array>
#include <stdexcept>

#include <QDebug>
#include <QString>
#include <QStringList>

namespace Utils
{
    template <typename T, std::size_t N, std::size_t Mandatory = N>
    class Version
    {
        static_assert(N > 0, "The number of version components may not be smaller than 1");
        static_assert(N >= Mandatory,
                      "The number of mandatory components may not be larger than the total number of components");

    public:
        typedef T ComponentType;
        typedef Version<T, N, Mandatory> ThisType;

        constexpr Version()
            : m_components {{}}
        {
        }

        constexpr Version(const ThisType &other) = default;

        template <typename ... Other>
        constexpr Version(Other ... components)
            : m_components {{components ...}}
        {
        }

        /**
         * @brief Creates version from string in format "x.y.z"
         *
         * @param version Version string in format "x.y.z"
         * @throws std::runtime_error if parsing fails
         */
        Version(const QString &version)
            : Version {version.split(QLatin1Char('.'))}
        {
        }

        /**
         * @brief Creates version from byte array in format "x.y.z"
         *
         * @param version Version string in format "x.y.z"
         * @throws std::runtime_error if parsing fails
         */
        Version(const QByteArray &version)
            : Version {version.split('.')}
        {
        }

        ComponentType majorNumber() const
        {
            static_assert(N >= 1, "The number of version components is too small");
            return (*this)[0];
        }

        ComponentType minorNumber() const
        {
            static_assert(N >= 2, "The number of version components is too small");
            return (*this)[1];
        }

        ComponentType revisionNumber() const
        {
            static_assert(N >= 3, "The number of version components is too small");
            return (*this)[2];
        }

        ComponentType patchNumber() const
        {
            static_assert(N >= 4, "The number of version components is too small");
            return (*this)[3];
        }

        ComponentType operator[](std::size_t i) const
        {
            return m_components.at(i);
        }

        operator QString() const
        {
            // find the last one non-zero component
            std::size_t lastSignificantIndex = N - 1;
            while (lastSignificantIndex > 0 && (*this)[lastSignificantIndex] == 0)
                --lastSignificantIndex;

            if (lastSignificantIndex + 1 < Mandatory)     // lastSignificantIndex >= 0
                lastSignificantIndex = Mandatory - 1;     // and Mandatory >= 1

            QString res = QString::number((*this)[0]);
            for (std::size_t i = 1; i <= lastSignificantIndex; ++i)
                res += QLatin1Char('.') + QString::number((*this)[i]);
            return res;
        }

        bool operator==(const ThisType &other) const
        {
            return m_components == other.m_components;
        }

        bool operator<(const ThisType &other) const
        {
            return m_components < other.m_components;
        }

        bool operator>(const ThisType &other) const
        {
            return m_components > other.m_components;
        }

        template <typename StringClassWithSplitMethod>
        static Version tryParse(const StringClassWithSplitMethod &s, const Version &defaultVersion)
        {
            try {
                return Version(s);
            }
            catch (std::runtime_error &er) {
                qDebug() << "Error parsing version:" << er.what();
                return defaultVersion;
            }
        }

    private:
        using ComponentsArray = std::array<T, N>;

        template <typename StringsList>
        static ComponentsArray parseList(const StringsList &versionParts)
        {
            if ((static_cast<std::size_t>(versionParts.size()) > N)
                || (static_cast<std::size_t>(versionParts.size()) < Mandatory))
                throw std::runtime_error ("Incorrect number of version components");

            bool ok = false;
            ComponentsArray res {{}};
            for (std::size_t i = 0; i < static_cast<std::size_t>(versionParts.size()); ++i) {
                res[i] = static_cast<T>(versionParts[i].toInt(&ok));
                if (!ok)
                    throw std::runtime_error("Can not parse version component");
            }
            return res;
        }

        template <typename StringsList>
        Version(const StringsList &versionParts)
            : m_components (parseList(versionParts)) // GCC 4.8.4 has problems with the uniform initializer here
        {
        }

        ComponentsArray m_components;
    };

    template <typename T, std::size_t N, std::size_t Mandatory>
    inline bool operator!=(const Version<T, N, Mandatory> &left, const Version<T, N, Mandatory> &right)
    {
        return !(left == right);
    }
}

#endif // FREENEXUS_UTILS_VERSION_H
