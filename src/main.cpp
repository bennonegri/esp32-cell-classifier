#include <Arduino.h>

#include "cell_adc.h"
#include "cell_classifier.h"
#include "cell_test.h"
#include "config.h"
#include "load_control.h"
#include "status_led.h"

namespace
{
bool cellPresent()
{
    return cell_adc::readCellVolts() > config::CELL_PRESENT_THRESHOLD_V;
}

void printResult(const CellTestResult &test, const Classification &match)
{
    Serial.printf("OCV:      %.3f V\n", test.ocvVolts);
    Serial.printf("Loaded:   %.3f V @ %.1f mA\n", test.loadedVolts, test.loadCurrentAmps * 1000.0f);
    if (isnan(test.internalResistanceOhm))
    {
        Serial.println("IR:       n/a (load current not regulated, classifying on OCV only)");
    }
    else
    {
        Serial.printf("IR:       %.1f mOhm\n", test.internalResistanceOhm * 1000.0f);
    }
    Serial.printf("Result:   %s (score %.2f)\n",
                  match.profile ? match.profile->name : "Unknown", match.score);
}
}  // namespace

void setup()
{
    load::begin();  // first, so the load is held off from boot
    Serial.begin(115200);
    cell_adc::begin();
    status_led::begin();

    Serial.println("Cell classifier ready - insert a cell");
}

void loop()
{
    if (!cellPresent())
    {
        delay(200);
        return;
    }

    Serial.println("Cell detected, settling...");
    status_led::show(status_led::OCV_COLOR);
    delay(config::CELL_INSERT_SETTLE_MS);

    const CellTestResult test = runCellTest();
    if (test.ocvVolts <= config::CELL_PRESENT_THRESHOLD_V)
    {
        Serial.println("Cell removed during test");
        status_led::off();
        return;
    }

    const Classification match = classifyCell(test.ocvVolts, test.internalResistanceOhm);
    printResult(test, match);

    status_led::show(match.profile ? match.profile->color : status_led::UNKNOWN_COLOR);
    delay(config::RESULT_DISPLAY_MS);
    status_led::off();

    Serial.println("Remove the cell to test another");
    while (cellPresent())
    {
        delay(200);
    }
}
