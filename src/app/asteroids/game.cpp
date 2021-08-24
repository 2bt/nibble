#include "game.hpp"
#include "../render.hpp"


namespace {


void plot_line(int x0, int y0, int x1, int y1, uint8_t c) {
    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    for (;;) {

        render::pixel(x0 & 127, y0 & 127, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


} // namespace


void AsteroidsGame::init() {
    tick = 0;
    ship.x = ship.y = (63 << 9);
    ship.vx = ship.vy = 0;
    ship.ang = 0;

    for (Bullet& b : bullets) b.ttl = 0;
    next_bullet = 0;
}


void AsteroidsGame::update() {
    ++tick;

    ship.ang += (button_down(fx::BTN_RIGHT) - button_down(fx::BTN_LEFT)) * 4;
    int si = my_sin(ship.ang);
    int co = my_cos(ship.ang);

    bool thrust = button_down(fx::BTN_UP) || button_down(fx::BTN_A);
    if (thrust) {
        ship.vx += si * 50;
        ship.vy -= co * 50;
    }
    ship.vx = ship.vx * ((1 << FRICTION_BITS) - 1) >> FRICTION_BITS;
    ship.vy = ship.vy * ((1 << FRICTION_BITS) - 1) >> FRICTION_BITS;

    ship.x += ship.vx >> 8;
    ship.y += ship.vy >> 8;
    ship.x &= (1 << 16) - 1;
    ship.y &= (1 << 16) - 1;



    // draw
    render::clear(0);

    int points[] = {
        -10, -10,
        0, -5,
        10, -10,
        0, 13,
    };
    int i = COUNT_OF(points) - 2;
    int x0 = ship.x + points[i] * co + points[i + 1] * si;
    int y0 = ship.y + points[i] * si - points[i + 1] * co;
    for (i = 0; i < COUNT_OF(points); i += 2) {
        int x1 = ship.x + points[i] * co + points[i + 1] * si;
        int y1 = ship.y + points[i] * si - points[i + 1] * co;
        plot_line(x0 >> 9, y0 >> 9, x1 >> 9, y1 >> 9, 7);
        x0 = x1;
        y0 = y1;
    }

    // shoot
    if (button_just_pressed(fx::BTN_B)) {
        Bullet& b = bullets[next_bullet];
        if (++next_bullet >= MAX_BULLETS) next_bullet = 0;

        b.ttl = 60;
        b.ang = ship.ang;
        b.x = x0;
        b.y = y0;
    }

    for (Bullet& b : bullets) {
        if (b.ttl == 0) continue;
        --b.ttl;
        int si = my_sin(b.ang);
        int co = my_cos(b.ang);

        b.x += si * 8;
        b.y -= co * 8;
        b.x &= (1 << 16) - 1;
        b.y &= (1 << 16) - 1;

        render::pixel(b.x >> 9, b.y >> 9, 7);
    }


    // flames
    if (thrust) {
        for (int i = 0; i < 4; ++i) {
            uint8_t a = ship.ang + (random.rand() & 31) - 15;
            int si = my_sin(a);
            int co = my_cos(a);

            int y = -25 + (random.rand() & 15);

            int x0 = ship.x + y * si;
            int y0 = ship.y - y * co;
            render::pixel(x0 >> 9, y0 >> 9, 8);
        }
    }
}
