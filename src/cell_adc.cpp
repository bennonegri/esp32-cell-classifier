#include "cell_adc.h"

#include <Arduino.h>

#include "config.h"

namespace cell_adc
{
namespace
{
// Cell voltage = ADC pin voltage * (R_TOP + R_BOTTOM) / R_BOTTOM,
// trimmed by the measured calibration factor.
constexpr float kDividerGain =
    config::CELL_CAL_GAIN *
    (config::DIVIDER_R_TOP_OHM + config::DIVIDER_R_BOTTOM_OHM) /
    config::DIVIDER_R_BOTTOM_OHM;

// Mean pin voltage in volts. analogReadMilliVolts applies the factory
// eFuse calibration, so no manual offset/gain correction is needed.
float averageVolts(uint8_t pin)
{
    uint32_t sumMilliVolts = 0;
    for (uint16_t i = 0; i < config::ADC_SAMPLES; i++)
    {
        sumMilliVolts += analogReadMilliVolts(pin);
    }
    return sumMilliVolts / (config::ADC_SAMPLES * 1000.0f);
}
}  // namespace

void begin()
{
    // 11 dB attenuation gives the widest input range (~0-2.5 V calibrated).
    // Vload is normally ~0.5 V, but the wide range lets a fault current
    // (up to 0.25 A at 10 ohm) still be measured.
    analogReadResolution(12);
    analogSetPinAttenuation(config::CELL_ADC_PIN, ADC_11db);
    analogSetPinAttenuation(config::LOAD_ADC_PIN, ADC_11db);
}

float readCellVolts()
{
    return averageVolts(config::CELL_ADC_PIN) * kDividerGain;
}

float readLoadVolts()
{
    return averageVolts(config::LOAD_ADC_PIN);
}

}  // namespace cell_adc
