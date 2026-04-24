/*
 * Stellaris LM4F120 Bi-Motor Control with PWM (Brushed motors with relay)
 * Adapted from ESP32 version for TM4C123G LaunchPad
 *
 * This sketch converts 2 PWM signals from ArduPilot to appropriate signals
 * for brushed motor controllers with directional relay control.
 *
 * Features:
 * - 2 PWM inputs (from ArduPilot flight controller)
 * - 2 PWM outputs (to motor controller)
 * - 2 Forward/Reverse direction pins
 * - COM9 @ 115200 baud serial communication
 *
 * Board: Stellaris LM4F120 (TM4C123G) LaunchPad
 */

// ===== SYSTEM INCLUDES =====
#include "energia_compat.h" // Must come before Arduino.h
#include <Arduino.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

// ===== LED PINS =====
#define LED_RED PF_1   // Red LED (pin 33 on board)
#define LED_BLUE PF_2  // Blue LED (pin 32 on board)
#define LED_GREEN PF_3 // Green LED (pin 31 on board)

// ===== REVERSAL PINS (Emergency stop / mode change) =====
#define REVERSAL_IN PF_0 // SW2 button (pin 34 on board) - input from controller
#define REVERSAL_OUT PA_6 // Output to indicate reversal mode

// ===== MOTOR 1 PINS (Left motor) =====
#define PROP_IN_PIN1 PB_6 // PWM input from ArduPilot (PE4 can also receive PWM)
#define PWM_PROP_PIN1 PE_4 // PWM output to motor (M0PWM6)
#define FORWARD_PIN1 PD_0  // Direction control - forward
#define STOP_PIN1 PA_2     // Stop/brake line (spare)
#define BRAKE_PIN1 PA_4    // Brake line (spare)

// ===== MOTOR 2 PINS (Right motor) =====
#define PROP_IN_PIN2 PB_7 // PWM input from ArduPilot (PE5 can also receive PWM)
#define PWM_PROP_PIN2 PE_5 // PWM output to motor (M0PWM7)
#define FORWARD_PIN2 PD_1  // Direction control - forward
#define STOP_PIN2 PA_3     // Stop/brake line (spare)
#define BRAKE_PIN2 PA_5    // Brake line (spare)

// ===== GLOBAL VARIABLES =====
long PropSignal1 = 0, PropulsionValue1 = 0;
long PropSignal2 = 0, PropulsionValue2 = 0;
volatile unsigned long pulseStart1 = 0, pulseStart2 = 0;
volatile long pulseDuration1 = 0, pulseDuration2 = 0;

// ===== PWM MEASUREMENT CONSTANTS =====
// User Requirements:
// < 1500µs = Reverse
// > 1500µs = Forward
// < 900µs or > 2000µs = Out of range
#define SIGNAL_MIN 900     // Minimum valid signal (µs)
#define SIGNAL_MAX 2000    // Maximum valid signal (µs)
#define NEUTRAL_POINT 1500 // Neutral/stop point (µs)
#define REVERSE_MIN 900    // Minimum reverse signal
#define FORWARD_MAX 2000   // Maximum forward signal

// ===== FUNCTION PROTOTYPES =====
void interruptHandler1();
void interruptHandler2();
void processPropulsionMotor1();
void processPropulsionMotor2();

