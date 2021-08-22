#include <fx.hpp>
#include "game.hpp"
#include "render.hpp"


uint8_t prev_button_bits = 0;

inline bool button_down(int b) {
    return (fx::button_bits >> b) & 1;
}
inline bool button_just_pressed(int b) {
    return button_down(b) & ~(prev_button_bits >> b) & 1;
}




char const T1[] PROGMEM = "ASTEROIDS";
char const T2[] PROGMEM = "BLONK";
char const T3[] PROGMEM = "INVADERZ";
char const T4[] PROGMEM = "SNAKE";
char const T5[] PROGMEM = "SUPER MARIO";
char const T6[] PROGMEM = "TETRIS";
char const* const TITLES[] PROGMEM = { T1, T2, T3, T4, T5, T6, };



uint32_t tick = 666;
int pos = 0;
int pos2 = 0;


void game::init() {
}



void game::update() {

    ++tick;

    if (button_just_pressed(fx::BTN_UP)   && pos > 0) --pos;
    if (button_just_pressed(fx::BTN_DOWN) && pos < COUNT_OF(TITLES) - 1) ++pos;

    prev_button_bits = fx::button_bits;

    if (pos2 < pos << 3) ++pos2;
    if (pos2 > pos << 3) --pos2;



    // draw

    // plasma
    uint8_t* p = fx::pixels;
    uint8_t* q = fx::pixels + fx::SCREEN_W;
    for (int j = 0; j < fx::SCREEN_H; j += 2) {
        for (int i = 0; i < fx::SCREEN_W; i += 2) {

            int x = my_sin(j + tick / 16);
            int y = my_sin(i + tick / 16);
            int xp = i + my_sin(x / 8 + tick / 2 - my_sin(y / 2 + tick) / 2) / 2;
            int yp = j + my_sin(y / 8 + tick / 2 + my_sin(x / 4 - tick) / 2) / 2;
            int f = xp + yp + tick / 2;
            uint8_t const pal[] = { 1, 5, 8, 5 };
            uint8_t c = pal[( f       / 64) & 0x3];
            uint8_t d = pal[((f + 32) / 64) & 0x3];
            *p++ = c;
            *p++ = d;
            *q++ = d;
            *q++ = c;
        }
        p += fx::SCREEN_W;
        q += fx::SCREEN_W;
    }

    // darken
    static uint8_t const DARK[] = { 0, 0, 1, 1, 2, 1, 5, 6, 2, 4, 9, 3, 1, 1, 2, 5 };
    p = fx::pixels + 56 * fx::SCREEN_W;
    for (int i = 0; i < fx::SCREEN_W * 16; ++i) *p++ = DARK[*p];

    // print titles
    for (int i = 0; i < COUNT_OF(TITLES); ++i) {

        int y = i * 16 - pos2 * 2;
        if (y > 16)  y = y / 2 + 8;
        if (y < -16) y = y / 2 - 8;

        char line[16];
        strcpy_P(line, TITLES[i]);

        render::print(28, 61 + y, line);
    }

}

