#include "nibble.hpp"

// #include <CircuitOS.h>
#include <Input/I2cExpander.h>
#include <Audio/Piezo.h>
#include <TFT_eSPI.h>


namespace {

enum {
    BUZZ_PIN = 12,
    BL_PIN   = 8,
};

TFT_eSPI     tft;
uint8_t      button_bits;
I2cExpander  expander;

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

} // namespace

void nibble::init() {
    tft.init();
    tft.writecommand(17); // wakeup command in case display driver is in sleep mode
    tft.invertDisplay(0);
    tft.setRotation(0);

    tft.fillScreen(TFT_BLACK);

    expander.begin(0x74, 4, 5);
    expander.pinMode(BL_PIN, OUTPUT);
    expander.pinWrite(BL_PIN, 1);
    Piezo.begin(BUZZ_PIN);
}

uint8_t nibble::button_bits() { return ~expander.portRead(); }
void    nibble::flush(uint8_t* pixels) { tft.push(pixels, PALETTE); }
