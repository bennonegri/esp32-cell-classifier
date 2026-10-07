// Voltage measurements. Each reading averages ADC_SAMPLES calibrated
// samples to reduce noise.
#pragma once

namespace cell_adc
{

// Set ADC resolution and attenuation. Call once in setup().
void begin();

// Cell terminal voltage in volts, scaled back up through the divider.
float readCellVolts();

// Voltage across RL in volts, or NAN when LOAD_SENSE_ADC_PIN is not fitted.
float readLoadSenseVolts();

}  // namespace cell_adc
