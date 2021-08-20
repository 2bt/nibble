#include <fx.hpp>
#include "game.hpp"
#include <algorithm>



struct Rect {
    int x, y, w, h;
    int left() const { return x; }
    int right() const { return x + w; }
    int top() const { return y; }
    int bottom() const { return y + h; }
    bool contains(int px, int py) const {
        return px >= x && px < right() && py >= y && py < bottom();
    }
    int center_x() const { return x + w / 2; }
    int center_y() const { return y + h / 2; }
    bool overlap(Rect const& r) const {
        return !(x >= r.right() || y >= r.bottom() || right() <= r.x || bottom() <= r.y);
    }
    int overlap_x(Rect const& r) const {
        if (!overlap(r)) return 0;
        int s = r.right() - x;
        int t = r.x - right();
        return abs(s) < abs(t) ? s : t;
    }
    int overlap_y(Rect const& r) const {
        if (!overlap(r)) return 0;
        int s = r.bottom() - y;
        int t = r.y - bottom();
        return abs(s) < abs(t) ? s : t;
    }
};


void draw_rect(bool filled, Rect const& rect, uint8_t color) {
    int x2 = rect.x + rect.w;
    int y2 = rect.y + rect.h;
    if (filled) {
        for (int y = rect.y; y < y2; ++y)
        for (int x = rect.x; x < x2; ++x) fx::pixel(x, y, color);
    }
    else {
        --x2;
        --y2;
        for (int x = rect.x; x <= x2; ++x) {
            fx::pixel(x, rect.y, color);
            fx::pixel(x, y2,     color);
        }
        for (int y = rect.y + 1; y < y2; ++y) {
            fx::pixel(rect.x, y, color);
            fx::pixel(x2,     y, color);
        }
    }
}


int tick;
int x, y;

void game::init() {
}

void game::update() {

    x += fx::button_down(fx::BTN_RIGHT) - fx::button_down(fx::BTN_LEFT);
    y += fx::button_down(fx::BTN_DOWN)  - fx::button_down(fx::BTN_UP);

    ++tick;
    fx::clear(7);
    for (int i = 0; i < 16; ++i) {
        draw_rect(true, {i * 8, 0, 8, 128}, i);
    }


    draw_rect(false, {x, y, 8, 8}, tick & 0xf);
    draw_rect(true, {x + 2, y + 2, 4, 4}, tick & 0xf);
}

