// This is all of the code present for d3d12, it will be thousands of lines long, I'm not thinking of expanding not separating it
// Thus the class definitions will be put here
#include <drivers.h>

#include <dxgi1_4.h>
#include <d3d12.h>

#include <SDL_syswm.h>

#include <unordered_set>

// All shader bytecode
static const BYTE VS_BYTES[] = {
     68,  88,  66,  67, 158,  65, 
    222, 209, 173, 179, 245, 210, 
    114, 222,  72, 121, 175, 172, 
     27,  23,   1,   0,   0,   0, 
     40,   6,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     24,   1,   0,   0, 220,   1, 
      0,   0,  80,   2,   0,   0, 
    172,   5,   0,   0,  82,  68, 
     69,  70, 220,   0,   0,   0, 
      1,   0,   0,   0,  72,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,   0,  17,   0,   0, 
    179,   0,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     67,  66, 117, 110, 105, 102, 
    111, 114, 109, 115,   0, 171, 
     60,   0,   0,   0,   2,   0, 
      0,   0,  96,   0,   0,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    144,   0,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
      2,   0,   0,   0, 156,   0, 
      0,   0,   0,   0,   0,   0, 
    172,   0,   0,   0,   8,   0, 
      0,   0,   8,   0,   0,   0, 
      2,   0,   0,   0, 156,   0, 
      0,   0,   0,   0,   0,   0, 
     86,  73,  69,  87,  80,  79, 
     82,  84,   0, 171, 171, 171, 
      1,   0,   3,   0,   1,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  67,  65, 
     77,  69,  82,  65,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  49,  48, 
     46,  49,   0, 171,  73,  83, 
     71,  78, 188,   0,   0,   0, 
      6,   0,   0,   0,   8,   0, 
      0,   0, 152,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,  15, 
      0,   0, 161,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  15,  15, 
      0,   0, 152,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   3,   3, 
      0,   0, 167,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,   3,   3, 
      0,   0, 167,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,   3,   3, 
      0,   0, 176,   0,   0,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,   1,   0,   0,   0, 
      5,   0,   0,   0,   1,   1, 
      0,   0,  80,  79,  83,  73, 
     84,  73,  79,  78,   0,  67, 
     79,  76,  79,  82,   0,  84, 
     69,  88,  67,  79,  79,  82, 
     68,   0,  83,  86,  95,  86, 
    101, 114, 116, 101, 120,  73, 
     68,   0,  79,  83,  71,  78, 
    108,   0,   0,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
     80,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     92,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   0,   0,   0, 
     98,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,  12,   0,   0, 
     83,  86,  95,  80, 111, 115, 
    105, 116, 105, 111, 110,   0, 
     67,  79,  76,  79,  82,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171,  83,  72, 
     68,  82,  84,   3,   0,   0, 
     64,   0,   1,   0, 213,   0, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   0,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   1,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   2,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   3,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   4,   0,   0,   0, 
     96,   0,   0,   4,  18,  16, 
     16,   0,   5,   0,   0,   0, 
      6,   0,   0,   0, 103,   0, 
      0,   4, 242,  32,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   2,   0, 
      0,   0, 104,   0,   0,   2, 
      4,   0,   0,   0, 105,   0, 
      0,   4,   0,   0,   0,   0, 
      4,   0,   0,   0,   4,   0, 
      0,   0,  54,   0,   0,   9, 
     50,  48,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   9, 
     50,  48,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   9, 
     50,  48,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   9, 
     50,  48,  32,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,  16,  16,   0, 
      5,   0,   0,   0,  54,   0, 
      0,   7,  50,   0,  16,   0, 
      0,   0,   0,   0,  70,  48, 
     32,   4,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    194,   0,  16,   0,   0,   0, 
      0,   0,   6,  20,  16,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   8,  50,   0,  16,   0, 
      1,   0,   0,   0, 230,  10, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,   9,  50,  32, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      3,   0,   0,   0,  70,  16, 
     16,   0,   4,   0,   0,   0, 
      0,   0,   0,   8,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   0,   0, 
      0,   0, 230, 138,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  14,   0,   0,   8, 
     18,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  64,  10, 128, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     54,   0,   0,   8, 162,   0, 
     16,   0,   2,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0, 128, 191, 
      0,   0,   0,   0,   0,   0, 
    128,  63,  15,   0,   0,   7, 
     34,   0,  16,   0,   3,   0, 
      0,   0, 134,   0,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
     14,   0,   0,   9,  66,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  64,  26, 128,  32, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,   7,  82,   0, 
     16,   0,   3,   0,   0,   0, 
      6,   2,  16,   0,   2,   0, 
      0,   0, 166,  27,  16,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   7, 130,   0,  16,   0, 
      3,   0,   0,   0, 150,   5, 
     16,   0,   0,   0,   0,   0, 
    230,  10,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  15,   0, 
      0,   7,  18,  32,  16,   0, 
      0,   0,   0,   0, 134,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   3,   0, 
      0,   0,  15,   0,   0,   7, 
     34,  32,  16,   0,   0,   0, 
      0,   0, 150,   5,  16,   0, 
      1,   0,   0,   0, 230,  10, 
     16,   0,   3,   0,   0,   0, 
     54,   0,   0,   8, 194,  32, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,  86,   0,   0,   5, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  30,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,  10, 242,  32,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0, 129, 128, 
    128,  59, 129, 128, 128,  59, 
    129, 128, 128,  59, 129, 128, 
    128,  59,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0,  24,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     11,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   5,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};

