// Electronic load driver.
//
// LOAD_PWM_PIN is RC-filtered to a DC voltage on the op-amp (+) input. The
// op-amp drives the MOSFET so the same voltage appears across RL, giving a
// load current of set voltage / LOAD_RESISTOR_OHM drawn from the cell.
#pragma once

namespace load
{

// Configure the PWM with the load off. Call first in setup().
void begin();

// Set the voltage held across RL, clamped to 0..GPIO_HIGH_VOLTAGE_V.
void setVoltage(float volts);

// Set 0 V across RL so no current is drawn.
void off();

}  // namespace load
