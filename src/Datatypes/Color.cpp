#include <assert.h>
#include <ctype.h>

#include "Datatypes/Color.h"

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
		(&r)[j] = n;
	}
}

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

void Color3::Print() const
{
	syslog("{ %d, %d, %d } ( %g%%, %g%%, %g%% )",
		r,
		g,
		b,

		r / 255.0f * 100,
		g / 255.0f * 100,
		b / 255.0f * 100
	);
}

void Color4::Print() const
{
	syslog("{ %d, %d, %d, %d } ( %g%%, %g%%, %g%%, %g%% )",
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
