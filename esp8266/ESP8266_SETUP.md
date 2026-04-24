# ESP8266 Bi-Motor PWM Controller Setup Guide

## Overview

**ESP8266** is the perfect alternative to Stellaris LM4F120 for this project:

| Feature | ESP8266 ✅ | Stellaris ⚠️ |
|---------|----------|-----------|
| **Setup Complexity** | Super simple | Complex (driver issues) |
| **USB Driver Issues** | None | Current issues |
| **Arduino IDE Support** | Native | Compatibility layers |
| **Board Cost** | $5-15 | $20+ |
| **WiFi** | Built-in! | No |
| **Availability** | Everywhere | Limited |
| **Documentation** | Excellent | TI-specific |

---

## What You Need

### Hardware
- **ESP8266 Development Board**
  - NodeMCU 1.0 (recommended) - ~$7
  - WeMos D1 Mini - ~$5
  - Generic ESP8266 - ~$8
  
- **USB Cable** (Micro USB for most boards)
- **Motor Controller** (same as before)
- **Motors** (same as before)

### Software
- **Arduino IDE** (free, download from arduino.cc)
- **ESP8266 Board Support** (installed via Arduino IDE)

---

## Quick Installation (15 minutes)

### Step 1: Install Arduino IDE

1. Download: https://www.arduino.cc/en/software
2. Install normally
3. Launch Arduino IDE

### Step 2: Add ESP8266 Support

1. **File → Preferences**
2. **Additional Boards Manager URLs**: Paste this:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Click **OK**

4. **Tools → Board → Boards Manager**
5. **Search**: "esp8266"
6. **Click Install** on "esp8266 by ESP8266 Community"
7. Wait for installation (~5 minutes)

### Step 3: Connect ESP8266

1. **Plug ESP8266 into USB** (Micro USB port)
2. Windows will install CH340 drivers automatically
3. Open **Device Manager** to verify COM port (should appear)

### Step 4: Configure Arduino IDE

1. **Tools → Board → ESP8266 Boards → NodeMCU 1.0 (ESP-12E Module)**
2. **Tools → Upload Speed → 115200**
3. **Tools → Port → COM# (your ESP8266)**

### Step 5: Load & Upload Sketch

1. **File → Open → esp8266_bimotorPWM.ino**
2. **Sketch → Verify** (should compile)
3. **Sketch → Upload** (10-30 seconds)
4. **Done!** ✅

---

## Pin Mapping

### ESP8266 Pinout

```
┌─────────────────────────────────────┐
│  ESP8266 NodeMCU Development Board  │
├─────────────────────────────────────┤
│ USB Micro (Power + Serial)          │
│                                     │
│ D1 ──────── GPIO5  (Direction 2)    │
│ D2 ──────── GPIO4  (Emergency Stop) │
│ D3 ──────── GPIO0  (Status LED 1)   │
│ D4 ──────── GPIO2  (Status LED 2)   │
│ D5 ──────── GPIO14 (PWM Input 1)    │
│ D6 ──────── GPIO12 (PWM Input 2)    │
│ D7 ──────── GPIO13 (PWM Output 1)   │
│ D8 ──────── GPIO15 (PWM Output 2)   │
│ D0 ──────── GPIO16 (Direction 1)    │
│                                     │
│ GND, 3V3, RST                       │
└─────────────────────────────────────┘
```

### Connection Guide

**From ArduPilot Flight Controller:**
```
ArduPilot CH1 PWM  →  ESP8266 D5 (PWM Input 1)
ArduPilot CH2 PWM  →  ESP8266 D6 (PWM Input 2)
ArduPilot GND      →  ESP8266 GND
```

**To Motor Controller:**
```
ESP8266 D7  →  Motor Controller PWM 1
ESP8266 D8  →  Motor Controller PWM 2
ESP8266 D0  →  Motor Controller Direction 1
ESP8266 D1  →  Motor Controller Direction 2
```

**Emergency Stop Button:**
```
3.3V  →  Button  →  ESP8266 D2 (with internal pullup)
```

**Status LEDs (Optional):**
```
3.3V  →  LED1 (470Ω resistor)  →  ESP8266 D3
3.3V  →  LED2 (470Ω resistor)  →  ESP8266 D4
```

