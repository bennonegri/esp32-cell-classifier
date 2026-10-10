#include "cell_test.h"

#include <Arduino.h>
#include <string.h>

#include "cell_adc.h"
#include "config.h"
#include "load_control.h"
#include "status_led.h"

namespace
{
constexpr uint8_t kWindow = config::OCV_STABLE_WINDOW;

// Seconds between the start of one window and the start of the next.
constexpr float kWindowSpanS =
    kWindow * config::OCV_SAMPLE_INTERVAL_MS / 1000.0f;

float mean(const float *values, uint8_t count)
{
    float sum = 0.0f;
    for (uint8_t i = 0; i < count; i++)
    {
        sum += values[i];
    }
    return sum / count;
}

// 1. Sample the unloaded cell until two consecutive windows agree within
//    OCV_STABLE_RATE_V_PER_S, then take the newest window mean as the OCV.
TestStatus waitForStableOcv(CellTestResult &result)
{
    float history[2 * kWindow];  // oldest first
    uint8_t count = 0;
    const uint32_t start = millis();

    while (true)
    {
        // Latest reading until both windows are full, then the newest mean.
        const float volts = cell_adc::readCellVolts();
        result.ocvVolts = volts;
        result.settleMs = millis() - start;
        if (cell_adc::overRange())
        {
            return TestStatus::AdcOverRange;
        }
        if (volts <= config::CELL_PRESENT_THRESHOLD_V)
        {
            return TestStatus::CellRemoved;
        }

        // Append, dropping the oldest sample once the history is full.
        if (count == 2 * kWindow)
        {
            memmove(history, history + 1, (count - 1) * sizeof(float));
            count--;
        }
        history[count++] = volts;

        if (count == 2 * kWindow)
        {
            const float older = mean(history, kWindow);
            const float newer = mean(history + kWindow, kWindow);
            result.ocvVolts = newer;
            if (fabsf(newer - older) / kWindowSpanS <=
                config::OCV_STABLE_RATE_V_PER_S)
            {
                return TestStatus::Ok;
            }
        }
        if (result.settleMs >= config::OCV_SETTLE_TIMEOUT_MS)
        {
            return TestStatus::OcvNotSettled;
        }
        status_led::wait(config::OCV_SAMPLE_INTERVAL_MS);
    }
}

// Read the loaded cell and RL voltages and check them against the limits.
TestStatus sampleLoad(CellTestResult &result, float setVolts)
{
    result.loadVolts = cell_adc::readLoadVolts();
    result.loadedVolts = cell_adc::readCellVolts();
    result.loadCurrentAmps = result.loadVolts / config::LOAD_RESISTOR_OHM;

    if (cell_adc::overRange())
    {
        return TestStatus::AdcOverRange;
    }
    if (result.loadCurrentAmps >
        result.setCurrentAmps * config::OVER_CURRENT_RATIO)
    {
        return TestStatus::OverCurrent;
    }
    // Below this the MOSFET can't hold the set current, so IR is invalid.
    if (result.loadedVolts < setVolts + config::MIN_LOAD_HEADROOM_V)
    {
        return TestStatus::CellSag;
    }
    return TestStatus::Ok;
}
}  // namespace

CellTestResult runCellTest()
{
    CellTestResult result{};
    result.internalResistanceOhm = NAN;
    load::off();
    cell_adc::clearOverRange();

    result.status = waitForStableOcv(result);
    if (result.status != TestStatus::Ok)
    {
        return result;
    }

    // 2. Load step at a current chosen by chemistry group. Checked every
    //    LOAD_CHECK_INTERVAL_MS, with the final reading at LOAD_TIME_MS.
    result.setCurrentAmps =
        result.ocvVolts > config::LITHIUM_OCV_THRESHOLD_V
            ? config::LITHIUM_LOAD_CURRENT_A
            : config::AA_LOAD_CURRENT_A;
    const float setVolts = result.setCurrentAmps * config::LOAD_RESISTOR_OHM;

    status_led::flash(status_led::YELLOW, config::STATUS_FLASH_HZ);
    load::setVoltage(setVolts);
    const uint32_t start = millis();

    while (result.status == TestStatus::Ok &&
           millis() - start < config::LOAD_TIME_MS)
    {
        status_led::wait(config::LOAD_CHECK_INTERVAL_MS);
        result.status = sampleLoad(result, setVolts);
    }

    if (result.status == TestStatus::Ok)
    {
        result.status = sampleLoad(result, setVolts);
    }
    
    load::off();

    // 3. IR = (OCV - V_loaded) / I, only if the load step completed.
    if (result.status == TestStatus::Ok && result.loadCurrentAmps > 0.0f)
    {
        result.internalResistanceOhm =
            (result.ocvVolts - result.loadedVolts) / result.loadCurrentAmps -
            config::FIXTURE_RESISTANCE_OHM;
    }

    return result;
}

bool isFault(TestStatus status)
{
    return status != TestStatus::Ok && status != TestStatus::CellRemoved;
}

const char *statusName(TestStatus status)
{
    switch (status)
    {
    case TestStatus::Ok:
        return "ok";
    case TestStatus::CellRemoved:
        return "cell removed";
    case TestStatus::OcvNotSettled:
        return "voltage did not settle";
    case TestStatus::AdcOverRange:
        return "ADC over range";
    case TestStatus::CellSag:
        return "cell sagged too low";
    case TestStatus::OverCurrent:
        return "over-current";
    }
    return "unknown";
}
