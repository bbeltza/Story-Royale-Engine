// This is all of the code present for d3d12, it will be thousands of lines long, I'm not thinking of expanding nor separating it
// Thus the class definitions will be put here
#include <Core/Render.h>

#include <dxgi1_4.h>
#include <d3d12.h>
#include <dxcommon/dxcommon.h>

#include <SDL_syswm.h>

#include <stack>
#include <cassert>

// All shader bytecode
static const BYTE D1_VS[] = {
     68,  88,  66,  67, 154,  96, 
    158, 231, 166, 251,  50, 105, 
    214, 166, 245,  21, 187,  11, 
     24, 145,   1,   0,   0,   0, 
    216,   6,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    104,   1,   0,   0,  76,   2, 
      0,   0, 192,   2,   0,   0, 
     92,   6,   0,   0,  82,  68, 
     69,  70,  44,   1,   0,   0, 
      2,   0,   0,   0, 112,   0, 
      0,   0,   2,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,   0,   1,   0,   0, 
      4,   1,   0,   0,  92,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    103,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  67,  66, 117, 110, 
    105, 102, 111, 114, 109, 115, 
      0,  36,  71, 108, 111,  98, 
     97, 108, 115,   0, 103,   0, 
      0,   0,   1,   0,   0,   0, 
    160,   0,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  92,   0, 
      0,   0,   1,   0,   0,   0, 
    208,   0,   0,   0,  64,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 184,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,   2,   0, 
      0,   0, 192,   0,   0,   0, 
      0,   0,   0,   0,  67,  65, 
     77,  69,  82,  65,   0, 171, 
      1,   0,   3,   0,   1,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 232,   0, 
      0,   0,   0,   0,   0,   0, 
     64,   0,   0,   0,   2,   0, 
      0,   0, 244,   0,   0,   0, 
      0,   0,   0,   0,  86,  73, 
     69,  87,  80,  79,  82,  84, 
      0, 171, 171, 171,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  49,  48,  46,  49,   0, 
     73,  83,  71,  78, 220,   0, 
      0,   0,   7,   0,   0,   0, 
      8,   0,   0,   0, 176,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,  15,   0,   0, 176,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   3,   0,   0, 185,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,  15,   0,   0, 191,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   1,   0,   0, 197,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
      3,   3,   0,   0, 197,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
      3,   3,   0,   0, 206,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   1,   0,   0,  80,  79, 
     83,  73,  84,  73,  79,  78, 
      0,  67,  79,  76,  79,  82, 
      0,  80,  83,  73,  90,  69, 
      0,  84,  69,  88,  67,  79, 
     79,  82,  68,   0,  83,  86, 
     95,  86, 101, 114, 116, 101, 
    120,  73,  68,   0, 171, 171, 
     79,  83,  71,  78, 108,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  92,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   0,   0,   0,  98,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      3,  12,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  67,  79, 
     76,  79,  82,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171,  83,  72,  68,  82, 
    148,   3,   0,   0,  64,   0, 
      1,   0, 229,   0,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,  95,   0,   0,   3, 
    242,  16,  16,   0,   0,   0, 
      0,   0,  95,   0,   0,   3, 
     50,  16,  16,   0,   1,   0, 
      0,   0,  95,   0,   0,   3, 
    242,  16,  16,   0,   2,   0, 
      0,   0,  95,   0,   0,   3, 
     18,  16,  16,   0,   3,   0, 
      0,   0,  95,   0,   0,   3, 
     50,  16,  16,   0,   4,   0, 
      0,   0,  95,   0,   0,   3, 
     50,  16,  16,   0,   5,   0, 
      0,   0,  96,   0,   0,   4, 
     18,  16,  16,   0,   6,   0, 
      0,   0,   6,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      2,   0,   0,   0, 104,   0, 
      0,   2,   4,   0,   0,   0, 
    105,   0,   0,   4,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   9,  50,  48,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   9,  50,  48,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   9,  50,  48,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   9,  50,  48,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,  16, 
     16,   0,   6,   0,   0,   0, 
     54,   0,   0,   7,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  48,  32,   4,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 194,   0,  16,   0, 
      0,   0,   0,   0,   6,  20, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   8,  50,   0, 
     16,   0,   1,   0,   0,   0, 
    230,  10,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
     50,  32,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,  70,  16, 
     16,   0,   4,   0,   0,   0, 
     70,  16,  16,   0,   5,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  26,  16,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     77,   0,   0,   7,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     18,   0,  16,   0,   3,   0, 
      0,   0,  10,  16,  16,   0, 
      3,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
     42,  16,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
     50,   0,  16,   0,   3,   0, 
      0,   0,   6,   0,  16,   0, 
      3,   0,   0,   0, 230,  26, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   8,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   2,   0,   0,   0, 
     58,  16,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  16,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    130,   0,  16,   0,   3,   0, 
      0,   0,  10,  16,  16,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
    134,   3,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  11, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0, 166, 138, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  70, 128, 
     32,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  66,   0, 
      0,   5,  50,   0,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   8, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     86,   5,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  50,   0,   0,  10, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   8, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   1,   0, 
      0,   0,  70,  30,  16,   0, 
      2,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    116,   0,   0,   0,  26,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,  10,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      5,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   7,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0
};

