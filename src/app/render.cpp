#include "render.hpp"


namespace render {
namespace {

uint8_t const FONT_DATA[] PROGMEM = {
    #include "font_data.inc"
};

} // namespace


uint8_t alpha = 0;
uint8_t palette[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};


template <bool TRANSPARENT, bool CLIP>
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
                if (c != alpha) *q = palette[c];
            }
            else *q = palette[c];
            ++q;
        }

        s += 128;
        p += fx::SCREEN_W;
    }
}

template void draw_sprite(Rect const& rect, int x, int y);

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
