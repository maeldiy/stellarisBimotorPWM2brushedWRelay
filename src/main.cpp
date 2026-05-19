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
#define LED_BLUE PF_2  // Blue LED (pin 32 on board)
#define LED_GREEN PF_3 // Green LED (pin 31 on board)

// ===== REVERSAL PINS (Emergency stop / mode change) =====
#define REVERSAL_IN PF_0 // SW2 button (pin 34 on board) - input from controller

// ===== PWM OUTPUT PINS (900-2000µs variable PWM) =====
#define PWM_OUT_PIN1 PE_3 // PWM output 1 (900-2000µs)
#define PWM_OUT_PIN2 PF_1 // PWM output 2 (900-2000µs)
#define PWM_OUT_PIN3 PA_6 // PWM output 3 (900-2000µs)

// ===== MOTOR 1 PINS (Left motor) =====
#define PROP_IN_PIN1                                                           \
  PF_4 // PWM input from ArduPilot (changed from PB_6 to avoid signal
       // interaction)
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

// ===== PWM OUTPUT TIMING VARIABLES =====
volatile unsigned long pwmPulseStart1 = 0, pwmPulseStart2 = 0,
                       pwmPulseStart3 = 0;
volatile uint16_t pwmPulseWidth1 = 1500, pwmPulseWidth2 = 1500,
                  pwmPulseWidth3 = 1500;
volatile boolean pwmPulseActive1 = false, pwmPulseActive2 = false,
                 pwmPulseActive3 = false;

// ===== PWM SWEEP VARIABLES (for testing) =====
volatile unsigned long lastSweepTime = 0;
volatile uint16_t pwmSweepValue = 900; // Start at 900µs
const uint16_t PWM_SWEEP_MIN = 900;    // Minimum sweep value
const uint16_t PWM_SWEEP_MAX = 1900;   // Maximum sweep value
const uint16_t PWM_SWEEP_STEP = 50;    // Increment by 50µs
const unsigned long SWEEP_INTERVAL = 1000; // Update every 1000ms (1 second)

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
void outputVariablePWM(uint8_t pinIndex, uint16_t pulseWidthMicros);
void updatePWMOutputs();
void updatePWMSweep();

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

  // ===== STATUS LED =====
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

  // ===== VARIABLE PWM OUTPUT PINS (900-2000µs) =====
  pinMode(PWM_OUT_PIN1, OUTPUT);
  pinMode(PWM_OUT_PIN2, OUTPUT);
  pinMode(PWM_OUT_PIN3, OUTPUT);
  digitalWrite(PWM_OUT_PIN1, LOW);
  digitalWrite(PWM_OUT_PIN2, LOW);
  digitalWrite(PWM_OUT_PIN3, LOW);

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
  Serial.println("Waiting for PWM signals...");
  Serial.println("Outputting 900-2000µs variable PWM to PE_3, PF_1, PA_6\n");
  Serial.println("PWM Sweep Mode: 900µs -> 1900µs (+50µs every 1s, auto-restart)\n");
  lastSweepTime = millis();
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

  // Update PWM sweep values
  updatePWMSweep();

  // Update variable PWM outputs
  updatePWMOutputs();

  // Handle reversal/emergency stop signal
  if (digitalRead(REVERSAL_IN) ==
      LOW) { // Button pressed (active low with pull-up)
    Serial.println("\n>>> REVERSAL SIGNAL RECEIVED - EMERGENCY STOP <<<");
    analogWrite(PWM_PROP_PIN1, 0);
    analogWrite(PWM_PROP_PIN2, 0);
    pwmPulseWidth1 = 1500; // Neutral
    pwmPulseWidth2 = 1500; // Neutral
    pwmPulseWidth3 = 1500; // Neutral
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
    delay(100);
  } else {
    digitalWrite(LED_BLUE, LOW);
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
    digitalWrite(LED_GREEN, LOW);
    analogWrite(PWM_PROP_PIN1, 0);
    PropulsionValue1 = 0;
    pwmPulseWidth1 = 1500; // Neutral
    pwmPulseWidth2 = 1500; // Neutral
    pwmPulseWidth3 = 1500; // Neutral
  }
  // ===== NEUTRAL POINT: 1500µs = STOP =====
  else if (PropSignal1 == NEUTRAL_POINT) {
    if (millis() - lastPrint < 100) {
      Serial.println("Neutral - STOP");
    }
    digitalWrite(LED_GREEN, LOW);
    analogWrite(PWM_PROP_PIN1, 0);
    PropulsionValue1 = 0;
    pwmPulseWidth1 = 1500; // Neutral
    pwmPulseWidth2 = 1500; // Neutral
    pwmPulseWidth3 = 1500; // Neutral
  }
  // ===== REVERSE: < 1500µs =====
  else if (PropSignal1 < NEUTRAL_POINT && PropSignal1 >= REVERSE_MIN) {
    if (millis() - lastPrint < 100) {
      Serial.println("Reverse");
    }
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(FORWARD_PIN1, LOW); // Reverse direction
    // Proportional reverse: 900µs (100%) to 1500µs (0%)
    uint32_t span = NEUTRAL_POINT - PropSignal1;
    PropulsionValue1 = map(span, 0, (NEUTRAL_POINT - REVERSE_MIN), 0, 255);
    PropulsionValue1 = constrain(PropulsionValue1, 0, 255);
    // Map to 900-1500µs range for reverse
    pwmPulseWidth1 = map(PropulsionValue1, 0, 255, 1500, 900);
    pwmPulseWidth3 = pwmPulseWidth1; // Copy to pin 3
    analogWrite(PWM_PROP_PIN1, PropulsionValue1);
  }
  // ===== FORWARD: > 1500µs =====
  else if (PropSignal1 > NEUTRAL_POINT && PropSignal1 <= FORWARD_MAX) {
    if (millis() - lastPrint < 100) {
      Serial.println("Forward");
    }
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(FORWARD_PIN1, HIGH); // Forward direction
    // Proportional forward: 1500µs (0%) to 2000µs (100%)
    uint32_t span = PropSignal1 - NEUTRAL_POINT;
    PropulsionValue1 = map(span, 0, (FORWARD_MAX - NEUTRAL_POINT), 0, 255);
    PropulsionValue1 = constrain(PropulsionValue1, 0, 255);
    // Map to 1500-2000µs range for forward
    pwmPulseWidth1 = map(PropulsionValue1, 0, 255, 1500, 2000);
    pwmPulseWidth3 = pwmPulseWidth1; // Copy to pin 3
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
    pwmPulseWidth2 = 1500; // Neutral
  }
  // ===== NEUTRAL POINT: 1500µs = STOP =====
  else if (PropSignal2 == NEUTRAL_POINT) {
    if (millis() - lastPrint < 100) {
      Serial.println("Motor2: Neutral - STOP");
    }
    analogWrite(PWM_PROP_PIN2, 0);
    PropulsionValue2 = 0;
    pwmPulseWidth2 = 1500; // Neutral
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
    // Map to 900-1500µs range for reverse
    pwmPulseWidth2 = map(PropulsionValue2, 0, 255, 1500, 900);
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
    // Map to 1500-2000µs range for forward
    pwmPulseWidth2 = map(PropulsionValue2, 0, 255, 1500, 2000);
    analogWrite(PWM_PROP_PIN2, PropulsionValue2);
  }
}

