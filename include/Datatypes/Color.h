#include <standard.h>
#include "syslog.h"

struct Color3
{
	Color3() = default;
	Color3(uint8_t R, uint8_t G, uint8_t B): r(R), g(G), b(B) {}
	Color3(uint8_t RGB): r(RGB), g(RGB), b(RGB) {}
	Color3(const Color3& other): r(other.r), g(other.g), b(other.b) {}

	Color3(int32_t HEX): b(HEX & 0xFF), g((HEX >> 8) & 0xFF), r((HEX >> 16) & 0xFF) {}
	Color3(const char* HEX);

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

	static const Color3 WHITE;

	void Print() const;
	inline void PrintLn() const { Print(); putchar('\n'); }
};

struct Color4
{
	Color4(): r(0), g(0), b(0), a(255) {};
	Color4(uint8_t R, uint8_t G, uint8_t B, uint8_t A) : r(R), g(G), b(B), a(A) {}
	Color4(uint8_t RGB, uint8_t A=255) : r(RGB), g(RGB), b(RGB), a(A) {}
	Color4(const Color4& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}

	Color4(int32_t HEX): a(HEX & 0xFF), b((HEX >> 8) & 0xFF), g((HEX >> 16) & 0xFF), r((HEX >> 24) & 0xFF) {}
	Color4(const char* HEX);

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	static const Color4 WHITE;

	void Print() const;
	inline void PrintLn() const { Print(); putchar('\n'); }
};