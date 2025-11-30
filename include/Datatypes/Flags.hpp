#pragma once
#include "utils/math.h"
#include "utils/logging.h"

template <typename _Num>
struct Flags
{
    static_assert(std::is_integral<_Num>::value, "Flags object must be of an integral type: Type must be a whole number");
    using Type = _Num;

    Flags(): m_data((_Num)0) {}
    Flags(_Num flags): m_data(flags) {}

    inline constexpr _Num Get() const { return m_data; }

    inline constexpr bool Has(_Num flag) const {return (m_data & flag) != 0;}

    inline void Toggle(_Num flag) {m_data ^= flag;}
    inline void ToggleOn(_Num flag) {m_data |= flag;}
    inline void ToggleOff(_Num flag) {m_data &= ~flag;}
    
    inline void Print() const { display(&ALOG); }
    inline void PrintLn() const { display(&NLOG); }

    operator _Num() const { return Get(); }

    private:

    _Num m_data;

    inline void display(logfunc_t _printer) const {
        const size_t bitsize = sizeof(m_data) * 8;

        char buff[bitsize + 1] = {0};
        for (size_t i = 0; i < bitsize; i++)
            buff[bitsize - i - 1] = 48 + ((m_data >> i) & 1);
        
        _printer("{ %s (0x%x) }", buff, m_data);
    }
};

typedef Flags<uint8_t> Flags8;
typedef Flags<uint16_t> Flags16;
typedef Flags<uint32_t> Flags32;
typedef Flags<uint64_t> Flags64;