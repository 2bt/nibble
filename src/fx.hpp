#pragma once

#include <cstdint>
#include <cstring>
#include <algorithm>


namespace fx {
    enum {
        BTN_UP,
        BTN_DOWN,
        BTN_LEFT,
        BTN_RIGHT,
        BTN_A,
        BTN_B,
        BTN_C,
        BTN_COUNT,

        SCREEN_W = 128,
        SCREEN_H = 128,
    };

    extern uint8_t pixels[SCREEN_W * SCREEN_H];

    bool button_down(int b);

    inline void clear(uint8_t color) {
        memset(pixels, color, sizeof(pixels));
    }

    inline void pixel(int x, int y, uint8_t color) {
        uint32_t q = x | y;
        if (q & ~127) return;
        pixels[y * SCREEN_W + x] = color;
    }
};


