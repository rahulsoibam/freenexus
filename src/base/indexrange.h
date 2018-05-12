#ifndef QBT_INDEXRANGE_H
#define QBT_INDEXRANGE_H

#include <QtGlobal>

// Interval is defined via [first;last]
template <typename Index>
class IndexInterval
{
public:
    using IndexType = Index;

    IndexInterval(IndexType first, IndexType last)
        : m_first {first}
        , m_last {last}
    {
        Q_ASSERT(first <= last);
    }

    IndexType first() const
    {
        return m_first;
    }

    IndexType last() const
    {
        return m_last;
    }

private:
    IndexType m_first;
    IndexType m_last;
};

template <typename T>
inline IndexInterval<T> makeInterval(T first, T last)
{
    return {first, last};
}

// range is defined via first index and size
template <typename Index, typename IndexDiff = Index>
class IndexRange
{
public:
    using IndexType = Index;
    using IndexDiffType = IndexDiff;

    constexpr IndexRange()
        : m_first {0}
        , m_size {0}
    {
    }

    constexpr IndexRange(IndexType first, IndexDiffType size)
        : m_first {first}
        , m_size {size}
    {
    }

    constexpr IndexRange(const IndexInterval<IndexType> &interval)
        : m_first {interval.first()}
        , m_size {interval.last() - interval.first() + 1}
    {
    }

    constexpr IndexType begin() const
    {
        return m_first;
    }

    constexpr IndexType end() const
    {
        return m_first + m_size;
    }

    constexpr IndexDiffType size() const
    {
        return m_size;
    }

    constexpr IndexType first() const
    {
        return m_first;
    }

    constexpr IndexType last() const
    {
        return m_first + m_size - 1;
    }

    constexpr bool isEmpty() const
    {
        return m_size == 0;
    }

private:
    IndexType m_first;
    IndexDiffType m_size;
};

#endif // QBT_INDEXRANGE_H
