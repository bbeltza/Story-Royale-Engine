#pragma once
#include <SDL_pixels.h>
#include <standard>
#include "datatypes/common.hpp"
#include "utils/math.h"

namespace sre
{
	struct col3
	{
		SRE_IMPLEMENT_DATATYPE("{ %d, %d, %d } ( %g%%, %g%%, %g%% )",
			r,
			g,
			b,

			r / 255.0 * 100,
			g / 255.0 * 100,
			b / 255.0 * 100
		)

		using T = uint8_t;

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

		static const col3 WHITE;
		static const col3 BLACK;

		static const col3 RED;
		static const col3 GREEN;
		static const col3 BLUE;
	};

	struct col4: public col3
	{
		SRE_IMPLEMENT_DATATYPE("{ %d, %d, %d, %d } ( %g%%, %g%%, %g%%, %g%% )",
			r,
			g,
			b,
			a,

			r / 255.0 * 100,
			g / 255.0 * 100,
			b / 255.0 * 100,
			a / 255.0 * 100
		)

		constexpr col4() = default;
		constexpr col4(T r, T g, T b): col3(r, g, b) {}
		constexpr col4(T r, T g, T b, T alpha): col3(r, g, b), a(alpha) {}
		constexpr col4(T rgb): col3(rgb) {}
		constexpr col4(T rgb, T alpha): col3(rgb), a(alpha) {}
		constexpr col4(const col4& copy): col3(copy), a(copy.a) {}


		constexpr col4(const col3& copy): col3(copy) {}
		constexpr col4(const col3& copy, T alpha): col3(copy), a(alpha) {}

		T a{0xFF};

		inline void add(const col4& other) { col3::add(other); a = add_component(a, other.a); }
		inline void sub(const col4& other) { col3::sub(other); a = sub_component(a, other.a); }
		inline void mul(const col4& other) { col3::mul(other); a = mul_component(a, other.a); }
		inline void operator +=(const col4& other) { return add(other); }
		inline void operator -=(const col4& other) { return sub(other); }
		inline void operator *=(const col4& other) { return mul(other); }
		
		constexpr col4 getAdd(const col4& other) const { return { col3::getAdd(other), add_component(a, other.a) }; }
		constexpr col4 getSub(const col4& other) const { return { col3::getSub(other), sub_component(a, other.a) }; }
		constexpr col4 getMul(const col4& other) const { return { col3::getMul(other), mul_component(a, other.a) }; }
		constexpr col4 operator +(const col4& other) const { return getAdd(other); }
		constexpr col4 operator -(const col4& other) const { return getSub(other); }
		constexpr col4 operator *(const col4& other) const { return getMul(other); }

		constexpr col4 operator *(float) const { return col4(); }

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

		static const col4 WHITE;
		static const col4 BLACK;
		static const col4 INVISIBLE;

		static const col4 RED;
		static const col4 GREEN;
		static const col4 BLUE;
	};

	constexpr col3 fromHEX(sre::u32 hex) { return { static_cast<col3::T>((hex >> 16) & 0xFF), static_cast<col3::T>((hex >> 8) & 0xFF), static_cast<col3::T>((hex >> 0) & 0xFF) }; }
	constexpr col4 fromHEX(sre::u32 hex, sre::col4::T alpha) { return { sre::fromHEX(hex), alpha }; }
}