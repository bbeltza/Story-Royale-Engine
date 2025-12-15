#pragma once
#include <SDL_pixels.h>
#include <standard>
#include "utils/logging.h"

namespace sre
{
	struct col3
	{
		using T = uint8_t;

		constexpr col3() = default;
		constexpr col3(T r, T g, T b): r(r), g(g), b(b) {}
		constexpr col3(T rgb): r(rgb), g(rgb), b(rgb) {}
		constexpr col3(const col3& copy): r(copy.r), g(copy.g), b(copy.b) {}

		T r{0};
		T g{0};
		T b{0};
	};

	struct col4: public col3
	{
		constexpr col4() = default;
		constexpr col4(T r, T g, T b): col3(r, g, b) {}
		constexpr col4(T r, T g, T b, T alpha): col3(r, g, b), a(alpha) {}
		constexpr col4(T rgb): col3(rgb) {}
		constexpr col4(T rgb, T alpha): col3(rgb), a(alpha) {}
		constexpr col4(const col4& copy): col3(copy), a(copy.a) {}

		T a{255};
	};
}

struct Color4;

struct Color3
{
	constexpr Color3(): Color3(static_cast<uint8_t>(0)) {}
	constexpr Color3(uint8_t R, uint8_t G, uint8_t B): r(R), g(G), b(B) {}
	constexpr Color3(uint8_t RGB): r(RGB), g(RGB), b(RGB) {}
	constexpr Color3(const Color3& other): r(other.r), g(other.g), b(other.b) {}

	constexpr Color3(uint32_t HEX): r((HEX >> 16) & 0xFF), g((HEX >> 8) & 0xFF), b(HEX & 0xFF) {}
	Color3(const char* HEX);

	uint8_t r;
	uint8_t g;
	uint8_t b;
	
	void Print() const { display(&ALOG); };
	void PrintLn() const { display(&NLOG); };

	void Add(const Color3& other);
	void Sub(const Color3& other);
	void Mul(const Color3& other);

	Color3 getAdd(const Color3& other) const;
	Color3 getSub(const Color3& other) const;
	Color3 getMul(const Color3& other) const;

	inline Color3 operator+(const Color3& other) const { return getAdd(other); }
	inline Color3 operator-(const Color3& other) const { return getSub(other); }
	inline Color3 operator*(const Color3& other) const { return getMul(other); }

	inline void operator+=(const Color3& other) { Add(other); }
	inline void operator-=(const Color3& other) { Sub(other); }
	inline void operator*=(const Color3& other) { Mul(other); }

	static const Color3 WHITE, BLACK;

	private:

	void display(logfunc_t _printer) const;
};

struct Color4
{
	constexpr Color4(): r(0), g(0), b(0), a(255) {};
	constexpr Color4(uint8_t R, uint8_t G, uint8_t B, uint8_t A=255) : r(R), g(G), b(B), a(A) {}
	constexpr Color4(uint8_t RGB, uint8_t A=255) : r(RGB), g(RGB), b(RGB), a(A) {}
	constexpr Color4(const Color4& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}
	constexpr Color4(const Color3& col3): r(col3.r), g(col3.b), b(col3.b), a(255) {}

	constexpr Color4(int32_t HEX): Color4((uint32_t)HEX) {}
	constexpr Color4(uint32_t HEX): r((HEX >> 24) & 0xFF), g((HEX >> 16) & 0xFF), b((HEX >> 8) & 0xFF), a(HEX & 0xFF) {}
	Color4(const char* HEX);


	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;	

	void Print() const { display(&ALOG); };
	void PrintLn() const { display(&NLOG); };

	void Add(const Color4& other);
	void Sub(const Color4& other);
	void Mul(const Color4& other);

	Color4 getAdd(const Color4& other) const;
	Color4 getSub(const Color4& other) const;
	Color4 getMul(const Color4& other) const;

	inline Color4 operator+(const Color4& other) const { return getAdd(other); }
	inline Color4 operator-(const Color4& other) const { return getSub(other); }
	inline Color4 operator*(const Color4& other) const { return getMul(other); }

	inline void operator+=(const Color4& other) { Add(other); }
	inline void operator-=(const Color4& other) { Sub(other); }
	inline void operator*=(const Color4& other) { Mul(other); }

	inline operator Color3&() const;
	inline operator SDL_Color&() const {return *(SDL_Color*)this;}

	static const Color4 WHITE, BLACK, INVISIBLE;

	private:

	void display(logfunc_t _printer) const;
};

inline Color4::operator Color3&() const { return *(Color3*)this; }