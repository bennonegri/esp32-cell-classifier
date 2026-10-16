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

bool overRangeSeen = false;

// Mean pin voltage in volts. analogReadMilliVolts applies the factory
// eFuse calibration. Flags readings above the calibrated range.
float averageVolts(uint8_t pin)
{
    uint32_t sumMilliVolts = 0;
    for (uint16_t i = 0; i < config::ADC_SAMPLES; i++)
    {
        sumMilliVolts += analogReadMilliVolts(pin);
    }
    const float volts = sumMilliVolts / (config::ADC_SAMPLES * 1000.0f);
    if (volts > config::ADC_MAX_PIN_V)
    {
        overRangeSeen = true;
    }
    return volts;
}
}  // namespace

void begin()
{
    // 11 dB attenuation gives the widest input range (~0-2.5 V calibrated).
    // Vload is normally 0.5-1 V, but the wide range lets a fault current
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

bool overRange()
{
    return overRangeSeen;
}

void clearOverRange()
{
    overRangeSeen = false;
}

}  // namespace cell_adc
