#include "status_led.h"

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "config.h"

namespace status_led
{
namespace
{
// Single WS2812 on the dev board; it expects GRB byte order.
Adafruit_NeoPixel led(1, config::LED_PIN, NEO_GRB + NEO_KHZ800);

Rgb currentColor = {0, 0, 0};
uint32_t flashPeriodMs = 0;  // 0 = solid
uint32_t flashStartMs = 0;
bool lit = false;

void write(bool on)
{
    if (on)
    {
        led.setPixelColor(
            0, led.Color(currentColor.r, currentColor.g, currentColor.b));
    }
    else
    {
        led.clear();
    }
    led.show();
    lit = on;
}
}  // namespace

void begin()
{
    led.begin();
    led.setBrightness(config::LED_BRIGHTNESS);
    off();
}

void show(Rgb color)
{
    currentColor = color;
    flashPeriodMs = 0;
    write(true);
}

void flash(Rgb color, float hz)
{
    currentColor = color;
    flashPeriodMs = lroundf(1000.0f / hz);
    flashStartMs = millis();
    write(true);
}

void off()
{
    flashPeriodMs = 0;
    write(false);
}

void update()
{
    if (flashPeriodMs == 0)
    {
        return;
    }
    // On for the first half of each period.
    const bool on =
        (millis() - flashStartMs) % flashPeriodMs < flashPeriodMs / 2;
    if (on != lit)
    {
        write(on);
    }
}

void wait(uint32_t ms)
{
    const uint32_t start = millis();
    while (millis() - start < ms)
    {
        update();
        delay(5);
    }
}

}  // namespace status_led