// ===== VARIABLE PWM OUTPUT FUNCTIONS =====
/**
 * Update PWM outputs on PE_3, PF_1, and PA_6
 * This function generates variable pulse width (900-2000µs) on the output pins
 * Called from main loop at regular intervals
 */
void updatePWMOutputs() {
  unsigned long now = micros();

  // ===== OUTPUT PIN 1 (PE_3) =====
  if (!pwmPulseActive1 && (now - pwmPulseStart1) >= (3000 - pwmPulseWidth1)) {
    // Time to start pulse
    digitalWrite(PWM_OUT_PIN1, HIGH);
    pwmPulseActive1 = true;
    pwmPulseStart1 = now;
  } else if (pwmPulseActive1 && (now - pwmPulseStart1) >= pwmPulseWidth1) {
    // Time to end pulse
    digitalWrite(PWM_OUT_PIN1, LOW);
    pwmPulseActive1 = false;
  }

  // ===== OUTPUT PIN 2 (PF_1) =====
  if (!pwmPulseActive2 && (now - pwmPulseStart2) >= (3000 - pwmPulseWidth2)) {
    // Time to start pulse
    digitalWrite(PWM_OUT_PIN2, HIGH);
    pwmPulseActive2 = true;
    pwmPulseStart2 = now;
  } else if (pwmPulseActive2 && (now - pwmPulseStart2) >= pwmPulseWidth2) {
    // Time to end pulse
    digitalWrite(PWM_OUT_PIN2, LOW);
    pwmPulseActive2 = false;
  }

  // ===== OUTPUT PIN 3 (PA_6) =====
  if (!pwmPulseActive3 && (now - pwmPulseStart3) >= (3000 - pwmPulseWidth3)) {
    // Time to start pulse
    digitalWrite(PWM_OUT_PIN3, HIGH);
    pwmPulseActive3 = true;
    pwmPulseStart3 = now;
  } else if (pwmPulseActive3 && (now - pwmPulseStart3) >= pwmPulseWidth3) {
    // Time to end pulse
    digitalWrite(PWM_OUT_PIN3, LOW);
    pwmPulseActive3 = false;
  }
}

/**
 * Output variable PWM pulse width to a specific pin
 * This is a helper function if you need to manually set PWM on a pin
 * pinIndex: 1=PE_3, 2=PF_1, 3=PA_6
 * pulseWidthMicros: 900-2000 microseconds
 */
void outputVariablePWM(uint8_t pinIndex, uint16_t pulseWidthMicros) {
  // Constrain pulse width to valid range
  uint16_t constrainedWidth = constrain(pulseWidthMicros, 900, 2000);

  switch (pinIndex) {
  case 1:
    pwmPulseWidth1 = constrainedWidth;
    break;
  case 2:
    pwmPulseWidth2 = constrainedWidth;
    break;
  case 3:
    pwmPulseWidth3 = constrainedWidth;
    break;
  default:
    // Invalid pin index
    break;
  }
}

/**
 * Update PWM sweep: incrementally increase pulse width from 900µs to 1900µs
 * Increments by 50µs every 1 second, then restarts at 900µs
 */
void updatePWMSweep() {
  unsigned long currentTime = millis();

  // Check if it's time to update the sweep value (every 1 second)
  if (currentTime - lastSweepTime >= SWEEP_INTERVAL) {
    lastSweepTime = currentTime;

    // Print current sweep value
    Serial.print("PWM Sweep: ");
    Serial.print(pwmSweepValue);
    Serial.println("µs");

    // Increment sweep value
    pwmSweepValue += PWM_SWEEP_STEP;

    // Check if we've reached the maximum, if so restart at minimum
    if (pwmSweepValue > PWM_SWEEP_MAX) {
      pwmSweepValue = PWM_SWEEP_MIN;
      Serial.println("PWM Sweep restarted at 900µs");
    }

    // Update all three PWM output pins with the current sweep value
    pwmPulseWidth1 = pwmSweepValue;
    pwmPulseWidth2 = pwmSweepValue;
    pwmPulseWidth3 = pwmSweepValue;
  }
}
