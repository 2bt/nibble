#include <nibble.hpp>
#include "fx.hpp"
#include "game/game.hpp"


namespace fx {
namespace {

constexpr uint16_t RGB(uint32_t rgb) {
    uint32_t r = (rgb >> 16) & 0xff;
    uint32_t g = (rgb >>  8) & 0xff;
    uint32_t b = (rgb >>  0) & 0xff;
    r = r *  0b11111 / 0xff;
    g = g * 0b111111 / 0xff;
    b = b *  0b11111 / 0xff;
    uint16_t color = (r << 11) | (g << 5) | b;
    return (color >> 8) | (color << 8);
}

uint16_t const PALETTE[16] = {
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

uint16_t pixels[SCREEN_W * SCREEN_H];

} // namespace

bool button_down(int b) { return nibble::button_down(b); }

void clear(uint8_t color) {
	for (uint16_t& p : pixels) p = PALETTE[color];
}

void pixel(int x, int y, uint8_t color) {
	if (x < 0 || x >= SCREEN_W) return;
	if (y < 0 || y >= SCREEN_H) return;
	pixels[y * SCREEN_W + x] = PALETTE[color];
}

} // namespace fx

extern "C" void setup() {
    nibble::init();
    game::init();
}

extern "C" void loop() {
    nibble::update();
    game::update();
    nibble::flush(fx::pixels);
}