static const BYTE PS_BYTES[] = {
     68,  88,  66,  67,  85, 143, 
      4, 154,  26, 237, 247,  69, 
    188,  94,  49, 161,  59,  78, 
    191,   7,   1,   0,   0,   0, 
    140,   2,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    204,   0,   0,   0,  64,   1, 
      0,   0, 116,   1,   0,   0, 
     16,   2,   0,   0,  82,  68, 
     69,  70, 144,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    255, 255,   0,  17,   0,   0, 
    103,   0,   0,   0,  92,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     99,   0,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 115, 115, 116,  97, 
    116, 101,   0, 116, 101, 120, 
      0,  77, 105,  99, 114, 111, 
    115, 111, 102, 116,  32,  40, 
     82,  41,  32,  72,  76,  83, 
     76,  32,  83, 104,  97, 100, 
    101, 114,  32,  67, 111, 109, 
    112, 105, 108, 101, 114,  32, 
     49,  48,  46,  49,   0, 171, 
     73,  83,  71,  78, 108,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  92,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,  15,   0,   0,  98,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      3,   3,   0,   0,  83,  86, 
     95,  80, 111, 115, 105, 116, 
    105, 111, 110,   0,  67,  79, 
     76,  79,  82,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  97, 114, 
    103, 101, 116,   0, 171, 171, 
     83,  72,  68,  82, 148,   0, 
      0,   0,  64,   0,   0,   0, 
     37,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      0,   0,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,  98,  16,   0,   3, 
    242,  16,  16,   0,   1,   0, 
      0,   0,  98,  16,   0,   3, 
     50,  16,  16,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0, 104,   0,   0,   2, 
      1,   0,   0,   0,  69,   0, 
      0,   9, 242,   0,  16,   0, 
      0,   0,   0,   0,  70,  16, 
     16,   0,   2,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70,  30,  16,   0,   1,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};

namespace sre
{
    template <typename T, size_t N>
    constexpr inline int countof(T (&arr)[N]) { return N; }
    template <typename T>
    constexpr inline int countof(T &el) { return 1; }
}

#ifndef NDEBUG
    #define __BREAK if (IsDebuggerPresent()) DebugBreak();
#else
    #define __BREAK
#endif

