#ifndef SRE_INTS_H
#define SRE_INTS_H

#include "stdint.h"
#include "stddef.h"

typedef int8_t sre_s8;
typedef int16_t sre_s16;
typedef int32_t sre_s32;
typedef int64_t sre_s64;

typedef uint8_t sre_u8;
typedef uint16_t sre_u16;
typedef uint32_t sre_u32;
typedef uint64_t sre_u64;

typedef intptr_t sre_sptr;
typedef uintptr_t sre_uptr;
typedef size_t sre_usize;

typedef sre_u8 sre_byte;

#ifdef __cplusplus

    namespace sre
    {
        using s8 = sre_s8;
        using s16 = sre_s16;
        using s32 = sre_s32;
        using s64 = sre_s64;

        using u8 = sre_u8;
        using u16 = sre_u16;
        using u32 = sre_u32;
        using u64 = sre_u64;

        using sptr = sre_sptr;
        using uptr = sre_uptr;
        using usize = sre_usize;

        using byte = sre_byte;
    }

    #undef byte

#endif // __cplusplus

#endif