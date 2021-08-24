#pragma once

#include "../util.hpp"


struct WormGame {
    void init();
    void update();


    int tick;


    int     x;
    int     y;
    uint8_t ang;
};
