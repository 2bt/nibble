#include "game.hpp"
#include "../render.hpp"


void WormGame::init() {
    tick = 0;

    x = 64 << 8;
    y = 64 << 8;
    ang = 0;

    render::clear(1);
}


void WormGame::update() {

    ang += (button_down(fx::BTN_RIGHT) - button_down(fx::BTN_LEFT)) * 4;

    x += my_sin(ang);
    y -= my_sin(ang + 64);


    render::pixel(x >> 8, y >> 8, 8);
}
