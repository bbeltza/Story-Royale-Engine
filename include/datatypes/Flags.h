#pragma once
#include "utils.h"

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
    inline void ToggleOff(_Num flag) {m_data &= ~flag;}

    operator _Num() const { return Get(); }

    private:

    _Num m_data;
};

typedef Flags<uint8_t> Flags8;
typedef Flags<uint16_t> Flags16;
typedef Flags<uint32_t> Flags32;
typedef Flags<uint64_t> Flags64;