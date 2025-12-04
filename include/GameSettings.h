#pragma once
#include <stdbool.h>

#include <Datatypes/Units.h>

#ifdef __cplusplus
#include <Datatypes/Vector.hpp>
typedef Vector2u _vec2;
#else
typedef unsigned int _vec2[2];
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
	_vec2 Resolution;
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
	Unit EntityDebugLineSize;
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
#else
	extern const struct _sre_gs game_settings;
#endif
