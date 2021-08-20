#include <fx.hpp>
#include "game.hpp"


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


void draw_rect(Rect const& rect, uint8_t color) {
    int x1 = std::max(rect.x, 0);
    int x2 = std::min<int>(rect.x + rect.w, fx::SCREEN_W);
    int y1 = std::max(rect.y, 0);
    int y2 = std::min<int>(rect.y + rect.h, fx::SCREEN_H);
    uint8_t* p = fx::pixels + y1 * fx::SCREEN_W + x1;
    for (int y = y1; y < y2; ++y) {
        uint8_t* q = p;
        for (int x = x1; x < x2; ++x) *q++ = color;
        p += fx::SCREEN_W;
    }
}


int tick = 0;
int x = 0;
int y = 0;

void game::init() {
}

void game::update() {

    x += fx::button_down(fx::BTN_RIGHT) - fx::button_down(fx::BTN_LEFT);
    y += fx::button_down(fx::BTN_DOWN)  - fx::button_down(fx::BTN_UP);

    x += 1;
    x &= 127;

    ++tick;
    // fx::clear(7);
    for (int i = 0; i < 16; ++i) {
        draw_rect({i * 8, 0, 8, 128}, (tick + i) & 0xf);
    }

    draw_rect({x, y, 8, 8}, 8);
}

