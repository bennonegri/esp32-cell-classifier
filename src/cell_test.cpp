#include "cell_test.h"

#include <Arduino.h>

#include "cell_adc.h"
#include "config.h"
#include "load_control.h"
#include "status_led.h"

namespace
{
// Read the loaded cell and RL voltages and check them against the limits.
CellFault sampleLoad(CellTestResult &result)
{
    result.loadVolts = cell_adc::readLoadVolts();
    result.loadedVolts = cell_adc::readCellVolts();

    if (result.loadVolts > config::MAX_LOAD_VOLTAGE_V)
    {
        return CellFault::OverCurrent;
    }
    // Below this the MOSFET can't hold the set current, so IR is invalid.
    if (result.loadedVolts <
        config::LOAD_SET_VOLTAGE_V + config::MIN_LOAD_HEADROOM_V)
    {
        return CellFault::CellSag;
    }
    return CellFault::None;
}
}  // namespace

CellTestResult runCellTest()
{
    CellTestResult result{};
    result.internalResistanceOhm = NAN;

    // 1. OCV: 0 V on the op-amp (+) input, so the MOSFET is off.
    load::off();
    status_led::wait(config::LOAD_OFF_SETTLE_MS);
    result.ocvVolts = cell_adc::readCellVolts();

    // 2. Load step: hold LOAD_SET_VOLTAGE_V across RL for LOAD_TIME_MS,
    //    checking for faults, then take the final reading at the end.
    status_led::flash(status_led::YELLOW, config::STATUS_FLASH_HZ);
    load::setVoltage(config::LOAD_SET_VOLTAGE_V);
    const uint32_t start = millis();
    while (result.fault == CellFault::None &&
           millis() - start < config::LOAD_TIME_MS)
    {
        status_led::wait(config::LOAD_CHECK_INTERVAL_MS);
        result.fault = sampleLoad(result);
    }
    if (result.fault == CellFault::None)
    {
        result.fault = sampleLoad(result);
    }
    load::off();

    result.loadCurrentAmps = result.loadVolts / config::LOAD_RESISTOR_OHM;

    // 3. IR = (OCV - V_loaded) / I, only if the load step completed.
    if (result.fault == CellFault::None && result.loadCurrentAmps > 0.0f)
    {
        result.internalResistanceOhm =
            (result.ocvVolts - result.loadedVolts) / result.loadCurrentAmps -
            config::FIXTURE_RESISTANCE_OHM;
    }

    return result;
}

const char *faultName(CellFault fault)
{
    switch (fault)
    {
    case CellFault::CellSag:
        return "cell sagged too low";
    case CellFault::OverCurrent:
        return "over-current";
    default:
        return "none";
    }
}
