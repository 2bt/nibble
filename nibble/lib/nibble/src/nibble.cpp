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
}

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

void nibble::update() {
	button_bits = ~expander.portRead();
}

bool nibble::button_down(int b) {
    return (button_bits >> b) & 1;
}

void nibble::flush(uint16_t* pixels) {
    tft.pushImage(0, 0, SCREEN_W, SCREEN_H, pixels);
}
