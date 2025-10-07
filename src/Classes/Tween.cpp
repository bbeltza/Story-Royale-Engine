#include "Classes/Tween.hpp"

#include "utils.h"

Tween::Set& Tween::get_tweens()
{
    static Set set;
    return set;
}

void Tween::global_update(TimeStamp delta)
{
    for (Tween* tween : get_tweens())
    {
        if (tween->m_Playing)
            tween->Update(delta);
    }
}

Tween::Tween(const Info* info, void* target, const void* src, TargetType type):
    m_info(info), m_type(type), Completed(this)
{
    m_target.u8 = (uint8_t*)target;
    m_src.u8 = (uint8_t*)src;
    m_start.i64 = &m_longstart;

    get_tweens().insert(this);
}

Tween::~Tween()
{
    get_tweens().erase(this);
}

void Tween::Play()
{
    m_Playing = true;
    switch (m_type)
    {
    case TT_uint8:
        *m_start.u8 = *m_target.u8;
        break;
    case TT_int8:
        *m_start.i8 = *m_target.i8;
        break;
    case TT_uint16:
        *m_start.u16 = *m_target.u16;
        break;
    case TT_int16:
        *m_start.i16 = *m_target.i16;
        break;
    case TT_uint32:
        *m_start.u32 = *m_target.u32;
        break;
    case TT_int32:
        *m_start.i32 = *m_target.i32;
        break;
    case TT_uint64:
        *m_start.u64 = *m_target.u64;
        break;
    case TT_int64:
        *m_start.i64 = *m_target.i64;
        break;
    case TT_float:
        *m_start.float_ = *m_target.float_;
        break;
    case TT_double:
        *m_start.double_ = *m_target.double_;
        break;
    default:
        break;
    }
}

TimeStamp Tween::Cancel()
{
    m_Playing = false;

    float elapse = m_elapsed;
    m_elapsed = 0;
    return elapse;
}

TimeStamp Tween::Pause()
{
    m_Playing = false;

    return m_elapsed;
}

void Tween::Update(TimeStamp delta)
{
    float alpha;
    m_elapsed += delta;
    if (m_elapsed >= m_info->duration)
    {
        alpha = 1;
        m_elapsed = 0;
        m_Playing = false;
    }
    else
        alpha = GetAlpha();
    switch (m_type)
    {
    case TT_uint8:
        *m_target.u8 = (uint8_t)ut_lerp(*m_start.u8, *m_src.u8, alpha);
        break;
    case TT_int8:
        *m_target.i8 = (int8_t)ut_lerp(*m_start.i8, *m_src.i8, alpha);
        break;
    case TT_uint16:
        *m_target.u16 = (uint16_t)ut_lerp(*m_start.u16, *m_src.u16, alpha);
        break;
    case TT_int16:
        *m_target.i16 = (int16_t)ut_lerp(*m_start.i16, *m_src.i16, alpha);
        break;
    case TT_uint32:
        *m_target.u32 = (uint32_t)ut_lerp(*m_start.u32, *m_src.u32, alpha);
        break;
    case TT_int32:
        *m_target.i32 = (int32_t)ut_lerp(*m_start.i32, *m_src.i32, alpha);
        break;
    case TT_uint64:
        *m_target.u64 = (uint64_t)ut_lerp(*m_start.u64, *m_src.u64, alpha);
        break;
    case TT_int64:
        *m_target.i64 = (int64_t)ut_lerp(*m_start.i64, *m_src.i64, alpha);
        break;
    case TT_float:
        *m_target.float_ = ut_lerp(*m_start.float_, *m_src.float_, alpha);
        break;
    case TT_double:
        *m_target.double_ = ut_lerp(*m_start.double_, *m_src.double_, alpha);
        break;
    default:
        break;
    }

    if (!m_Playing)
        Completed.Fire(nullptr); // Firing after setting the target values is better
}