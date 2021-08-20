#include <nibble.hpp>
#include "fx.hpp"
#include "game/game.hpp"
#include <cstring>


namespace fx {
namespace {

uint8_t pixels[SCREEN_W * SCREEN_H];

} // namespace

bool button_down(int b) { return nibble::button_down(b); }

void clear(uint8_t color) {
    memset(pixels, color, sizeof(pixels));
}

void pixel(int x, int y, uint8_t color) {
    uint32_t q = x | y;
    if (q & ~127) return;
    pixels[y * SCREEN_W + x] = color;
}

uint8_t* pixel_data() { return pixels; }

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