static const BYTE D2_VS[] = {
     68,  88,  66,  67,  69,  29, 
    199,  91, 158, 245, 176,  70, 
     89, 165, 115, 228,  32, 115, 
      4,  52,   1,   0,   0,   0, 
     52,   4,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    104,   1,   0,   0, 216,   1, 
      0,   0,  76,   2,   0,   0, 
    184,   3,   0,   0,  82,  68, 
     69,  70,  44,   1,   0,   0, 
      2,   0,   0,   0, 112,   0, 
      0,   0,   2,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,   0,   1,   0,   0, 
      4,   1,   0,   0,  92,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    103,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  67,  66, 117, 110, 
    105, 102, 111, 114, 109, 115, 
      0,  36,  71, 108, 111,  98, 
     97, 108, 115,   0, 103,   0, 
      0,   0,   1,   0,   0,   0, 
    160,   0,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  92,   0, 
      0,   0,   1,   0,   0,   0, 
    208,   0,   0,   0,  64,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 184,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,   2,   0, 
      0,   0, 192,   0,   0,   0, 
      0,   0,   0,   0,  67,  65, 
     77,  69,  82,  65,   0, 171, 
      1,   0,   3,   0,   1,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 232,   0, 
      0,   0,   0,   0,   0,   0, 
     64,   0,   0,   0,   2,   0, 
      0,   0, 244,   0,   0,   0, 
      0,   0,   0,   0,  86,  73, 
     69,  87,  80,  79,  82,  84, 
      0, 171, 171, 171,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  49,  48,  46,  49,   0, 
     73,  83,  71,  78, 104,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,  15,   0,   0,  86,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   3,   0,   0,  95,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      3,   3,   0,   0,  67,  79, 
     76,  79,  82,   0,  80,  79, 
     83,  73,  84,  73,  79,  78, 
      0,  84,  69,  88,  67,  79, 
     79,  82,  68,   0,  79,  83, 
     71,  78, 108,   0,   0,   0, 
      3,   0,   0,   0,   8,   0, 
      0,   0,  80,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  92,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  15,   0, 
      0,   0,  98,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   3,  12, 
      0,   0,  83,  86,  95,  80, 
     79,  83,  73,  84,  73,  79, 
     78,   0,  67,  79,  76,  79, 
     82,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0, 171, 
     83,  72,  68,  82, 100,   1, 
      0,   0,  64,   0,   1,   0, 
     89,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   0,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   1,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   2,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      2,   0,   0,   0, 104,   0, 
      0,   2,   2,   0,   0,   0, 
     56,   0,   0,   8,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   1,   0, 
      0,   0, 166, 138,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  66,   0,   0,   5, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   8,  50,   0,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
     70, 128,  32,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,   8, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     86,   5,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  50,   0,   0,  10, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   8, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   1,   0, 
      0,   0,  70,  30,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  50,  32,  16,   0, 
      2,   0,   0,   0,  70,  16, 
     16,   0,   2,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
      9,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   6,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};

