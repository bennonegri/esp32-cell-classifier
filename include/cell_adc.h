#pragma once

// Calibrated, averaged ADC readings of the cell and load sense voltages.
namespace cell_adc
{

void begin();

// Cell terminal voltage in volts, corrected for the resistor divider.
float readCellVolts();

// Voltage across RL in volts, or NAN when no sense pin is fitted.
float readLoadSenseVolts();

}  // namespace cell_adc
