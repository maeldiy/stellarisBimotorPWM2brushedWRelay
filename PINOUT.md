# Stellaris LM4F120 Pinout Configuration

## TM4C123G LaunchPad Pin Definitions (40-pin LQFP)

### Motor Control Pins

#### Motor 1 Configuration
```
PropInPin1  = PB_6  (PORTB Pin 6) - PWM Input with interrupt capability
PwmPropPin1 = PE_4  (PORTE Pin 4) - PWM Output (M0PWM6)
ForwardPin1 = PD_0  (PORTD Pin 0) - Direction control
StopPin1    = PA_2  (PORTA Pin 2) - Stop control (spare)
BrakePin1   = PA_4  (PORTA Pin 4) - Brake control (spare)
```

#### Motor 2 Configuration
```
PropInPin2  = PB_7  (PORTB Pin 7) - PWM Input with interrupt capability
PwmPropPin2 = PE_5  (PORTE Pin 5) - PWM Output (M0PWM7)
ForwardPin2 = PD_1  (PORTD Pin 1) - Direction control
StopPin2    = PA_3  (PORTA Pin 3) - Stop control (spare)
BrakePin2   = PA_5  (PORTA Pin 5) - Brake control (spare)
```

### Control & Status Pins

#### Reversal / Emergency Stop
```
ReversalIn  = PF_0  (PORTF Pin 0) - SW2 button input (active LOW)
ReversalOut = PA_6  (PORTA Pin 6) - Emergency mode output
```

#### Status Indicators
```
LED_RED     = PF_1  (PORTF Pin 1) - On-board red LED
LED_BLUE    = PF_2  (PORTF Pin 2) - On-board blue LED
LED_GREEN   = PF_3  (PORTF Pin 3) - On-board green LED
```

## PWM Output Capabilities

### M0PWM Module Pins (40-pin LQFP)

| PWM Signal | Primary Pin | Secondary Pin | Port/Pin | Notes |
|-----------|-------------|---------------|----------|-------|
| M0PWM0 | PB6 | PA6 | PORTB.6 / PORTA.6 | - |
| M0PWM1 | PB7 | PA7 | PORTB.7 / PORTA.7 | - |
| M0PWM2 | PC4 | - | PORTC.4 | - |
| M0PWM3 | PC5 | - | PORTC.5 | - |
| M0PWM4 | PD0 | - | PORTD.0 | - |
| M0PWM5 | PD1 | - | PORTD.1 | - |
| M0PWM6 | PE4 | - | PORTE.4 | ✓ Used for Motor 1 |
| M0PWM7 | PE5 | - | PORTE.5 | ✓ Used for Motor 2 |

## Frequency and Duty Cycle

### PWM Configuration
- **Frequency**: 1kHz (default for analogWrite)
- **Resolution**: 8-bit PWM (0-255)
- **Output Voltage**: 3.3V logic levels

### Timing
- **PWM Period**: 1ms
- **Min High Time**: 0µs (0% duty)
- **Max High Time**: 1000µs (100% duty)

## Port Pin Mapping (LQFP40)

```
PORTA (pins 10-17, 36-37):
  PA0 - Pin 10    | PA4 - Pin 14
  PA1 - Pin 11    | PA5 - Pin 15 (JTAG TCK)
  PA2 - Pin 12    | PA6 - Pin 16 (JTAG TDO)
  PA3 - Pin 13    | PA7 - Pin 37

PORTB (pins 18-20, 45-47):
  PB0 - Pin 45    | PB4 - Pin 20
  PB1 - Pin 46    | PB5 - Pin 19 (JTAG TDI)
  PB2 - Pin 47    | PB6 - Pin 18 ✓ Motor 1 PWM Input
  PB3 - Pin 21    | PB7 - Pin 19 ✓ Motor 2 PWM Input

PORTC (pins 23-26):
  PC0 - Pin 26    | PC2 - Pin 24 (JTAG TCO)
  PC1 - Pin 25    | PC3 - Pin 23 (JTAG TMS)

PORTD (pins 27-29, 38):
  PD0 - Pin 27 ✓ Forward Pin 1  | PD2 - Pin 29
  PD1 - Pin 28 ✓ Forward Pin 2  | PD3 - Pin 38 (USB)

PORTE (pins 30-34):
  PE0 - Pin 30    | PE3 - Pin 33
  PE1 - Pin 31    | PE4 - Pin 34 ✓ PWM Motor 1
  PE2 - Pin 32    | PE5 - Pin 35 ✓ PWM Motor 2

PORTF (pins 39-43, 48):
  PF0 - Pin 48 ✓ Emergency Stop | PF2 - Pin 41 (Blue LED)
  PF1 - Pin 39 (Red LED)        | PF3 - Pin 40 (Green LED)
  PF4 - Pin 42 (SW1)            | PF5 - N/A (No Pin)
```

## Signal Levels

All I/O pins use **3.3V logic levels**:
- **HIGH**: 3.3V (min 2.4V)
- **LOW**: 0V (max 0.3V)

## Special Functions

### Interrupt Support
All GPIO pins support external interrupts (rising edge, falling edge, both edges)

### PWM Frequency
The firmware uses Timer0 for PWM generation at 1kHz (standard Arduino frequency)

### Pull-up Configuration
- ReversalIn (PF0) uses internal pull-up (SW2 is active LOW)
- Other inputs: No pull-up/pull-down unless specified

## Development Notes

### Pin Selection Rationale

**Motor 1 PWM Input (PB6)**
- Port B selected for convenient grouping with Motor 2
- Pin 6 provides interrupt capability for pulse measurement
- PE4 alternative available if conflict arises

**Motor 2 PWM Input (PB7)**
- Adjacent to Motor 1 input on same port
- Pin 7 provides interrupt capability
- PE5 alternative available if conflict arises

**PWM Outputs (PE4, PE5)**
- Use Port E for physical separation from input signals
- M0PWM6/M0PWM7 for clean PWM output
- 1kHz frequency suitable for DC motor control

**Direction Pins (PD0, PD1)**
- Port D selected for clean digital control signals
- Adjacent pins for logical grouping
- Standard GPIO (no special functions)

**Emergency Stop (PF0)**
- Connected to SW2 on LaunchPad board
- Uses internal pull-up for reliable button detection
- Active LOW for safety (default HIGH = normal operation)

## Compatibility Notes

- All pins support 3.3V digital I/O
- No 5V tolerant pins on this package
- Ensure external signals are 3.3V logic
- Use level shifters if 5V signals needed

## Related Documentation

- TM4C123GH6PM Datasheet: Section 6.1 (GPIO), Section 19 (PWM)
- LaunchPad User Guide: Pin mapping and schematics
- Energia Reference: Arduino-compatible PWM and GPIO functions
