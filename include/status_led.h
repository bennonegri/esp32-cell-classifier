// On-board addressable LED and the colours used to show test progress.
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

// Test phase colours. Result colours per chemistry are in cell_classifier.
constexpr Rgb OCV_COLOR = {255, 255, 255};      // white: settle + OCV
constexpr Rgb DISCHARGE_COLOR = {255, 200, 0};  // bright yellow: load on
constexpr Rgb UNKNOWN_COLOR = {255, 0, 0};      // bright red: no match

// Initialise the LED at LED_BRIGHTNESS and turn it off.
void begin();

// Show a colour, scaled by LED_BRIGHTNESS.
void show(Rgb color);

void off();

}  // namespace status_led
