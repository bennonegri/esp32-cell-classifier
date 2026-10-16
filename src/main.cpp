// ESP32-C3 cell classifier.
//
// Waits for a cell, measures its open-circuit voltage and internal
// resistance, classifies the chemistry and shows the state on the LED:
//   green  solid        ready, insert a cell
//   yellow solid        cell detected: waiting for a stable OCV
//   yellow flash 3 Hz   load on, measuring IR
//   result solid        chemistry for RESULT_DISPLAY_MS (see classifier),
//                       or red if no chemistry matched
//   red    flash 5 Hz   fault, load off, until the cell is removed: OCV
//                       did not settle, ADC over range, cell sagged or
//                       over-current
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
    const float settleS = test.settleMs / 1000.0f;
    if (test.status == TestStatus::OcvNotSettled)
    {
        Serial.printf("OCV:      %.3f V (not settled after %.1f s)\n",
                      test.ocvVolts, settleS);
    }
    else if (test.setCurrentAmps == 0.0f)
    {
        // Faulted before the OCV settled; show the last reading.
        Serial.printf("Vcell:    %.3f V\n", test.ocvVolts);
    }
    else
    {
        Serial.printf("OCV:      %.3f V (settled in %.1f s)\n",
                      test.ocvVolts, settleS);
    }
    if (test.setCurrentAmps > 0.0f)
    {
        Serial.printf("Loaded:   %.3f V @ %.1f mA, set %.1f mA "
                      "(Vload %.3f V)\n",
                      test.loadedVolts, test.loadCurrentAmps * 1000.0f,
                      test.setCurrentAmps * 1000.0f, test.loadVolts);
    }
}

// Load off and flash red until the cell is removed.
void faultState(TestStatus status)
{
    load::off();
    Serial.printf("FAULT:    %s. Remove the cell.\n", statusName(status));
    status_led::flash(status_led::RED, config::FAULT_FLASH_HZ);
    waitForRemoval();
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

    Serial.println("Cell detected, waiting for a stable OCV...");
    status_led::show(status_led::YELLOW);

    const CellTestResult test = runCellTest();
    if (test.status == TestStatus::CellRemoved || !cellPresent())
    {
        Serial.println("Cell removed during test");
        return;
    }

    printMeasurements(test);

    if (isFault(test.status))
    {
        faultState(test.status);
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
