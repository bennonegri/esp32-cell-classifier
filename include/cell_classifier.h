#pragma once

#include "status_led.h"

struct CellProfile
{
    const char *name;
    float ocvTypicalVolts;
    float ocvSigmaVolts;     // 1-sigma spread of OCV across state of charge and brands
    float irTypicalOhm;
    float irSpreadFactor;    // 1-sigma spread of IR as a multiplicative factor (2 = x0.5 .. x2)
    Rgb color;               // pastel colour shown on the LED
};

struct Classification
{
    const CellProfile *profile;  // nullptr if no profile matched well enough
    float score;                 // squared normalised distance to the best profile; lower is better
};

// Pass NAN for irOhm to classify on OCV alone.
Classification classifyCell(float ocvVolts, float irOhm);

extern const Rgb kUnknownCellColor;
