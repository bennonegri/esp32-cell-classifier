// Build-time settings for the cell tester. Everything that depends on the
// circuit or the test procedure lives here so it can be tuned in one place.
#pragma once

#include <stdint.h>

namespace config
{

// ---- Pins and RC filters (ESP32-C3) ----
// Each analogue line has a first-order RC low-pass, fc = 1 / (2 pi R C).
//
// GPIO_1  PWM -> 47k series, 100nF to GND -> op-amp (+) input.
//         tau 4.7 ms, fc 33.9 Hz. At 20 kHz the ripple is ~4.5 mV p-p at
//         0.5 V out; a step settles to 0.1% in ~33 ms (7 tau).
// GPIO_2  Vload (top of RL) -> 4.7k series, 100nF to GND -> ADC.
//         tau 0.47 ms, fc 339 Hz.
// GPIO_3  Vcell -> 27k / 39k divider, 100nF from ADC pin to GND.
//         Filter R is the divider's Thevenin resistance, 27k || 39k = 16k:
//         tau 1.6 ms, fc 99.8 Hz.
// All three settle far faster than the 1 s load step.

constexpr uint8_t LED_PIN = 8;        // on-board WS2812 addressable LED
constexpr uint8_t LOAD_PWM_PIN = 1;   // GPIO_1: sets voltage across RL
constexpr uint8_t LOAD_ADC_PIN = 2;   // GPIO_2: Vload (ADC1 channel 2)
constexpr uint8_t CELL_ADC_PIN = 3;   // GPIO_3: Vcell (ADC1 channel 3)

// ---- Cell voltage divider ----
// Vcell -> R_TOP -> ADC pin -> R_BOTTOM -> GND. Ratio 39/66 = 0.591.
// The ESP32-C3 ADC is calibrated to ~2.5 V (11 dB attenuation). 4.2 V gives
// 2.48 V here: no margin, and resistor tolerance can push it over. 33k top
// (2.28 V) or 39k top / 27k bottom (1.72 V) would leave headroom.
constexpr float DIVIDER_R_TOP_OHM = 27000.0f;
constexpr float DIVIDER_R_BOTTOM_OHM = 39000.0f;

// Vcell calibration: true / reported. From a DMM check at 2.9-3.3 V the
// firmware read 0.53% high, consistent with 1% divider resistor tolerance.
constexpr float CELL_CAL_GAIN = 0.9947f;

// ---- Electronic load ----
// The op-amp drives the MOSFET to hold the set voltage across RL, so the
// load current is LOAD_SET_VOLTAGE_V / LOAD_RESISTOR_OHM (0.5/10 = 50 mA).
constexpr float LOAD_RESISTOR_OHM = 10.0f;   // RL
constexpr float LOAD_SET_VOLTAGE_V = 0.5f;   // voltage across RL under load

// PWM high level. Measure on the board; it sets the duty-to-volts scale.
constexpr float GPIO_HIGH_VOLTAGE_V = 3.3f;

// ~590x above the 33.9 Hz RC corner; see the GPIO_1 filter notes above.
constexpr uint32_t LOAD_PWM_FREQ_HZ = 20000;
constexpr uint8_t LOAD_PWM_RESOLUTION_BITS = 10;  // 3.2 mV per step
constexpr uint8_t LOAD_PWM_CHANNEL = 0;           // LEDC channel

// ---- Measurement timing ----
constexpr uint32_t CELL_INSERT_SETTLE_MS = 5000;  // contacts + cell settle
constexpr uint32_t LOAD_OFF_SETTLE_MS = 100;      // RC filters settle
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
