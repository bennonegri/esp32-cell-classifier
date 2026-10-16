// Cell chemistry classification from OCV and internal resistance (IR).
//
// Each chemistry has a typical OCV and IR with a 1-sigma spread. A reading
// is scored by its squared normalised distance from each profile:
//   score = ((OCV - typ) / sigma)^2 + IR_WEIGHT * (log(IR / typ) / log(k))^2
// IR is compared on a log scale because it varies by multiples, not offsets.
// The lowest score wins; above MAX_MATCH_SCORE the cell is unknown.
#pragma once

#include "status_led.h"

struct CellProfile
{
    const char *name;
    float ocvTypicalVolts;
    float ocvSigmaVolts;   // 1-sigma OCV spread across charge and brands
    float irTypicalOhm;
    float irSpreadFactor;  // 1-sigma IR spread k: 2 means typ/2 .. typ*2
    Rgb color;             // result colour for the LED
};

struct Classification
{
    const CellProfile *profile;  // best match, or nullptr if unknown
    float score;                 // best match score; lower is closer
};

// Classify a cell. Pass NAN for irOhm to classify on OCV alone.
Classification classifyCell(float ocvVolts, float irOhm);
