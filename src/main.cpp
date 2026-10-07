#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 8
#define NUM_LEDS 1

Adafruit_NeoPixel led(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
    Serial.begin(115200);

    led.begin();
    led.clear();
    led.show();

    Serial.println("Program started");
}

void loop()
{
    static const struct
    {
        const char *name;
        uint32_t color;
    } colors[] = {
        {"GREEN", Adafruit_NeoPixel::Color(0, 255, 0)},
        {"RED", Adafruit_NeoPixel::Color(255, 0, 0)},
        {"BLUE", Adafruit_NeoPixel::Color(0, 0, 255)},
    };
    static size_t index = 0;

    // 1 second period, 50% duty cycle: 500 ms on, 500 ms off
    Serial.printf("LED %s\n", colors[index].name);

    led.setPixelColor(0, colors[index].color);
    led.show();

    delay(500);

    led.clear();
    led.show();

    delay(500);

    index = (index + 1) % (sizeof(colors) / sizeof(colors[0]));
}
