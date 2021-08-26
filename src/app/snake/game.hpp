#pragma once

#include "../util.hpp"


struct SnakeGame {
    void init();
    void update();

    void title_screen();
    void new_game();
    void spawn_apple();

    Random random;

    enum {
        MAX_LEN = 2048,
    };

    enum {
        S_HIGH_SCORES,
        S_NORMAL,
        S_GAME_OVER,
    };

    int      tick;

    uint8_t  snake_x[MAX_LEN];
    uint8_t  snake_y[MAX_LEN];
    uint16_t head;
    uint16_t tail;
    uint16_t len;
    uint16_t target_len;;
    uint8_t  dir;
    uint8_t  step;

    uint8_t  apple_x;
    uint8_t  apple_y;

    uint16_t score;
    uint8_t  state;


    HighScores high_scores;
    uint8_t    new_high_score;
    uint8_t    current_letter;
    char       name[4];
};
