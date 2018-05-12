#ifndef UTILS_RANDOM_H
#define UTILS_RANDOM_H

#include <cstdint>

namespace Utils
{
    namespace Random
    {
        uint32_t rand(const uint32_t min = 0, const uint32_t max = UINT32_MAX);
    }
}

#endif // UTILS_FS_H