static const BYTE C_PS[] = {
     68,  88,  66,  67,   8,  18, 
      4,  70, 226,  56, 206,  34, 
     66,  59, 102,  69, 166, 221, 
     96, 254,   1,   0,   0,   0, 
    140,   2,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    204,   0,   0,   0,  64,   1, 
      0,   0, 116,   1,   0,   0, 
     16,   2,   0,   0,  82,  68, 
     69,  70, 144,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    255, 255,   0,   1,   0,   0, 
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
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  67,  79, 
     76,  79,  82,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  65,  82, 
     71,  69,  84,   0, 171, 171, 
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

#define SRE_DXRELEASE(iunknown) if (iunknown) (iunknown)->Release()
#define SRE_DXSAFERELEASE(iunknown) SRE_DXRELEASE(iunknown); iunknown = nullptr

struct sred3d12_cache
{
    D3D12_VIEWPORT viewport;
    bool vsync;
};

struct alignas(256) sred3d12_cbuffer
{
    FLOAT viewport[16];
};

struct sred3d12_dbuff
{
    std::vector<ID3D12Resource*> dxfreequeue;
    ID3D12Resource* dxresource;
    D3D12_GPU_VIRTUAL_ADDRESS baseaddr;
    UINT index;
    UINT capacity;

    BYTE* mapped;
    
    D3D12_HEAP_TYPE heaptype = static_cast<D3D12_HEAP_TYPE>(0);
public:
    ~sred3d12_dbuff();

    inline void reset() { index = 0; _freequeued(); }
    bool init(ID3D12Device* dxdevice, UINT base_capacity);
    bool resize(ID3D12Device* dxdevice, UINT new_capacity);
    
    UINT append(ID3D12Device* dxdevice, const void* data, UINT size)
    {
        if (index + size > capacity)
        {
            resize(dxdevice, capacity * 2 + size);
        }   
        
        memcpy(mapped + index, data, size);
        index += size;
        return index - size;
    }
private:
    void _freequeued();
};

struct sred3d12_texture
{
    ID3D12Resource* dxstaging{};
    ID3D12Resource* dxresource{};
    UINT srvoffset{};
    UINT srvid{}; // Last srv capacity value in which gpu_descriptor was set during binding, gpu_descriptor is just a cache value
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_descriptor{};

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    UINT sizepertexel;
    bool needsupdate;
};

struct sred3d12_dlls
{
    HMODULE d3d12 = LoadLibrary("D3D12.dll");
    HMODULE dxgi = LoadLibrary("dxgi.dll");

    ~sred3d12_dlls()
    {
        FreeLibrary(d3d12);
        FreeLibrary(dxgi);
    }
};

struct sred3d12_inst: sre::RenderDriver
{
    using texture_type = sred3d12_texture;
    friend struct sreD3D12ImGuiData;

    sred3d12_inst(SDL_Window* window, int* outstatus);
    ~sred3d12_inst();

    static constexpr int FRAMEBUFFER_COUNT = 2;
private:
    sred3d12_dlls dlls;
    // DirectX members
    IDXGISwapChain3* dxswapchain;
    
    ID3D12Device* dxdevice;
    ID3D12CommandAllocator* dxcmd_allocators[FRAMEBUFFER_COUNT];
    ID3D12CommandQueue* dxcmd_queue;
    ID3D12GraphicsCommandList* dxcmd_list;
    ID3D12DescriptorHeap* dxrtvheap;

    ID3D12DescriptorHeap* dxsrvheapstaging; // Staging descriptor heap
    ID3D12DescriptorHeap* dxsrvheapsv; // Shader-visible
    ID3D12RootSignature* dxrootsignature;

    ID3D12PipelineState* dxpipeline_blendstates1[4];

    // 3 different pipeline state groups, yeah... For each primitive category...
    ID3D12PipelineState* dxpipeline_blendstates2_tri[4];
    ID3D12PipelineState* dxpipeline_blendstates2_lin[4];
    ID3D12PipelineState* dxpipeline_blendstates2_pts[4];

    ID3D12Resource* dxrender_targets[FRAMEBUFFER_COUNT]{};
    
    D3D12_HEAP_TYPE buffer_heaptype = D3D12_HEAP_TYPE_UPLOAD;

    // Some DirectX state members
    UINT current_blendstate;
    UINT current_drawtype;
    UINT current_frameindex = 0;

    UINT rtvheap_increment;
    UINT srvheap_increment;
    // Data to handle SRV heap descriptors
    UINT srvlast = 0;
    UINT srvcap = 0;
    std::stack<UINT> srvfreelist;

    ID3D12Fence* dxfence;
    HANDLE hfence;
    UINT64 fencevalue = 1;

    ID3D12Resource* cbuffer;
    sred3d12_cbuffer* cbuffermap;

    struct {
        sred3d12_dbuff d1{};
        sred3d12_dbuff d2c{};
        sred3d12_dbuff d2p{};
    } drawdatas[FRAMEBUFFER_COUNT], *current_drawdata{};

    alignas(sred3d12_texture) char basictexture[sizeof(sred3d12_texture)]; // Allocate it as buffer of bytes to initialize it later

    sred3d12_cache caches{};
public:
    void draw1(const sre::RenderInstance1* instances, size_t instance_count);
    void draw2(const sre::RenderInstance2* instance, size_t point_count);
            
    void begin(const float clear[4]);
    void end();
    
    void set_camerastate(sre::vec2ut camera);
    void set_vsync(bool enable) {
        caches.vsync = enable;
    }
    void set_texturestate(sred3d12_texture* texture);
    void set_viewportstate(int w, int h, sre::unit scale);
    void set_blendstate(sre::blendMode blending);
    void set_scissorstate(const sre::rect2Di* rectangle);
                
    bool texture_setup(sred3d12_texture* texture, sre::SDLpixelFormat format, int w, int h, sre::SDLpixelFormat* outformat);
    bool texture_update(sred3d12_texture* texture, const sre::rect2Di* region, const void* pixels, int pitch);
    void texture_destroy(sred3d12_texture* texture);
private:
    UINT _srvallocate();
    void _srvfree(UINT offs);
    
    void _waitforgpu(); // Wait for the GPU to finish all commands

    bool _pipelinesetup();
};

SRE_EXTERN_C_VAR sre::RenderDriverHelper<sred3d12_inst> sred3d12{"Direct3D 12 (joke/fun)"}; 

sred3d12_inst::sred3d12_inst(SDL_Window* window, int* outstatus)
{
    // I can do two things on the window to make a Direct3D 12 device
    // 1- Create an SDL renderer (with the d3d12 hint or driver index), then call SDL_RenderGetD3D12Device and work with the returned device
    // 2- Screw everything and make the device all alone from scratch, I would need to then get the HWND of the window (from surely the syswm API) and setup everything...

        // The second one sounds fun as hell!

    SDL_SysWMinfo syswm;
    SDL_VERSION(&syswm.version);
    if (SDL_GetWindowWMInfo(window, &syswm) == SDL_FALSE || syswm.subsystem != SDL_SYSWM_WINDOWS)
    {
        *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
        return;
    }

    HRESULT hr;

    // Load these functions dynamically, maybe look someday if I can try and find adapters
    {
        IDXGIFactory4* dxfactory = NULL;
        
        #ifndef NDEBUG
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_OPENGL)
        {
            // Give a welcoming warning, but still proceed
            sre::logmsg("DirectX 12: Enabling the debug layer on an OpenGL window (one that has the SDL_WINDOW_OPENGL flag) is broken, and would thus not allow any device creation. If you still want the debug layer, please remove or comment the SDL_WINDOW_OPENGL flag in the window creation code right at \"src/core/window.c\". Thanks!", SRE_LOG_WARN);
        }
        else
        {
            ID3D12Debug* dxdebug = NULL;
            SRE_DXGETADDR(D3D12GetDebugInterface, PFN_D3D12_GET_DEBUG_INTERFACE, dlls.d3d12);
            if (pD3D12GetDebugInterface && pD3D12GetDebugInterface(IID_PPV_ARGS(&dxdebug)) == S_OK)
            {
                dxdebug->EnableDebugLayer();
                dxdebug->Release();
            }
        }
        #endif
                        
        SRE_DXGETADDR(CreateDXGIFactory1, PFN_CREATE_DXGI_FACTORY1, dlls.dxgi);
        SRE_DXGETADDR(D3D12CreateDevice, PFN_D3D12_CREATE_DEVICE, dlls.d3d12);
        if (!pD3D12CreateDevice || !pCreateDXGIFactory1)
        {
            *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
            return;
        }

        SRE_DXCALLC(pCreateDXGIFactory1(IID_PPV_ARGS(&dxfactory))); // Enum adapters? Maybe?
        SRE_DXCALL(pD3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dxdevice)));
        if (FAILED(hr))
        {
            if (hr == DXGI_ERROR_UNSUPPORTED)
                *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
            return;
        }

        for (int i = 0; i < FRAMEBUFFER_COUNT; i++)
        {
            SRE_DXCALLC(dxdevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&dxcmd_allocators[i])));
        }
        SRE_DXCALLC(dxdevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, dxcmd_allocators[0], NULL, IID_PPV_ARGS(&dxcmd_list)));
        SRE_DXCALLC(dxcmd_list->Close());

        D3D12_COMMAND_QUEUE_DESC cmdqueue_desc{};
        cmdqueue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        SRE_DXCALLC(dxdevice->CreateCommandQueue(&cmdqueue_desc, IID_PPV_ARGS(&dxcmd_queue)));

        #define SR_DXGI_SWAPCHAIN_FLAGS (DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING)

        DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
        swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.BufferCount = 2;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.Flags = SR_DXGI_SWAPCHAIN_FLAGS;
        swapchain_desc.Scaling = DXGI_SCALING_NONE;

        SRE_DXCALLC(dxfactory->CreateSwapChainForHwnd(
            dxcmd_queue,
            syswm.info.win.window, // Window gotten from SDL_Window*
            &swapchain_desc,
            NULL,
            NULL,
            (IDXGISwapChain1**)&dxswapchain
        ));

        SRE_DXCALLC(dxfactory->MakeWindowAssociation(syswm.info.win.window, DXGI_MWA_NO_WINDOW_CHANGES));

        dxfactory->Release();
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS16 options16;
        SRE_DXCALL(dxdevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS16, &options16, sizeof(options16)));
        if (SUCCEEDED(hr))
            buffer_heaptype = D3D12_HEAP_TYPE_GPU_UPLOAD;
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC dheap_desc{};
        dheap_desc.NumDescriptors = 2;
        dheap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        SRE_DXCALLC(dxdevice->CreateDescriptorHeap(&dheap_desc, IID_PPV_ARGS(&dxrtvheap)));

        rtvheap_increment = dxdevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        srvheap_increment = dxdevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    if (!_pipelinesetup())
        return;

    SRE_DXCALLC(dxdevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&dxfence)));
    hfence = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!hfence)
        return;
    
    UINT32 WHITE = UINT32_MAX;
    sre::rect2Di whiteregion{ 0, 1 };
    if (!texture_setup(reinterpret_cast<texture_type*>(basictexture), SDL_PIXELFORMAT_UNKNOWN, 1, 1, NULL))
        return;
    if (!texture_update(reinterpret_cast<texture_type*>(basictexture), &whiteregion, &WHITE, 4))
        return;
    
    *outstatus = SRE_RENDERSTATUS_SUCCEEDED;
}

