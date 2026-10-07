// Cell measurement sequence: open-circuit voltage, then a load step to
// back-calculate internal resistance (IR).
#pragma once

struct CellTestResult
{
    float ocvVolts;               // open-circuit voltage, load off
    float loadedVolts;            // terminal voltage after LOAD_TIME_MS
    float loadCurrentAmps;        // set-point or measured load current
    float internalResistanceOhm;  // NAN if load current wasn't regulated
};

// Measure OCV, apply the load, measure the loaded voltage and compute IR.
// Blocks for ~LOAD_OFF_SETTLE_MS + LOAD_TIME_MS. Load is off on return.
CellTestResult runCellTest();
