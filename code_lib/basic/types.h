#ifndef TYPES_H
#define TYPES_H

#define internal static
#define global	 static

#include <cstdint>
#include <stdarg.h>
#include <math.h>

typedef unsigned char u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef char     s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef int32_t b32; 
typedef float   r32;
typedef double  r64;
typedef unsigned long ulong;

union r32u {
  r32 value;
  u8 bytes[4];
};

#endif // !TYPES_H
