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


uint8_t const FONT_DATA[] PROGMEM = {
    #include "font_data.inc"
};


namespace render {


inline void clear(uint8_t color) {
    memset(fx::pixels, color, fx::SCREEN_W * fx::SCREEN_H);
}

inline void pixel(int x, int y, uint8_t color) {
    uint32_t q = x | y;
    if (q & ~(fx::SCREEN_W - 1)) return;
    fx::pixels[y * fx::SCREEN_W + x] = color;
}

uint8_t alpha = 0;

template <bool TRANSPARENT = true, bool CLIP = true>
void draw_sprite(Rect const& rect, int x, int y) {

    int sx = rect.x;
    int sy = rect.y;
    int x2 = x + rect.w;
    int y2 = y + rect.h;

    if (CLIP) {
        if (x < 0) {
            sx -= x;
            x = 0;
        }
        if (y < 0) {
            sy -= y;
            y = 0;
        }
        if (x2 > fx::SCREEN_W) x2 = fx::SCREEN_W;
        if (y2 > fx::SCREEN_H) y2 = fx::SCREEN_H;
    }

    uint8_t const* s = FONT_DATA + sy * 128 + sx;
    uint8_t*       p = fx::pixels + y * fx::SCREEN_W + x;

    for (int j = y; j < y2; ++j) {

        uint8_t const* t = s;
        uint8_t*       q = p;

        for (int i = x; i < x2; ++i) {
            uint8_t c = pgm_read_byte(t++);
            if (TRANSPARENT) {
                if (c != alpha) *q = c;
            }
            else *q = c;
            ++q;
        }

        s += 128;
        p += fx::SCREEN_W;
    }
}

void print(int x, int y, char const* str) {
    for (char c; (c = *str++);) {
        draw_sprite({(c % 16) << 3, (8 + c / 16) << 3, 6, 8}, x, y);
        x += 6;
    }
}


void fill_rect(Rect const& rect, uint8_t color) {
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


} // namespace render


bool button_down(int b) { return (fx::button_bits >> b) & 1; }


int tick = 0;
int x = 32;
int y = 32;

void game::init() {
}

void game::update() {

    x += button_down(fx::BTN_RIGHT) - button_down(fx::BTN_LEFT);
    y += button_down(fx::BTN_DOWN)  - button_down(fx::BTN_UP);


    ++tick;
    render::clear(0);
    for (int i = 0; i < 16; ++i) {
        render::fill_rect({i * 8, 0, 8, 128}, (tick + i) & 0xf);
    }

    render::print(x, y +  0, "BLONK");
    render::print(x, y +  8, "INVADERZ");
    render::print(x, y + 16, "SNAKE");
    render::print(x, y + 24, "SUPER MARIO");
}

