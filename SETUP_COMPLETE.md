# Setup Complete Summary

## ✅ Project Status

Your Stellaris LM4F120 Bi-Motor PWM Controller project is now fully set up and pushed to GitHub!

**Repository**: https://github.com/maeldiy/stellarisBimotorPWM2brushedWRelay

---

## 📁 Project Structure

```
stellarisBimotorPWM2brushedWRelay/
├── src/
│   └── main.cpp                 # Main firmware (adapted for LM4F120)
├── .vscode/
│   ├── settings.json            # VS Code editor configuration
│   ├── tasks.json               # Build & upload tasks
│   ├── launch.json              # Debug configuration
│   └── extensions.json          # Recommended extensions
├── platformio.ini               # PlatformIO configuration
├── README.md                    # Project overview & setup guide
├── PINOUT.md                    # Detailed pinout documentation
├── TROUBLESHOOTING.md           # Common issues & solutions
├── SETUP_COMPLETE.md            # This file
└── .gitignore                   # Git ignore patterns
```

---

## 🎯 Key Adaptations from ESP32 to Stellaris LM4F120

### Hardware Changes

| Feature | ESP32 | LM4F120 |
|---------|-------|---------|
| **Input PWM 1** | GPIO5 | PB6 (Pin 18) |
| **Input PWM 2** | GPIO26 | PB7 (Pin 19) |
| **Output PWM 1** | GPIO4 | PE4 (Pin 34) / M0PWM6 |
| **Output PWM 2** | GPIO32 | PE5 (Pin 35) / M0PWM7 |
| **Forward 1** | GPIO23 | PD0 (Pin 27) |
| **Forward 2** | GPIO33 | PD1 (Pin 28) |
| **Reversal Input** | GPIO14 | PF0 (Pin 48) - SW2 |
| **Reversal Output** | GPIO27 | PA6 (Pin 16) |
| **Status LED** | GPIO2 | PF1 (Red) / PF2 (Blue) / PF3 (Green) |
| **Serial** | 115200 | 115200 ✓ COM9 |
| **Framework** | Arduino ESP32 | Arduino Energia (TM4C123) |

### Code Changes

1. **Pin Definitions**: Converted to Stellaris port notation (e.g., `PA_6`, `PB_6`)
2. **PWM Measurement**: Implemented interrupt handlers for accurate pulse duration measurement
3. **Status Indicators**: Expanded to RGB LED for better diagnostics
4. **Serial Communication**: Maintained at 115200 baud for COM9
5. **PWM Output**: Uses M0PWM module (native hardware PWM on PE4/PE5)

---

## 🚀 Quick Start Guide

### Step 1: Install PlatformIO IDE

1. Open **VS Code**
2. Go to **Extensions** (Ctrl+Shift+X)
3. Search for "**PlatformIO IDE**"
4. Click **Install**
5. Reload VS Code (Ctrl+K, Ctrl+R)

### Step 2: Open the Project

1. **File → Open Folder**
2. Select: `C:\Users\mael\Documents\mael\stellarisBimotorPWM2brushedWRelay`
3. PlatformIO will auto-detect and load the environment

### Step 3: Verify Board Connection

```bash
# In PlatformIO terminal (Ctrl+` to open):
pio device list

# Should show COM9 with Stellaris device info
```

### Step 4: Build and Upload

**Option A: Using VS Code Tasks**
1. Press `Ctrl+Shift+B` (Build)
2. Or `Ctrl+Shift+P` → search "Tasks: Run Task"
3. Select "Upload to Board"

**Option B: Using Terminal**
```bash
pio run -t upload
```

### Step 5: Monitor Serial Output

```bash
pio device monitor  # Automatically uses COM9 @ 115200 baud
```

Expected output:
```
========================================
Stellaris LM4F120 Bi-Motor PWM Controller
========================================