// ===== SETUP =====
void setup() {
  // Initialize serial communication @ 115200 baud
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n========================================");
  Serial.println("Stellaris LM4F120 Bi-Motor PWM Controller");
  Serial.println("========================================\n");

  // ===== REVERSAL CONTROL =====
  pinMode(REVERSAL_IN, INPUT_PULLUP); // Input with internal pull-up
  pinMode(REVERSAL_OUT, OUTPUT);
  digitalWrite(REVERSAL_OUT, LOW);

  // ===== STATUS LED =====
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

  // ===== MOTOR 1 INITIALIZATION =====
  pinMode(PROP_IN_PIN1, INPUT);   // PWM input
  pinMode(PWM_PROP_PIN1, OUTPUT); // PWM output
  pinMode(FORWARD_PIN1, OUTPUT);  // Direction
  pinMode(STOP_PIN1, OUTPUT);     // Stop (unused currently)
  pinMode(BRAKE_PIN1, OUTPUT);    // Brake (unused currently)

  // Initialize motor 1 pins to safe state
  digitalWrite(FORWARD_PIN1, LOW);
  digitalWrite(STOP_PIN1, LOW);
  digitalWrite(BRAKE_PIN1, LOW);
  analogWrite(PWM_PROP_PIN1, 0);

  // Attach interrupt for PWM measurement on motor 1
  attachInterrupt(digitalPinToInterrupt(PROP_IN_PIN1), interruptHandler1,
                  CHANGE);

  // ===== MOTOR 2 INITIALIZATION =====
  pinMode(PROP_IN_PIN2, INPUT);   // PWM input
  pinMode(PWM_PROP_PIN2, OUTPUT); // PWM output
  pinMode(FORWARD_PIN2, OUTPUT);  // Direction
  pinMode(STOP_PIN2, OUTPUT);     // Stop (unused currently)
  pinMode(BRAKE_PIN2, OUTPUT);    // Brake (unused currently)

  // Initialize motor 2 pins to safe state
  digitalWrite(FORWARD_PIN2, LOW);
  digitalWrite(STOP_PIN2, LOW);
  digitalWrite(BRAKE_PIN2, LOW);
  analogWrite(PWM_PROP_PIN2, 0);

  // Attach interrupt for PWM measurement on motor 2
  attachInterrupt(digitalPinToInterrupt(PROP_IN_PIN2), interruptHandler2,
                  CHANGE);

  Serial.println("Initialization complete.");
  Serial.println("Waiting for PWM signals...\n");
  delay(1000);
}

// ===== MAIN LOOP =====
void loop() {
  // Read the latest PWM signal durations (measured by interrupts)
  noInterrupts();
  PropSignal1 = pulseDuration1;
  PropSignal2 = pulseDuration2;
  interrupts();

  // Process motor 1
  processPropulsionMotor1();

  // Process motor 2
  processPropulsionMotor2();

  // Handle reversal/emergency stop signal
  if (digitalRead(REVERSAL_IN) ==
      LOW) { // Button pressed (active low with pull-up)
    Serial.println("\n>>> REVERSAL SIGNAL RECEIVED - EMERGENCY STOP <<<");
    analogWrite(PWM_PROP_PIN1, 0);
    analogWrite(PWM_PROP_PIN2, 0);
    digitalWrite(REVERSAL_OUT, HIGH);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    delay(100);
  } else {
    digitalWrite(REVERSAL_OUT, LOW);
    digitalWrite(LED_RED, LOW);
  }

  delay(100); // Main loop cycle time
}

// ===== INTERRUPT HANDLERS FOR PWM MEASUREMENT =====
/**
 * Interrupt handler for motor 1 PWM input measurement
 * Measures the HIGH pulse duration
 */
void interruptHandler1() {
  if (digitalRead(PROP_IN_PIN1) == HIGH) {
    // Rising edge - start measurement
    pulseStart1 = micros();
  } else {
    // Falling edge - end measurement
    pulseDuration1 = micros() - pulseStart1;
  }
}

/**
 * Interrupt handler for motor 2 PWM input measurement
 * Measures the HIGH pulse duration
 */
void interruptHandler2() {
  if (digitalRead(PROP_IN_PIN2) == HIGH) {
    // Rising edge - start measurement
    pulseStart2 = micros();
  } else {
    // Falling edge - end measurement
    pulseDuration2 = micros() - pulseStart2;
  }
}

// ===== PROPULSION ALGORITHM FOR MOTOR 1 =====
/**
 * Process PWM signal for motor 1:
 * - Out of bounds: stop (0 output)
 * - < 900µs: dead zone (stop)
 * - 900-1400µs: forward motion (proportional)
 * - > 1400µs: high-speed forward or error condition (prevent harsh startup)
 */
