// Voltage measurements. Each reading averages ADC_SAMPLES calibrated
// samples to reduce noise.
#pragma once

namespace cell_adc
{

// Set ADC resolution and attenuation. Call once in setup().
void begin();

// Cell terminal voltage in volts, scaled back up through the divider.
float readCellVolts();

// Voltage across RL (Vload) in volts. Load current = Vload / RL.
float readLoadVolts();

}  // namespace cell_adc
