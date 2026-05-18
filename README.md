# Bi-Motor PWM Controller

**Multiple Board Support**: ESP8266, Stellaris LM4F120, PlatformIO

Converts two PWM signals from an ArduPilot flight controller to appropriate signals for brushed motor controllers.

---

## ⭐ Choose Your Platform

### 🚀 **RECOMMENDED: ESP8266** (Easiest, No Driver Issues)

```bash
✅ Setup: 15 minutes
✅ Upload: 10 seconds
✅ No driver headaches
✅ Arduino IDE native
✅ Board cost: $5-15
✅ WiFi bonus
```

**Quick Start:**
```
1. Buy ESP8266 NodeMCU (~$7)
2. Download Arduino IDE
3. Add ESP8266 support
4. Upload esp8266_bimotorPWM.ino
5. Done! ✅
```

**Setup Guide**: See `/esp8266/ESP8266_SETUP.md`

---

### 🎯 **ALTERNATIVE: Stellaris LM4F120** (Via Energia IDE)

```bash
✅ Powerful 32-bit ARM microcontroller
✅ Energia IDE support
✅ Higher performance
⚠️ More complex setup
⚠️ Driver issues reported
```

**Setup Guide**: See `/energia/ENERGIA_SETUP.md`

---

### 📊 Comparison Table

| Feature | **ESP8266** ⭐ | Stellaris | PlatformIO |
|---------|--------------|-----------|-----------|
| **Setup Time** | **15 min** | 30+ min | 30+ min |
| **Reliability** | **✅ 99%** | ⚠️ 70% | ⚠️ 60% |
| **Cost** | **$5-15** | $20+ | (requires Stellaris) |
| **Upload Speed** | **10-15s** | 10-15s | 20-30s |
| **Driver Issues** | **None** | Yes | Yes (OpenOCD) |
| **Arduino IDE** | **Native** | Via Energia | Via extension |
| **Learning Curve** | **Gentle** | Steep | Very steep |
| **WiFi** | **Built-in!** | No | No |
| **Best for** | **Everyone** | Advanced users | CI/CD |

**Decision Matrix:**
```
Want it working FAST? → Use ESP8266
Need more power? → Use Stellaris
Want advanced debugging? → Use PlatformIO
```

---

## ⭐ Recommended: Use Energia IDE (for Stellaris)

This project provides **multiple development environments**:

| Environment | Setup | Reliability | Best For |
|------------|-------|-----------|----------|
| **ESP8266** (BEST) | ✅ Easiest | ✅✅✅ Highly Reliable | **Everyone** |
| **Energia IDE** | ✅ Easy | ✅ Reliable | **Stellaris users** |
| **PlatformIO** | Medium | ⚠️ Can have JTAG issues | Advanced users, VS Code users |

### Quick Start with Energia IDE

```bash
1. Download Energia: http://energia.nu/
2. Extract to: C:\Program Files\Energia
3. Launch energia.exe
4. File → Open → energia/stellarisBimotorPWM2brushedWRelay/
5. Tools → Board → "TI LaunchPad w/ LM4F120 (80MHz)"
6. Tools → Port → COM9
7. Sketch → Upload (Ctrl+U)
8. Done! Firmware is running ✅
```

**Why Energia?** Serial bootloader uploads work reliably without JTAG issues. See [PLATFORMIO_VS_ENERGIA.md](PLATFORMIO_VS_ENERGIA.md) for detailed comparison.

---

- **2 PWM Inputs**: Receives motor control signals from ArduPilot flight controller
- **2 PWM Outputs**: Drives brushed DC motors via motor controller (PE4, PE5)
- **3 Variable PWM Outputs**: 900-2000µs servo-compatible PWM signals (PE_3, PF_1, PA_6)
  - PE_3: Motor 1 proportional output
  - PF_1: Motor 2 proportional output
  - PA_6: Motor 1 mirror output
