#include "game.hpp"
#include "../render.hpp"


void WormGame::init() {
    tick = 0;

    x = 64 << 9;
    y = 64 << 9;
    ang = 0;

    render::clear(7);
    render::fill_rect({1, 1, 126, 126}, 1);
}


void WormGame::update() {

    ang += (button_down(fx::BTN_RIGHT) - button_down(fx::BTN_LEFT)) * 4;

    x += my_sin(ang) * 3;
    y -= my_cos(ang) * 3;


    render::pixel(x >> 9, y >> 9, 8);
}