// Since cleaning up would turn into a hazy mess, this does ONLY report for errors, it doesn't return execution! (At least for now)
#define SRE_DXCALL(x) hr = x;                                                                           \
do { if (FAILED(hr)) {                                                                                  \
    sre::log<sre::LOGCATEGORY_ERROR>("DIRECT3D12: '" #x "' failed, line %u: '%s' (0x%X)", __LINE__, DXHRTOSTRING(hr), hr); \
    __BREAK                                                                                             \
}} while(0)

#define SRE_DXRELEASE(iunknown) if (iunknown) (iunknown)->Release()
#define SRE_DXSAFERELEASE(iunknown) SRE_DXRELEASE(iunknown); iunknown = nullptr

// Convert HRESULT error codes into a constant string
const char* DXHRTOSTRING(HRESULT hr)
{
    #define _FMT_CASE(x) case x: return #x
    switch (hr)
    {
        _FMT_CASE(DXGI_ERROR_ACCESS_DENIED);
        _FMT_CASE(DXGI_ERROR_ACCESS_LOST);
        _FMT_CASE(DXGI_ERROR_ALREADY_EXISTS);
        _FMT_CASE(DXGI_ERROR_CANNOT_PROTECT_CONTENT);
        _FMT_CASE(DXGI_ERROR_DEVICE_HUNG);
        _FMT_CASE(DXGI_ERROR_DEVICE_REMOVED);
        _FMT_CASE(DXGI_ERROR_DEVICE_RESET);
        _FMT_CASE(DXGI_ERROR_DRIVER_INTERNAL_ERROR);
        _FMT_CASE(DXGI_ERROR_FRAME_STATISTICS_DISJOINT);
        _FMT_CASE(DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE);
        _FMT_CASE(DXGI_ERROR_INVALID_CALL);
        _FMT_CASE(DXGI_ERROR_MORE_DATA);
        _FMT_CASE(DXGI_ERROR_NAME_ALREADY_EXISTS);
        _FMT_CASE(DXGI_ERROR_NONEXCLUSIVE);
        _FMT_CASE(DXGI_ERROR_NOT_CURRENTLY_AVAILABLE);
        _FMT_CASE(DXGI_ERROR_NOT_FOUND);
        _FMT_CASE(DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE);
        _FMT_CASE(DXGI_ERROR_SDK_COMPONENT_MISSING);
        _FMT_CASE(DXGI_ERROR_SESSION_DISCONNECTED);
        _FMT_CASE(DXGI_ERROR_UNSUPPORTED);
        _FMT_CASE(DXGI_ERROR_WAIT_TIMEOUT);
        _FMT_CASE(DXGI_ERROR_WAS_STILL_DRAWING);
        
        _FMT_CASE(D3D12_ERROR_ADAPTER_NOT_FOUND);
        _FMT_CASE(D3D12_ERROR_DRIVER_VERSION_MISMATCH);
        _FMT_CASE(E_FAIL);
        _FMT_CASE(E_INVALIDARG);
        _FMT_CASE(E_OUTOFMEMORY);
        _FMT_CASE(E_NOTIMPL);
        _FMT_CASE(S_FALSE);
        _FMT_CASE(S_OK);

        _FMT_CASE(DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED);
        _FMT_CASE(DXGI_ERROR_REMOTE_OUTOFMEMORY);
        default: return NULL;
    }
    #undef _FMT_CASE
}

// This??
typedef IDXGIFactory4 ID3D12Factory;
typedef IDXGISwapChain3 ID3D12SwapChain;

#define __inst static_cast<sre_d3d12*>(video->userdata)
#define __tex static_cast<sre_d3d12texture*>(texture)

struct sre_d3d12;

struct sre_d3d12texture
{
    sre_d3d12texture(sre_d3d12* video, sre::vec2i size, SDL_PixelFormatEnum format);
    ~sre_d3d12texture();
    
    bool valid() { return m_resource != NULL; }
    
    bool update(const void* pixels, int pitch);
    bool size(int* pw, int* ph) { *pw = m_size.x; *ph = m_size.y; return true; }
    SDL_PixelFormatEnum format() { return m_format; }
    
    void bind(ID3D12GraphicsCommandList* dxcmd_list);
    void free(sre_d3d12* video);

    int getx() { return m_size.x; }
    int gety() { return m_size.y; }
private:
    SDL_PixelFormatEnum m_format = SDL_PIXELFORMAT_UNKNOWN;
    sre::vec2i m_size{};

    ID3D12Resource* m_resource{};
    UINT m_srvoffset{};
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpudesc{};
};

struct sre_d3d12
{
    ~sre_d3d12();

    // DirectX members
    IDXGIFactory4* dxfactory;
    IDXGISwapChain3* dxswapchain;
    
    ID3D12Device* dxdevice;
    ID3D12CommandAllocator* dxcmd_allocator;
    ID3D12CommandQueue* dxcmd_queue;
    ID3D12GraphicsCommandList* dxcmd_list;
    ID3D12DescriptorHeap* dxrtvheap;
    ID3D12DescriptorHeap* dxsrvheap;
    ID3D12RootSignature* dxrootsignature;
    ID3D12Heap* dxheap;

    ID3D12PipelineState* dxpipeline_blendstates[5];
    ID3D12Resource* dxrender_targets[2];

    int current_dxpipelinestate;
    
    // Some DirectX state members
    UINT current_frameindex;
    UINT rtvheap_increment;
    UINT srvheap_increment;

    // Data to handle SRV heap descriptors
    UINT srvlast;
    UINT srvcap = 300;
    std::unordered_set<UINT> srvfreelist;
    
    ID3D12Fence* dxfence;
    HANDLE hfence;

    struct VBO_INPUT
    {
        sre::rect2Dut rect;
        sre::col4 color;
        sre::vec2ut anchor;

        sre::vec2f tuv;
        sre::vec2f toffset;
    };
    struct alignas(256) CBUFFER
    {
        sre::vec2f viewport;
        sre::vec2f camera;
    };

    ID3D12Resource* cbuffer;
    ID3D12Resource* basicvbo_container; // Basic container for rect data, holds 64kb in total of rect data
    //D3D12_VERTEX_BUFFER_VIEW basic_vboview;
    D3D12_GPU_VIRTUAL_ADDRESS basic_vboaddr;
    sre::uptr basicvbo_index;

    CBUFFER* cbuffermap;
    VBO_INPUT* basicvbomap;

    alignas(sre_d3d12texture) char basictexture[sizeof(sre_d3d12texture)]; // Allocate it as buffer of bytes to initialize it later
private:
    D3D12_VIEWPORT m_viewport;
public:
    void present();
    bool viewport(sre::vec2i osize, sre::vec2ut vsize);
    bool vsync(int mode) { return false; }
    bool blend(sre_DrawBlending blend);

    bool clear(sre::col4 color, sre::vec2f camera);
    bool clip(sre::rect2Dut rect) { return false; }

    bool draw_fill(const sre_DDFill* data);
    bool draw_lines(const sre_DDLines* data) { return false; }
    bool draw_rect(const sre_DDRect* data);
    bool draw_rrect(const sre_DDRRect* data) { return false; }
    bool draw_texture(const sre_DDTexture* data);
    bool draw_rtexture(const sre_DDRTexture* data) { return false; }

    private:
        void _waitforgpu(); // Wait for the GPU to finish all commands

        void _drawvbo(const VBO_INPUT& input);
        void _setcameracbuf(bool usecam);

    #ifndef IMGUI_DISABLE
        public:
            bool imgui_init();
    #endif
};

static const sre_videodriverInterface sred3d12_interface{
    [](sre_videodriver* video) { delete __inst; },
    [](const sre_videodriver* video) { __inst->present(); },
    [](const sre_videodriver* video, int w, int h) { return __inst->viewport({w, h}, video->size); },
    [](const sre_videodriver* video, int vsync) { return __inst->vsync(vsync); },
    [](const sre_videodriver* video, sre_DrawBlending blend) { return __inst->blend(blend); },
    [](const sre_videodriver* video, void* texture, int w, int h, SDL_PixelFormatEnum format) { new(texture) sre_d3d12texture(__inst, {w, h}, format); return __tex->valid(); },
    [](const sre_videodriver* video, void* texture, const void* pixels, int pitch) { return __tex->update(pixels, pitch); },
    [](const sre_videodriver* video, void* texture) { __tex->free(__inst); __tex->~sre_d3d12texture(); },
    [](const sre_videodriver* video, void* texture, int* w, int* h) { return __tex->size(w, h); },
    [](const sre_videodriver* video, void* texture) { return __tex->format(); },

    [](const sre_videodriver* video, const sre::col4* color) { return __inst->clear(*color, video->center - video->camera); },
    [](const sre_videodriver* video, const sre::rect2Dut* rect) { return __inst->clip(*rect); },

    [](const sre_videodriver* video, const sre_DDFill* data) { return __inst->draw_fill(data); },
    [](const sre_videodriver* video, const sre_DDLines* data) { return __inst->draw_lines(data); },
    [](const sre_videodriver* video, const sre_DDRect* data) { return __inst->draw_rect(data); },
    [](const sre_videodriver* video, const sre_DDRRect* data) { return __inst->draw_rrect(data); },
    [](const sre_videodriver* video, const sre_DDTexture* data) { return __inst->draw_texture(data); },   
    [](const sre_videodriver* video, const sre_DDRTexture* data) { return __inst->draw_rtexture(data); }    
};

#ifndef IMGUI_DISABLE
    extern const sre_videodriverImGuiInterface sred3d12imgui_interface;
#endif

static bool setup_pipeline(sre_d3d12* inst);
static bool create_targets(sre_d3d12* inst);

extern "C" bool sred3d12_init(sre_videodriver* video, SDL_Window* window)
{
    // I can do two things on the window to make a Direct3D 12 device
    // 1- Create an SDL renderer (with the d3d12 hint or driver index), then call SDL_RenderGetD3D12Device and work with the returned device
    // 2- Screw everything and make the device all alone from scratch, I would need to then get the HWND of the window (from surely the syswm API) and setup everything...

        // The second one sounds fun as hell!

    SDL_SysWMinfo syswm;
    SDL_VERSION(&syswm.version);
    if (SDL_GetWindowWMInfo(window, &syswm) == SDL_FALSE)
        return false;
    SDL_assert(syswm.subsystem == SDL_SYSWM_WINDOWS); // WINRT too?

    auto inst = new sre_d3d12{};
    std::unique_ptr<sre_d3d12> _holder{inst};

    HRESULT hr;

    // Create factory and maybe get an adapter?
    SRE_DXCALL(CreateDXGIFactory1(IID_PPV_ARGS(&inst->dxfactory)));
    SRE_DXCALL(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&inst->dxdevice)));

    {
        D3D12_COMMAND_QUEUE_DESC cmdqueue_desc{};
        cmdqueue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        SRE_DXCALL(inst->dxdevice->CreateCommandQueue(&cmdqueue_desc, IID_PPV_ARGS(&inst->dxcmd_queue)));
        SRE_DXCALL(inst->dxdevice->CreateCommandAllocator(cmdqueue_desc.Type, IID_PPV_ARGS(&inst->dxcmd_allocator)));
        SRE_DXCALL(inst->dxdevice->CreateCommandList(0, cmdqueue_desc.Type, inst->dxcmd_allocator, NULL, IID_PPV_ARGS(&inst->dxcmd_list)));
        SRE_DXCALL(inst->dxcmd_list->Close());
    }

    {
        DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
        swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.BufferCount = 2;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        SRE_DXCALL(inst->dxfactory->CreateSwapChainForHwnd(
            inst->dxcmd_queue,
            syswm.info.win.window, // Window gotten from SDL_Window*
            &swapchain_desc,
            NULL,
            NULL,
            (IDXGISwapChain1**)&inst->dxswapchain
        ));
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC dheap_desc{};
        dheap_desc.NumDescriptors = 2;
        dheap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        SRE_DXCALL(inst->dxdevice->CreateDescriptorHeap(&dheap_desc, IID_PPV_ARGS(&inst->dxrtvheap)));
        dheap_desc.NumDescriptors = inst->srvcap; // I guess this will be resized and recreated... Ughh
        dheap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        dheap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        SRE_DXCALL(inst->dxdevice->CreateDescriptorHeap(&dheap_desc, IID_PPV_ARGS(&inst->dxsrvheap)));

        inst->rtvheap_increment = inst->dxdevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        inst->srvheap_increment = inst->dxdevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    
    if (!SUCCEEDED(hr))
        return false;

    if (!create_targets(inst))
        return false;

    if (!setup_pipeline(inst))
        return false;

    SRE_DXCALL(inst->dxdevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&inst->dxfence)));
    inst->hfence = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!inst->hfence)
        return false;

    UINT32 WHITE = UINT32_MAX;
    new(inst->basictexture) sre_d3d12texture(inst, 1, SDL_PIXELFORMAT_UNKNOWN);
    reinterpret_cast<sre_d3d12texture*>(inst->basictexture)->update(&WHITE, 4);

    #undef interface

    _holder.release();
    video->texture_size = sizeof(sre_d3d12texture);
    video->userdata = inst;
    video->interface = &sred3d12_interface;

    #ifndef IMGUI_DISABLE
        video->imgui = &sred3d12imgui_interface;
    #endif

    return true;
}

