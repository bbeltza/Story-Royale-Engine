#pragma once
#include "datatypes/common.hpp"

#include "utils/math.h"
#include "utils/logging.h"

namespace sre
{
    template <typename T>
    struct flags
    {
        SRE_IMPLEMENT_DATATYPE_PRINT

        static_assert(std::is_integral<T>::value, "Flags object must be of an integral type: Type must be a whole number");
        using type = T;

        constexpr flags() = default;
        constexpr flags(T _flags) : m_data(_flags) {}
        constexpr flags(const flags &copy) : m_data(copy.m_data) {}

        constexpr T get() const { return m_data; }
        constexpr bool has(T flag) const { return (m_data & flag) != 0; }

        inline void toggle(T flag) { m_data ^= flag; }
        inline void toggle_on(T flag) { m_data |= flag; }
        inline void toggle_off(T flag) { m_data &= ~flag; }

        template <typename... Args> inline void toggle(Args... args) { toggle(get_pack(args...)); }
        template <typename... Args> inline void toggle_on(Args... args) { toggle_on(get_pack(args...)); }
        template <typename... Args> inline void toggle_off(Args... args) { toggle_off(get_pack(args...)); }

        constexpr bool operator ==(const flags& other) { return m_data == other.m_data; }
        constexpr bool operator !=(const flags& other) { return m_data != other.m_data; }

        constexpr operator T() { return get(); }

        template <typename... Args> constexpr flags(Args... args) : m_data(get_pack(args...)) {}
        template <typename... Args> static constexpr T get_pack(T first, Args... args) { return first | get_pack(args...); }
        static constexpr T get_pack(T first) { return first; }

        inline void display(logfunc_t _printer) const
        {
            constexpr size_t bitsize = sizeof(m_data) * 8;
            char buff[sizeof(m_data)][9] = { 0 };

            for (size_t i = 0; i < bitsize; i++)
                buff[(sizeof(m_data) - 1) - i / 8][7 - i % 8] = '0' + ((m_data >> i) & 1);

            for (size_t i = 0; i < sizeof(m_data) - 1; i++)
                buff[i][8] = ' ';

            _printer("{ %s (0x%x) }", buff, m_data);
        }
    private:
        T m_data{};
    };

    using flags8 = flags<uint8_t>;
    using flags16 = flags<uint16_t>;
    using flags32 = flags<uint32_t>;
    using flags64 = flags<uint64_t>;
}