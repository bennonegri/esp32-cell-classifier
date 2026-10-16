// Cell measurement sequence: wait for a stable open-circuit voltage, then
// apply a load step to back-calculate internal resistance (IR).
#pragma once

#include <stdint.h>

enum class TestStatus
{
    Ok,
    CellRemoved,    // cell fell below CELL_PRESENT_THRESHOLD_V while settling
    OcvNotSettled,  // OCV not stable within OCV_SETTLE_TIMEOUT_MS
    AdcOverRange,   // an ADC pin read above ADC_MAX_PIN_V
    CellSag,        // cell fell below set voltage + MIN_LOAD_HEADROOM_V
    OverCurrent,    // current above set current x OVER_CURRENT_RATIO
};

struct CellTestResult
{
    TestStatus status;
    float ocvVolts;               // stable OCV, or last reading on a fault
    uint32_t settleMs;            // time spent waiting for a stable OCV
    float setCurrentAmps;         // requested load current
    float loadedVolts;            // terminal voltage at the end of the load
    float loadVolts;              // measured voltage across RL
    float loadCurrentAmps;        // loadVolts / LOAD_RESISTOR_OHM
    float internalResistanceOhm;  // NAN unless status is Ok
};

// Wait for a stable OCV (up to OCV_SETTLE_TIMEOUT_MS), then apply the load
// for LOAD_TIME_MS and compute IR. Stops at the first fault. The load is
// off on return.
CellTestResult runCellTest();

// True for statuses that should put the tester in the fault state.
bool isFault(TestStatus status);

const char *statusName(TestStatus status);
