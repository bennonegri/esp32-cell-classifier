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

void begin();
void show(Rgb color);
void off();

}  // namespace status_led
