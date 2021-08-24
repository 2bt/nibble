#pragma once

#include "util.hpp"


namespace render {


inline void clear(uint8_t color) {
    memset(fx::pixels, color, fx::SCREEN_W * fx::SCREEN_H);
}

inline void pixel(int x, int y, uint8_t color) {
    uint32_t q = x | y;
    if (q & ~(fx::SCREEN_W - 1)) return;
    fx::pixels[y * fx::SCREEN_W + x] = color;
}


extern uint8_t alpha;
extern uint8_t palette[16];

template <bool TRANSPARENT = true, bool CLIP = true> void draw_sprite(Rect const& rect, int x, int y);
extern template void draw_sprite(Rect const& rect, int x, int y);


inline void draw_glyph(int x, int y, char c) {
    draw_sprite({(c % 16) << 3, (8 + c / 16) << 3, 6, 8}, x, y);
}
inline void print(int x, int y, char const* str) {
    for (char c; (c = *str++);) {
        draw_glyph(x, y, c);
        x += 6;
    }
}

void fill_rect(Rect const& rect, uint8_t color);


} // namespace render
