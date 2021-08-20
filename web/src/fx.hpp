#pragma once

#include <stdint.h>


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

	bool button_down(int b);
    void clear(uint8_t color);
    void pixel(int x, int y, uint8_t color);
};


