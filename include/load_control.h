#pragma once

// Electronic load driver. The PWM on LOAD_PWM_PIN is RC-filtered to a DC set
// voltage that the op-amp holds across RL, so the load current is
// set voltage / LOAD_RESISTOR_OHM.
namespace load
{

// Call first in setup() so the load is held off from boot.
void begin();

// Set the voltage held across RL, clamped to [0, GPIO_HIGH_VOLTAGE_V].
void setVoltage(float volts);

void off();

}  // namespace load
