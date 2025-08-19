#pragma once
#include "Engine.h"
#include "Datatypes/TimeStamp.h"

class Tween
{
public:
enum EasingStyle
{
    Es_Linear,
};
enum EasingDirection
{
    Ed_InOut,
    Ed_In,
    Ed_Out
};
enum TargetType
{
    TT_uint8,
    TT_int8,

    TT_uint16,
    TT_int16,
    
    TT_uint32,
    TT_int32,
    
    TT_uint64,
    TT_int64,

    TT_float,
    TT_double
};

struct Info
    {
        float duration;
        EasingStyle easingStyle;
        EasingDirection esingDirection;

        float delay;
        int repeat;
        bool reverse;
    };
public:
    Tween(Info* info, void* target, const void* src, TargetType type);
    ~Tween();

    inline void SetTarget(void* target) {m_target.u8 = (uint8_t*)target;}

    void Play();
    TimeStamp Cancel();
    TimeStamp Pause();

    Signal Completed;

    inline float GetAlpha() {return m_info->duration ? m_elapsed/m_info->duration : 1;}
private:
    Info* m_info;
    union TypePointers
    {
        uint8_t *u8;
        int8_t *i8;

        uint16_t *u16;
        int16_t *i16;

        uint32_t *u32;
        int32_t *i32;

        uint64_t *u64;
        int64_t *i64;

        float *float_;
        double *double_;
    } m_target, m_src, m_start;


    TargetType m_type;

    TimeStamp m_elapsed = 0;
    bool m_Playing = false;

    int64_t m_longstart = 0;

    void Update(TimeStamp delta);

    static void global_update(TimeStamp delta);
    static std::unordered_set<Tween*>* s_tweens;
    friend EngineClass;
};