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
    int pos;
    int rot;
    bool drop;
    int stone;
    bool first_stone_frame;

    int stone_permutation[STONE_COUNT];
    int stone_perm_pos;

    int button;
    int button_tick;

    uint8_t grid[GRID_W * GRID_H + 1];


    HighScores high_scores;
    uint8_t    new_high_score;
    uint8_t    current_letter;
    char       name[4];
};
