/**
 * ESP8266 Bi-Motor PWM Controller
 * Converts ArduPilot PWM signals to brushed motor controller signals
 *
 * Board: ESP8266 (NodeMCU, WeMos D1, etc.)
 * Serial: 115200 baud
 *
 * PWM Input Pins:  D5 (GPIO14), D6 (GPIO12)
 * PWM Output Pins: D7 (GPIO13), D8 (GPIO15)
 * Direction Pins:  D0 (GPIO16), D1 (GPIO5)
 * Emergency Stop:  D2 (GPIO4)
 * Status LEDs:     D3 (GPIO0), D4 (GPIO2)
 */

// ============ PIN CONFIGURATION (ESP8266) ============

// Motor 1 (Left Motor)
#define PROP_IN_PIN1 D5 // GPIO14 - PWM input from ArduPilot
#define PWM_OUT_PIN1 D7 // GPIO13 - PWM output to motor controller
#define FORWARD_PIN1 D0 // GPIO16 - Direction control (forward/stop)

// Motor 2 (Right Motor)
#define PROP_IN_PIN2 D6 // GPIO12 - PWM input from ArduPilot
#define PWM_OUT_PIN2 D8 // GPIO15 - PWM output to motor controller
#define FORWARD_PIN2 D1 // GPIO5  - Direction control (forward/stop)

// Control Signals
#define REVERSAL_PIN D2 // GPIO4  - Emergency stop (active LOW)
#define REVERSAL_OUT RX // TX pin not used - reversal indication

// Status LEDs
#define LED_RED D3  // GPIO0  - Error/out-of-bounds
#define LED_BLUE D4 // GPIO2  - Dead zone (onboard LED on D4)

// ============ SIGNAL PARAMETERS ============
// User Requirements:
// < 1500µs = Reverse
// > 1500µs = Forward
// < 900µs or > 2000µs = Out of range

#define SIGNAL_MIN 900      // Minimum safe PWM pulse (µs)
#define SIGNAL_MAX 2000     // Maximum safe PWM pulse (µs)
#define NEUTRAL_POINT 1500  // Neutral/stop point (µs)
#define REVERSE_MIN 900     // Minimum reverse signal
#define REVERSE_MAX 1500    // Maximum reverse signal (stop)
#define FORWARD_MIN 1500    // Minimum forward signal (stop)
#define FORWARD_MAX 2000    // Maximum forward signal
#define PWM_FREQUENCY 50    // Hz - Standard RC frequency
#define PWM_RESOLUTION 1023 // ESP8266 PWM 0-1023

// ============ GLOBAL VARIABLES ============

volatile uint32_t prop_timer1 = 0;
volatile uint32_t prop_timer2 = 0;
volatile uint32_t pulse_start1 = 0;
volatile uint32_t pulse_start2 = 0;
volatile uint32_t last_pulse_width1 = 0;
volatile uint32_t last_pulse_width2 = 0;

uint8_t motor1_speed = 0;
uint8_t motor2_speed = 0;
bool reversal_active = false;
uint32_t last_update = 0;

// ============ INTERRUPT HANDLERS ============

// Motor 1 PWM input interrupt
void IRAM_ATTR interruptHandler1() {
  uint32_t current_time = micros();

  if (digitalRead(PROP_IN_PIN1) == HIGH) {
    // Rising edge - start pulse measurement
    pulse_start1 = current_time;
  } else {
    // Falling edge - calculate pulse width
    if (pulse_start1 > 0) {
      prop_timer1 = current_time - pulse_start1;
      last_pulse_width1 = prop_timer1;
    }
  }
}

// Motor 2 PWM input interrupt
void IRAM_ATTR interruptHandler2() {
  uint32_t current_time = micros();

  if (digitalRead(PROP_IN_PIN2) == HIGH) {
    // Rising edge - start pulse measurement
    pulse_start2 = current_time;
  } else {
    // Falling edge - calculate pulse width
    if (pulse_start2 > 0) {
      prop_timer2 = current_time - pulse_start2;
      last_pulse_width2 = prop_timer2;
    }
  }
}

// ============ SETUP ============

void setup() {
  // Serial communication
  Serial.begin(115200);
  delay(500);

  // Print welcome message
  Serial.println("\n\n================================");
  Serial.println("ESP8266 Bi-Motor PWM Controller");
  Serial.println("================================");
  Serial.println("Board: ESP8266 (NodeMCU/WeMos)");
  Serial.println("Serial: 115200 baud");
  Serial.println("Status: Initializing...");

  // Configure input pins
  pinMode(PROP_IN_PIN1, INPUT);
  pinMode(PROP_IN_PIN2, INPUT);

  // Configure output pins
  pinMode(PWM_OUT_PIN1, OUTPUT);
  pinMode(PWM_OUT_PIN2, OUTPUT);
  pinMode(FORWARD_PIN1, OUTPUT);
  pinMode(FORWARD_PIN2, OUTPUT);

  // Configure control pins
  pinMode(REVERSAL_PIN, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // Initialize outputs to safe state
  digitalWrite(FORWARD_PIN1, LOW);
  digitalWrite(FORWARD_PIN2, LOW);
  analogWrite(PWM_OUT_PIN1, 0);
  analogWrite(PWM_OUT_PIN2, 0);

  // LED test
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);

  // Attach interrupts for PWM input measurement
  attachInterrupt(digitalPinToInterrupt(PROP_IN_PIN1), interruptHandler1,
                  CHANGE);
  attachInterrupt(digitalPinToInterrupt(PROP_IN_PIN2), interruptHandler2,
                  CHANGE);

  Serial.println("✓ PWM inputs configured (D5, D6)");
  Serial.println("✓ PWM outputs configured (D7, D8)");
  Serial.println("✓ Direction pins configured (D0, D1)");
  Serial.println("✓ Emergency stop configured (D2)");
  Serial.println("✓ Status LEDs configured (D3, D4)");
  Serial.println("✓ Initialization complete!");
  Serial.println("\nWaiting for PWM signals...\n");

  last_update = millis();
}

