#pragma once
// courtesy of dekrypt (rip)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <stdatomic.h>
#ifdef __unix__
#	include <unistd.h>
#elif defined(_WIN32)
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <windows.h>
#	include <timeapi.h>
#	undef near
#	undef far
#endif

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef size_t usz;

typedef float f32;
typedef double f64;

#define ABORT() exit(-1)
#define COLOR(code, headword, padding, s, ...) \
      fprintf(stderr, "\x1B[" code "m\x1B[1m" headword "\x1B[90m\x1B[1m"  \
              padding "%s(%u)" ":%s:\x1B[0m " s "\n", __FILE__, __LINE__, \
              __func__, ##__VA_ARGS__)
#define INFO(s, ...) COLOR("32", "info", "    ", s, ##__VA_ARGS__)

#define WARN(s, ...) COLOR("33", "warn", "    ", s, ##__VA_ARGS__)
#ifdef USE_DEBUG
#	define DEBUG(s, ...) COLOR("34", "debug", "   ", s, ##__VA_ARGS__)
#else
#	define DEBUG(...)
#endif
// Windows is funny
#ifdef ERROR
#	undef ERROR
#endif
#define ERROR_(s, ...) COLOR("31", "error", "   ", s, ##__VA_ARGS__)
#define ERROR ERROR_
#define DIE(msg, ...) do { \
        ERROR(msg, ##__VA_ARGS__); \
        ABORT(); \
} while (0)
#define ASSERT(cnd, msg, ...) \
        if (!(cnd)) \
                DIE("failed to " msg, ##__VA_ARGS__)
#define ASSERTZ(cnd, msg, ...) \
        if (!(cnd)) \
                DIE(msg, ##__VA_ARGS__)
#define CHK(cnd, msg, ...) \
        if ((cnd)) \
                COLOR("32", "ok", "      ", msg, ##__VA_ARGS__); \
        else { \
                COLOR("31", "fail", "    ", msg, ##__VA_ARGS__); \
		ABORT(); \
	}

#define MCHK(cnd, msg, ...) \
        if ((cnd)) \
                COLOR("32", "ok", "      ", msg, ##__VA_ARGS__); \
        else { \
                COLOR("31", "fail", "    ", msg, ##__VA_ARGS__); \
	}

#define MOCHK(cnd, msg, ...) \
        if ((cnd)) \
                COLOR("32", "ok", "      ", msg, ##__VA_ARGS__); \
        else { \
                COLOR("31", "fail", "    ", msg, ##__VA_ARGS__); \
                return; \
	}

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define COUNTOF(array) (sizeof(array) / sizeof(*array))

#define LIKELY(x) __builtin_expect(x, 1)
#define UNLIKELY(x) __builtin_expect(x, 1)

#define UNUSED __attribute__((unused))
#define VKOK(stat) ((stat) == VK_SUCCESS)

#define CRCOLOR(code, headword, padding, s, ...) \
      fprintf(stderr, "\r\x1B[" code "m\x1B[1m" headword "\x1B[90m\x1B[1m"  \
              padding "%s(%u)" ":%s:\x1B[0m " s, __FILE__, __LINE__, \
              __func__, ##__VA_ARGS__)
#define CRINFO(s, ...) CRCOLOR("32", "info", "    ", s, ##__VA_ARGS__)
#define CRWARN(s, ...) CRCOLOR("33", "warn", "    ", s, ##__VA_ARGS__)


u64 hash(const void *p, usz sz, u64 seed);
u32 getnprocs(void);

typedef __attribute((aligned(8))) f32 Vec2[2];
typedef __attribute((aligned(16))) f32 Vec3[3];
typedef __attribute((aligned(16))) f32 Vec4[4];
typedef __attribute((aligned(32))) Vec4 Mat4[4];
typedef struct Vec2s {Vec2 v;} Vec2s;
typedef struct Vec3s {Vec3 v;} Vec3s;
typedef struct Vec4s {Vec4 v;} Vec4s;
typedef struct Mat4s {Mat4 v;} Mat4s;
