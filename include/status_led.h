#pragma once

#include <stdint.h>

struct Rgb
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// On-board addressable LED.
namespace status_led
{

constexpr Rgb OCV_COLOR = {255, 255, 255};        // white: settling and measuring OCV
constexpr Rgb DISCHARGE_COLOR = {255, 200, 0};    // bright yellow: load step applied
constexpr Rgb UNKNOWN_COLOR = {255, 0, 0};        // bright red: no chemistry matched

void begin();
void show(Rgb color);
void off();

}  // namespace status_led
