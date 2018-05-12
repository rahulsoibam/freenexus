#pragma once

namespace Dict
{
    // To be used with QMap, QHash and it's variants
    template <typename Dictionary, typename BinaryPredicate>
    void removeIf(Dictionary &&dict, BinaryPredicate p)
    {
        auto it = dict.begin();
        while (it != dict.end())
            it = (p(it.key(), it.value()) ? dict.erase(it) : it + 1);
    }
}
