#pragma once
#include <utils/math.h>
#include <ints.h>
#include <type_traits>

namespace sre
{
    template <typename T>
    struct flags
    {
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

        inline void operator =(T value) { m_data = value; }
        constexpr bool operator ==(const flags& other) const { return m_data == other.m_data; }
        constexpr bool operator !=(const flags& other) const { return m_data != other.m_data; }

        constexpr operator T() const { return get(); }

        template <typename... Args> constexpr flags(Args... args) : m_data(get_pack(args...)) {}
        template <typename... Args> static constexpr T get_pack(T first, Args... args) { return first | get_pack(args...); }
        static constexpr T get_pack(T first) { return first; }

        inline void println() const
        {
            
        }
    private:
        T m_data{};
    };

    using flags8 = flags<uint8_t>;
    using flags16 = flags<uint16_t>;
    using flags32 = flags<uint32_t>;
    using flags64 = flags<uint64_t>;
    using flagsptr = flags<size_t>;
}

#include <ostream>
#include <bitset>
#include <climits>

template <typename Char, typename Traits, typename T>
std::basic_ostream<Char, Traits>& operator <<(std::basic_ostream<Char, Traits>& os, const sre::flags<T>& flags)
{
    T data = flags.get();
    std::bitset<sizeof(data) * CHAR_BIT> bitset{data};

    auto oldflags = os.setf(std::ios::hex);
    os << "{ " << bitset << "(" << data << ") }";
    return os;
}