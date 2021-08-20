#include "fx.hpp"
#include "game/game.hpp"


namespace fx {
namespace {

constexpr uint32_t RGB(uint32_t rgb) {
    return ((rgb >> 16) & 0x0000ff)
         | ((rgb      ) & 0x00ff00)
         | ((rgb << 16) & 0xff0000)
         | 0xff000000;
}

uint32_t const PALETTE[16] = {
    RGB(0x000000),
    RGB(0x1D2B53),
    RGB(0x7E2553),
    RGB(0x008751),
    RGB(0xAB5236),
    RGB(0x5F574F),
    RGB(0xC2C3C7),
    RGB(0xFFF1E8),
    RGB(0xFF004D),
    RGB(0xFFA300),
    RGB(0xFFEC27),
    RGB(0x00E436),
    RGB(0x29ADFF),
    RGB(0x83769C),
    RGB(0xFF77A8),
    RGB(0xFFCCAA),
};

uint8_t  button_bits;
uint32_t pixels[SCREEN_W * SCREEN_H];

} // namespace

bool button_down(int b) {
    return (button_bits >> b) & 1;
}
void clear(uint8_t color) {
    for (uint32_t& p : pixels) p = PALETTE[color];
}
void pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= SCREEN_W) return;
    if (y < 0 || y >= SCREEN_H) return;
    pixels[y * SCREEN_W + x] = PALETTE[color];
}

} // namespace fx

#define EXPORT __attribute__((visibility("default"))) extern "C"

EXPORT void init() {
    game::init();
}
EXPORT void update(uint32_t bits) {
    fx::button_bits = bits;
    game::update();
}
EXPORT uint32_t const* pixels() { return fx::pixels; }
