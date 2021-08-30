#include "game.hpp"
#include "../render.hpp"


#define HIGH_SCORES_FILE "/snake"


void SnakeGame::init() {

    if (!high_scores.load(HIGH_SCORES_FILE)) {
        high_scores.reset();
        for (int i = HighScores::LEN; i > 0; --i) high_scores.add("...", i * 10);
        high_scores.save(HIGH_SCORES_FILE);
    }
    name[0] = 'A';
    name[1] = 'A';
    name[2] = 'A';

    title_screen();
}

void SnakeGame::title_screen() {
    new_game();
    state = S_HIGH_SCORES;
}

void SnakeGame::new_game() {
    state = S_NORMAL;
    tick = 0;

    step = 0;
    score = 0;
    dir  = 3;
    tail = 0;
    head = 1;
    len  = 1;
    target_len = 16;
    snake_x[head] = 63;
    snake_y[head] = 126;

    spawn_apple();

    current_letter = 0;
    new_high_score = false;
}

void SnakeGame::spawn_apple() {
    for (int _ = 0; _ < 10; ++_) {
        apple_x = random.rand() & 0x7c;
        apple_y = random.rand() & 0x7c;

        int x0 = apple_x - 1;
        int y0 = apple_y - 1;
        int x1 = x0 + 8;
        int y1 = y0 + 8;

        bool collision = false;
        for (int i = head; i != tail; i = (i - 1) & (MAX_LEN - 1)) {
            int x = snake_x[i];
            int y = snake_y[i];
            if (x >= x0 && x < x1 && y >= y0 && y < y1) {
                collision = true;
                break;
            }
        }
        if (!collision) break;
    }
}

void SnakeGame::update() {

    if (state == S_NORMAL || state == S_HIGH_SCORES) {

        if (state == S_NORMAL) {
            if (dir != 2 && button_just_pressed(fx::BTN_LEFT))  dir = 0;
            if (dir != 0 && button_just_pressed(fx::BTN_RIGHT)) dir = 2;
            if (dir != 3 && button_just_pressed(fx::BTN_DOWN))  dir = 1;
            if (dir != 1 && button_just_pressed(fx::BTN_UP))    dir = 3;
        }
        else {
            if (--tick <= 0) {
                tick = target_len + (random.rand() & 31);
                int r = (random.rand() * 7 >> 4) & 3;
                if (dir != 2 && r == 1) dir = 0;
                if (dir != 0 && r == 2) dir = 2;
                if (dir != 3 && r == 3) dir = 1;
                if (dir != 1 && r == 4) dir = 3;
            }
        }

        if (++step >= 3) step = 0;
        if (step < 2) {
            int h = head;
            head = (head + 1) & (MAX_LEN - 1);
            if (target_len > len) ++len;
            else tail = (tail + 1) & (MAX_LEN - 1);
            int8_t const DIRS[] = { -1, 0, 1, 0, -1 };
            int x = snake_x[head] = (snake_x[h] + DIRS[dir    ]) & 127;
            int y = snake_y[head] = (snake_y[h] + DIRS[dir + 1]) & 127;

            if (fx::pixels[y * 128 + x] == 11) {
                state = S_GAME_OVER;
                new_high_score = high_scores.is_high_score(score);
            }


            int x0 = apple_x - 1;
            int y0 = apple_y - 1;
            if (x0 > x) x0 -= 128;
            if (y0 > y) y0 -= 128;

            int x1 = x0 + 8;
            int y1 = y0 + 8;
            if (x >= x0 && x < x1 && y >= y0 && y < y1) {
                target_len += 8;
                score += 1;
                spawn_apple();
            }
        }
    }


    // draw
    render::clear(0);

    if (state != S_HIGH_SCORES) {
        char str[8];
        itos(str, score);
        render::print(1, 1, str);
    }


    for (int y = 0; y < 6; ++y)
    for (int x = 0; x < 6; ++x) {
        if ((x == 0 || x == 5) && (y == 0 || y == 5)) continue;
        render::pixel((apple_x + x) & 127,
                      (apple_y + y) & 127,
                      8);
    }
    render::pixel((apple_x + 1) & 127,
                  (apple_y + 1) & 127,
                  7);


    for (int i = head; i != tail; i = (i - 1) & (MAX_LEN - 1)) {
        uint8_t x0 = snake_x[i];
        uint8_t y0 = snake_y[i];
        uint8_t x1 = (x0 - 1) & 127;
        uint8_t y1 = (y0 - 1) & 127;
        uint8_t x2 = (x0 + 1) & 127;
        uint8_t y2 = (y0 + 1) & 127;

        uint8_t c = ((i - head) & 3) == 0 ? 3 : 11;

        fx::pixels[y0 * 128 + x1] = c;
        fx::pixels[y0 * 128 + x2] = c;
        fx::pixels[y1 * 128 + x0] = c;
        fx::pixels[y2 * 128 + x0] = c;
    }



    if (state == S_GAME_OVER) {
        ++tick;
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
        if (++tick > 127 && (button_just_pressed(fx::BTN_A) | button_just_pressed(fx::BTN_B))) {
            if (new_high_score) {
                high_scores.add(name, score);
                high_scores.save(HIGH_SCORES_FILE);
            }

            title_screen();
        }
    }
    else if (state == S_HIGH_SCORES) {
        render::palette[7] = 10;
        render::print(49, 16, "SNAKE");
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
            new_game();
        }
    }
}
