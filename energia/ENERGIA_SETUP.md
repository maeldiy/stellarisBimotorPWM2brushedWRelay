# Energia IDE - Setup Guide for Stellaris LM4F120

## What is Energia?

**Energia** is an Arduino-compatible IDE built specifically for TI microcontrollers (Stellaris/TM4C series). It's maintained by TI and provides a simpler development experience than using OpenOCD directly.

**Key Advantages over PlatformIO with OpenOCD:**
- ✅ Native support for Stellaris/TM4C boards
- ✅ Serial bootloader upload (no JTAG issues)
- ✅ Built-in serial monitor
- ✅ Arduino-compatible API (same as PlatformIO)
- ✅ Easier to use for beginners
- ✅ Active TI community support

---

## Installation

### Step 1: Download Energia

1. Visit: **http://energia.nu/**
2. Click **Download** (or go directly to: http://energia.nu/download/)
3. Choose **Windows** version
4. Download the latest release (currently 1.8.11E23)

### Step 2: Install Energia

```
1. Extract the downloaded .zip file
   - Recommended location: C:\Program Files\Energia

2. The extracted folder contains:
   - energia.exe (the IDE)
   - No installation needed - it's portable!

3. Create shortcut to desktop (optional)
   - Right-click energia.exe → Send to → Desktop (create shortcut)
```

### Step 3: Install Board Package

First time you run Energia, you may need to install the TM4C123 board package:

```
1. Launch energia.exe
2. Go to Tools → Board → Boards Manager
3. Search for "TM4C123" or "Stellaris"
4. Find "TM4C123 Boards" or "Energia - TI Boards"
5. Click Install (downloads automatically)
6. Wait for installation to complete
```

### Step 4: Select Your Board

```
1. Tools → Board
2. Select: "TI LaunchPad w/ LM4F120 (80MHz)" 
   or "TI LaunchPad (Stellaris) w/ LM4F120"
3. Tools → Port → Select COM9
```

---

## Using Energia

### Opening the Sketch

```
1. Launch Energia
2. File → Open
3. Navigate to:
   C:\Users\mael\Documents\mael\stellarisBimotorPWM2brushedWRelay\energia\stellarisBimotorPWM2brushedWRelay\

4. Select: stellarisBimotorPWM2brushedWRelay.ino
5. Click Open
```

### Building the Sketch

```
1. Sketch → Verify/Compile
   (Or press: Ctrl+R)

Expected output:
  Binary sketch size: 11,860 bytes (of a 262,144 byte maximum)
  Sketch size now: 11,860 bytes (estimated max: 273,408 bytes)
```

### Uploading to Board

```
1. Connect LaunchPad via USB (COM9)
2. Sketch → Upload
   (Or press: Ctrl+U)

3. Press RST button if upload hangs

Expected output:
  Uploading via serial port...
  DONE
  [SUCCESS] Firmware uploaded!
```

---

## Serial Monitor

### Viewing Output

```
1. Tools → Serial Monitor
   (Or press: Ctrl+Shift+M)

2. Set baud rate to: 115200
3. You should see:
   ========================================
   Stellaris LM4F120 Bi-Motor PWM Controller
   Energia IDE Edition
   ========================================
   
   Initialization complete.
   Waiting for PWM signals...
```

### Sending Commands (optional)

```
1. Type in the text box at bottom
2. Press Send (or Enter)
3. Code can read this via Serial.read()
```

---

## Project Structure

The Energia sketch is organized as:

```
energia/
└── stellarisBimotorPWM2brushedWRelay/
    └── stellarisBimotorPWM2brushedWRelay.ino    # Main sketch
```

**Note**: Energia folders must follow this naming convention:
- Folder name must match .ino filename
- Example: `mySketch.ino` → folder must be `mySketch/`

---

## Differences from PlatformIO Version

| Aspect | PlatformIO | Energia |
|--------|-----------|---------|
| **Upload Method** | OpenOCD (JTAG) ⚠️ problematic | Serial bootloader ✅ reliable |
| **IDE** | Command-line + VS Code | Standalone IDE |
| **Learning Curve** | Steeper (CLI-based) | Gentler (IDE-based) |
| **Speed** | Fast uploads | Standard speed |
| **Code Changes** | None needed | Minimal (.ino format) |
| **Community** | Large PlatformIO community | Active TI/Energia community |

---

## Code Compatibility

### What Changed from PlatformIO Version:
1. **File extension**: `.cpp` → `.ino`
2. **Removed includes**: `energia_compat.h` not needed (Energia has native support)
3. **API functions**: All exactly the same
   - `pinMode()`, `digitalWrite()`, `analogWrite()`
   - `Serial.begin()`, `Serial.print()`
   - `attachInterrupt()`, `micros()`, `millis()`
   - `map()`, `noInterrupts()`, `interrupts()`

### Energia-Specific Notes:
- Pin definitions work with Energia naming (e.g., `PF_0`, `PB_6`, `PE_4`)
- `Serial` is `Serial` (just like Arduino)
- Energia automatically includes necessary headers
- No need for compatibility layer

---

## Troubleshooting

### Issue: Board Not Detected

```
1. Check Device Manager (COM9 should exist)
2. Install Stellaris USB drivers if needed:
   - Visit: https://www.ti.com/tool/lm4f120_launchpad
   - Download "Stellaris USB Drivers"

3. In Energia:
   - Tools → Port → (refresh list)
   - Select COM9
```

### Issue: Upload Fails

```
1. Press RST button on board
2. Try upload again within 10 seconds
3. Or: Hold RST, press SSO, release RST for bootloader mode
4. Try again immediately
```

### Issue: Serial Monitor Shows Garbage

```
1. Verify baud rate is 115200 (critical!)
2. Check serial monitor dropdown at bottom-right
3. Restart serial monitor
4. Press RST button on board
```

### Issue: "Sketch is not a valid Energia sketch"

```
1. Verify folder structure:
   - File: stellarisBimotorPWM2brushedWRelay/stellarisBimotorPWM2brushedWRelay.ino
   - Folder must match filename!

2. If copied incorrectly:
   - Close Energia
   - Rename folder to match .ino filename
   - Reopen Energia
```

---

## Advanced Tips

### Modify Signal Thresholds

Edit in the sketch (around line 32):

```cpp
#define SIGNAL_MIN 700      // Minimum valid signal
#define SIGNAL_MAX 2000     // Maximum valid signal
#define DEAD_ZONE 900       // Below this = no forward motion
#define FORWARD_THRESHOLD 1400  // Above this = high-speed mode
```

Then upload again with Ctrl+U.

### Adjust PWM Frequency

Energia uses standard Arduino PWM frequency. To change:

```cpp
// In setup(), add:
analogWriteFrequency(10000);  // 10kHz instead of default 1kHz
```

### Add Debug Output

Insert anywhere in code:

```cpp
Serial.print("Variable: ");
Serial.println(myVariable);
```

Then view in serial monitor.

---

## Energia vs Arduino vs Energia

```
Arduino IDE
    ↓
    ├→ Arduino (for Arduino boards)
    ├→ Energia (for TI boards) ← YOU ARE HERE
    └→ mbed IDE (for ARM boards)

All use the same Arduino API!
```

---

## Quick Reference

| Action | Keyboard | Menu |
|--------|----------|------|
| Verify/Compile | Ctrl+R | Sketch → Verify/Compile |
| Upload | Ctrl+U | Sketch → Upload |
| Serial Monitor | Ctrl+Shift+M | Tools → Serial Monitor |
| Open File | Ctrl+O | File → Open |
| New Sketch | Ctrl+N | File → New |
| Save | Ctrl+S | File → Save |

---

## Next Steps

1. ✅ **Install Energia** from http://energia.nu/
2. ✅ **Select board** as TI LaunchPad LM4F120
3. ✅ **Select port** as COM9
4. ✅ **Open sketch** from `energia/stellarisBimotorPWM2brushedWRelay/`
5. ✅ **Compile** with Ctrl+R
6. ✅ **Upload** with Ctrl+U
7. ✅ **Monitor** with Ctrl+Shift+M

---

## Resources

- **Energia Website**: http://energia.nu/
- **Energia Reference**: http://energia.nu/reference/
- **Board Support**: https://github.com/energia/Energia
- **TI LaunchPad Docs**: http://www.ti.com/tool/lm4f120_launchpad
- **Community Forum**: https://forum.43oh.com/ (43oh.com)

---

## Comparison: How to Switch Back to PlatformIO

If you prefer PlatformIO later:

```bash
# From project root:
pio run -t upload

# Uses src/main.cpp with OpenOCD (requires bootloader mode)
```

Both methods work! Energia is just easier/more stable for serial uploads.

---

*Created: April 18, 2026*
*For Energia 1.8.11E23 and TM4C123 boards*
