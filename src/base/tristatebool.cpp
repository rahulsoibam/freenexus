#include "tristatebool.h"

const TriStateBool TriStateBool::Undefined(-1);
const TriStateBool TriStateBool::False(0);
const TriStateBool TriStateBool::True(1);

TriStateBool::operator int() const
{
    return m_value;
}

TriStateBool::TriStateBool(int value)
{
    if (value < 0)
        m_value = -1;
    else if (value > 0)
        m_value = 1;
    else
        m_value = 0;
}

bool TriStateBool::operator==(const TriStateBool &other) const
{
    return (m_value == other.m_value);
}

bool TriStateBool::operator!=(const TriStateBool &other) const
{
    return !operator==(other);
}
