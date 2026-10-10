#include "cell_classifier.h"

#include <math.h>

#include "config.h"

namespace
{
// Typical resting values for healthy cells. OCV spreads cover the usable
// state of charge. IR is DC resistance at ~1 s; it varies a lot by brand
// and age, so it has a wide spread.
const CellProfile kProfiles[] = {
    // name           OCV    sigma  IR      k     colour
    {"AA Alkaline",   1.50f, 0.12f, 0.150f, 2.0f, {255, 80, 0}},     // orange
    {"AA NiMH",       1.32f, 0.08f, 0.040f, 2.0f, {255, 255, 255}},  // white
    {"14500 Li-ion",  3.80f, 0.25f, 0.150f, 2.0f, {0, 0, 255}},      // blue
    {"14500 LiFePO4", 3.28f, 0.08f, 0.080f, 2.0f, {160, 0, 255}},    // purple
};

// Floor for the log IR term; noise can give ~0 or negative IR.
constexpr float kMinIrOhm = 0.005f;

// Squared normalised distance of a reading from a profile.
float matchScore(const CellProfile &profile, float ocvVolts, float irOhm)
{
    const float ocvZ =
        (ocvVolts - profile.ocvTypicalVolts) / profile.ocvSigmaVolts;
    float score = ocvZ * ocvZ;

    if (!isnan(irOhm))
    {
        const float irZ =
            logf(fmaxf(irOhm, kMinIrOhm) / profile.irTypicalOhm) /
            logf(profile.irSpreadFactor);
        score += config::IR_WEIGHT * irZ * irZ;
    }
    return score;
}
}  // namespace

Classification classifyCell(float ocvVolts, float irOhm)
{
    Classification best = {nullptr, INFINITY};
    for (const CellProfile &profile : kProfiles)
    {
        const float score = matchScore(profile, ocvVolts, irOhm);
        if (score < best.score)
        {
            best = {&profile, score};
        }
    }

    // Keep the score for logging, but report no match if it's too far off.
    if (best.score > config::MAX_MATCH_SCORE)
    {
        best.profile = nullptr;
    }
    return best;
}
