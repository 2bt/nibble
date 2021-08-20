#include <nibble.hpp>
#include "fx.hpp"
#include "game/game.hpp"


namespace fx {

uint8_t pixels[SCREEN_W * SCREEN_H];

bool button_down(int b) { return nibble::button_down(b); }

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