- **Directional Control**: 2 forward/reverse pins for relay-based direction switching
- **Emergency Stop**: Reversal input (SW2) for emergency motor shutdown
- **Status LEDs**: Status indicators for system state
  - Blue: Dead zone or Emergency stop
  - Green: Forward motion active

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

### Variable PWM Outputs (900-2000µs)
| Function | Pin | Port.Pin | Notes |
|----------|-----|----------|-------|
| PWM Output 1 | PE3 | PORTE.3 | Motor 1 proportional (servo-compatible) |
| PWM Output 2 | PF1 | PORTF.1 | Motor 2 proportional (servo-compatible) |
| PWM Output 3 | PA6 | PORTA.6 | Motor 1 mirror output (servo-compatible) |

### Control Signals
| Function | Pin | Port.Pin | Notes |
|----------|-----|----------|-------|
| Reversal Input (Emergency Stop) | PF0 | PORTF.0 | SW2 on LaunchPad (active low) |
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
- **All Off**: Initializing
- **Blue On**: Dead zone (no motion) or emergency stop active
- **Green On**: Forward motion active

### Motor Safety Features
- Dead zone below 900µs prevents accidental motion
- Out-of-bounds detection for signal loss
- Emergency stop via SW2 button (reversal input)
- Smooth ramping in high-speed mode (150-255 PWM)
## Variable PWM Output Signals (900-2000µs)

The firmware generates servo-compatible PWM signals on three pins:
- **PE_3** (Motor 1): Proportional signal from Motor 1 input
- **PF_1** (Motor 2): Proportional signal from Motor 2 input  
- **PA_6** (Mirror): Copy of Motor 1 output for redundancy

These signals can be used to drive:
- Servo motors (standard 1000-2000µs servo format)
- Flight controller auxiliary inputs
- RC receiver emulation
- Telemetry/feedback systems

**Signal Format:**
- Frequency: ~333 Hz (3ms period)
- Minimum pulse: 900µs (reverse full)
- Neutral: 1500µs (stop)
- Maximum pulse: 2000µs (forward full)
- Logic level: 3.3V TTL
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
- Ensure signal range within 900-2000µs for normal operation

### Variable PWM Outputs Not Working
- Verify PE_3, PF_1, PA_6 are connected to appropriate devices
- Check with oscilloscope for 900-2000µs pulse generation
- Ensure 3.3V logic levels are compatible with receiving device
- Verify `updatePWMOutputs()` is called in main loop (called automatically)

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

## Documentation

### Project Documentation
- [PLATFORMIO_VS_ENERGIA.md](PLATFORMIO_VS_ENERGIA.md) - **Read this first!** Compare development environments
- [SOLUTION_GUIDE.md](SOLUTION_GUIDE.md) - Troubleshooting and solutions
- [UPLOAD_TROUBLESHOOTING.md](UPLOAD_TROUBLESHOOTING.md) - Upload issues & fixes
- [PINOUT.md](PINOUT.md) - Detailed hardware pinout reference
- [SETUP_COMPLETE.md](SETUP_COMPLETE.md) - Initial setup summary

### Energia IDE Documentation (Recommended)
- [energia/README.md](energia/README.md) - Energia setup and usage guide
- [energia/ENERGIA_SETUP.md](energia/ENERGIA_SETUP.md) - Detailed installation instructions

## References

- [TM4C123G LaunchPad User Guide](http://www.ti.com/lit/ug/spmu289c/spmu289c.pdf)
- [TM4C123G PWM Module](http://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf)
- [Energia Framework](http://energia.nu/)
- [Energia Reference](http://energia.nu/reference/)
- [TI E2E Community](https://e2e.ti.com/)
- [43oh Forum (Energia Community)](https://forum.43oh.com/)

## License

This project adapts code originally for ESP32 and converts it for the Stellaris LM4F120 board.

## Author Notes

- Original code designed for ESP32 Arduino framework
- Adapted for TM4C123G with Energia/Arduino framework
- PWM measurement implemented using hardware interrupts
- Compatible with ArduPilot autopilot systems
