// Parameters for the hardware I/O test.
#pragma once

#include <stdint.h>

namespace params
{

// ---- Pins and RC filters (ESP32-C3) ----
// Each analogue line has a first-order RC low-pass, fc = 1 / (2 pi R C).
//
// GPIO_1  PWM -> 47k series, 100nF to GND -> op-amp (+) input.
//         tau 4.7 ms, fc 33.9 Hz. 20 kHz PWM ripple ~4.5 mV p-p at 0.5 V.
// GPIO_2  Vload (top of RL) -> 4.7k series, 100nF to GND -> ADC.
//         tau 0.47 ms, fc 339 Hz.
// GPIO_3  Vbat -> 27k / 39k divider, 100nF from ADC pin to GND.
//         Filter R = 27k || 39k = 16k: tau 1.6 ms, fc 99.8 Hz.
constexpr uint8_t LED_PIN = 8;   // on-board WS2812
constexpr uint8_t PWM_PIN = 1;   // GPIO_1
constexpr uint8_t VLOAD_PIN = 2; // GPIO_2
constexpr uint8_t VBAT_PIN = 3;  // GPIO_3

// ---- Circuit values ----
constexpr float LOAD_RESISTOR_OHM = 10.0f;   // RL
constexpr float DIVIDER_R_TOP_OHM = 27000.0f;
constexpr float DIVIDER_R_BOTTOM_OHM = 39000.0f;

// Vbat calibration: true / reported. From a DMM check at 2.9-3.3 V the
// firmware read 0.53% high, consistent with 1% divider resistor tolerance.
constexpr float VBAT_CAL_GAIN = 0.9947f;

constexpr float GPIO_HIGH_V = 3.3f;  // PWM high level; measure on the board

// ---- PWM ----
constexpr uint32_t PWM_FREQ_HZ = 20000;
constexpr uint8_t PWM_RESOLUTION_BITS = 10;  // 3.2 mV per step
constexpr uint8_t PWM_CHANNEL = 0;           // LEDC channel

// ---- Sweep ----
// Voltages held across RL, each for STEP_HOLD_MS, repeating forever.
constexpr float SWEEP_VOLTS[] = {0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
constexpr uint32_t STEP_HOLD_MS = 5000;

// ---- Over-current ----
// Checked every CHECK_INTERVAL_MS. Above the limit the load is turned off
// and the LED latches red until reset.
constexpr float MAX_CURRENT_A = 0.2f;
constexpr uint32_t CHECK_INTERVAL_MS = 50;

// ---- Misc ----
constexpr uint16_t ADC_SAMPLES = 64;    // averaged per reading
constexpr uint8_t LED_BRIGHTNESS = 64;  // 0-255

}  // namespace params
