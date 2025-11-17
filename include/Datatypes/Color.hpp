#pragma once
#include <SDL_pixels.h>
#include <standard>
#include "utils/logging.h"

struct Color4;

struct Color3
{
	Color3() = default;
	Color3(uint8_t R, uint8_t G, uint8_t B): r(R), g(G), b(B) {}
	Color3(uint8_t RGB): r(RGB), g(RGB), b(RGB) {}
	Color3(const Color3& other): r(other.r), g(other.g), b(other.b) {}

	Color3(uint32_t HEX): r((HEX >> 16) & 0xFF), g((HEX >> 8) & 0xFF), b(HEX & 0xFF) {}
	Color3(const char* HEX);

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

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

	inline operator Color4() const;

	static const Color3 WHITE, BLACK;

	private:

	void display(logfunc_t _printer) const;
};

struct Color4
{
	Color4(): r(0), g(0), b(0), a(255) {};
	Color4(uint8_t R, uint8_t G, uint8_t B, uint8_t A=255) : r(R), g(G), b(B), a(A) {}
	Color4(uint8_t RGB, uint8_t A=255) : r(RGB), g(RGB), b(RGB), a(A) {}
	Color4(const Color4& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}

	Color4(int32_t HEX): Color4((uint32_t)HEX) {}
	Color4(uint32_t HEX): r((HEX >> 24) & 0xFF), g((HEX >> 16) & 0xFF), b((HEX >> 8) & 0xFF), a(HEX & 0xFF) {}
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

inline Color3::operator Color4() const { return {r, g, b, 255}; }
inline Color4::operator Color3&() const { return *(Color3*)this; }