sre_d3d12::~sre_d3d12()
{
    this->_waitforgpu();

    reinterpret_cast<sre_d3d12texture*>(this->basictexture)->~sre_d3d12texture();

    this->dxcmd_allocator->Release();
    this->dxcmd_list->Release();
    this->dxcmd_queue->Release();
    this->dxdevice->Release();
    this->dxfactory->Release();
    this->dxswapchain->Release();
    this->dxrtvheap->Release();
    this->dxsrvheap->Release();
    this->dxrootsignature->Release();
    this->dxfence->Release();
    this->dxheap->Release();

    this->basicvbo_container->Release();
    for (int i = 0; i < sre::countof(dxrender_targets); i++)
        dxrender_targets[i]->Release();
    for (int i = 0; i < sre::countof(dxpipeline_blendstates); i++)
        SRE_DXRELEASE(dxpipeline_blendstates[i]);

    CloseHandle(this->hfence);
}

void sre_d3d12::_waitforgpu()
{
    HRESULT hr;
    SRE_DXCALL(dxcmd_queue->Signal(dxfence, 1));

    SRE_DXCALL(dxfence->SetEventOnCompletion(1, hfence));
    WaitForSingleObject(hfence, INFINITE);
    SRE_DXCALL(dxfence->Signal(0));
}

