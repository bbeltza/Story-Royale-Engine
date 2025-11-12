#pragma once

#ifdef __cplusplus
#define __ASSERT_H <cassert>
#else
#define __ASSERT_H <assert.h>
#endif

#ifdef NDEBUG
#undef NDEBUG
#include __ASSERT_H
#define NDEBUG
#else
#include __ASSERT_H
#endif

#undef __ASSERT_H