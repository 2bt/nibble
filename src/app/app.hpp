#pragma once


enum {
    G_MENU = -1,
    G_ASTEROIDS,
    G_SNAKE,
    G_WORM,
};


namespace app {
    void init(int game = G_MENU);
    void update();
}
