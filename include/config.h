#pragma once

#include <stdint.h>

// Hardware and measurement parameters for the cell tester. Everything that
// depends on the circuit build lives here so it can be tuned in one place.
namespace config
{

// ---- Pins (ESP32-C3) ----
constexpr uint8_t LED_PIN = 8;          // on-board WS2812 addressable LED
constexpr uint8_t LOAD_PWM_PIN = 1;     // GPIO_1: PWM -> 47k/100nF RC filter -> op-amp (+) input
constexpr uint8_t CELL_ADC_PIN = 2;     // GPIO_2: cell voltage through the divider (ADC1_CH2)
constexpr int LOAD_SENSE_ADC_PIN = -1;  // optional ADC tap on the top of RL to measure the real
                                        // load current; -1 = not fitted (current assumed from set point)

// ---- Cell voltage divider: Vcell -> R_TOP -> ADC pin -> R_BOTTOM -> GND ----
// Keep the ADC pin below ~2.5 V at the highest expected cell voltage (4.2 V);
// that is the calibrated range of the ESP32-C3 ADC at 11 dB attenuation.
constexpr float DIVIDER_R_TOP_OHM = 33000.0f;
constexpr float DIVIDER_R_BOTTOM_OHM = 68000.0f;

// ---- Electronic load (op-amp + MOSFET holding the set voltage across RL) ----
constexpr float LOAD_RESISTOR_OHM = 10.0f;     // RL
constexpr float LOAD_SET_VOLTAGE_V = 0.5f;     // voltage across RL during the load step -> I = V / RL
constexpr float GPIO_HIGH_VOLTAGE_V = 3.3f;    // PWM high level; measure and calibrate on the board
constexpr uint32_t LOAD_PWM_FREQ_HZ = 20000;   // RC corner is 3.4 Hz, so ripple at 20 kHz is ~5 mV p-p
constexpr uint8_t LOAD_PWM_RESOLUTION_BITS = 10;
constexpr uint8_t LOAD_PWM_CHANNEL = 0;        // LEDC channel

// ---- Measurement timing ----
constexpr uint32_t CELL_INSERT_SETTLE_MS = 500;  // let the holder contacts settle after insertion
constexpr uint32_t LOAD_OFF_SETTLE_MS = 100;     // RC filter (tau = 4.7 ms) and op-amp settle before OCV
constexpr uint32_t LOAD_TIME_MS = 1000;          // load applied before reading the terminal voltage
constexpr uint16_t ADC_SAMPLES = 64;             // samples averaged per voltage reading

// ---- Detection and internal resistance ----
constexpr float CELL_PRESENT_THRESHOLD_V = 0.3f;
constexpr float MIN_LOAD_HEADROOM_V = 0.2f;     // loaded cell must stay this far above the RL voltage
                                                // for the MOSFET to regulate the current
constexpr float FIXTURE_RESISTANCE_OHM = 0.0f;  // holder + wiring resistance in the current path,
                                                // subtracted from the measured internal resistance

// ---- Classification ----
constexpr float IR_WEIGHT = 1.0f;          // weight of the internal resistance term vs OCV
constexpr float MAX_MATCH_SCORE = 16.0f;   // worse best match than this (~4 sigma) -> unknown cell

// ---- Result display ----
constexpr uint32_t RESULT_DISPLAY_MS = 10000;
constexpr uint8_t LED_BRIGHTNESS = 64;  // 0-255

}  // namespace config
