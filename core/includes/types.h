#pragma once

#include <cassert>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef i8 b8;
typedef i32 b32;
typedef i64 b64;

typedef float f32;
typedef double f64;

inline void checkBytes() {
    assert(sizeof(i8) == 1);
    assert(sizeof(i16) == 2);
    assert(sizeof(i32) == 4);
    assert(sizeof(i64) == 8);

    assert(sizeof(u8) == 1);
    assert(sizeof(u16) == 2);
    assert(sizeof(u32) == 4);
    assert(sizeof(u64) == 8);

    assert(sizeof(b8) == 1);
    assert(sizeof(b32) == 4);
    assert(sizeof(b64) == 8);

    assert(sizeof(f32) == 4);
    assert(sizeof(f64) == 8);
}