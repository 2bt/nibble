#include <nibble.hpp>
#include "fx.hpp"
#include "app/app.hpp"


#include <LittleFS.h>

namespace fx {

uint8_t button_bits;
uint8_t pixels[SCREEN_W * SCREEN_H];


bool try_load(char const* filename, void* data, int len) {
    if (!LittleFS.exists(filename)) return false;
    File file = LittleFS.open(filename, "r");
    if (!file) return false;
    file.readBytes((char*) data, len);
    file.close();
    return true;
}


void store(char const* filename, void const* data, int len) {
    File file = LittleFS.open(filename, "w");
    if(!file) return;
    file.write((char const*) data, len);
    file.close();
}


} // namespace fx

static long my_time;

extern "C" void setup() {
    LittleFS.begin();
    nibble::init();
    app::init();
    my_time = millis();
}

extern "C" void loop() {
    fx::button_bits = nibble::button_bits();
    app::update();
    nibble::flush(fx::pixels);

    long t = my_time;
    my_time = millis();
    int d = t + 16 - my_time;
    if (d > 0) delay(d);
}
