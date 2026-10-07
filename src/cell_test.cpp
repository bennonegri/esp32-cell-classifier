#include "cell_test.h"

#include <Arduino.h>

#include "cell_adc.h"
#include "config.h"
#include "load_control.h"

CellTestResult runCellTest()
{
    CellTestResult result{};

    // 1. Load off (0 V at the non-inverting input) and measure OCV.
    load::off();
    delay(config::LOAD_OFF_SETTLE_MS);
    result.ocvVolts = cell_adc::readCellVolts();

    // 2. Hold LOAD_SET_VOLTAGE_V across RL and measure the terminal voltage.
    load::setVoltage(config::LOAD_SET_VOLTAGE_V);
    delay(config::LOAD_TIME_MS);
    result.loadedVolts = cell_adc::readCellVolts();
    const float senseVolts = cell_adc::readLoadSenseVolts();
    load::off();

    const float rlVolts = isnan(senseVolts) ? config::LOAD_SET_VOLTAGE_V : senseVolts;
    result.loadCurrentAmps = rlVolts / config::LOAD_RESISTOR_OHM;

    // 3. Back-calculate internal resistance. If the cell sagged too close to the
    // RL voltage the MOSFET can't regulate, so the assumed current is wrong.
    const bool currentRegulated =
        result.loadedVolts >= config::LOAD_SET_VOLTAGE_V + config::MIN_LOAD_HEADROOM_V;
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
