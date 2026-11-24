#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

/* --- primitive types --- */
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;
typedef long double f128;

/* --- int limits --- */
#define I8_MIN ((i8)0x80)
#define I8_MAX ((i8)0x7f)
#define I16_MIN ((i16)0x8000)
#define I16_MAX ((i16)0x7fff)
#define I32_MIN ((i32)0x80000000)
#define I32_MAX ((i32)0x7fffffff)
#define I64_MIN ((i64)0x8000000000000000)
#define I64_MAX ((i64)0x7fffffffffffffff)

#define U8_MIN ((u8)0x00)
#define U8_MAX ((u8)0xff)
#define U16_MIN ((u16)0x0000)
#define U16_MAX ((u16)0xffff)
#define U32_MIN ((u32)0x00000000)
#define U32_MAX ((u32)0xffffffff)
#define U64_MIN ((u64)0x0000000000000000)
#define U64_MAX ((u64)0xffffffffffffffff)

/* --- helper macros --- */
#define ARR_LEN(a) (sizeof(a)/sizeof(*(a)))
#define MIN(a, b) (((a) < (b))? (a) : (b))
#define MAX(a, b) (((a) > (b))? (a) : (b))
#define CLAMP(a, x, b) (((x) < (a))? (a) : ((x) > (b))? (b) : (x))
#define SWAP(a, b) do { a ^= b; b ^= a; a ^= b; } while (0)

#define KB(x) ((u64)(x) << 10)
#define MB(x) ((u64)(x) << 20)
#define GB(x) ((u64)(x) << 30)

#define IS_POW2(x) (!((x) & ((x) - 1)))

#define __DEFER(f, v) \
  auto void f(u8*); \
  __attribute__((__cleanup__(f))) u8 v; \
  void f(u8*)
#define __DEFER1(n) __DEFER(__DEFER_FUNC ## n, __DEFER_VAR ## n)
#define __DEFER2(n) __DEFER1(n)
#define defer __DEFER2(__COUNTER__)

/* --- arena --- */
typedef struct {
  u8 *buffer;
  size_t size, capacity;
} Arena;

// #define ArenaInit(cap) (Arena){ .buffer = malloc(cap), .capacity = cap, }

Arena ArenaInit(size_t capacity) {
  Arena a = {
    .buffer = malloc(capacity),
    .capacity = capacity,
  };

  if (!a.buffer) {
    fprintf(stderr, "Failed to allocate memory for arena\n");
    exit(1);
  }

  return a;
}

void *ArenaAlloc(Arena *a, void *item, size_t size) {
  if (a->capacity - a->size < size) {
    fprintf(stderr, "Arena out of memory\n");
    return NULL;
  }

  void *cur = &(a->buffer)[a->size + 1];
  memcpy(cur, item, size);
  a->size += size;

  return cur;
}

void ArenaFree(Arena *a) {
  free(a->buffer);
}

/* --- string view --- */
#define STR_FMT "%.*s"
#define STR_ARG(s) (i32)(s).length, (s).data

typedef struct {
  char *data;
  size_t length;
} String;

String S(char *str) {
  return (String){
    .data = str,
    .length = strlen(str),
  };
}

void StrTrim(String *s, i8 dir) {
  if (s->length <= 0) return;

  if (dir > 0) s->data += abs(dir);
  s->length -= abs(dir);
}

void StrTrimSpace(String *s) {
  for (; isspace(s->data[0]); StrTrim(s, 1));
  for (; isspace(s->data[s->length - 1]); StrTrim(s, -1));
}

String StrSplit(String *s, char delim) {
  for (size_t i = 0; i < s->length; i++) {
    if (s->data[i] == delim) {
      s->length -= &s->data[s->length] - &s->data[i];
      return S(&s->data[i] + 1);
    }
  }

  return S("");
}
