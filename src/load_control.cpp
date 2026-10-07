#include "load_control.h"

#include <Arduino.h>
#include <esp_arduino_version.h>

#include "config.h"

namespace load
{
namespace
{
constexpr uint32_t kMaxDuty = (1u << config::LOAD_PWM_RESOLUTION_BITS) - 1;

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
    // Drive the pin low before the PWM peripheral takes it over.
    pinMode(config::LOAD_PWM_PIN, OUTPUT);
    digitalWrite(config::LOAD_PWM_PIN, LOW);

#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttachChannel(config::LOAD_PWM_PIN, config::LOAD_PWM_FREQ_HZ,
                      config::LOAD_PWM_RESOLUTION_BITS, config::LOAD_PWM_CHANNEL);
#else
    ledcSetup(config::LOAD_PWM_CHANNEL, config::LOAD_PWM_FREQ_HZ, config::LOAD_PWM_RESOLUTION_BITS);
    ledcAttachPin(config::LOAD_PWM_PIN, config::LOAD_PWM_CHANNEL);
#endif

    off();
}

void setVoltage(float volts)
{
    volts = constrain(volts, 0.0f, config::GPIO_HIGH_VOLTAGE_V);
    writeDuty(lroundf(volts / config::GPIO_HIGH_VOLTAGE_V * kMaxDuty));
}

void off()
{
    writeDuty(0);
}

}  // namespace load
