// On-board addressable LED: solid or flashing (50% duty) status colours.
//
// Flashing is timed from millis(), so it only advances while update() is
// called. Use wait() instead of delay() wherever the LED may be flashing.
#pragma once

#include <stdint.h>

struct Rgb
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

namespace status_led
{

// Status colours. Result colours per chemistry are in cell_classifier.
constexpr Rgb GREEN = {0, 255, 0};     // ready (solid) / remove cell (flash)
constexpr Rgb YELLOW = {255, 200, 0};  // settle + OCV (solid) / load (flash)
constexpr Rgb RED = {255, 0, 0};       // unknown cell (solid) / fault (flash)

// Initialise the LED at LED_BRIGHTNESS and turn it off.
void begin();

// Show a solid colour, scaled by LED_BRIGHTNESS.
void show(Rgb color);

// Flash a colour at hz with a 50% duty cycle, starting on.
void flash(Rgb color, float hz);

void off();

// Advance the flash pattern. Call at least every ~10 ms while flashing.
void update();

// delay() that keeps the LED flashing.
void wait(uint32_t ms);

}  // namespace status_led