---

## Serial Monitor Output

Once uploaded, open **Tools → Serial Monitor** (115200 baud):

```
================================
ESP8266 Bi-Motor PWM Controller
================================
Board: ESP8266 (NodeMCU/WeMos)
Serial: 115200 baud
Status: Initializing...
✓ PWM inputs configured (D5, D6)
✓ PWM outputs configured (D7, D8)
✓ Direction pins configured (D0, D1)
✓ Emergency stop configured (D2)
✓ Status LEDs configured (D3, D4)
✓ Initialization complete!

Waiting for PWM signals...
```

If you see this: **✅ Working!**

---

## Signal Interpretation

Same as Stellaris version:

| Pulse Width | Behavior |
|-------------|----------|
| **<700µs or >2000µs** | ❌ Out of bounds - motor stops, red LED |
| **700-900µs** | 🔵 Dead zone - motor neutral |
| **900-1400µs** | 🟢 Forward proportional (0-100%) |
| **>1400µs** | 🟢 High-speed mode (100% throttle) |

---

## Uploading (Step-by-Step)

### Successful Upload Looks Like:

```
Compiling...
Sketch uses 12345 bytes (11%) of program storage space...
Uploading 12345 bytes to flash...
........ [progress bars] ........
SHA256: ................
Leaving...
Hard resetting via RTS pin...
```

**Then look at serial monitor - you should see the welcome message!**

### If Upload Fails:

1. **"Port not available"**
   - Check Device Manager for COM port
   - Verify USB cable works
   - Try different USB port

2. **"Connection refused"**
   - Hold D0 (GPIO16) to GND during upload
   - Try again

3. **"COM port timeout"**
   - Try **Tools → Upload Speed → 115200** (slower)
   - Or try 230400

---

## Troubleshooting

### Issue: Sketch compiles but won't upload

**Solution:**
```
1. Check Tools → Port (should show ESP8266 port)
2. Try: Tools → Erase Flash → All Flash Contents
3. Then upload again
```

### Issue: Serial monitor shows garbage

**Problem**: Wrong baud rate
**Solution**: 
```
Tools → Serial Monitor
Check bottom right: should be 115200 baud
```

### Issue: No COM port shows in Arduino IDE

**Problem**: Drivers not installed
**Solution**:
```
1. Device Manager → Other devices (yellow ⚠️)
2. Right-click → Update driver
3. Search automatically for drivers
4. Windows will find CH340 driver
```

### Issue: Upload hangs at "Uploading"

**Solution:**
```
1. Press Reset button on board
2. Or hold GPIO0 to GND during upload
3. Or try different USB port
```

---

## GPIO Voltage Levels

**Important**: ESP8266 operates at **3.3V** (NOT 5V!)

- ✅ Safe: 3.3V inputs/outputs
- ⚠️ Risky: 5V inputs (may damage board)
- ❌ Wrong: 5V direct connection

**Safe connection to 5V devices:**
```
Use voltage divider:
5V device  →  10kΩ resistor  →  ESP8266 input (D5/D6)
                               ↓
                              GND ← 20kΩ resistor
```

For PWM inputs (5V), this is not usually necessary if pulses are short, but recommended for safety.

---

## Development Tools

### Updating Code

Once working, update the sketch anytime:

1. Edit `esp8266_bimotorPWM.ino`
2. **Sketch → Verify** (check for errors)
3. **Sketch → Upload** (10 seconds)
4. Check **Serial Monitor**

### Creating Backups

Save your working version:
```
Copy esp8266_bimotorPWM.ino to safe location
Or commit to Git: git add -A; git commit -m "Working ESP8266 version"
```

---

## Performance Specs

| Metric | Value |
|--------|-------|
| **Clock Speed** | 80 MHz (or 160 MHz) |
| **RAM** | 160 KB (40KB fast + 120KB) |
| **Flash** | 4 MB (typical) |
| **Startup Time** | ~500ms (slower than Stellaris) |
| **Sketch Size** | ~25 KB |
| **PWM Resolution** | 8-bit (0-255) |
| **PWM Frequency** | 50 Hz (adjustable) |
| **Serial Speed** | Up to 2 Mbps (using 115200) |

