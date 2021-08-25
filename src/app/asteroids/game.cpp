#include "game.hpp"
#include "../render.hpp"


#define HIGH_SCORES_FILE "/asteroids"


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


enum {
    FRICTION_BITS = 6,
    SPEED         = 50,
    SHAPE_COUNT   = 3,
};


} // namespace


void AsteroidsGame::init() {

    if (!high_scores.load(HIGH_SCORES_FILE)) {
        high_scores.reset();
        for (int i = HighScores::LEN; i > 0; --i) high_scores.add("...", i * 100);
        high_scores.save(HIGH_SCORES_FILE);
    }
    name[0] = 'A';
    name[1] = 'A';
    name[2] = 'A';

    score = 0;
    init_level(0);

    state = S_HIGH_SCORES;

    spawn_asteroid(random.rand(), random.rand(), 2);
    spawn_asteroid(random.rand(), random.rand(), 1);
    spawn_asteroid(random.rand(), random.rand(), 1);
    spawn_asteroid(random.rand(), random.rand(), 0);
    spawn_asteroid(random.rand(), random.rand(), 0);
    spawn_asteroid(random.rand(), random.rand(), 0);
}

void AsteroidsGame::init_level(int l) {
    level = l;
    state = S_LEVEL_START;
    tick = 0;
    ship.x = ship.y = (63 << 9);
    ship.vx = ship.vy = 0;
    ship.ang = 0;

    next_bullet = 0;
    next_shape = 0;
    next_particle = 0;
    for (Asteroid& a : asteroids) a.health = 0;
    for (Bullet& b : bullets) b.ttl = 0;
    for (Particle& p : particles) p.ttl = 0;
    for (int i = 0; i < level; ++i) {
        if (i & 1) spawn_asteroid(0, random.rand(), 2);
        else       spawn_asteroid(random.rand(), 0, 2);
    }

    current_letter = 0;
    new_high_score = false;
}

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
        a.vang = random.rand() >> (6 + size);
        return;
    }
}

void AsteroidsGame::spawn_explosion(int x, int y, int size) {
    int count = 8 << size;
    for (int i = 0; i < count; ++i) {
        Particle& p = particles[next_particle];
        if (++next_particle >= MAX_PARTICLES) next_particle = 0;

        uint8_t ang = (random.rand() & 63) | ((i & 3) << 6);
        int si = my_sin(ang);
        int co = my_cos(ang);

        uint16_t v = (random.rand() >> 10) + 100;

        p.ttl   = 3 + (random.rand() & 15);
        p.color = 8 + (i / 4 & 1);
        p.vx = si * v >> 6;
        p.vy = co * v >> 6;
        p.x = x >> 1;
        p.y = y >> 1;
        p.x += p.vx * size;
        p.y += p.vy * size;
    }
}

