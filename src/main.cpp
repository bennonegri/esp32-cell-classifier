// ESP32-C3 cell classifier.
//
// Waits for a cell, measures its open-circuit voltage and internal
// resistance, classifies the chemistry and shows the state on the LED:
//   green  solid        ready, insert a cell
//   yellow solid        cell detected: settling and measuring OCV
//   yellow flash 3 Hz   load on, measuring IR
//   result solid        chemistry for RESULT_DISPLAY_MS (see classifier),
//                       or red if no chemistry matched
//   red    flash 5 Hz   fault: cell sagged or over-current; until removed
//   green  flash 3 Hz   remove the cell to test another
#include <Arduino.h>

#include "cell_adc.h"
#include "cell_classifier.h"
#include "cell_test.h"
#include "config.h"
#include "load_control.h"
#include "status_led.h"

namespace
{
constexpr uint32_t kPollIntervalMs = 200;  // cell insert/removal polling

bool cellPresent()
{
    return cell_adc::readCellVolts() > config::CELL_PRESENT_THRESHOLD_V;
}

// Block until the cell is removed, keeping the LED pattern running.
void waitForRemoval()
{
    while (cellPresent())
    {
        status_led::wait(kPollIntervalMs);
    }
}

void printMeasurements(const CellTestResult &test)
{
    Serial.printf("OCV:      %.3f V\n", test.ocvVolts);
    Serial.printf("Loaded:   %.3f V @ %.1f mA (Vload %.3f V)\n",
                  test.loadedVolts, test.loadCurrentAmps * 1000.0f,
                  test.loadVolts);
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
        status_led::show(status_led::GREEN);
        status_led::wait(kPollIntervalMs);
        return;
    }

    // Let the contacts and cell voltage settle after insertion.
    Serial.println("Cell detected, settling...");
    status_led::show(status_led::YELLOW);
    status_led::wait(config::CELL_INSERT_SETTLE_MS);

    const CellTestResult test = runCellTest();
    if (!cellPresent())
    {
        Serial.println("Cell removed during test");
        return;
    }

    printMeasurements(test);

    if (test.fault != CellFault::None)
    {
        Serial.printf("FAULT:    %s. Remove the cell.\n",
                      faultName(test.fault));
        status_led::flash(status_led::RED, config::FAULT_FLASH_HZ);
        waitForRemoval();
        return;
    }

    const Classification match =
        classifyCell(test.ocvVolts, test.internalResistanceOhm);
    Serial.printf("IR:       %.1f mOhm\n",
                  test.internalResistanceOhm * 1000.0f);
    Serial.printf("Result:   %s (score %.2f)\n",
                  match.profile ? match.profile->name : "Unknown",
                  match.score);

    status_led::show(match.profile ? match.profile->color
                                   : status_led::RED);
    status_led::wait(config::RESULT_DISPLAY_MS);

    // Wait for removal so the same cell isn't tested again.
    Serial.println("Remove the cell to test another");
    status_led::flash(status_led::GREEN, config::STATUS_FLASH_HZ);
    waitForRemoval();
}