bool create_targets(sre_d3d12* inst)
{
    HRESULT hr;

    D3D12_CPU_DESCRIPTOR_HANDLE cpudesc = inst->dxrtvheap->GetCPUDescriptorHandleForHeapStart();
    for (int i = 0; i < sre::countof(inst->dxrender_targets); i++, cpudesc.ptr+=inst->rtvheap_increment)
    {
        SRE_DXCALL(inst->dxswapchain->GetBuffer(i, IID_PPV_ARGS(&inst->dxrender_targets[i])));
        inst->dxdevice->CreateRenderTargetView(inst->dxrender_targets[i], NULL, cpudesc);
    }

    return SUCCEEDED(hr);
}

extern void (*blend_functions[5])(D3D12_RENDER_TARGET_BLEND_DESC& desc);

bool setup_pipeline(sre_d3d12* inst)
{
    static const D3D12_INPUT_ELEMENT_DESC INPUTS[] = {
        /* transform      */ {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* color          */ {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* anchor         */ {"POSITION", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* texture's max uv value */ {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* texture's offset       */ {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
    };
    static const D3D12_DESCRIPTOR_RANGE DESCRANGES[] = {
        { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, 0 }
    };
    static const D3D12_ROOT_PARAMETER CBUFFERS[] = {
        /* global cb */ { D3D12_ROOT_PARAMETER_TYPE_CBV, {0, 0}, D3D12_SHADER_VISIBILITY_VERTEX },
        /* textures  */ { D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, {sre::countof(DESCRANGES), DESCRANGES}, D3D12_SHADER_VISIBILITY_PIXEL}
    };

    HRESULT hr;

    {   // Root Signature setup
        ID3DBlob* rsblob;
        ID3DBlob* rserr;
        D3D12_ROOT_SIGNATURE_DESC rsdesc{};
        rsdesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        rsdesc.NumParameters = sre::countof(CBUFFERS);
        rsdesc.pParameters = CBUFFERS;

        D3D12_STATIC_SAMPLER_DESC sampler_desc{};
        sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler_desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        sampler_desc.ShaderRegister = 0;
        sampler_desc.RegisterSpace = 0;
        rsdesc.NumStaticSamplers = 1;
        rsdesc.pStaticSamplers = &sampler_desc;

        SRE_DXCALL(D3D12SerializeRootSignature(&rsdesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rsblob, &rserr));
        if (rserr)
        {
            sre::log<sre::LOGCATEGORY_ERROR>("Failed serializing D3D12 root signature: %.*s", rserr->GetBufferSize(), rserr->GetBufferPointer());
            rserr->Release();
            return false;
        }

        SRE_DXCALL(inst->dxdevice->CreateRootSignature(0, rsblob->GetBufferPointer(), rsblob->GetBufferSize(), IID_PPV_ARGS(&inst->dxrootsignature)));
        rsblob->Release();
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pstate_desc{};
    pstate_desc.pRootSignature = inst->dxrootsignature;
    pstate_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pstate_desc.InputLayout.NumElements = sre::countof(INPUTS);
    pstate_desc.InputLayout.pInputElementDescs = INPUTS;
    pstate_desc.NumRenderTargets = 1;
    pstate_desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pstate_desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    pstate_desc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    pstate_desc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    pstate_desc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    pstate_desc.RasterizerState.DepthClipEnable = FALSE;
    pstate_desc.SampleMask = UINT_MAX;
    pstate_desc.SampleDesc.Count = 1;
    pstate_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    pstate_desc.VS.pShaderBytecode = VS_BYTES;
    pstate_desc.VS.BytecodeLength = sizeof(VS_BYTES);
    pstate_desc.PS.pShaderBytecode = PS_BYTES;
    pstate_desc.PS.BytecodeLength = sizeof(PS_BYTES);
    
    pstate_desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    pstate_desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    pstate_desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    pstate_desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    for (int i = 0; i < sre::countof(inst->dxpipeline_blendstates); i++)
    {
        blend_functions[i](pstate_desc.BlendState.RenderTarget[0]);
        SRE_DXCALL(inst->dxdevice->CreateGraphicsPipelineState(&pstate_desc, IID_PPV_ARGS(&inst->dxpipeline_blendstates[i])));
    }

    {
        D3D12_HEAP_DESC heap_desc{};
        heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
        heap_desc.Alignment = 0;
        heap_desc.SizeInBytes = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        heap_desc.Properties.Type = D3D12_HEAP_TYPE_CUSTOM;
        heap_desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
        heap_desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

        SRE_DXCALL(inst->dxdevice->CreateHeap(&heap_desc, IID_PPV_ARGS(&inst->dxheap)));

        D3D12_RESOURCE_DESC resource_desc{};
        resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resource_desc.Width = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        resource_desc.Height = 1;
        resource_desc.DepthOrArraySize = 1;
        resource_desc.MipLevels = 1;
        resource_desc.Format = DXGI_FORMAT_UNKNOWN;
        resource_desc.SampleDesc.Count = 1;
        resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        SRE_DXCALL(inst->dxdevice->CreatePlacedResource(
            inst->dxheap, 0,
            &resource_desc,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            NULL,
            IID_PPV_ARGS(&inst->basicvbo_container)
        ));
        
        inst->basic_vboaddr = inst->basicvbo_container->GetGPUVirtualAddress();
    }

    {   // Constant buffers
        D3D12_HEAP_PROPERTIES heap_properties{};
        heap_properties.Type = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC resource_desc{};
        resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resource_desc.Width = sizeof(sre_d3d12::CBUFFER)*2; // Two identical cbuffers where the first one doesn't contain any camera offset, while the second one does!
        resource_desc.Height = 1;
        resource_desc.DepthOrArraySize = 1;
        resource_desc.MipLevels = 1;
        resource_desc.Format = DXGI_FORMAT_UNKNOWN;
        resource_desc.SampleDesc.Count = 1;
        resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        SRE_DXCALL(inst->dxdevice->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &resource_desc,
            D3D12_RESOURCE_STATE_COMMON,
            NULL,
            IID_PPV_ARGS(&inst->cbuffer)
        ));
    }
    D3D12_RANGE range{0, 0};
    SRE_DXCALL(inst->basicvbo_container->Map(0, &range, reinterpret_cast<void**>(&inst->basicvbomap)));
    SRE_DXCALL(inst->cbuffer->Map(0, &range, reinterpret_cast<void**>(&inst->cbuffermap)));
    
    return SUCCEEDED(hr);
}

bool sre_d3d12::clear(sre::col4 color, sre::vec2f camera)
{
    HRESULT hr{};
    _waitforgpu();
    
    SRE_DXCALL(dxcmd_allocator->Reset());
    SRE_DXCALL(dxcmd_list->Reset(dxcmd_allocator, dxpipeline_blendstates[current_dxpipelinestate]));

    D3D12_RECT scr{0, 0, (LONG)m_viewport.Width, (LONG)m_viewport.Height};
    dxcmd_list->SetDescriptorHeaps(1, &dxsrvheap);
    dxcmd_list->SetGraphicsRootSignature(dxrootsignature);
    dxcmd_list->RSSetViewports(1, &m_viewport);
    dxcmd_list->RSSetScissorRects(1, &scr);
    dxcmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLEFAN);
    reinterpret_cast<sre_d3d12texture*>(basictexture)->bind(dxcmd_list);

    D3D12_RESOURCE_BARRIER rbtransition{};
    rbtransition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    rbtransition.Transition.pResource = dxrender_targets[current_frameindex];
    rbtransition.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    rbtransition.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    rbtransition.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    dxcmd_list->ResourceBarrier(1, &rbtransition);

    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtv = dxrtvheap->GetCPUDescriptorHandleForHeapStart();
        rtv.ptr += current_frameindex*rtvheap_increment;

        FLOAT fcolor[4] = {
            color.r/255.0f,
            color.g/255.0f,
            color.b/255.0f,
            1
        };

        dxcmd_list->OMSetRenderTargets(1, &rtv, FALSE, NULL);
        dxcmd_list->ClearRenderTargetView(rtv, fcolor, 0, NULL);
    }

    basicvbo_index = 0;
    cbuffermap[1].camera = camera;

    return SUCCEEDED(hr);
}

void sre_d3d12::present()
{
    HRESULT hr;
    
    {
        D3D12_RESOURCE_BARRIER transitions[] = {
            { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, {
                dxrender_targets[current_frameindex],
                D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT
            } }
        };
        dxcmd_list->ResourceBarrier(1, transitions);
        SRE_DXCALL(dxcmd_list->Close());
    }
    
    ID3D12CommandList* cmd_lists[] = { dxcmd_list };
    dxcmd_queue->ExecuteCommandLists(1, cmd_lists);
    SRE_DXCALL(dxswapchain->Present(0, 0));
    
    current_frameindex = dxswapchain->GetCurrentBackBufferIndex();
}

bool sre_d3d12::viewport(sre::vec2i osize, sre::vec2ut vsize)
{
    _waitforgpu();

    HRESULT hr;

    cbuffermap[0].viewport = vsize;
    cbuffermap[1].viewport = vsize;

    m_viewport.Width = static_cast<FLOAT>(osize.x);
    m_viewport.Height = static_cast<FLOAT>(osize.y);

    for (int i = 0; i < sre::countof(dxrender_targets); i++)
    {
        if (!dxrender_targets[i]) break;
        dxrender_targets[i]->Release();
        dxrender_targets[i] = NULL;
    }
    SRE_DXCALL(dxswapchain->ResizeBuffers(sre::countof(dxrender_targets), osize.x, osize.y, DXGI_FORMAT_UNKNOWN, 0));

    if (!create_targets(this))
        abort();

    current_frameindex = 0;

    return true;
}

bool sre_d3d12::blend(sre_DrawBlending blend)
{
    int state;
    switch (blend)
    {
        case SRE_BLEND_NONE: state = 0; break;
        case SRE_BLEND_BLEND: state = 1; break;
        case SRE_BLEND_ADD: state = 2; break;
        case SRE_BLEND_MOD: state = 3; break;
        case SRE_BLEND_MUL: state = 4; break;
        default: abort(); return false;
    }

    current_dxpipelinestate = state;
    dxcmd_list->SetPipelineState(dxpipeline_blendstates[state]);
    return true;
}

void sre_d3d12::_drawvbo(const VBO_INPUT& input)
{
    D3D12_VERTEX_BUFFER_VIEW vboview{};
    vboview.BufferLocation = basic_vboaddr + basicvbo_index * sizeof(VBO_INPUT);
    vboview.SizeInBytes = sizeof(VBO_INPUT);

    basicvbomap[basicvbo_index] = input;
    basicvbo_index++;

    dxcmd_list->IASetVertexBuffers(0, 1, &vboview);
    dxcmd_list->DrawInstanced(4, 1, 0, 0);
}

void sre_d3d12::_setcameracbuf(bool usecam)
{
    // if (lastcamstate != usecam)
    dxcmd_list->SetGraphicsRootConstantBufferView(0, cbuffer->GetGPUVirtualAddress() + sizeof(CBUFFER) * usecam);
}

bool sre_d3d12::draw_fill(const sre_DDFill* data)
{
    if (!data->color.a) return true;

    reinterpret_cast<sre_d3d12texture*>(basictexture)->bind(dxcmd_list);
    _setcameracbuf(false);
    _drawvbo({
        { 0, {D3D12_FLOAT32_MAX} }, // FLOAT32_MAX hell yeah!
        data->color,
        0
        });

    return true;
}

bool sre_d3d12::draw_rect(const sre_DDRect* data)
{
    if (!data->color.a) return true;

    reinterpret_cast<sre_d3d12texture*>(basictexture)->bind(dxcmd_list);

    _setcameracbuf(data->flags & SRE_DRAWFLAGS_USECAM);
    _drawvbo({
        data->rect,
        data->color,
        data->anchor
        });

    return true; // As DX command lists run directly on the GPU they return void so there'd be little to no purpose on returning a status code in these draw functions
}

bool sre_d3d12::draw_texture(const sre_DDTexture* data)
{
    auto* texture = static_cast<sre_d3d12texture*>(sre_get_texture(data->texture));
    texture->bind(dxcmd_list);

    sre::vec2f uv{1};
    sre::vec2f offs{0};
    if (data->region.size.x)
    {
        float size = static_cast<float>(texture->getx());
        uv.x = data->region.size.x / size;
        if (data->region.position.x)
            offs.x = data->region.position.x / size;
    }
    if (data->region.size.y)
    {
        float size = static_cast<float>(texture->gety());
        uv.y = data->region.size.y / size;
        if (data->region.position.y)
            offs.y = data->region.position.y / size;
    }

    if (data->flags & SRE_DRAWFLAGS_FLIPX)
    {
        offs.x += uv.x;
        uv.x = -uv.x;
    }
    if (data->flags & SRE_DRAWFLAGS_FLIPY)
    {
        offs.y += uv.y;
        uv.y = -uv.y;
    }

    _setcameracbuf(data->flags & SRE_DRAWFLAGS_USECAM);
    _drawvbo({
        data->rect,
        data->modulate,
        data->anchor,
        uv,
        offs
    });
    return true;
}

sre_d3d12texture::~sre_d3d12texture()
{
    SRE_DXRELEASE(m_resource);
}

sre_d3d12texture::sre_d3d12texture(sre_d3d12* video, sre::vec2i size, SDL_PixelFormatEnum format)
{
    HRESULT hr;

    D3D12_HEAP_PROPERTIES heap_properties{};
    heap_properties.Type = D3D12_HEAP_TYPE_GPU_UPLOAD;

    D3D12_RESOURCE_DESC texture_desc{};
    texture_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_desc.Width = size.x;
    texture_desc.Height = size.y;
    texture_desc.DepthOrArraySize = 1;
    texture_desc.MipLevels = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UINT; // Default format for now, might make a function to chose the best format for this but for now using the exact same Swapchain's format is enough
                                                        // Note that `format` is just a hint to chose a seemimgly better format for conversions, you must get the format of the texture returned in order to convert it and update its pixels
    texture_desc.SampleDesc.Count = 1;

    SRE_DXCALL(video->dxdevice->CreateCommittedResource(
        &heap_properties, D3D12_HEAP_FLAG_NONE,
        &texture_desc,
        D3D12_RESOURCE_STATE_COMMON,
        NULL, IID_PPV_ARGS(&m_resource)
    ));

    if (SUCCEEDED(hr))
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
        srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuhandle = video->dxsrvheap->GetCPUDescriptorHandleForHeapStart();
        cpuhandle.ptr += video->srvlast;
        video->dxdevice->CreateShaderResourceView(m_resource, &srv_desc, cpuhandle);


        m_srvoffset = video->srvlast;
        m_gpudesc = video->dxsrvheap->GetGPUDescriptorHandleForHeapStart();
        m_gpudesc.ptr += m_srvoffset;
        m_size = size;
        m_format = SDL_PIXELFORMAT_RGBA32;

        video->srvlast += video->srvheap_increment;
    }
}

void sre_d3d12texture::free(sre_d3d12* video)
{
    video->srvfreelist.insert(m_srvoffset);
}

bool sre_d3d12texture::update(const void* pixels, int pitch)
{
    HRESULT hr;

    SRE_DXCALL(m_resource->Map(0, NULL, NULL));
    SRE_DXCALL(m_resource->WriteToSubresource(0, NULL, pixels, pitch, 1));
    
    D3D12_RANGE range{};
    m_resource->Unmap(0, &range);
    return SUCCEEDED(hr);
}

void sre_d3d12texture::bind(ID3D12GraphicsCommandList* dxcmd_list)
{
    // Code to bind texture, I DON'T know yet how to do it...
    dxcmd_list->SetGraphicsRootDescriptorTable(1, m_gpudesc);
}

//

static void (*blend_functions[5])(D3D12_RENDER_TARGET_BLEND_DESC& desc) = {
    [](D3D12_RENDER_TARGET_BLEND_DESC& desc) { // BLEND_NONE
        desc.BlendEnable = FALSE;
        desc.LogicOpEnable = FALSE;
    },
    [](D3D12_RENDER_TARGET_BLEND_DESC& desc) { // BLEND_BLEND
        desc.BlendEnable = TRUE;
        desc.LogicOpEnable = FALSE;
        
        desc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        desc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        desc.BlendOp = D3D12_BLEND_OP_ADD;
    },
    [](D3D12_RENDER_TARGET_BLEND_DESC& desc) { // BLEND_MOD
        desc.BlendEnable = TRUE;

        desc.SrcBlend = D3D12_BLEND_DEST_COLOR;
        desc.DestBlend = D3D12_BLEND_ZERO;
        desc.BlendOp = D3D12_BLEND_OP_ADD;
    },
    [](D3D12_RENDER_TARGET_BLEND_DESC& desc) { // BLEND_ADD
        desc.BlendEnable = TRUE;

        desc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        desc.DestBlend = D3D12_BLEND_ONE;
        desc.BlendOp = D3D12_BLEND_OP_ADD;
    },
    [](D3D12_RENDER_TARGET_BLEND_DESC& desc) { // BLEND_MUL
        desc.BlendEnable = TRUE;

        desc.SrcBlend = D3D12_BLEND_DEST_COLOR;
        desc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        desc.BlendOp = D3D12_BLEND_OP_ADD;
    }
};

// ImGui

#ifndef IMGUI_DISABLE
    #include <backends/imgui_impl_dx12.h>

    static const sre_videodriverImGuiInterface sred3d12imgui_interface = {
        [](const sre_videodriver* video) { return __inst->imgui_init(); },
        []() { ImGui_ImplDX12_NewFrame(); },
        [](struct ImDrawData* ImDrawData, const sre_videodriver* video) { ImGui_ImplDX12_RenderDrawData(ImDrawData, __inst->dxcmd_list); }
    };

    static void imgui_srvalloc(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* ocpu, D3D12_GPU_DESCRIPTOR_HANDLE* ogpu)
    {
        auto inst = static_cast<sre_d3d12*>(info->UserData);
        
        D3D12_CPU_DESCRIPTOR_HANDLE cpu = inst->dxsrvheap->GetCPUDescriptorHandleForHeapStart();
        D3D12_GPU_DESCRIPTOR_HANDLE gpu = inst->dxsrvheap->GetGPUDescriptorHandleForHeapStart();

        cpu.ptr += inst->srvlast;
        gpu.ptr += inst->srvlast;

        *ocpu = cpu;
        *ogpu = gpu;

        inst->srvlast += inst->srvheap_increment;
    }

    static void imgui_srvfree(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu)
    {
        // Does nothing for now, should implement the free list allocator soon
    }

    bool sre_d3d12::imgui_init()
    {
        ImGui_ImplDX12_InitInfo info{};
        info.UserData = this;
        info.Device = dxdevice;
        info.CommandQueue = dxcmd_queue;
        info.NumFramesInFlight = sre::countof(dxrender_targets);
        info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

        info.SrvDescriptorHeap = dxsrvheap; // Using my srv descriptor heap, I was going to separate it but I don't care
        info.SrvDescriptorAllocFn = imgui_srvalloc;
        info.SrvDescriptorFreeFn = imgui_srvfree;

        if (ImGui_ImplDX12_Init(&info))
        {

            return true;
        }

        return false;
    }
#endif