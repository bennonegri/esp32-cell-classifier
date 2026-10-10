// Hardware I/O test for the cell tester front end.
//
// Power the load from a supply in place of the cell. The PWM sweeps the
// voltage held across RL through params::SWEEP_VOLTS, and after each step
// the terminal shows:
//   Vset   requested voltage across RL, and the PWM duty for it
//   Vload  measured voltage across RL, and current I = Vload / RL
//   Iexp   expected current, Vset / RL
//   Vbat   supply voltage, measured through the divider
// A healthy circuit shows I tracking Iexp.
//
// If the current exceeds params::MAX_CURRENT_A the load is turned off, the
// LED latches red and readings are reported every second until reset.
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <esp_arduino_version.h>

#include "params.h"

using namespace params;

namespace
{
constexpr uint32_t kMaxDuty = (1u << PWM_RESOLUTION_BITS) - 1;

Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);
bool faulted = false;

// The LEDC API is addressed by channel in arduino-esp32 2.x, by pin in 3.x.
void writeDuty(uint32_t duty)
{
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(PWM_PIN, duty);
#else
    ledcWrite(PWM_CHANNEL, duty);
#endif
}

// Average of ADC_SAMPLES calibrated readings, in volts.
float readPinVolts(uint8_t pin)
{
    uint32_t sumMilliVolts = 0;
    for (uint16_t i = 0; i < ADC_SAMPLES; i++)
    {
        sumMilliVolts += analogReadMilliVolts(pin);
    }
    return sumMilliVolts / (ADC_SAMPLES * 1000.0f);
}

float readLoadAmps()
{
    return readPinVolts(VLOAD_PIN) / LOAD_RESISTOR_OHM;
}

float readBatteryVolts()
{
    return readPinVolts(VBAT_PIN) *
           (DIVIDER_R_TOP_OHM + DIVIDER_R_BOTTOM_OHM) / DIVIDER_R_BOTTOM_OHM;
}

void latchFault(float amps)
{
    writeDuty(0);
    faulted = true;
    led.setPixelColor(0, led.Color(255, 0, 0));
    led.show();
    Serial.printf("\nFAULT: %.1f mA > %.0f mA limit. Load off, LED red. "
                  "Reset to clear.\n",
                  amps * 1000.0f, MAX_CURRENT_A * 1000.0f);
}

// Hold the current step for STEP_HOLD_MS, checking for over-current.
// Returns false if a fault was latched.
bool holdStep()
{
    const uint32_t start = millis();
    while (millis() - start < STEP_HOLD_MS)
    {
        const float amps = readLoadAmps();
        if (amps > MAX_CURRENT_A)
        {
            latchFault(amps);
            return false;
        }
        delay(CHECK_INTERVAL_MS);
    }
    return true;
}
}  // namespace

void setup()
{
    // Hold the load off before anything else.
    pinMode(PWM_PIN, OUTPUT);
    digitalWrite(PWM_PIN, LOW);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttachChannel(PWM_PIN, PWM_FREQ_HZ, PWM_RESOLUTION_BITS, PWM_CHANNEL);
#else
    ledcSetup(PWM_CHANNEL, PWM_FREQ_HZ, PWM_RESOLUTION_BITS);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);
#endif
    writeDuty(0);

    Serial.begin(115200);

    // 11 dB attenuation: ~0-2.5 V calibrated input range.
    analogReadResolution(12);
    analogSetPinAttenuation(VLOAD_PIN, ADC_11db);
    analogSetPinAttenuation(VBAT_PIN, ADC_11db);

    led.begin();
    led.setBrightness(LED_BRIGHTNESS);
    led.clear();
    led.show();

    Serial.printf("\nI/O test: PWM GPIO%u, Vload GPIO%u, Vbat GPIO%u, "
                  "RL %.1f ohm, limit %.0f mA\n",
                  PWM_PIN, VLOAD_PIN, VBAT_PIN, LOAD_RESISTOR_OHM,
                  MAX_CURRENT_A * 1000.0f);
}

void loop()
{
    if (faulted)
    {
        // With the load off the current should be ~0. If not, the MOSFET
        // or op-amp is holding the load on.
        Serial.printf("FAULT (load off): I %.1f mA, Vbat %.3f V\n",
                      readLoadAmps() * 1000.0f, readBatteryVolts());
        delay(1000);
        return;
    }

    Serial.println("\n Vset     duty      | Vload    I         Iexp     "
                   " | Vbat");
    for (const float setVolts : SWEEP_VOLTS)
    {
        const uint32_t duty = lroundf(setVolts / GPIO_HIGH_V * kMaxDuty);
        writeDuty(duty);
        if (!holdStep())
        {
            return;
        }

        const float loadVolts = readPinVolts(VLOAD_PIN);
        Serial.printf(" %.3f V  %4lu/%lu | %.3f V  %5.1f mA  %5.1f mA "
                      " | %.3f V\n",
                      setVolts, (unsigned long)duty, (unsigned long)kMaxDuty,
                      loadVolts, loadVolts / LOAD_RESISTOR_OHM * 1000.0f,
                      setVolts / LOAD_RESISTOR_OHM * 1000.0f,
                      readBatteryVolts());
    }
}
