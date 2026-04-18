# Energia IDE Project Structure

## Overview

This folder contains the **Energia IDE** version of the Stellaris LM4F120 Bi-Motor PWM Controller project.

**Energia** is the recommended development environment for this project because it:
- Uses native serial bootloader uploads (no JTAG issues)
- Provides a user-friendly Arduino-compatible IDE
- Eliminates OpenOCD connection problems
- Offers better support for TI boards

---

## Folder Structure

```
energia/
├── ENERGIA_SETUP.md                           # Complete setup guide (READ FIRST!)
├── README.md                                  # This file
├── stellarisBimotorPWM2brushedWRelay/         # Energia sketch folder
│   └── stellarisBimotorPWM2brushedWRelay.ino # Main sketch file
└── [other Energia libraries and config files, optional]
```

---

## Getting Started (Quick Start)

### 1. Download & Install Energia
```
Visit: http://energia.nu/download/
Download Windows version
Extract to: C:\Program Files\Energia
```

### 2. Open the Sketch
```
1. Launch Energia IDE
2. File → Open
3. Navigate to: energia/stellarisBimotorPWM2brushedWRelay/
4. Open: stellarisBimotorPWM2brushedWRelay.ino
```

### 3. Configure Board & Port
```
Tools → Board → "TI LaunchPad w/ LM4F120 (80MHz)"
Tools → Port → COM9
```

### 4. Compile & Upload
```
Sketch → Upload (or Ctrl+U)
Wait for "SUCCESS" message
```

### 5. Monitor Output
```
Tools → Serial Monitor (or Ctrl+Shift+M)
Set baud rate: 115200
Watch for initialization messages
```

---

## File: stellarisBimotorPWM2brushedWRelay.ino

This is the main sketch file containing:
- Motor control logic
- PWM signal measurement via interrupts
- Direction control
- Emergency stop (SW2 button)
- RGB LED status indicators
- Serial debugging output

**Important**: The folder name MUST match the .ino filename for Energia to work correctly!

---

## Comparison: PlatformIO vs Energia

### Which Should I Use?

| Use **Energia** if: | Use **PlatformIO** if: |
|-------------------|----------------------|
| ✅ You want easy uploads | ✅ You prefer command-line |
| ✅ You like Arduino IDE | ✅ You use VS Code |
| ✅ You want stable connections | ✅ You need advanced debugging |
| ✅ You're new to embedded dev | ✅ You want CI/CD integration |
| ✅ Serial bootloader is fine | ✅ You need JTAG debugging |

**Recommendation**: Start with **Energia** to avoid upload issues, then try PlatformIO later if needed.

---

## Features of This Sketch

### Input Signals (from ArduPilot)
- **PB6** (Pin 18): Motor 1 PWM input
- **PB7** (Pin 19): Motor 2 PWM input

### Output Signals (to Motor Controller)
- **PE4** (Pin 34): Motor 1 PWM output (M0PWM6)
- **PE5** (Pin 35): Motor 2 PWM output (M0PWM7)
- **PD0** (Pin 27): Motor 1 direction control
- **PD1** (Pin 28): Motor 2 direction control

### Status & Control
- **PF0** (Pin 48): Emergency stop (SW2 button)
- **PF1** (Pin 39): Red LED (error indicator)
- **PF2** (Pin 41): Blue LED (dead zone indicator)
- **PF3** (Pin 40): Green LED (high-speed indicator)

### Serial Communication
- **Port**: COM9
- **Baud Rate**: 115200 bps
- **Data**: 8 bits, 1 stop bit, no parity

---

## Signal Interpretation

The firmware interprets PWM signals (in microseconds):

| Signal Range | Interpretation | Motor State |
|-------------|-----------------|-------------|
| < 700µs or > 2000µs | **Out of bounds** | STOP (error) |
| 700-900µs | **Dead zone** | STOP (no motion) |
| 900-1400µs | **Forward motion** | Proportional speed |
| > 1400µs | **High-speed forward** | Enhanced power |

### ArduPilot Configuration
```
Typical ArduPilot PWM output for brushed motors:
- 1000µs = Stop
- 1500µs = Mid-range forward
- 2000µs = Full forward
```

---

## Customization Guide

### Change Signal Thresholds

Edit lines ~32-35:

```cpp
#define SIGNAL_MIN 700      // Minimum valid signal
#define SIGNAL_MAX 2000     // Maximum valid signal
#define DEAD_ZONE 900       // Below this = no forward motion
#define FORWARD_THRESHOLD 1400  // Above this = high-speed mode
```

Then recompile and upload.

### Modify LED Behavior

Edit in `processPropulsionMotor1()` and `processPropulsionMotor2()` functions:

```cpp
digitalWrite(LED_GREEN, HIGH);   // Turn on green LED
digitalWrite(LED_BLUE, LOW);     // Turn off blue LED
```

### Add Extra Serial Output

Insert anywhere in code:

```cpp
Serial.print("Debug: ");
Serial.println(debugValue);
```

View in serial monitor (Ctrl+Shift+M).

---

## Troubleshooting

### "Port COM9 not showing"
```
1. Check Windows Device Manager
2. Verify Stellaris USB drivers installed
3. Try different USB cable
4. Reinstall drivers if needed
```

### "Upload hangs"
```
1. Press RST button on board
2. Try upload again
3. Or enter bootloader: Hold RST, press SSO, release RST
```

### "Serial Monitor shows garbage"
```
1. Verify baud rate is 115200
2. Check dropdown at bottom-right of monitor
3. Press RST button after opening monitor
```

