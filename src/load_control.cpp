#include "load_control.h"

#include <Arduino.h>
#include <esp_arduino_version.h>

#include "config.h"

namespace load
{
namespace
{
constexpr uint32_t kMaxDuty = (1u << config::LOAD_PWM_RESOLUTION_BITS) - 1;

// The LEDC API is addressed by channel in arduino-esp32 2.x, by pin in 3.x.
void writeDuty(uint32_t duty)
{
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(config::LOAD_PWM_PIN, duty);
#else
    ledcWrite(config::LOAD_PWM_CHANNEL, duty);
#endif
}
}  // namespace

void begin()
{
    // Hold the pin low until the PWM takes over so the load stays off.
    pinMode(config::LOAD_PWM_PIN, OUTPUT);
    digitalWrite(config::LOAD_PWM_PIN, LOW);

#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttachChannel(config::LOAD_PWM_PIN, config::LOAD_PWM_FREQ_HZ,
                      config::LOAD_PWM_RESOLUTION_BITS,
                      config::LOAD_PWM_CHANNEL);
#else
    ledcSetup(config::LOAD_PWM_CHANNEL, config::LOAD_PWM_FREQ_HZ,
              config::LOAD_PWM_RESOLUTION_BITS);
    ledcAttachPin(config::LOAD_PWM_PIN, config::LOAD_PWM_CHANNEL);
#endif

    off();
}

void setVoltage(float volts)
{
    // Filtered PWM voltage = duty fraction * GPIO high level.
    volts = constrain(volts, 0.0f, config::GPIO_HIGH_VOLTAGE_V);
    writeDuty(lroundf(volts / config::GPIO_HIGH_VOLTAGE_V * kMaxDuty));
}

void off()
{
    writeDuty(0);
}

}  // namespace load
