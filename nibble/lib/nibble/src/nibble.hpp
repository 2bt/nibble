#pragma once

#include <cstdint>


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


namespace nibble {
    void    init();
    uint8_t button_bits();
    void    flush(uint8_t* pixels);
};
