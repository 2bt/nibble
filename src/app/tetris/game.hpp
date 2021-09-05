#pragma once

#include "../util.hpp"


struct TetrisGame {
    void init();
    void update();

    void title_screen();
    void new_game();

    void new_stone();
    bool check_collision();
    void paint_stone(uint8_t color);

    enum {
        GRID_W      = 11,
        GRID_H      = 22,
        STONE_COUNT = 7,
    };

    Random random;

    enum {
        S_HIGH_SCORES,
        S_NORMAL,
        S_GAME_OVER,
    };
    uint8_t  state;
    uint16_t score;
    uint16_t lines;
    int tick;
    int16_t pos;
    uint8_t rot;
    uint8_t stone;
    uint8_t first_stone_frame;

    uint8_t stone_permutation[STONE_COUNT];
    int8_t  stone_perm_pos;

    uint8_t button;
    int button_tick;

    uint8_t grid[GRID_W * GRID_H + 1];


    HighScores high_scores;
    uint8_t    new_high_score;
    uint8_t    current_letter;
    char       name[4];
};
