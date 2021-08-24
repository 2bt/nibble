#pragma once

#include "../util.hpp"


struct AsteroidsGame {
    void init();
    void update();

    enum {
        FRICTION_BITS = 6,
        MAX_BULLETS   = 4,
    };

    Random  random;
    int     tick;

    struct {
        int     x;
        int     y;
        int     vx;
        int     vy;
        uint8_t ang;
    } ship;

    struct Bullet {
        uint8_t ttl;
        uint8_t ang;
        int x;
        int y;
    };
    Bullet  bullets[MAX_BULLETS];
    uint8_t next_bullet;
};
