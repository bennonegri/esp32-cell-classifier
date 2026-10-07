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
    Serial.println("LED ON");

    led.setPixelColor(0, led.Color(255, 0, 0)); // Red
    led.show();

    delay(500);

    Serial.println("LED OFF");

    led.clear();
    led.show();

    delay(500);
}