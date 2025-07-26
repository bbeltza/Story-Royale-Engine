#pragma once

template <typename _Num>
struct Flags
{
    using Type = _Num;

    Flags(): m_data((_Num)0) {}
    Flags(_Num flags): m_data(flags) {}

    inline constexpr _Num Get() const { return m_data; }

    inline constexpr bool Has(_Num flag) const {return (m_data & flag) != 0;}

    inline void Toggle(_Num flag) {m_data ^= flag;}
    inline void ToggleOn(_Num flag) {m_data |= flag;}
    inline void ToggleOff(_Num flag) {m_data &= flag;}

    private:

    _Num m_data;
};

typedef Flags<char> Flags8;
typedef Flags<short> Flags16;
typedef Flags<long> Flags32;
typedef Flags<long long> Flags64;