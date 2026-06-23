#ifndef SRE_COLOR_H
#define SRE_COLOR_H

#include <ints.h>
#ifdef __cplusplus
#include <SDL_pixels.h>
#include <utils/math.h>

namespace sre
{
	struct col3
	{
		using T = sre::u8;

		constexpr col3() = default;
		constexpr col3(T r, T g, T b): r(r), g(g), b(b) {}
		constexpr col3(T rgb): r(rgb), g(rgb), b(rgb) {}
		constexpr col3(const col3& copy): r(copy.r), g(copy.g), b(copy.b) {}

		T r{0};
		T g{0};
		T b{0};

		inline void add(const col3& other) { r = add_component(r, other.r); g = add_component(g, other.g); b = add_component(b, other.b); }
		inline void sub(const col3& other) { r = sub_component(r, other.r); g = sub_component(g, other.g); b = sub_component(b, other.b); }
		inline void mul(const col3& other) { r = mul_component(r, other.r); g = mul_component(g, other.g); b = mul_component(b, other.b); }

		constexpr col3 getAdd(const col3& other) const { return { add_component(r, other.r), add_component(g, other.g), add_component(b, other.b) }; }
		constexpr col3 getSub(const col3& other) const { return { sub_component(r, other.r), sub_component(g, other.g), sub_component(b, other.b) }; }
		constexpr col3 getMul(const col3& other) const { return { mul_component(r, other.r), mul_component(g, other.g), mul_component(b, other.b) }; }

		constexpr static T add_component(T first, T second) { return ut_min(first + second, 0xFF); }
		constexpr static T sub_component(T first, T second) { return ut_max(first - second, 0x00); }
		constexpr static T mul_component(T first, T second) { return (first * second) / 0xFF; }

		constexpr col3 operator +(const col3& other) const { return getAdd(other); }
		constexpr col3 operator -(const col3& other) const { return getSub(other); }
		constexpr col3 operator *(const col3& other) const { return getMul(other); }

		inline void operator +=(const col3& other) { return add(other); }
		inline void operator -=(const col3& other) { return sub(other); }
		inline void operator *=(const col3& other) { return mul(other); }

		static constexpr col3 fromNormalized(double r, double g, double b) { return col3{ static_cast<T>(r * 255), static_cast<T>(g * 255), static_cast<T>(b * 255) }; }
	};

	struct alignas(int) col4
	{
		using T = col3::T;

		constexpr col4(): color3() {}
		constexpr col4(T r, T g, T b): color3(r, g, b) {}
		constexpr col4(T r, T g, T b, T alpha): color3(r, g, b), a(alpha) {}
		constexpr col4(T rgb): color3(rgb) {}
		constexpr col4(T rgb, T alpha): color3(rgb), a(alpha) {}
		constexpr col4(const col4& copy): color3(copy), a(copy.a) {}

		constexpr col4(const col3& copy): color3(copy) {}
		constexpr col4(const col3& copy, T alpha): color3(copy), a(alpha) {}

		union
		{
			col3 color3;
			struct
			{
				T r;
				T g;
				T b;
			};
		};
		T a{0xFF};

		inline void add(const col4& other) { color3.add(other); a = col3::add_component(a, other.a); }
		inline void sub(const col4& other) { color3.sub(other); a = col3::sub_component(a, other.a); }
		inline void mul(const col4& other) { color3.mul(other); a = col3::mul_component(a, other.a); }
		inline void operator +=(const col4& other) { return add(other); }
		inline void operator -=(const col4& other) { return sub(other); }
		inline void operator *=(const col4& other) { return mul(other); }
		
		constexpr col4 getAdd(const col4& other) const { return { color3.getAdd(other), col3::add_component(a, other.a) }; }
		constexpr col4 getSub(const col4& other) const { return { color3.getSub(other), col3::sub_component(a, other.a) }; }
		constexpr col4 getMul(const col4& other) const { return { color3.getMul(other), col3::mul_component(a, other.a) }; }
		constexpr col4 operator +(const col4& other) const { return getAdd(other); }
		constexpr col4 operator -(const col4& other) const { return getSub(other); }
		constexpr col4 operator *(const col4& other) const { return getMul(other); }

		inline operator col3&() { return color3; }
		constexpr operator const col3&() const { return color3; }

		constexpr bool operator ==(const col4& other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }
		constexpr bool operator !=(const col4& other) const { return !operator ==(other); }

		using lerp_tag = T;
		constexpr col4 lerp(const col4& goal, float alpha) const
		{
			return col4{ static_cast<T>(ut_lerp(r, goal.r, alpha)),
				static_cast<T>(ut_lerp(g, goal.g, alpha)),
				static_cast<T>(ut_lerp(b, goal.b, alpha)),
				static_cast<T>(ut_lerp(a, goal.a, alpha)) };
		}

		constexpr SDL_Color toSDL() const { return { r, g, b, a }; }

		static constexpr col4 fromNormalized(double r, double g, double b, double a) { return col4{ col3::fromNormalized(r, g, b), static_cast<T>(a * 255) }; }
	};

	constexpr col3 fromHEX(sre::u32 hex) { return { static_cast<col3::T>((hex >> 16) & 0xFF), static_cast<col3::T>((hex >> 8) & 0xFF), static_cast<col3::T>((hex >> 0) & 0xFF) }; }
	constexpr col4 fromHEX(sre::u32 hex, sre::col4::T alpha) { return { sre::fromHEX(hex), alpha }; }

	constexpr col4 WHITE = { 0xFF, 0xFF, 0xFF };
	constexpr col4 BLACK = { 0x00, 0x00, 0x00 };
	constexpr col4 INVISIBLE = { 0x00, 0x00, 0x00, 0x00 };
	
	constexpr col4 RED = { 0xFF, 0x00, 0x00 };
	constexpr col4 GREEN = { 0x00, 0xFF, 0x00 };
	constexpr col4 BLUE = { 0x00, 0x00, 0xFF };

}

#include <ostream>

template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator <<(std::basic_ostream<Char, Traits>& os, const sre::col3& col)
{
    os << "{ " << static_cast<int>(col.r) << " ; " << static_cast<int>(col.g) << " ; " << static_cast<int>(col.b) << " }";
    return os;
}

template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator <<(std::basic_ostream<Char, Traits>& os, const sre::col4& col)
{
    os << "{ " << static_cast<int>(col.r) << " , " << static_cast<int>(col.g) << " , " << static_cast<int>(col.b) << " | " << static_cast<int>(col.a) << " }";
    return os;
}

#define sre_col3 sre::col3
#define sre_col4 sre::col4

#else // More color utilities that are available in C++'s color implementation will also be here

#include <stdint.h>
typedef uint8_t sre_colT;

typedef struct sre_col3
{
    sre_colT r;
    sre_colT g;
    sre_colT b;
} sre_col3;

typedef union sre_col4
{
    sre_col3 col3;     
    struct {
        sre_colT r;        
        sre_colT g;        
        sre_colT b;
        sre_colT a;
    };
} sre_col4;

/*
#define SRE_WHITE4 ((sre_col4){255, 255, 255, 255})
#define SRE_WHITE3 ((sre_col3){255, 255, 255})
*/

#endif


#endif