Initialization complete.
Waiting for PWM signals...
```

---

## 📋 Pin Allocation Summary

### Motor 1 Pins (Physical Location)
- **Input Signal**: PB6 (Pin 18) - receives PWM from ArduPilot
- **Output PWM**: PE4 (Pin 34) - drives motor controller
- **Direction**: PD0 (Pin 27) - controls direction relay
- **Spare**: PA2, PA4 - available for additional control

### Motor 2 Pins (Physical Location)
- **Input Signal**: PB7 (Pin 19) - receives PWM from ArduPilot
- **Output PWM**: PE5 (Pin 35) - drives motor controller
- **Direction**: PD1 (Pin 28) - controls direction relay
- **Spare**: PA3, PA5 - available for additional control

### Control Pins
- **Emergency Stop**: PF0 (Pin 48) - SW2 button (active LOW)
- **Stop Output**: PA6 (Pin 16) - signals reversal/emergency mode
- **Status LEDs**: PF1 (Red), PF2 (Blue), PF3 (Green)

---

## 🔧 Configuration Details

### Serial Communication
- **Port**: COM9
- **Baud Rate**: 115200 bps
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None

### PWM Measurement
- **Input Pins**: PB6, PB7 (interrupt-based)
- **Measurement Method**: Rising/falling edge interrupts
- **Units**: Microseconds (µs)
- **Frequency**: 50Hz typical (ArduPilot standard)

### Signal Interpretation
- **< 700µs or > 2000µs**: Error - motor stops
- **700-900µs**: Dead zone - no motion
- **900-1400µs**: Forward motion - proportional speed
- **> 1400µs**: High-speed mode

### PWM Output
- **Frequency**: 1kHz (1000 Hz)
- **Resolution**: 8-bit (0-255)
- **Voltage Level**: 3.3V logic
- **Output Pins**: PE4, PE5 (M0PWM6, M0PWM7)

---

## 📚 Documentation Files

### README.md
- Project overview
- Features and capabilities
- Hardware pinout table
- Installation steps
- Signal interpretation
- Status LED indicators
- References to official datasheets

### PINOUT.md
- Detailed pin mappings
- PWM capability matrix
- Port pin definitions (LQFP40)
- Special functions
- Development rationale
- Compatibility notes

### TROUBLESHOOTING.md
- Common issues and solutions
- Serial communication problems
- Board detection issues
- PWM verification steps
- Hardware diagnostics
- Emergency recovery procedures
- Performance tuning guide

---

## 🔌 Hardware Connections Required

### Inputs (from ArduPilot Flight Controller)
- **Channel 1 PWM** → PB6 (Pin 18)
- **Channel 2 PWM** → PB7 (Pin 19)
- **GND** → LaunchPad GND

### Outputs (to Motor Controller)
- **PWM 1** → PE4 (Pin 34) → Motor 1 speed controller
- **PWM 2** → PE5 (Pin 35) → Motor 2 speed controller
- **DIR 1** → PD0 (Pin 27) → Motor 1 relay
- **DIR 2** → PD1 (Pin 28) → Motor 2 relay
- **GND** → Motor controller GND (common ground)

### Emergency Stop
- **SW2 Button** (PF0, Pin 48) - already on board
- Triggers reversal/emergency stop signal

### Power Supply
- USB power (during programming) OR
- External 5V/12V power to LaunchPad VIN

---

## 🟢 LED Status Indicators

| Red LED | Blue LED | Green LED | Status |
|---------|----------|-----------|--------|
| OFF | OFF | OFF | Boot/Normal operation |
| ON | OFF | OFF | Signal error (out of bounds) |
| OFF | ON | OFF | Dead zone (no motion) |
| OFF | OFF | ON | High-speed forward mode |
| ON | - | - | Emergency stop active |

---

## 🔐 Safety Features

1. **Dead Zone Detection**: Motors won't start below 900µs
2. **Out-of-Bounds Protection**: Stops motors if signal > 2000µs
3. **Emergency Stop**: SW2 button immediately kills all motors
4. **Signal Validation**: Continuous checking for valid PWM ranges
5. **LED Diagnostics**: Visual feedback for system state

---

## 📊 Testing the Setup

### Minimal Test (No Motors Connected)

1. Connect LaunchPad via USB to COM9
2. Build and upload code
3. Open serial monitor
4. Observe initialization messages
5. Check that LEDs respond to button presses

### Full System Test (With Motors)

1. Connect motor controller PWM inputs to PE4/PE5
2. Connect direction pins PD0/PD1
3. Provide 5V/12V to motor controller
4. Upload code
5. Send test signal (1500µs PWM) to PB6/PB7
6. Verify motor speed response
7. Press SW2 to verify emergency stop

---

## 🌐 GitHub Repository

**URL**: https://github.com/maeldiy/stellarisBimotorPWM2brushedWRelay

**Contents Uploaded**:
- ✅ Main firmware (src/main.cpp)
- ✅ PlatformIO configuration
- ✅ VS Code workspace settings
- ✅ Comprehensive documentation
- ✅ Build and debugging configurations
- ✅ .gitignore for clean repository

---

## 📦 Dependencies & Libraries

The project uses:
- **PlatformIO TiTiva** platform (includes Energia framework)
- **Arduino-compatible Core** for LM4F120
- No external libraries required (uses hardware interrupts)

### Optional Additions
- For debugging: `OpenOCD` / `GDB`
- For analysis: oscilloscope (recommended for signal verification)

---

## 🎓 Development Workflow

### Build Process
```bash
# Full rebuild
pio run -t clean
pio run

