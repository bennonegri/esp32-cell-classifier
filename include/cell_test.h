// Cell measurement sequence: open-circuit voltage, then a load step to
// back-calculate internal resistance (IR).
#pragma once

enum class CellFault
{
    None,
    CellSag,      // cell fell below LOAD_SET_VOLTAGE_V + MIN_LOAD_HEADROOM_V
    OverCurrent,  // Vload rose above MAX_LOAD_VOLTAGE_V
};

struct CellTestResult
{
    float ocvVolts;               // open-circuit voltage, load off
    float loadedVolts;            // terminal voltage at the end of the load
    float loadVolts;              // measured voltage across RL
    float loadCurrentAmps;        // loadVolts / LOAD_RESISTOR_OHM
    float internalResistanceOhm;  // NAN if a fault ended the load step
    CellFault fault;
};

// Measure OCV, apply the load, measure the loaded voltage and compute IR.
// The load is checked every LOAD_CHECK_INTERVAL_MS and stopped early on a
// fault. Blocks for ~LOAD_OFF_SETTLE_MS + LOAD_TIME_MS. Load is off on
// return.
CellTestResult runCellTest();

const char *faultName(CellFault fault);
