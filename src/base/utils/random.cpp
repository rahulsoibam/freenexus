#include "random.h"

#include <limits>
#include <random>

#include <QtGlobal>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <Ntsecapi.h>
#else  // Q_OS_WIN
#include <cstdio>
#endif

#include "misc.h"

namespace
{
#ifdef Q_OS_WIN
    class RandomLayer
    {
    // need to satisfy UniformRandomBitGenerator requirements
    public:
        using result_type = uint32_t;

        RandomLayer()
            : m_rtlGenRandom {Utils::Misc::loadWinAPI<PRTLGENRANDOM>("Advapi32.dll", "SystemFunction036")}
        {
            if (!m_rtlGenRandom)
                qFatal("Failed to load RtlGenRandom()");
        }

        static constexpr result_type min()
        {
            return std::numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return std::numeric_limits<result_type>::max();
        }

        result_type operator()()
        {
            result_type buf = 0;
            const bool result = m_rtlGenRandom(&buf, sizeof(buf));
            if (!result)
                qFatal("RtlGenRandom() failed");

            return buf;
        }

    private:
        using PRTLGENRANDOM = BOOLEAN (WINAPI *)(PVOID, ULONG);
        const PRTLGENRANDOM m_rtlGenRandom;
    };
#else  // Q_OS_WIN
    class RandomLayer
    {
    // need to satisfy UniformRandomBitGenerator requirements
    public:
        using result_type = uint32_t;

        RandomLayer()
            : m_randDev {fopen("/dev/urandom", "rb")}
        {
            if (!m_randDev)
                qFatal("Failed to open /dev/urandom");
        }

        ~RandomLayer()
        {
            fclose(m_randDev);
        }

        static constexpr result_type min()
        {
            return std::numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return std::numeric_limits<result_type>::max();
        }

        result_type operator()() const
        {
            result_type buf = 0;
            if (fread(&buf, sizeof(buf), 1, m_randDev) != 1)
                qFatal("Read /dev/urandom error");

            return buf;
        }

    private:
        FILE *m_randDev;
    };
#endif
}

uint32_t Utils::Random::rand(const uint32_t min, const uint32_t max)
{
    static RandomLayer layer;

    // new distribution is cheap: https://stackoverflow.com/a/19036349
    std::uniform_int_distribution<uint32_t> uniform(min, max);

    return uniform(layer);
}
