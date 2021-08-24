#include <nibble.hpp>
#include "fx.hpp"
#include "app/app.hpp"


namespace fx {

uint8_t button_bits;
uint8_t pixels[SCREEN_W * SCREEN_H];

} // namespace fx

extern "C" void setup() {
    nibble::init();
    app::init();
}

extern "C" void loop() {
    fx::button_bits = nibble::button_bits();
    app::update();
    nibble::flush(fx::pixels);
}
