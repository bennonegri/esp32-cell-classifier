#include "cell_test.h"

#include <Arduino.h>

#include "cell_adc.h"
#include "config.h"
#include "load_control.h"
#include "status_led.h"

CellTestResult runCellTest()
{
    CellTestResult result{};

    // 1. OCV: 0 V on the op-amp (+) input, so the MOSFET is off.
    status_led::show(status_led::OCV_COLOR);
    load::off();
    delay(config::LOAD_OFF_SETTLE_MS);
    result.ocvVolts = cell_adc::readCellVolts();

    // 2. Load step: hold LOAD_SET_VOLTAGE_V across RL for LOAD_TIME_MS,
    //    then read the terminal voltage and turn the load straight off.
    status_led::show(status_led::DISCHARGE_COLOR);
    load::setVoltage(config::LOAD_SET_VOLTAGE_V);
    delay(config::LOAD_TIME_MS);
    result.loadedVolts = cell_adc::readCellVolts();
    const float senseVolts = cell_adc::readLoadSenseVolts();
    load::off();

    // Use the measured RL voltage if a sense pin is fitted.
    const float rlVolts =
        isnan(senseVolts) ? config::LOAD_SET_VOLTAGE_V : senseVolts;
    result.loadCurrentAmps = rlVolts / config::LOAD_RESISTOR_OHM;

    // 3. IR = (OCV - V_loaded) / I. Only valid if the cell stayed far enough
    //    above the RL voltage for the MOSFET to hold the set current.
    const bool currentRegulated =
        result.loadedVolts >=
        config::LOAD_SET_VOLTAGE_V + config::MIN_LOAD_HEADROOM_V;
    if (currentRegulated && result.loadCurrentAmps > 0.0f)
    {
        result.internalResistanceOhm =
            (result.ocvVolts - result.loadedVolts) / result.loadCurrentAmps -
            config::FIXTURE_RESISTANCE_OHM;
    }
    else
    {
        result.internalResistanceOhm = NAN;
    }

    return result;
}
