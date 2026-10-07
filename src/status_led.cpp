#include "status_led.h"

#include <Adafruit_NeoPixel.h>

#include "config.h"

namespace status_led
{
namespace
{
Adafruit_NeoPixel led(1, config::LED_PIN, NEO_GRB + NEO_KHZ800);
}

void begin()
{
    led.begin();
    led.setBrightness(config::LED_BRIGHTNESS);
    off();
}

void show(Rgb color)
{
    led.setPixelColor(0, led.Color(color.r, color.g, color.b));
    led.show();
}

void off()
{
    led.clear();
    led.show();
}

}  // namespace status_led
