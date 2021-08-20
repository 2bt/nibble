#pragma once

#include <stdint.h>

namespace std {
    template <class T> T min(T const& x, T const& y) { return x < y ? x : y; }
    template <class T> T max(T const& x, T const& y) { return x > y ? x : y; }
}
template <class T> T abs(T const& x) { return x < 0 ? -x : x; }


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

    bool     button_down(int b);
    void     clear(uint8_t color);
    void     pixel(int x, int y, uint8_t color);
    uint8_t* pixel_data();
};