# Quick build (incremental)
pio run

# Verbose output
pio run -v
```

### Upload Process
```bash
# Standard upload
pio run -t upload

# Erase and reprogram (if corruption)
pio run -t erase
pio run -t upload
```

### Monitoring
```bash
# Open serial monitor
pio device monitor

# With filters
pio device monitor --filter=hexlify
```

---

## 💡 Tips & Best Practices

1. **Before Each Upload**
   - Check USB connection
   - Verify COM9 in Device Manager
   - Press RST button if upload fails

2. **Signal Monitoring**
   - Use oscilloscope to verify incoming PWM
   - Check 3.3V logic levels
   - Verify ~50Hz frequency from ArduPilot

3. **Debugging**
   - Enable serial monitor before uploading
   - Watch for error messages
   - Check LED status for visual feedback

4. **Code Modifications**
   - Edit signal thresholds in #define section
   - Adjust PWM frequency as needed
   - Keep interrupts as-is (critical for timing)

---

## 🆘 Quick Troubleshooting

| Issue | Quick Fix |
|-------|-----------|
| **USB not detected** | Verify driver installation, try different USB port |
| **Upload timeout** | Press RST button, try upper baud rate |
| **No serial output** | Check baud rate matches (115200), reset board |
| **Motor won't move** | Verify signal 900-1400µs, check motor power |
| **Wrong direction** | Swap PD0 or PD1 logic (swap HIGH/LOW) |
| **Erratic behavior** | Check signal noise with oscilloscope |

For detailed troubleshooting, see **TROUBLESHOOTING.md**

---

## 📞 Next Steps

1. ✅ **Install PlatformIO IDE** in VS Code
2. ✅ **Open project folder** in VS Code
3. ✅ **Verify board detection** via `pio device list`
4. ✅ **Build and upload** the firmware
5. ✅ **Connect motor hardware** as documented
6. ✅ **Test with test signals** before flight system

---

## 📖 Additional Resources

- **Stellaris LM4F120 Homepage**: https://www.ti.com/product/LM4F120H5QR
- **Datasheet**: http://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf
- **LaunchPad User Guide**: http://www.ti.com/lit/ug/spmu289c/spmu289c.pdf
- **Energia Project**: http://energia.nu/
- **PlatformIO Docs**: https://docs.platformio.org/
- **ArduPilot Wiki**: https://ardupilot.org/

---

## ✨ Project Ready!

Your Stellaris LM4F120 bi-motor controller is now fully configured and ready for development. All code has been adapted from the ESP32 version, thoroughly documented, and pushed to GitHub.

**Start building!** 🚀

For questions or issues, refer to the documentation files or check GitHub Issues.

---

*Generated: April 18, 2026*
*Last Updated: Initial Setup Complete*
