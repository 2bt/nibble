#include <nibble.hpp>
#include "fx.hpp"
#include "game/game.hpp"


namespace fx {

uint8_t button_bits;
uint8_t pixels[SCREEN_W * SCREEN_H];

} // namespace fx

extern "C" void setup() {
    nibble::init();
    game::init();
}

extern "C" void loop() {
    fx::button_bits = nibble::button_bits();
    game::update();
    nibble::flush(fx::pixels);
}
