#pragma once

#ifdef WASM

#include <stdint.h>

namespace std {
    template <class T> T min(T const& x, T const& y) { return x < y ? x : y; }
    template <class T> T max(T const& x, T const& y) { return x > y ? x : y; }
}
template <class T> T abs(T const& x) { return x < 0 ? -x : x; }
inline void memset(void* s, int c, int n) {
    for (uint8_t* b = (uint8_t*) s; n--;) *b++ = c;
}
inline void memcpy(void* dst, void const* src, int n) {
    uint8_t* d = (uint8_t*) dst;
    uint8_t const* s = (uint8_t const*) src;
    while (n-- > 0) *d++ = *s++;
}
inline void strcpy(char* dst, char const* src) {
    while ((*dst++ = *src++));
}
inline int strlen(char const* str) {
    int l = 0;
    while (*str++) ++l;
    return l;
}

#else

#include <cstdint>
#include <cstring>
#include <algorithm>

#endif

#ifndef PROGMEM

#define PROGMEM
#define pgm_read_byte(x) *(uint8_t const*)(x)
#define strcpy_P strcpy

#endif


namespace fx {
    enum {
        BTN_UP,
        BTN_DOWN,
        BTN_LEFT,
        BTN_RIGHT,
        BTN_B,
        BTN_A,
        BTN_C,
        BTN_COUNT,

        SCREEN_W = 128,
        SCREEN_H = 128,
    };

    extern uint8_t pixels[SCREEN_W * SCREEN_H];
    extern uint8_t button_bits;

    bool try_load(char const* filename, void* data, int len);
    void store(char const* filename, void const* data, int len);
};


