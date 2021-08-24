#pragma once


struct WormGame {
    int tick;

    void init() {
        tick = 0;
    }
    void update() {
        render::clear(tick++ & 0xf);
    }
};
