#include <fx.hpp>
#include "app.hpp"
#include "render.hpp"

#include "asteroids/game.hpp"
#include "snake/game.hpp"
#include "worm/game.hpp"


namespace {

char const T1[] PROGMEM = "ASTEROIDS";
char const T2[] PROGMEM = "SNAKE";
char const T3[] PROGMEM = "WORM";
char const* const TITLES[] PROGMEM = { T1, T2, T3, };


int current_game;


struct Menu {
    uint32_t tick;
    int      pos;
    int      smooth_pos;
    void init();
    void update();
};
void Menu::init() {
    pos = 0;
    if (current_game != G_MENU) pos = current_game;
    smooth_pos = pos << 3;
    tick = 666;
}
void Menu::update() {
    ++tick;

    if (button_just_pressed(fx::BTN_UP)   && pos > 0) --pos;
    if (button_just_pressed(fx::BTN_DOWN) && pos < COUNT_OF(TITLES) - 1) ++pos;

    if (smooth_pos < pos << 3) ++smooth_pos;
    if (smooth_pos > pos << 3) --smooth_pos;


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
    for (int i = 0; i < fx::SCREEN_W * 16; ++i, ++p) *p = DARK[*p];

    // print titles
    for (int i = 0; i < COUNT_OF(TITLES); ++i) {

        int y = i * 16 - smooth_pos * 2;
        if (y > 16)  y = y / 2 + 8;
        if (y < -16) y = y / 2 - 8;

        char line[16];
        strcpy_P(line, TITLES[i]);

        render::print(28, 61 + y, line);
    }


    // start game
    if (fx::button_bits & ((1 << fx::BTN_A) | (1 << fx::BTN_B))) {
        app::init(pos);
    }
}



union GameUnion {
    Menu          menu;
    AsteroidsGame asteroids;
    SnakeGame     snake;
    WormGame      worm;
} game_union;


} // namespace



void app::init(int game) {

    switch (game) {
    case G_MENU:      game_union.menu.init();      break;
    case G_ASTEROIDS: game_union.asteroids.init(); break;
    case G_SNAKE:     game_union.snake.init();     break;
    case G_WORM:      game_union.worm.init();      break;
    default: break;
    }
    current_game = game;
}


void app::update() {
    switch (current_game) {
    case G_MENU:      game_union.menu.update();      break;
    case G_ASTEROIDS: game_union.asteroids.update(); break;
    case G_SNAKE:     game_union.snake.update();     break;
    case G_WORM:      game_union.worm.update();      break;
    default: break;
    }
    if (current_game != G_MENU && button_just_pressed(fx::BTN_C)) app::init(G_MENU);
    prev_button_bits = fx::button_bits;
}