sred3d12_inst::~sred3d12_inst()
{
    _waitforgpu();

    texture_destroy(reinterpret_cast<texture_type*>(basictexture));

    for (int i = 0; i < sre::countof(dxcmd_allocators); i++)
        dxcmd_allocators[i]->Release();
    dxcmd_list->Release();
    dxcmd_queue->Release();
    dxdevice->Release();
    dxswapchain->Release();
    dxrtvheap->Release();
    dxrootsignature->Release();
    dxfence->Release();

    cbuffer->Release();

    if (dxsrvheapstaging)
    {
        assert(dxsrvheapsv != NULL);
        dxsrvheapsv->Release();
        dxsrvheapstaging->Release();
    }

    for (int i = 0; i < sre::countof(dxrender_targets); i++)
        dxrender_targets[i]->Release();
    for (int i = 0; i < sre::countof(dxpipeline_blendstates1); i++)
        dxpipeline_blendstates1[i]->Release();

    for (int i = 0; i < sre::countof(dxpipeline_blendstates2_tri); i++)
        dxpipeline_blendstates2_tri[i]->Release();
    for (int i = 0; i < sre::countof(dxpipeline_blendstates2_lin); i++)
        dxpipeline_blendstates2_lin[i]->Release();
    for (int i = 0; i < sre::countof(dxpipeline_blendstates2_pts); i++)
        dxpipeline_blendstates2_pts[i]->Release();

    CloseHandle(hfence);
}

void sred3d12_inst::_waitforgpu()
{  
    HRESULT hr;
    UINT64 oldvalue = fencevalue++;
    SRE_DXCALL(dxcmd_queue->Signal(dxfence, fencevalue));
    
    UINT64 value = dxfence->GetCompletedValue();
    if (value < oldvalue) {
        SRE_DXCALLC(dxfence->SetEventOnCompletion(oldvalue, hfence));
        WaitForSingleObject(hfence, INFINITE);
    }
}