---

## Bonus Features (ESP8266 Extras)

### WiFi (Optional - Not Used Here)

The ESP8266 has built-in WiFi! You could add:
```cpp
#include <ESP8266WiFi.h>

// Monitor motor status remotely
// Stream telemetry data via UDP
// Control motors via web interface
```

But not required - stick with serial for now.

### OTA (Over-The-Air Updates)

```cpp
#include <ArduinoOTA.h>

// Upload new firmware via WiFi (no USB needed!)
// Set up: ArduinoOTA.begin();
```

Not implemented here, but possible future feature.

---

## Board Recommendations

### NodeMCU 1.0 (Recommended) ⭐⭐⭐
- **Price**: ~$7
- **Size**: 58 x 31 mm
- **Features**: ✅ Built-in USB, ✅ Large, ✅ Easy to use
- **Voltage**: 5V-tolerant inputs (some pins)
- **Where**: AliExpress, Amazon, local stores

### WeMos D1 Mini
- **Price**: ~$5
- **Size**: 34 x 26 mm (tiny!)
- **Features**: ✅ Compact, ✅ Built-in USB
- **Voltage**: 3.3V only
- **Where**: AliExpress, Amazon

### Generic ESP8266 + USB Adapter
- **Price**: ~$8 total
- **Size**: Varies
- **Features**: Cheap
- **Voltage**: 3.3V
- **Downside**: Requires separate USB cable

---

## Getting Started Checklist

- [ ] Download Arduino IDE
- [ ] Install ESP8266 support
- [ ] Connect ESP8266 via USB
- [ ] Select NodeMCU board + COM port
- [ ] Open `esp8266_bimotorPWM.ino`
- [ ] Verify code compiles
- [ ] Upload to board
- [ ] Open Serial Monitor (115200 baud)
- [ ] See "Initialization complete!" message
- [ ] Connect motor PWM inputs to D5, D6
- [ ] Test motor response
- [ ] Done! ✅

---

## Comparison: Stellaris vs ESP8266

| Feature | Stellaris LM4F120 | ESP8266 |
|---------|------------------|---------|
| **Setup Time** | 30+ minutes | 15 minutes |
| **Driver Issues** | ❌ Current problem | ✅ None |
| **USB Support** | JTAG only | USB Serial |
| **Arduino IDE** | Via Energia | Native |
| **Learning Curve** | Steep | Gentle |
| **Performance** | ⭐⭐⭐⭐⭐ (faster) | ⭐⭐⭐ (sufficient) |
| **Cost** | $20+ | $5-15 |
| **WiFi** | No | Yes! |
| **Availability** | Limited | Everywhere |
| **Community** | Small (TI) | Large (Arduino) |
| **First Blink** | 45 minutes | 10 minutes |

**For this project: ESP8266 is the better choice!** ✅

---

## Next Steps

1. **Order ESP8266** (~$7, arrives in 1-2 weeks)
2. **Follow setup guide above** (15 minutes)
3. **Upload sketch** (5 minutes)
4. **Connect motors** (10 minutes)
5. **Test** (5 minutes)
6. **Total**: 1 hour vs 2+ hours with Stellaris

---

## Support Resources

### Arduino IDE
- https://www.arduino.cc/
- https://arduino.cc/en/Guide/

### ESP8266 Community
- https://github.com/esp8266/Arduino
- https://arduino-esp8266.readthedocs.io/
- Forums: https://arduino.cc/forum/

### Tutorials
- Blinking LED: https://create.arduino.cc/projecthub/
- PWM guide: https://create.arduino.cc/projecthub/

---

## Questions?

- **How long does upload take?** → 10-30 seconds (much faster than PlatformIO!)
- **Can I use different ESP8266 board?** → Yes, any ESP8266 works (just select in Arduino IDE)
- **Do I need WiFi?** → No, WiFi is optional. Serial works fine.
- **Is 3.3V enough?** → Yes, works with 5V motor controllers too

---

*Updated: April 19, 2026*
*ESP8266 Alternative - No Driver Issues!*
