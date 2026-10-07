// Build-time settings for the cell tester. Everything that depends on the
// circuit or the test procedure lives here so it can be tuned in one place.
#pragma once

#include <stdint.h>

namespace config
{

// ---- Pins (ESP32-C3) ----

// On-board WS2812 addressable LED.
constexpr uint8_t LED_PIN = 8;

// GPIO_1: PWM output. A 47k/100nF RC filter turns it into a DC voltage on
// the op-amp (+) input, which sets the voltage held across RL.
constexpr uint8_t LOAD_PWM_PIN = 1;

// GPIO_2: cell voltage through the resistor divider (ADC1 channel 2).
constexpr uint8_t CELL_ADC_PIN = 2;

// Optional ADC pin wired to the top of RL to measure the real load current.
// -1 = not fitted; the current is then assumed from LOAD_SET_VOLTAGE_V.
constexpr int LOAD_SENSE_ADC_PIN = -1;

// ---- Cell voltage divider ----
// Vcell -> R_TOP -> ADC pin -> R_BOTTOM -> GND.
// The ESP32-C3 ADC is only calibrated up to ~2.5 V (11 dB attenuation), so
// keep the pin below that at 4.2 V. 33k/68k gives 2.83 V; 47k/56k gives
// 2.28 V and is recommended.
constexpr float DIVIDER_R_TOP_OHM = 33000.0f;
constexpr float DIVIDER_R_BOTTOM_OHM = 68000.0f;

// ---- Electronic load ----
// The op-amp drives the MOSFET to hold the set voltage across RL, so the
// load current is LOAD_SET_VOLTAGE_V / LOAD_RESISTOR_OHM (0.5/10 = 50 mA).
constexpr float LOAD_RESISTOR_OHM = 10.0f;   // RL
constexpr float LOAD_SET_VOLTAGE_V = 0.5f;   // voltage across RL under load

// PWM high level. Measure on the board; it sets the duty-to-volts scale.
constexpr float GPIO_HIGH_VOLTAGE_V = 3.3f;

// RC corner is 3.4 Hz, so 20 kHz leaves ~5 mV p-p ripple at the op-amp.
constexpr uint32_t LOAD_PWM_FREQ_HZ = 20000;
constexpr uint8_t LOAD_PWM_RESOLUTION_BITS = 10;  // 3.2 mV per step
constexpr uint8_t LOAD_PWM_CHANNEL = 0;           // LEDC channel

// ---- Measurement timing ----
constexpr uint32_t CELL_INSERT_SETTLE_MS = 5000;  // contacts + cell settle
constexpr uint32_t LOAD_OFF_SETTLE_MS = 100;      // RC (tau 4.7 ms) settle
constexpr uint32_t LOAD_TIME_MS = 1000;           // load on before reading
constexpr uint16_t ADC_SAMPLES = 64;              // averaged per reading

// ---- Detection and internal resistance ----

// Above this the divider sees a cell; with no cell it reads ~0 V.
constexpr float CELL_PRESENT_THRESHOLD_V = 0.3f;

// The loaded cell must stay this far above the RL voltage for the MOSFET to
// regulate. Below it the current is unknown and IR is not reported.
constexpr float MIN_LOAD_HEADROOM_V = 0.2f;

// Holder and wiring resistance in the current path. It is measured along
// with the cell, so it is subtracted from the computed IR.
constexpr float FIXTURE_RESISTANCE_OHM = 0.0f;

// ---- Classification ----

// Weight of the IR term relative to OCV. Lower it if IR readings are noisy.
constexpr float IR_WEIGHT = 1.0f;

// Best match score above this (~4 sigma) is reported as an unknown cell.
constexpr float MAX_MATCH_SCORE = 16.0f;

// ---- Result display ----
constexpr uint32_t RESULT_DISPLAY_MS = 10000;  // result colour on time
constexpr uint8_t LED_BRIGHTNESS = 64;         // 0-255, scales all colours

}  // namespace config
