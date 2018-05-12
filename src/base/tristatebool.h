#ifndef TRISTATEBOOL_H
#define TRISTATEBOOL_H

class TriStateBool
{
public:
    static const TriStateBool Undefined;
    static const TriStateBool False;
    static const TriStateBool True;

    TriStateBool() = default;
    TriStateBool(const TriStateBool &other) = default;

    explicit TriStateBool(int value);
    explicit operator int() const;

    TriStateBool &operator=(const TriStateBool &other) = default;
    bool operator==(const TriStateBool &other) const;
    bool operator!=(const TriStateBool &other) const;

private:
    signed char m_value = -1; // Undefined by default
};

#endif // TRISTATEBOOL_H
