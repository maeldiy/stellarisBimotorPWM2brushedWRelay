# Stellaris LM4F120 Bi-Motor PWM Controller

Adapted from ESP32 version for TM4C123G (Stellaris LM4F120) LaunchPad

## Overview

This project converts two PWM signals from an ArduPilot flight controller to appropriate signals for brushed motor controllers with relay-based directional control.

**Board**: Stellaris LM4F120 (TM4C123G) LaunchPad  
**Interface**: COM9 @ 115200 baud  
**Framework**: Arduino (Energia)

## Features

- **2 PWM Inputs**: Receives motor control signals from ArduPilot flight controller
- **2 PWM Outputs**: Drives brushed DC motors via motor controller
- **Directional Control**: 2 forward/reverse pins for relay-based direction switching
- **Emergency Stop**: Reversal input (SW2) for emergency motor shutdown
- **Status LEDs**: RGB LED indicators for system state
  - Red: Error/out-of-bounds signal
  - Blue: Dead zone (no motion)
  - Green: High-speed mode

## Hardware Pinout

### Motor 1 (Left Motor)
| Function | Pin | Port.Pin | Notes |
|----------|-----|----------|-------|
| PWM Input | PB6 | PORTB.6 | Receives PWM from ArduPilot |
| PWM Output | PE4 | PORTE.4 | M0PWM6 - to motor controller |
| Forward/Reverse | PD0 | PORTD.0 | Direction control |
| Stop (spare) | PA2 | PORTA.2 | Currently unused |
| Brake (spare) | PA4 | PORTA.4 | Currently unused |

### Motor 2 (Right Motor)
| Function | Pin | Port.Pin | Notes |
|----------|-----|----------|-------|
| PWM Input | PB7 | PORTB.7 | Receives PWM from ArduPilot |
| PWM Output | PE5 | PORTE.5 | M0PWM7 - to motor controller |
| Forward/Reverse | PD1 | PORTD.1 | Direction control |
| Stop (spare) | PA3 | PORTA.3 | Currently unused |
| Brake (spare) | PA5 | PORTA.5 | Currently unused |

### Control Signals
| Function | Pin | Port.Pin | Notes |
|----------|-----|----------|-------|
| Reversal Input (Emergency Stop) | PF0 | PORTF.0 | SW2 on LaunchPad (active low) |
| Reversal Output | PA6 | PORTA.6 | Indicates reversal/emergency mode |
| Status LED Red | PF1 | PORTF.1 | On-board red LED |
| Status LED Blue | PF2 | PORTF.2 | On-board blue LED |
| Status LED Green | PF3 | PORTF.3 | On-board green LED |

## Setup Instructions

### Prerequisites
- Visual Studio Code
- PlatformIO IDE extension
- USB cable (for programming and serial communication)

### Installation

1. **Install PlatformIO**
   - In VS Code, go to Extensions
   - Search for "PlatformIO IDE"
   - Click Install

2. **Clone/Setup Repository**
   ```bash
   cd C:\Users\mael\Documents\mael\stellarisBimotorPWM2brushedWRelay
   # Files already initialized
   ```

3. **Configure Serial Port**
   - Update `platformio.ini` if using different COM port
   - Default: COM9 @ 115200 baud

4. **Build and Upload**
   ```bash
   # In PlatformIO terminal
   pio run -t upload
   ```

5. **Monitor Serial Output**
   ```bash
   pio device monitor
   ```

## Signal Interpretation

### PWM Signal Ranges (microseconds)
- **< 700µs or > 2000µs**: Out of bounds - motor stops, red LED on
- **700-900µs**: Dead zone - motor stopped, neutral
- **900-1400µs**: Forward motion - proportional speed
- **> 1400µs**: High-speed mode - enhanced forward motion

### Command Signal Mapping
From ArduPilot:
- 1000µs = Motor stop (dead zone)
- 1500µs = Mid-range forward
- 2000µs = Full forward

## Development Notes

### Interrupt-Based PWM Measurement
The firmware uses hardware interrupts on PB6 and PB7 to measure incoming PWM pulse widths. This provides accurate timing without blocking the main loop.

### LED Status Indicators
- **All Off**: Initializing or reversal active
- **Red On**: Signal out of bounds (safety error)
- **Blue On**: Dead zone - no motion
- **Green On**: High-speed forward mode

### Motor Safety Features
- Dead zone below 900µs prevents accidental motion
- Out-of-bounds detection for signal loss
- Emergency stop via SW2 button (reversal input)
- Smooth ramping in high-speed mode (150-255 PWM)

## Troubleshooting

### No Serial Communication
- Check COM port in `platformio.ini`
- Verify USB driver installation
- Try different USB cable

### PWM Signals Not Detected
- Verify PB6/PB7 connected to signal source
- Check signal frequency (typically 50Hz for RC servos)
- Use oscilloscope to verify signal amplitude (3.3V logic levels)

### Motors Not Responding
- Check PWM output pins (PE4/PE5) connections
- Verify motor controller power supply
- Check direction pins (PD0/PD1) logic levels
- Ensure signal range within 900-1400µs for normal operation

## Stellaris LM4F120 Pin Capabilities

**PWM-Capable Pins (M0PWM module)**:
- PE4 (M0PWM6) ✓ Used for Motor 1
- PE5 (M0PWM7) ✓ Used for Motor 2
- PE6, PE7 (alternative PWM pins)
- PB6, PB7, PC4, PC5, PD0, PD1 (additional PWM options)

**General I/O Ports Available**:
- PORTA: PA0-PA7
- PORTB: PB0-PB7
- PORTC: PC0-PC7
- PORTD: PD0-PD7
- PORTE: PE0-PE5
- PORTF: PF0-PF4

**Interrupt-Capable Pins**: All GPIO pins support external interrupts

## References

- [TM4C123G LaunchPad User Guide](http://www.ti.com/lit/ug/spmu289c/spmu289c.pdf)
- [TM4C123G PWM Module](http://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf)
- [Energia Framework](http://energia.nu/)

## License

This project adapts code originally for ESP32 and converts it for the Stellaris LM4F120 board.

## Author Notes

- Original code designed for ESP32 Arduino framework
- Adapted for TM4C123G with Energia/Arduino framework
- PWM measurement implemented using hardware interrupts
- Compatible with ArduPilot autopilot systems
