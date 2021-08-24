#include "game.hpp"
#include "../render.hpp"


namespace {


void plot_line(int x0, int y0, int x1, int y1, uint8_t c) {
    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    for (;;) {
        // wrap coords
        render::pixel(x0 & 127, y0 & 127, c);

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


} // namespace

void AsteroidsGame::spawn_asteroid(int x, int y, int size) {
    for (Asteroid& a : asteroids) {
        if (a.health > 0) continue;
        a.size   = size;
        a.health = size + 1;
        a.shape  = next_shape;
        if (++next_shape == SHAPE_COUNT) next_shape = 0;

        a.x = x;
        a.y = y;

        uint8_t ang = random.rand();
        uint16_t v = (random.rand() >> (7 + size)) + 100;

        a.vx   = my_sin(ang) * v >> 8;
        a.vy   = my_cos(ang) * v >> 8;
        a.ang  = random.rand();
        a.vang = random.rand();
        return;
    }
}

void AsteroidsGame::init() {
    tick = 0;
    init_level(1);
}
void AsteroidsGame::init_level(int l) {
    level = l;

    ship.x = ship.y = (63 << 9);
    ship.vx = ship.vy = 0;
    ship.ang = 0;

    next_bullet = 0;
    next_shape = 0;
    for (Asteroid& a : asteroids) a.health = 0;
    for (Bullet& b : bullets) b.ttl = 0;

    for (int i = 0; i < level; ++i) {
        if (i & 1) {
            spawn_asteroid(0, random.rand(), 2);
        }
        else {
            spawn_asteroid(random.rand(), 0, 2);
        }
    }
}


void AsteroidsGame::update() {
    enum {
        FRICTION_BITS = 6,
        SPEED         = 50,
    };

    ++tick;

    // move ship
    ship.ang += (button_down(fx::BTN_RIGHT) - button_down(fx::BTN_LEFT)) * 4;
    int si = my_sin(ship.ang);
    int co = my_cos(ship.ang);

    bool thrust = button_down(fx::BTN_UP) || button_down(fx::BTN_A);
    if (thrust) {
        ship.vx += si * SPEED;
        ship.vy -= co * SPEED;
    }
    ship.vx = ship.vx * ((1 << FRICTION_BITS) - 1) >> FRICTION_BITS;
    ship.vy = ship.vy * ((1 << FRICTION_BITS) - 1) >> FRICTION_BITS;

    ship.x += ship.vx >> 8;
    ship.y += ship.vy >> 8;
    ship.x &= (1 << 16) - 1;
    ship.y &= (1 << 16) - 1;



    // draw
    render::clear(0);

    int8_t const POINTS[] = {
        -10, -10,
        0, -5,
        10, -10,
        0, 12,
    };
    int i = COUNT_OF(POINTS) - 2;
    int x0 = ship.x + POINTS[i] * co + POINTS[i + 1] * si;
    int y0 = ship.y + POINTS[i] * si - POINTS[i + 1] * co;
    for (i = 0; i < COUNT_OF(POINTS); i += 2) {
        int x1 = ship.x + POINTS[i] * co + POINTS[i + 1] * si;
        int y1 = ship.y + POINTS[i] * si - POINTS[i + 1] * co;
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

    // bullets
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
        for (int i = 0; i < 3; ++i) {
            uint8_t a = ship.ang + (random.rand() & 31) - 15;
            int si = my_sin(a);
            int co = my_cos(a);

            int y = -25 + (random.rand() & 15);

            int x0 = ship.x + y * si;
            int y0 = ship.y - y * co;
            render::pixel((x0 >> 9) & 127, (y0 >> 9) & 127, 8 + i);
        }
    }


    enum {
        LEN = 24
    };
    int8_t const ASTEROID_POINTS[3][LEN] = {
        { -3, 4, 1, 4, 4, 2, 4, 1, 1, 0, 4, -2, 2, -4, 1, -3, -2, -4, -4, -1, -4, 2, -1, 2, },
        { -2, 4,-2, 4, -2, 4, 0, 2, 2, 4, 4, 2, 3, 0, 4, -2, 1, -4, -2, -4, -4, -2, -4, 2, },
        { -2, 4, 0, 3, 2, 4, 4, 2, 2, 1, 4, -1, 2, -4, -1, -3, -2, -4, -4, -2, -3, 0, -4, 2 },
    };


    for (Asteroid& a : asteroids) {
        if (a.health == 0) continue;

        a.ang += a.vang;
        a.x += a.vx;
        a.y += a.vy;
        a.x &= (1 << 16) - 1;
        a.y &= (1 << 16) - 1;


        int8_t const (&POINTS)[LEN] = ASTEROID_POINTS[a.shape];
        uint8_t const SCALES[] = { 4, 8, 12 };
        uint8_t s = SCALES[a.size];

        uint8_t color = 7;

        // ship collision
        int dx = abs(ship.x - a.x);
        int dy = abs(ship.y - a.y);
        dx >>= 9;
        dy >>= 9;
        if (dx > 64) dx = fx::SCREEN_W - dx;
        if (dy > 64) dx = fx::SCREEN_H - dx;
        int r = s + 2;
        if (dx * dx + dy * dy < r * r) {
            color = 8;
        }

        // bullet collision
        for (Bullet& b : bullets) {
            if (b.ttl == 0) continue;
            int dx = abs(b.x - a.x);
            int dy = abs(b.y - a.y);
            dx >>= 9;
            dy >>= 9;
            if (dx > 64) dx = fx::SCREEN_W - dx;
            if (dy > 64) dx = fx::SCREEN_H - dx;
            int r = s + 1;
            if (dx * dx + dy * dy < r * r) {
                b.ttl = 0;
                if (a.health == 1 && a.size > 0) {
                    spawn_asteroid(a.x, a.y, a.size - 1);
                    spawn_asteroid(a.x, a.y, a.size - 1);
                    spawn_asteroid(a.x, a.y, a.size - 1);
                }
                --a.health;
            }
        }



        int si = my_sin(a.ang >> 8) * s;
        int co = my_cos(a.ang >> 8) * s;

        int i = LEN - 2;
        int x0 = a.x + POINTS[i] * co + POINTS[i + 1] * si;
        int y0 = a.y + POINTS[i] * si - POINTS[i + 1] * co;

        for (i = 0; i < LEN; i += 2) {
            int x1 = a.x + POINTS[i] * co + POINTS[i + 1] * si;
            int y1 = a.y + POINTS[i] * si - POINTS[i + 1] * co;
            plot_line(x0 >> 9, y0 >> 9, x1 >> 9, y1 >> 9, color);
            x0 = x1;
            y0 = y1;
        }
    }
}
