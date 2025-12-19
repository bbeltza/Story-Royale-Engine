#pragma once
#include <stdbool.h>

#include <datatypes/units.h>

#ifdef __cplusplus
#include <datatypes/vector.hpp>
#define vec2u sre::vec2u
#else
typedef unsigned int vec2u[2];
#endif

enum _sre_gs_interpolation
{
	SRE_NEAREST,
	SRE_LINEAR,
	SRE_CUBIC,
	SRE_GAUSSIAN
};

struct _sre_gs_window
{
	vec2u Resolution;
	unsigned int TargetFPS;
	bool VSync;
	bool Resizable;
	bool Scaled;
};
struct _sre_gs_audio
{
	unsigned short Frequency;
	unsigned char Filter;
	bool Mono;
};
struct _sre_gs_debug
{
	sre_unit EntityDebugLineSize;
};


struct _sre_gs
{
	const char* Title;
	struct _sre_gs_window WindowOptions;
	struct _sre_gs_audio AudioOptions;
	struct _sre_gs_debug DebugOptions;
};

#ifdef __cplusplus
	namespace sre
	{
		extern "C" _sre_gs game_settings;
	}

	#undef vec2u
#else
	extern const struct _sre_gs game_settings;
#endif
