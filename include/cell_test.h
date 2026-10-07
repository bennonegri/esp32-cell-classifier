#pragma once

struct CellTestResult
{
    float ocvVolts;               // open-circuit voltage before the load step
    float loadedVolts;            // terminal voltage after LOAD_TIME_MS under load
    float loadCurrentAmps;        // load current (assumed from set point, or measured)
    float internalResistanceOhm;  // NAN if the load current could not be regulated
};

// Runs the OCV / load-step measurement. The load is always off on return.
CellTestResult runCellTest();
