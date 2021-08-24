#pragma once

#include "../util.hpp"


struct AsteroidsGame {
    void init();
    void update();

    void spawn_asteroid(int x, int y, int size);
    void spawn_explosion(int x, int y, int size);
    void init_level(int l);

    enum {
        MAX_BULLETS   = 4,
        MAX_ASTEROIDS = 64,
        MAX_PARTICLES = 64,
    };

    struct Bullet {
        uint8_t ttl;
        uint8_t ang;
        int     x;
        int     y;
    };

    struct Asteroid {
        int      x;
        int      y;
        int16_t  vx;
        int16_t  vy;
        uint16_t ang;
        int16_t  vang;
        uint8_t  size;
        uint8_t  health;
        uint8_t  shape;
    };

    struct Particle {
        uint8_t ttl;
        uint8_t color;
        int     x;
        int     y;
        int16_t vx;
        int16_t vy;
    };

    Random  random;
    int     tick;
    int     level;

    struct {
        int     x;
        int     y;
        int     vx;
        int     vy;
        uint8_t ang;
    } ship;

    Bullet    bullets[MAX_BULLETS];
    Asteroid  asteroids[MAX_ASTEROIDS];
    Particle  particles[MAX_PARTICLES];
    uint8_t   next_bullet;
    uint8_t   next_shape;
    uint8_t   next_particle;
    int       score;
};