static void (*blend_functions[4])(D3D12_RENDER_TARGET_BLEND_DESC& desc) = {
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

bool sred3d12_inst::_pipelinesetup()
{
    static const D3D12_INPUT_ELEMENT_DESC D1_INPUTS[] = {
        /* transform      */ {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* anchor         */ {"POSITION", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* color          */ {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* angle          */ {"PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* uv             */ {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* uv_offset      */ {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
    };
    static const D3D12_INPUT_ELEMENT_DESC D2_INPUTS[] = {
        /* color  */ {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        /* points.pos */ {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        /* points.uv  */ {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}        
    };

    static const D3D12_DESCRIPTOR_RANGE DESCRANGES[] = {
        { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, 0 }
    };
    D3D12_ROOT_PARAMETER ROOT_PARAMS[] = {
        /* global cb     */ D3D12_ROOT_PARAMETER{ D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_VERTEX },
        /* textures      */ D3D12_ROOT_PARAMETER{ D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_PIXEL},
        /* camera vector */ D3D12_ROOT_PARAMETER{ D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, D3D12_SHADER_VISIBILITY_VERTEX }
    };
    ROOT_PARAMS[0].Descriptor = { 0, 0 };
    ROOT_PARAMS[1].DescriptorTable = { 1, DESCRANGES };
    ROOT_PARAMS[2].Constants = {1, 0, 2};

    HRESULT hr;

    {   // Root Signature setup
        ID3DBlob* rsblob;
        ID3DBlob* rserr;
        D3D12_ROOT_SIGNATURE_DESC rsdesc{};
        rsdesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        rsdesc.NumParameters = sre::countof(ROOT_PARAMS);
        rsdesc.pParameters = ROOT_PARAMS;

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

        SRE_DXGETADDR(D3D12SerializeRootSignature, PFN_D3D12_SERIALIZE_ROOT_SIGNATURE, dlls.d3d12);
        assert(pD3D12SerializeRootSignature != NULL);

        SRE_DXCALL(pD3D12SerializeRootSignature(&rsdesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rsblob, &rserr));
        if (rserr)
        {
            sre::error(SRE_ERR_FAIL, rserr->GetBufferPointer());
            rserr->Release();
            return false;
        }

        SRE_DXCALL(dxdevice->CreateRootSignature(0, rsblob->GetBufferPointer(), rsblob->GetBufferSize(), IID_PPV_ARGS(&dxrootsignature)));
        rsblob->Release();
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pstate_desc{};
    pstate_desc.pRootSignature = dxrootsignature;
    pstate_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
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

    pstate_desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    pstate_desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    pstate_desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    pstate_desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

    pstate_desc.PS.pShaderBytecode = C_PS;
    pstate_desc.PS.BytecodeLength = sizeof(C_PS);

    // draw1 setup
    pstate_desc.VS.pShaderBytecode = D1_VS;
    pstate_desc.VS.BytecodeLength = sizeof(D1_VS);

    pstate_desc.InputLayout.pInputElementDescs = D1_INPUTS;
    pstate_desc.InputLayout.NumElements = sre::countof(D1_INPUTS);
    
    for (int i = 0; i < sre::countof(dxpipeline_blendstates1); i++)
    {
        blend_functions[i](pstate_desc.BlendState.RenderTarget[0]);
        SRE_DXCALL(dxdevice->CreateGraphicsPipelineState(&pstate_desc, IID_PPV_ARGS(&dxpipeline_blendstates1[i])));
    }

    // draw2 setup
    pstate_desc.VS.pShaderBytecode = D2_VS;
    pstate_desc.VS.BytecodeLength = sizeof(D2_VS);

    pstate_desc.InputLayout.pInputElementDescs = D2_INPUTS;
    pstate_desc.InputLayout.NumElements = sre::countof(D2_INPUTS);

    for (int i = 0; i < sre::countof(dxpipeline_blendstates2_tri); i++)
    {
        blend_functions[i](pstate_desc.BlendState.RenderTarget[0]);
        SRE_DXCALL(dxdevice->CreateGraphicsPipelineState(&pstate_desc, IID_PPV_ARGS(&dxpipeline_blendstates2_tri[i])));
    }

    pstate_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    for (int i = 0; i < sre::countof(dxpipeline_blendstates2_lin); i++)
    {
        blend_functions[i](pstate_desc.BlendState.RenderTarget[0]);
        SRE_DXCALL(dxdevice->CreateGraphicsPipelineState(&pstate_desc, IID_PPV_ARGS(&dxpipeline_blendstates2_lin[i])));
    }
    pstate_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    for (int i = 0; i < sre::countof(dxpipeline_blendstates2_pts); i++)
    {
        blend_functions[i](pstate_desc.BlendState.RenderTarget[0]);
        SRE_DXCALL(dxdevice->CreateGraphicsPipelineState(&pstate_desc, IID_PPV_ARGS(&dxpipeline_blendstates2_pts[i])));
    }
    //

    for (int i = 0; i < sre::countof(drawdatas); i++)
    {
        drawdatas[i].d1.heaptype = buffer_heaptype;
        drawdatas[i].d2c.heaptype = buffer_heaptype;
        drawdatas[i].d2p.heaptype = buffer_heaptype;

        if (!drawdatas[i].d1.init(dxdevice, sizeof(sre::RenderInstance1)*255))
            return false;
        if (!drawdatas[i].d2c.init(dxdevice, sizeof(sre::col4)*255))
            return false;
        if (!drawdatas[i].d2p.init(dxdevice, sizeof(sre::RenderPoint)*255))
            return false;
    }
    

    {   // Constant buffer
        D3D12_HEAP_PROPERTIES heap_properties{};
        heap_properties.Type = buffer_heaptype;

        D3D12_RESOURCE_DESC resource_desc{};
        resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resource_desc.Width = sizeof(sred3d12_cbuffer);
        resource_desc.Height = 1;
        resource_desc.DepthOrArraySize = 1;
        resource_desc.MipLevels = 1;
        resource_desc.Format = DXGI_FORMAT_UNKNOWN;
        resource_desc.SampleDesc.Count = 1;
        resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        SRE_DXCALL(dxdevice->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &resource_desc,
            D3D12_RESOURCE_STATE_COMMON,
            NULL,
            IID_PPV_ARGS(&cbuffer)
        ));
    }
    D3D12_RANGE range{0, 0};
    SRE_DXCALL(cbuffer->Map(0, &range, reinterpret_cast<void**>(&cbuffermap)));
    
    return SUCCEEDED(hr);
}

bool sred3d12_dbuff::init(ID3D12Device* dxdevice, UINT base_capacity)
{
    assert(heaptype != 0);

    HRESULT hr;
    D3D12_HEAP_PROPERTIES heap_properties{};
    heap_properties.Type = heaptype;

    D3D12_RESOURCE_DESC resource_desc{};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = base_capacity;
    resource_desc.Height = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.MipLevels = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    SRE_DXCALLF(dxdevice->CreateCommittedResource(
        &heap_properties, D3D12_HEAP_FLAG_NONE,
        //dxheap, 0,
        &resource_desc,
        D3D12_RESOURCE_STATE_COMMON,
        NULL,
        IID_PPV_ARGS(&dxresource)
    ));
    
    this->baseaddr = dxresource->GetGPUVirtualAddress();
    this->capacity = base_capacity;
    this->index = 0;

    D3D12_RANGE range{0, 0};
    SRE_DXCALLF(dxresource->Map(0, &range, reinterpret_cast<void**>(&mapped)));
    return true;
}

bool sred3d12_dbuff::resize(ID3D12Device* dxdevice, UINT capacity)
{
    dxresource->Unmap(0, NULL);
    dxfreequeue.push_back(dxresource);
    return init(dxdevice, capacity);
}

void sred3d12_dbuff::_freequeued()
{
    for (auto res : dxfreequeue)
        res->Release();

    dxfreequeue.clear();
}

sred3d12_dbuff::~sred3d12_dbuff()
{
    if (!dxresource)
    {
        sre::log(SRE_LOG_WARN "[DirectX]: You got a resource buffer that hasn't been initialized yet");
        return;
    }

    dxresource->Unmap(0, NULL);
    dxresource->Release();
    _freequeued();
}

void sred3d12_inst::begin(const float color[4])
{
    HRESULT hr{};
    current_frameindex = dxswapchain->GetCurrentBackBufferIndex();

    ID3D12CommandAllocator* cmdallocator_acquired = dxcmd_allocators[current_frameindex];

    SRE_DXCALL(cmdallocator_acquired->Reset());
    SRE_DXCALL(dxcmd_list->Reset(cmdallocator_acquired, NULL));

    D3D12_RECT scr{0, 0, (LONG)caches.viewport.Width, (LONG)caches.viewport.Height};
    dxcmd_list->SetDescriptorHeaps(1, &dxsrvheapsv);
    dxcmd_list->SetGraphicsRootSignature(dxrootsignature);
    dxcmd_list->SetGraphicsRootConstantBufferView(0, cbuffer->GetGPUVirtualAddress());
    dxcmd_list->RSSetViewports(1, &caches.viewport);
    dxcmd_list->RSSetScissorRects(1, &scr);

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

        dxcmd_list->OMSetRenderTargets(1, &rtv, FALSE, NULL);
        dxcmd_list->ClearRenderTargetView(rtv, color, 0, NULL);
    }

    current_drawdata = &drawdatas[current_frameindex];
    current_drawdata->d1.reset();
    current_drawdata->d2c.reset();
    current_drawdata->d2p.reset();

    current_drawtype = 0;
}

void sred3d12_inst::end()
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
    SRE_DXCALL(dxswapchain->Present(caches.vsync, !caches.vsync ? DXGI_PRESENT_ALLOW_TEARING : 0));
    if (hr == DXGI_ERROR_DEVICE_REMOVED)
    {
        HRESULT reason = dxdevice->GetDeviceRemovedReason();
        sre::critical(SRE_ERR_DIRECTX_HR, "The device has been removed. Reason: ", DXHRTOSTRING(reason), reason);
        abort();
        return;
    }
    
    _waitforgpu();
}

void sred3d12_inst::set_viewportstate(int w, int h, sre::unit scale)
{
    _waitforgpu();

    HRESULT hr;

    FLOAT fw = static_cast<FLOAT>(w), fh = static_cast<FLOAT>(h);

    caches.viewport.Width = fw;
    caches.viewport.Height = fh;

    FLOAT mat[16] = {
        2.0f/fw, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f/fh, 0.0f, 0.0f,
        0.0f, 0.0f, scale, 0.0,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
    memcpy(cbuffermap->viewport, mat, sizeof(mat));

    for (int i = 0; i < sre::countof(dxrender_targets); i++)
    {
        if (!dxrender_targets[i]) break;
        dxrender_targets[i]->Release();
        dxrender_targets[i] = NULL;
    }
    SRE_DXCALL(dxswapchain->ResizeBuffers(sre::countof(dxrender_targets), w, h, DXGI_FORMAT_UNKNOWN, SR_DXGI_SWAPCHAIN_FLAGS));

    D3D12_CPU_DESCRIPTOR_HANDLE cpudesc = dxrtvheap->GetCPUDescriptorHandleForHeapStart();
    for (int i = 0; i < sre::countof(dxrender_targets); i++, cpudesc.ptr+=rtvheap_increment)
    {
        SRE_DXCALL(dxswapchain->GetBuffer(i, IID_PPV_ARGS(&dxrender_targets[i])));
        dxdevice->CreateRenderTargetView(dxrender_targets[i], NULL, cpudesc);
    }
}

void sred3d12_inst::set_blendstate(sre::blendMode mode)
{
    current_blendstate = mode;
    
    ID3D12PipelineState** ppstatearr;
    switch (current_drawtype)
    {
        case 1: ppstatearr = dxpipeline_blendstates1; break;
        default: return;
    }

    dxcmd_list->SetPipelineState(ppstatearr[mode]);
}

void sred3d12_inst::set_camerastate(sre::vec2ut camera)
{
    dxcmd_list->SetGraphicsRoot32BitConstants(2, 2, &camera, 0);
}

void sred3d12_inst::set_scissorstate(const sre::rect2Di* rectangle)
{
    D3D12_RECT scissors = {
        rectangle->position.x,
        rectangle->position.y,
        rectangle->position.x + rectangle->size.x,
        rectangle->position.y + rectangle->size.y,
    };
    dxcmd_list->RSSetScissorRects(1, &scissors);
}

void sred3d12_inst::set_texturestate(texture_type* texture)
{
    texture = texture ? texture : reinterpret_cast<sred3d12_texture*>(basictexture);

    if (texture->needsupdate)
    {
        D3D12_RESOURCE_BARRIER transition{};
        transition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        transition.Transition.pResource = texture->dxresource;
        transition.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        transition.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

        D3D12_TEXTURE_COPY_LOCATION dstloc{};
        dstloc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstloc.pResource = texture->dxresource;

        D3D12_TEXTURE_COPY_LOCATION srcloc{};
        srcloc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        srcloc.pResource = texture->dxstaging;
        srcloc.PlacedFootprint = texture->footprint;

        dxcmd_list->ResourceBarrier(1, &transition);
        dxcmd_list->CopyTextureRegion(&dstloc, 0, 0, 0, &srcloc, NULL);
        transition.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        transition.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        dxcmd_list->ResourceBarrier(1, &transition);
        texture->needsupdate = false;
    }

    if (texture->srvid != srvcap)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE gpudesc = dxsrvheapsv->GetGPUDescriptorHandleForHeapStart();
        gpudesc.ptr += texture->srvoffset;
        texture->gpu_descriptor = gpudesc;
        texture->srvid = srvcap;
    }
    dxcmd_list->SetGraphicsRootDescriptorTable(1, texture->gpu_descriptor);
}

void sred3d12_inst::draw1(const sre::RenderInstance1* instances, size_t instance_count)
{
    if (1)
    {
        dxcmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        dxcmd_list->SetPipelineState(dxpipeline_blendstates1[current_blendstate]);
        current_drawtype = 1;
    }

    UINT UINT_instcount = static_cast<UINT>(instance_count);
    UINT index = current_drawdata->d1.append(dxdevice, instances, sizeof(sre::RenderInstance1)*UINT_instcount);

    D3D12_VERTEX_BUFFER_VIEW vboview{};
    vboview.BufferLocation = current_drawdata->d1.baseaddr + index;
    vboview.SizeInBytes = sizeof(sre::RenderInstance1)*UINT_instcount;
    vboview.StrideInBytes = sizeof(sre::RenderInstance1);
    dxcmd_list->IASetVertexBuffers(0, 1, &vboview);
    dxcmd_list->DrawInstanced(4, static_cast<UINT>(instance_count), 0, 0);
}

void sred3d12_inst::draw2(const sre::RenderInstance2* instance, size_t point_count)
{
    D3D12_PRIMITIVE_TOPOLOGY topology;
    switch (instance->mode)
    {
        case SRE_PRIMITIVE_TRIANGLES: topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
        case SRE_PRIMITIVE_TRIANGLESTRIP: topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
        case SRE_PRIMITIVE_LINEPERLINE: topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
        case SRE_PRIMITIVE_LINESTRIP: topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
        case SRE_PRIMITIVE_POINTS: topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
        default: abort();
    }

    ID3D12PipelineState* const* pipelinestate;
    switch (instance->mode)
    {
        case SRE_PRIMITIVE_TRIANGLES:
        case SRE_PRIMITIVE_TRIANGLESTRIP:
            current_drawtype = 2;
            pipelinestate = dxpipeline_blendstates2_tri;
            break;
        case SRE_PRIMITIVE_LINEPERLINE:
        case SRE_PRIMITIVE_LINESTRIP:
            current_drawtype = 3;
            pipelinestate = dxpipeline_blendstates2_lin;
            break;
        case SRE_PRIMITIVE_POINTS:
            current_drawtype = 4;
            pipelinestate = dxpipeline_blendstates2_pts;
            break;
        default: abort();
    }
        
    dxcmd_list->SetPipelineState(pipelinestate[current_blendstate]);
    dxcmd_list->IASetPrimitiveTopology(topology);

    UINT UINT_ptcount = static_cast<UINT>(point_count);
    UINT indexc = current_drawdata->d2c.append(dxdevice, &instance->color, sizeof(sre::col4));
    UINT indexp = current_drawdata->d2p.append(dxdevice, instance->points, sizeof(sre::RenderPoint)*UINT_ptcount);

    D3D12_VERTEX_BUFFER_VIEW vboviews[2]{};
    vboviews[0].BufferLocation = current_drawdata->d2c.baseaddr + indexc;
    vboviews[0].SizeInBytes = sizeof(sre::col4);
    vboviews[0].StrideInBytes = sizeof(sre::col4);
    vboviews[1].BufferLocation = current_drawdata->d2p.baseaddr + indexp;
    vboviews[1].SizeInBytes = sizeof(sre::RenderPoint)*UINT_ptcount;
    vboviews[1].StrideInBytes = sizeof(sre::RenderPoint);
    dxcmd_list->IASetVertexBuffers(0, 2, vboviews);
    dxcmd_list->DrawInstanced(UINT_ptcount, 1, 0, 0);
}


UINT sred3d12_inst::_srvallocate()
{
    HRESULT hr;
    UINT offset;
    if (srvfreelist.empty())
    {
        offset = srvlast;
        srvlast += srvheap_increment;

        if (srvlast / srvheap_increment > srvcap)
        {
            UINT old_cap = srvcap;
            srvcap += 16;

            ID3D12DescriptorHeap* old_stagingdheap = dxsrvheapstaging;
            ID3D12DescriptorHeap* old_svdheap = dxsrvheapsv;

            D3D12_DESCRIPTOR_HEAP_DESC dheap_desc{};
            dheap_desc.NumDescriptors = srvcap;
            dheap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            SRE_DXCALL(dxdevice->CreateDescriptorHeap(&dheap_desc, IID_PPV_ARGS(&dxsrvheapstaging)));

            dheap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            SRE_DXCALL(dxdevice->CreateDescriptorHeap(&dheap_desc, IID_PPV_ARGS(&dxsrvheapsv)));

            if (old_cap)
            {
                assert(old_stagingdheap != NULL);
                assert(old_svdheap != NULL);

                dxdevice->CopyDescriptorsSimple(old_cap,
                                                dxsrvheapstaging->GetCPUDescriptorHandleForHeapStart(),
                                                old_stagingdheap->GetCPUDescriptorHandleForHeapStart(),
                                                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
                                            );
                old_stagingdheap->Release();
                old_svdheap->Release();

                dxdevice->CopyDescriptorsSimple(old_cap,
                                                dxsrvheapsv->GetCPUDescriptorHandleForHeapStart(),
                                                dxsrvheapstaging->GetCPUDescriptorHandleForHeapStart(),
                                                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
                                            );
            }
        }
    }
    else
    {
        assert(dxsrvheapstaging != NULL);
        assert(dxsrvheapsv != NULL);
        
        offset = srvfreelist.top();
        srvfreelist.pop();
    }

    return offset;
}

void sred3d12_inst::_srvfree(UINT offs)
{
    srvfreelist.push(offs);
}

void sred3d12_inst::texture_destroy(texture_type* texture)
{
    if (texture->dxresource)
    {
        // If the GPU hasn't finished every single command, wait for them
        // Any of these resources may still be in use in the last frames' commands that surely haven't been finished yet
        if (dxfence->GetCompletedValue() < fencevalue)
        {
            dxfence->SetEventOnCompletion(fencevalue, hfence);
            WaitForSingleObject(hfence, INFINITE);
        }

        assert(texture->dxstaging != NULL);
        texture->dxresource->Release();
        texture->dxstaging->Release();
    }

    _srvfree(texture->srvoffset);
}

bool sred3d12_inst::texture_setup(texture_type* texture, sre::SDLpixelFormat formathint, int w, int h, sre::SDLpixelFormat* outformat)
{
    HRESULT hr;

    D3D12_HEAP_PROPERTIES heap_properties{};
    heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC texture_desc{};
    texture_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_desc.Width = w;
    texture_desc.Height = h;
    texture_desc.DepthOrArraySize = 1;
    texture_desc.MipLevels = 1;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UINT; // Default format for now, might make a function to chose the best format for this but for now using the exact same Swapchain's format is enough
                                                        // Note that `format` is just a hint to chose a seemimgly better format for conversions, you must get the format of the texture returned in order to convert it and update its pixels

    SRE_DXCALLF(dxdevice->CreateCommittedResource(
        &heap_properties, D3D12_HEAP_FLAG_NONE,
        &texture_desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        NULL, IID_PPV_ARGS(&texture->dxresource)
    ));

    UINT rows;
    UINT64 rowsize, bytesize;
    dxdevice->GetCopyableFootprints(&texture_desc, 0, 1, 0, &texture->footprint, &rows, &rowsize, &bytesize);

    heap_properties.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC staging_desc{};
    staging_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    staging_desc.Width = bytesize;
    staging_desc.Height = 1;
    staging_desc.DepthOrArraySize = 1;
    staging_desc.MipLevels = 1;
    staging_desc.SampleDesc.Count = 1;
    staging_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    SRE_DXCALLF(dxdevice->CreateCommittedResource(
                                                &heap_properties, D3D12_HEAP_FLAG_NONE,
                                                &staging_desc, D3D12_RESOURCE_STATE_COPY_SOURCE,
                                                NULL, IID_PPV_ARGS(&texture->dxstaging)
    ));

    {
        UINT offset = _srvallocate();

        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
        srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuhandlesrc = dxsrvheapstaging->GetCPUDescriptorHandleForHeapStart();
        cpuhandlesrc.ptr += offset;
        dxdevice->CreateShaderResourceView(texture->dxresource, &srv_desc, cpuhandlesrc);

        D3D12_CPU_DESCRIPTOR_HANDLE cpuhandledst = dxsrvheapsv->GetCPUDescriptorHandleForHeapStart();
        cpuhandledst.ptr += offset;
        dxdevice->CopyDescriptorsSimple(1, cpuhandledst, cpuhandlesrc, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        texture->srvoffset = offset;
        texture->srvid = 0;
        texture->sizepertexel = static_cast<UINT>(rowsize / w);
        texture->needsupdate = false;
        if (outformat)
            *outformat = SDL_PIXELFORMAT_RGBA32;

        return true;
    }
}

bool sred3d12_inst::texture_update(texture_type* texture, const sre::rect2Di* region, const void* pixels, int pitch)
{
    HRESULT hr;

    // Update the staging buffer and request to update the texture resource inside default GPU memory
    BYTE* mapped;
    D3D12_RANGE read{};
    SRE_DXCALLF(texture->dxstaging->Map(0, &read, reinterpret_cast<void**>(&mapped)));

    UINT byteoffset = region->position.x*texture->sizepertexel + region->position.y*texture->footprint.Footprint.RowPitch;
    BYTE* ptrdst = mapped + byteoffset;
    const BYTE* ptrsrc = static_cast<const BYTE*>(pixels);
    for (int y = 0; y < region->size.y; y++, ptrdst += texture->footprint.Footprint.RowPitch, ptrsrc += pitch)
        memcpy(ptrdst, ptrsrc, pitch);

    D3D12_RANGE written{ byteoffset, byteoffset + region->size.y*texture->footprint.Footprint.RowPitch };
    texture->dxstaging->Unmap(0, NULL);
    texture->needsupdate = true;

    return true;
}

// ImGui

#if 1
    #include <ImGui.hpp>

    // Apparently ImGui's d3d12 implementation uses CreateDXGIFactory1, linked statically so we're handling that
    static HRESULT WINAPI _CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void **ppFactory)
    {
        HMODULE dxgidll = GetModuleHandleA("dxgi.dll"); // We have DXGI's dll already loaded in and owned so a simple GetModuleHandle is what we want
        SRE_DXGETADDR(CreateDXGIFactory1, PFN_CREATE_DXGI_FACTORY1, dxgidll);
        return pCreateDXGIFactory1(riid, ppFactory);
    }
    
    #define CreateDXGIFactory1 _CreateDXGIFactory1
    #include <../imgui/backends/imgui_impl_dx12.cpp>
    #undef CreateDXGIFactory1

    struct sreD3D12ImGuiData: sre::ImGuiRenderInterface<sred3d12_inst>
    {
        int Init(driver_type* inst, SDL_Window* window) override;
        void NewFrame(driver_type* inst) override;
        void RenderDrawData(driver_type* inst, ImDrawData* imDrawData) override;
        void Shutdown() override;

        static void imgui_srvalloc(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* ocpu, D3D12_GPU_DESCRIPTOR_HANDLE* ogpu);
        static void imgui_srvfree(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu);
    };

    SRE_EXTERN_C_VAR sreD3D12ImGuiData sred3d12imgui{};

    void sreD3D12ImGuiData::imgui_srvalloc(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* ocpu, D3D12_GPU_DESCRIPTOR_HANDLE* ogpu)
    {
        auto inst = static_cast<sred3d12_inst*>(info->UserData);
        
        D3D12_CPU_DESCRIPTOR_HANDLE cpu = inst->dxsrvheapsv->GetCPUDescriptorHandleForHeapStart();
        D3D12_GPU_DESCRIPTOR_HANDLE gpu = inst->dxsrvheapsv->GetGPUDescriptorHandleForHeapStart();

        UINT offs = inst->_srvallocate();

        cpu.ptr += offs;
        gpu.ptr += offs;

        *ocpu = cpu;
        *ogpu = gpu;
    }

    void sreD3D12ImGuiData::imgui_srvfree(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu)
    {
        // Does nothing for now, should implement the free list allocator soon
        auto inst = static_cast<sred3d12_inst*>(info->UserData);
        
        D3D12_CPU_DESCRIPTOR_HANDLE basecpu = inst->dxsrvheapsv->GetCPUDescriptorHandleForHeapStart();

        inst->_srvfree(static_cast<UINT>(cpu.ptr - basecpu.ptr));
    }

    int sreD3D12ImGuiData::Init(driver_type* inst, SDL_Window* window)
    {
        ImGui_ImplDX12_InitInfo info{};
        info.UserData = inst;
        info.Device = inst->dxdevice;
        info.CommandQueue = inst->dxcmd_queue;
        info.NumFramesInFlight = sre::countof(inst->dxrender_targets);
        info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

        info.SrvDescriptorHeap = inst->dxsrvheapstaging; // Using my srv descriptor heap, I was going to separate it but I don't care
        info.SrvDescriptorAllocFn = imgui_srvalloc;
        info.SrvDescriptorFreeFn = imgui_srvfree;

        if (!ImGui_ImplDX12_Init(&info))
            return SRE_RENDERSTATUS_FAILED;
        return SRE_RENDERSTATUS_SUCCEEDED;
    }

    void sreD3D12ImGuiData::NewFrame(driver_type*) { ImGui_ImplDX12_NewFrame(); }
    void sreD3D12ImGuiData::RenderDrawData(driver_type* inst, ImDrawData* imDrawData) { ImGui_ImplDX12_RenderDrawData(imDrawData, inst->dxcmd_list); }
    void sreD3D12ImGuiData::Shutdown() { ImGui_ImplDX12_Shutdown(); }
#endif