### "Compilation errors"
```
1. Verify board selected: Tools → Board
2. Should be: "TI LaunchPad w/ LM4F120 (80MHz)"
3. Check Tools → Port is COM9
4. Restart Energia
```

---

## Serial Monitor Output

### Expected Output After Upload

```
========================================
Stellaris LM4F120 Bi-Motor PWM Controller
Energia IDE Edition
========================================

Initialization complete.
Waiting for PWM signals...

Motor1 - Signal: 1500µs | Output: 128 | Forward motion
Motor2 - Signal: 1500µs | Output: 128 | Forward motion
```

### Diagnostic Messages

| Message | Meaning | Action |
|---------|---------|--------|
| "Out of range" | Signal too high/low | Check signal source |
| "Dead zone" | Signal in dead zone (< 900µs) | Increase throttle |
| "Forward motion" | Normal operation | All good! |
| "High-speed forward" | Above threshold (> 1400µs) | Monitor motor speed |
| "EMERGENCY STOP" | SW2 button pressed | Motors killed for safety |

---

## Hardware Setup

### Minimal Test (No Motors)
```
1. Connect LaunchPad to COM9
2. Build and upload sketch
3. Open serial monitor
4. Watch for initialization messages
```

### Full System Test (With Motors)
```
1. Connect ArduPilot PWM to PB6, PB7
2. Connect motor controller PWM outputs (PE4, PE5)
3. Connect direction pins (PD0, PD1)
4. Provide 5V/12V power to motor controller
5. Upload sketch
6. Test motor response via ArduPilot
7. Press SW2 to verify emergency stop
```

### Pinout Reference

```
LaunchPad Physical Pins (LQFP40):

Motor 1:
  Input Signal:  PB6 (Pin 18)
  PWM Output:    PE4 (Pin 34)
  Direction:     PD0 (Pin 27)

Motor 2:
  Input Signal:  PB7 (Pin 19)
  PWM Output:    PE5 (Pin 35)
  Direction:     PD1 (Pin 28)

Status:
  Emergency:     PF0 (Pin 48) - SW2
  Red LED:       PF1 (Pin 39)
  Blue LED:      PF2 (Pin 41)
  Green LED:     PF3 (Pin 40)

Power:
  GND:           Pins 8, 17, 23, 29, 36, 37, 43
  VCC (3.3V):    Pins 2, 11
```

---

## Development Workflow

### Typical Workflow

```
1. Modify code in Energia
2. Press Ctrl+R to compile
3. Fix any errors (shown in bottom panel)
4. Press Ctrl+U to upload
5. Press Ctrl+Shift+M to open serial monitor
6. Watch output and adjust as needed
7. Repeat from step 1
```

### Useful Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+R | Verify/Compile |
| Ctrl+U | Upload |
| Ctrl+Shift+M | Serial Monitor |
| Ctrl+N | New Sketch |
| Ctrl+O | Open File |
| Ctrl+S | Save |
| Ctrl+F | Find |

---

## Migration from PlatformIO

If you want to switch back to PlatformIO:

```bash
# Original PlatformIO version still exists in src/main.cpp
cd C:\Users\mael\Documents\mael\stellarisBimotorPWM2brushedWRelay
pio run -t upload
```

**Note**: PlatformIO uses OpenOCD (JTAG) which has connection issues. Energia is recommended.

---

## Resources

### Documentation
- **ENERGIA_SETUP.md** - Complete setup instructions (detailed)
- **../README.md** - Project overview
- **../PINOUT.md** - Hardware pinout details
- **../SOLUTION_GUIDE.md** - Troubleshooting guide

### External Links
- **Energia IDE**: http://energia.nu/
- **Reference**: http://energia.nu/reference/
- **TI Community**: https://e2e.ti.com/
- **43oh Forum**: https://forum.43oh.com/ (active Energia community)

---

## Project Metadata

- **Board**: TI LaunchPad (Stellaris) LM4F120
- **Microcontroller**: TM4C123GH6PM
- **CPU Speed**: 80 MHz
- **Flash**: 256 KB
- **RAM**: 32 KB
- **IDE**: Energia 1.8.11E23+
- **Framework**: Arduino-compatible (Wiring)
- **Serial Port**: COM9 @ 115200 baud

---

## License & Credits

- **Original ESP32 Code**: Adapted for Stellaris LM4F120
- **Framework**: Energia (maintained by TI)
- **Compatible with**: ArduPilot autopilot systems
- **Motor Controller**: Brushed DC motors with directional relay

---

## Support & Help

1. **Can't upload?**
   - Check ENERGIA_SETUP.md troubleshooting section
   - Verify COM9 in Device Manager
   - Try power cycle of board

2. **Code questions?**
   - See comments in .ino file
   - Check Energia reference: http://energia.nu/reference/

3. **Hardware issues?**
   - Verify connections match PINOUT.md
   - Test with multimeter for signal verification

4. **Community help?**
   - TI E2E Community: https://e2e.ti.com/
   - Energia Forum: https://forum.43oh.com/
   - 43oh (TI board experts): https://www.43oh.com/

---

## Next Steps

1. **Install Energia** → http://energia.nu/download/
2. **Read ENERGIA_SETUP.md** → Full setup guide
3. **Open the sketch** in Energia
4. **Compile** (Ctrl+R) and verify no errors
5. **Upload** (Ctrl+U) to your board
6. **Monitor** (Ctrl+Shift+M) and test

**Happy coding! 🚀**

---

*Last Updated: April 18, 2026*
*For Energia IDE Users*
