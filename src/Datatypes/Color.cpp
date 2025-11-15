#include <assert.h>
#include <ctype.h>

#include "utils.h"

#include "Datatypes/Color.hpp"

#define c3getop(macro) { return Color3(macro(r), macro(g), macro(b)); }
#define c4getop(macro) { return Color4(macro(r), macro(g), macro(b), macro(a)); }

#define colgetadd(comp) ut_min(255, this->comp + other.comp)
#define colgetsub(comp) ut_max(0, this->comp - other.comp)
#define colgetmul(comp) (this->comp * other.comp) / 255

#define coladd(comp) this->comp = colgetadd(comp)
#define colsub(comp) this->comp = colgetsub(comp)
#define colmul(comp) this->comp = colgetmul(comp)

uint8_t xdigit(char digit)
{
	assert("Parsing digit contains non-hexadecimal digits" && isxdigit(digit));

	if (digit < 'A')
		return (digit - '0');
	else if (digit < 'a')
		return (10 + (digit - 'A'));
	else
		return (10 + (digit - 'a'));
}

#pragma region Color3
Color3::Color3(const char* HEX) // 12 34 56
{
	size_t len = strlen(HEX);
	assert("Too few characters to parse hexadecimal string on Color3" && !(len < 6));

	HEX += len - 6; // Just take the 6 last characters

	for (size_t j = 0; j < 3; j++)
	{
		uint8_t n = 0;
		for (size_t i = 0; i < 2; i++)
		{
			char digit = HEX[j * 2 + i];
			
			n |= xdigit(digit) << (4 * (1 - i));
		}
		reinterpret_cast<uint8_t*>(this)[j] = n;
	}
}

void Color3::display(logfunc_t _printer) const
{
	_printer("{ %d, %d, %d } ( %g%%, %g%%, %g%% )",
		r,
		g,
		b,

		r / 255.0f * 100,
		g / 255.0f * 100,
		b / 255.0f * 100
	);
}

void Color3::Add(const Color3& other) { coladd(r); coladd(g); coladd(b); }
void Color3::Sub(const Color3& other) { colsub(r); colsub(g); colsub(b); }
void Color3::Mul(const Color3& other) { colmul(r); colmul(g); colmul(b); }

Color3 Color3::getAdd(const Color3& other) const c3getop(colgetadd)
Color3 Color3::getSub(const Color3& other) const c3getop(colgetsub)
Color3 Color3::getMul(const Color3& other) const c3getop(colgetmul)

#pragma endregion
#pragma region Color4
Color4::Color4(const char* HEX)
{
	size_t len = strlen(HEX);
	assert("Too few characters to parse hexadecimal string on Color4" && !(len < 8));

	HEX += len - 8;

	for (size_t j = 0; j < 4; j++)
	{
		uint8_t n = 0;
		for (size_t i = 0; i < 2; i++)
		{
			char digit = HEX[j * 2 + i];

			n |= xdigit(digit) << (4 * (1 - i));
		}
		(&r)[j] = n;
	}
}

void Color4::display(logfunc_t _printer) const
{
	_printer("{ %d, %d, %d, %d } ( %g%%, %g%%, %g%%, %g%% )",
		r,
		g,
		b,
		a,
		
		r / 255.0f * 100,
		g / 255.0f * 100,
		b / 255.0f * 100,
		a / 255.0f * 100
	);
}

void Color4::Add(const Color4& other) { coladd(r); coladd(g); coladd(b); coladd(a); }
void Color4::Sub(const Color4& other) { colsub(r); colsub(g); colsub(b); colsub(a); }
void Color4::Mul(const Color4& other) { colmul(r); colmul(g); colmul(b); colmul(a); }

Color4 Color4::getAdd(const Color4& other) const c4getop(colgetadd)
Color4 Color4::getSub(const Color4& other) const c4getop(colgetsub)
Color4 Color4::getMul(const Color4& other) const c4getop(colgetmul)
#pragma endregion
#pragma region ColorDefinitions
const Color3 Color3::WHITE(255, 255, 255);
const Color3 Color3::BLACK(0, 0, 0);

const Color4 Color4::WHITE(255, 255, 255, 255);
const Color4 Color4::BLACK(0, 0, 0, 255);
const Color4 Color4::INVISIBLE(0, 0, 0, 0);
#pragma endregion