// ============ MAIN LOOP ============

void loop() {
  // Update motor control every 100ms
  if (millis() - last_update >= 100) {
    last_update = millis();

    // Check emergency stop button
    reversal_active = (digitalRead(REVERSAL_PIN) == LOW);

    // Process motors only if no emergency stop
    if (!reversal_active) {
      processPropulsionMotor1();
      processPropulsionMotor2();
    } else {
      // Emergency stop: kill motors
      analogWrite(PWM_OUT_PIN1, 0);
      analogWrite(PWM_OUT_PIN2, 0);
      digitalWrite(FORWARD_PIN1, LOW);
      digitalWrite(FORWARD_PIN2, LOW);
      digitalWrite(LED_RED, HIGH); // Red LED on during emergency
      digitalWrite(LED_BLUE, LOW);
    }
  }
}

// ============ MOTOR PROCESSING ============

void processPropulsionMotor1() {
  uint32_t pulse = last_pulse_width1;

  // Check signal validity: < 900µs or > 2000µs = out of range
  if (pulse < SIGNAL_MIN || pulse > SIGNAL_MAX) {
    // Out of bounds - motor off, red LED on
    analogWrite(PWM_OUT_PIN1, 0);
    digitalWrite(FORWARD_PIN1, LOW);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_BLUE, LOW);
    motor1_speed = 0;
    return;
  }

  // Neutral point: 1500µs = stop
  if (pulse == NEUTRAL_POINT) {
    analogWrite(PWM_OUT_PIN1, 0);
    digitalWrite(FORWARD_PIN1, LOW);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_RED, LOW);
    motor1_speed = 0;
    return;
  }

  // REVERSE: < 1500µs
  if (pulse < NEUTRAL_POINT && pulse >= REVERSE_MIN) {
    // Proportional reverse speed: 900µs (100%) to 1500µs (0%)
    uint32_t span = NEUTRAL_POINT - pulse;
    uint8_t speed = map(span, 0, (NEUTRAL_POINT - REVERSE_MIN), 0, 255);
    speed = constrain(speed, 0, 255);

    // Set direction pin LOW for reverse
    digitalWrite(FORWARD_PIN1, LOW);
    analogWrite(PWM_OUT_PIN1, speed);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLUE, LOW);

    motor1_speed = 255 - speed; // Store as negative equivalent
  }

  // FORWARD: > 1500µs
  if (pulse > NEUTRAL_POINT && pulse <= FORWARD_MAX) {
    // Proportional forward speed: 1500µs (0%) to 2000µs (100%)
    uint32_t span = pulse - NEUTRAL_POINT;
    uint8_t speed = map(span, 0, (FORWARD_MAX - NEUTRAL_POINT), 0, 255);
    speed = constrain(speed, 0, 255);

    // Set direction pin HIGH for forward
    digitalWrite(FORWARD_PIN1, HIGH);
    analogWrite(PWM_OUT_PIN1, speed);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLUE, LOW);

    motor1_speed = speed;
  }
}

void processPropulsionMotor2() {
  uint32_t pulse = last_pulse_width2;

  // Check signal validity: < 900µs or > 2000µs = out of range
  if (pulse < SIGNAL_MIN || pulse > SIGNAL_MAX) {
    // Out of bounds - motor off, red LED on
    analogWrite(PWM_OUT_PIN2, 0);
    digitalWrite(FORWARD_PIN2, LOW);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_BLUE, LOW);
    motor2_speed = 0;
    return;
  }

  // Neutral point: 1500µs = stop
  if (pulse == NEUTRAL_POINT) {
    analogWrite(PWM_OUT_PIN2, 0);
    digitalWrite(FORWARD_PIN2, LOW);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_RED, LOW);
    motor2_speed = 0;
    return;
  }

  // REVERSE: < 1500µs
  if (pulse < NEUTRAL_POINT && pulse >= REVERSE_MIN) {
    // Proportional reverse speed: 900µs (100%) to 1500µs (0%)
    uint32_t span = NEUTRAL_POINT - pulse;
    uint8_t speed = map(span, 0, (NEUTRAL_POINT - REVERSE_MIN), 0, 255);
    speed = constrain(speed, 0, 255);

    // Set direction pin LOW for reverse
    digitalWrite(FORWARD_PIN2, LOW);
    analogWrite(PWM_OUT_PIN2, speed);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLUE, LOW);

    motor2_speed = 255 - speed; // Store as negative equivalent
  }

  // FORWARD: > 1500µs
  if (pulse > NEUTRAL_POINT && pulse <= FORWARD_MAX) {
    // Proportional forward speed: 1500µs (0%) to 2000µs (100%)
    uint32_t span = pulse - NEUTRAL_POINT;
    uint8_t speed = map(span, 0, (FORWARD_MAX - NEUTRAL_POINT), 0, 255);
    speed = constrain(speed, 0, 255);

    // Set direction pin HIGH for forward
    digitalWrite(FORWARD_PIN2, HIGH);
    analogWrite(PWM_OUT_PIN2, speed);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLUE, LOW);

    motor2_speed = speed;
  }
}

// ============ DIAGNOSTIC FUNCTIONS ============

void printStatus() {
  Serial.print("M1: ");
  Serial.print(last_pulse_width1);
  Serial.print("µs → ");
  Serial.print(motor1_speed);
  Serial.print("  |  M2: ");
  Serial.print(last_pulse_width2);
  Serial.print("µs → ");
  Serial.println(motor2_speed);
}