void processPropulsionMotor1() {
  static unsigned long lastPrint = 0;

  // Only print diagnostics every 500ms to avoid flooding serial
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();

    Serial.print("Motor1 - Signal: ");
    Serial.print(PropSignal1);
    Serial.print("µs | Output: ");
    Serial.print(PropulsionValue1);
    Serial.print(" | ");
  }

  // ===== OUT OF BOUNDS CHECK: < 900µs or > 2000µs =====
  if (PropSignal1 < SIGNAL_MIN || PropSignal1 > SIGNAL_MAX) {
    if (millis() - lastPrint < 100) {
      Serial.println("ERROR: Out of range - STOP");
    }
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_BLUE, LOW);
    analogWrite(PWM_PROP_PIN1, 0);
    PropulsionValue1 = 0;
  }
  // ===== NEUTRAL POINT: 1500µs = STOP =====
  else if (PropSignal1 == NEUTRAL_POINT) {
    if (millis() - lastPrint < 100) {
      Serial.println("Neutral - STOP");
    }
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_RED, LOW);
    analogWrite(PWM_PROP_PIN1, 0);
    PropulsionValue1 = 0;
  }
  // ===== REVERSE: < 1500µs =====
  else if (PropSignal1 < NEUTRAL_POINT && PropSignal1 >= REVERSE_MIN) {
    if (millis() - lastPrint < 100) {
      Serial.println("Reverse");
    }
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(FORWARD_PIN1, LOW); // Reverse direction
    // Proportional reverse: 900µs (100%) to 1500µs (0%)
    uint32_t span = NEUTRAL_POINT - PropSignal1;
    PropulsionValue1 = map(span, 0, (NEUTRAL_POINT - REVERSE_MIN), 0, 255);
    PropulsionValue1 = constrain(PropulsionValue1, 0, 255);
    analogWrite(PWM_PROP_PIN1, PropulsionValue1);
  }
  // ===== FORWARD: > 1500µs =====
  else if (PropSignal1 > NEUTRAL_POINT && PropSignal1 <= FORWARD_MAX) {
    if (millis() - lastPrint < 100) {
      Serial.println("Forward");
    }
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(FORWARD_PIN1, HIGH); // Forward direction
    // Proportional forward: 1500µs (0%) to 2000µs (100%)
    uint32_t span = PropSignal1 - NEUTRAL_POINT;
    PropulsionValue1 = map(span, 0, (FORWARD_MAX - NEUTRAL_POINT), 0, 255);
    PropulsionValue1 = constrain(PropulsionValue1, 0, 255);
    analogWrite(PWM_PROP_PIN1, PropulsionValue1);
  }
}

// ===== PROPULSION ALGORITHM FOR MOTOR 2 =====
/**
 * Process PWM signal for motor 2
 * Same logic as motor 1 - Bidirectional control
 */
void processPropulsionMotor2() {
  static unsigned long lastPrint = 0;

  // Only print diagnostics every 500ms
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();

    Serial.print("Motor2 - Signal: ");
    Serial.print(PropSignal2);
    Serial.print("µs | Output: ");
    Serial.println(PropulsionValue2);
  }

  // ===== OUT OF BOUNDS CHECK: < 900µs or > 2000µs =====
  if (PropSignal2 < SIGNAL_MIN || PropSignal2 > SIGNAL_MAX) {
    if (millis() - lastPrint < 100) {
      Serial.println("Motor2: ERROR - Out of range");
    }
    analogWrite(PWM_PROP_PIN2, 0);
    PropulsionValue2 = 0;
  }
  // ===== NEUTRAL POINT: 1500µs = STOP =====
  else if (PropSignal2 == NEUTRAL_POINT) {
    if (millis() - lastPrint < 100) {
      Serial.println("Motor2: Neutral - STOP");
    }
    analogWrite(PWM_PROP_PIN2, 0);
    PropulsionValue2 = 0;
  }
  // ===== REVERSE: < 1500µs =====
  else if (PropSignal2 < NEUTRAL_POINT && PropSignal2 >= REVERSE_MIN) {
    if (millis() - lastPrint < 100) {
      Serial.println("Motor2: Reverse");
    }
    digitalWrite(FORWARD_PIN2, LOW); // Reverse direction
    // Proportional reverse: 900µs (100%) to 1500µs (0%)
    uint32_t span = NEUTRAL_POINT - PropSignal2;
    PropulsionValue2 = map(span, 0, (NEUTRAL_POINT - REVERSE_MIN), 0, 255);
    PropulsionValue2 = constrain(PropulsionValue2, 0, 255);
    analogWrite(PWM_PROP_PIN2, PropulsionValue2);
  }
  // ===== FORWARD: > 1500µs =====
  else if (PropSignal2 > NEUTRAL_POINT && PropSignal2 <= FORWARD_MAX) {
    if (millis() - lastPrint < 100) {
      Serial.println("Motor2: Forward");
    }
    digitalWrite(FORWARD_PIN2, HIGH); // Forward direction
    // Proportional forward: 1500µs (0%) to 2000µs (100%)
    uint32_t span = PropSignal2 - NEUTRAL_POINT;
    PropulsionValue2 = map(span, 0, (FORWARD_MAX - NEUTRAL_POINT), 0, 255);
    PropulsionValue2 = constrain(PropulsionValue2, 0, 255);
    analogWrite(PWM_PROP_PIN2, PropulsionValue2);
  }
}
