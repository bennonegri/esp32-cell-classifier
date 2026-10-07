#include "cell_adc.h"

#include <Arduino.h>

#include "config.h"

namespace cell_adc
{
namespace
{
constexpr float kDividerGain =
    (config::DIVIDER_R_TOP_OHM + config::DIVIDER_R_BOTTOM_OHM) / config::DIVIDER_R_BOTTOM_OHM;

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
    analogReadResolution(12);
    analogSetPinAttenuation(config::CELL_ADC_PIN, ADC_11db);
    if (config::LOAD_SENSE_ADC_PIN >= 0)
    {
        analogSetPinAttenuation(config::LOAD_SENSE_ADC_PIN, ADC_11db);
    }
}

float readCellVolts()
{
    return averageVolts(config::CELL_ADC_PIN) * kDividerGain;
}

float readLoadSenseVolts()
{
    if (config::LOAD_SENSE_ADC_PIN < 0)
    {
        return NAN;
    }
    return averageVolts(config::LOAD_SENSE_ADC_PIN);
}

}  // namespace cell_adc