void AsteroidsGame::update() {

    // move ship
    int  dang   = 0;
    bool thrust = false;
    bool shoot  = false;

    if (state == S_NORMAL) {
        dang   = (button_down(fx::BTN_RIGHT) - button_down(fx::BTN_LEFT)) * 4;
        thrust = button_down(fx::BTN_UP) || button_down(fx::BTN_A);
        shoot  = button_just_pressed(fx::BTN_B);
    }

    ship.ang += dang;
    int si = my_sin(ship.ang);
    int co = my_cos(ship.ang);

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


    // clear screen
    {
        uint8_t* p = fx::pixels;
        uint8_t* q = fx::pixels + fx::SCREEN_W;
        for (int y = 0; y < fx::SCREEN_H; y += 2) {
            for (int x = 0; x < fx::SCREEN_W; ++x) {
                *p++ = 0;
                *q++ = 1;
            }
            p += fx::SCREEN_W;
            q += fx::SCREEN_W;
        }
    }


    if (state == S_LEVEL_START || state == S_NORMAL) {
        // draw ship
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
        if (thrust) {
            for (int i = 0; i < 3; ++i) {
                uint8_t a = ship.ang + (random.rand() & 31) - 15;
                int si = my_sin(a);
                int co = my_cos(a);

                int y = -25 + (random.rand() & 15);

                int x0 = ship.x + y * si;
                int y0 = ship.y - y * co;
                render::pixel((x0 >> 9) & 127, (y0 >> 9) & 127, 10);
            }
        }

        // shoot
        if (shoot) {
            Bullet& b = bullets[next_bullet];
            if (++next_bullet >= MAX_BULLETS) next_bullet = 0;
            b.ttl = 60;
            b.ang = ship.ang;
            b.x = x0;
            b.y = y0;
        }
    }

    // bullets
    for (Bullet& b : bullets) {
        if (b.ttl == 0) continue;
        --b.ttl;
        int si = my_sin(b.ang);
        int co = my_cos(b.ang);
        int x = b.x;
        int y = b.y;
        b.x += si * 8;
        b.y -= co * 8;
        plot_line(x >> 9, y >> 9, b.x >> 9, b.y >> 9, 11);
        b.x &= (1 << 16) - 1;
        b.y &= (1 << 16) - 1;
    }


    enum { LEN = 24 };
    int8_t const ASTEROID_POINTS[SHAPE_COUNT][LEN] = {
        { -3, 4, 1, 4, 4, 2, 4, 1, 1, 0, 4, -2, 2, -4, 1, -3, -2, -4, -4, -1, -4, 2, -1, 2, },
        { -2, 4,-2, 4, -2, 4, 0, 2, 2, 4, 4, 2, 3, 0, 4, -2, 1, -4, -2, -4, -4, -2, -4, 2, },
        { -2, 4, 0, 3, 2, 4, 4, 2, 2, 1, 4, -1, 2, -4, -1, -3, -2, -4, -4, -2, -3, 0, -4, 2 },
    };


    int level_cleared = true;
    for (Asteroid& a : asteroids) {
        if (a.health == 0) continue;
        level_cleared = false;

        a.ang += a.vang;
        a.x += a.vx;
        a.y += a.vy;
        a.x &= (1 << 16) - 1;
        a.y &= (1 << 16) - 1;


        int8_t const (&POINTS)[LEN] = ASTEROID_POINTS[a.shape];
        uint8_t const SCALES[SHAPE_COUNT] = { 4, 8, 12 };
        uint8_t s = SCALES[a.size];

        // ship collision
        if (state == S_NORMAL) {
            int dx = abs(ship.x - a.x);
            int dy = abs(ship.y - a.y);
            dx >>= 9;
            dy >>= 9;
            if (dx > 64) dx = fx::SCREEN_W - dx;
            if (dy > 64) dy = fx::SCREEN_H - dy;
            int r = (s + 2) * (s + 3);
            if (dx * dx + dy * dy < r) {
                spawn_explosion(ship.x, ship.y, 2);
                spawn_explosion(ship.x, ship.y, 1);
                spawn_explosion(ship.x, ship.y, 0);

                state = S_GAME_OVER;

                // check for high score
                if (score > high_scores.entries[9].score) {
                    new_high_score = true;
                }
            }
        }


        // bullet collision
        for (Bullet& b : bullets) {
            if (b.ttl == 0) continue;
            int dx = abs(b.x - a.x);
            int dy = abs(b.y - a.y);
            dx >>= 9;
            dy >>= 9;
            if (dx > 64) dx = fx::SCREEN_W - dx;
            if (dy > 64) dy = fx::SCREEN_H - dy;
            int r = s + 1;
            if (dx * dx + dy * dy < r * r) {
                b.ttl = 0;
                if (a.health == 1) {
                    uint8_t const SCORES[] = { 10, 5, 2 };
                    score += SCORES[a.size];
                    spawn_explosion(a.x, a.y, a.size);
                    if (a.size > 0) {
                        spawn_asteroid(a.x, a.y, a.size - 1);
                        spawn_asteroid(a.x, a.y, a.size - 1);
                        spawn_asteroid(a.x, a.y, a.size - 1);
                    }
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
            plot_line(x0 >> 9, y0 >> 9, x1 >> 9, y1 >> 9, 7);
            x0 = x1;
            y0 = y1;
        }
    }


    // particles
    for (Particle& p : particles) {
        if (p.ttl == 0) continue;
        --p.ttl;
        int x = p.x;
        int y = p.y;
        p.x += p.vx;
        p.y += p.vy;
        plot_line(x >> 8, y >> 8, p.x >> 8, p.y >> 8, p.color);
    }


    if (state != S_HIGH_SCORES) {
        // score
        char str[8];
        itos(str, score);
        render::print(128 - strlen(str) * 6, 1, str);
    }


    if (state == S_LEVEL_START) {
        char str[12] = "LEVEL ";
        itos(str + 6, level);
        if (tick < 40 || (tick & 2)) render::print(43, 32, str);
        if (++tick > 80) {
            state = S_NORMAL;
            tick = 0;
        }
    }
    else if (state == S_GAME_OVER) {
        if (++tick > 63) {
            if ((tick & 63) < 48) render::print(37, 32, "GAME OVER");
            if (new_high_score) {
                render::palette[7] = 10;
                render::print(22, 60, "NEW HIGH SCORE");
                render::palette[7] = 7;


                if (button_just_pressed(fx::BTN_LEFT) && current_letter > 0) --current_letter;
                if (button_just_pressed(fx::BTN_RIGHT) && current_letter < 2) ++current_letter;
                if (button_just_pressed(fx::BTN_UP)) {
                    ++name[current_letter];
                    if      (name[current_letter] == 'Z' + 1) name[current_letter] = '0';
                    else if (name[current_letter] == '9' + 1) name[current_letter] = 'A';
                }
                if (button_just_pressed(fx::BTN_DOWN)) {
                    --name[current_letter];
                    if      (name[current_letter] == 'A' - 1) name[current_letter] = '9';
                    else if (name[current_letter] == '0' - 1) name[current_letter] = 'Z';
                }

                for (int i = 0; i < 3; ++i) {
                    if (i != current_letter || (tick & 4)) {
                        render::draw_glyph(55 + i * 6, 88, name[i]);
                    }
                }

            }
        }
        if (++tick > 127 && (button_just_pressed(fx::BTN_A) | button_just_pressed(fx::BTN_B))) {
            if (new_high_score) {
                high_scores.add(name, score);
                high_scores.save(HIGH_SCORES_FILE);
            }
            state = S_HIGH_SCORES;
            score = 0;
        }
    }
    else if (state == S_HIGH_SCORES) {
        render::palette[7] = 10;
        render::print(37, 16, "ASTEROIDS");
        render::palette[7] = 7;
        for (int i = 0; i < HighScores::LEN; ++i) {
            int y = i * 8 + 1 + 32;
            char str[8];
            itos(str, i + 1);
            int l = i < 9 ? 1 : 2;
            str[l] = '.';
            str[l + 1] = 0;
            render::print(25 - l * 6, y, str);
            itos(str, high_scores.entries[i].score);
            render::print(74 - strlen(str) * 6, y, str);
            render::print(92, y, high_scores.entries[i].name);
        }

        if (button_just_pressed(fx::BTN_A) | button_just_pressed(fx::BTN_B)) {
            init_level(1);
        }

    }
    else if (level_cleared && ++tick > 80) {
        // next level
        init_level(level + 1);
    }
}
