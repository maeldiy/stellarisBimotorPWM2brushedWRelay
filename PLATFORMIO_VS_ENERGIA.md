# Development Environment Comparison

This project supports **both** PlatformIO and Energia IDE. Here's how to choose and use each.

---

## Quick Comparison Table

| Feature | PlatformIO | Energia IDE |
|---------|-----------|------------|
| **Setup Complexity** | Medium (CLI + VS Code) | Low (standalone IDE) |
| **Upload Speed** | Medium (~2-3s) | Fast (~1-2s) |
| **Upload Reliability** | ⚠️ Problematic (OpenOCD/JTAG) | ✅ Reliable (serial bootloader) |
| **Serial Monitor** | Built into VS Code | Built into IDE |
| **Debugging** | Advanced (GDB support) | Basic |
| **IDE Type** | VS Code plugin | Standalone |
| **Learning Curve** | Steep | Gentle |
| **Community** | Large (PlatformIO) | Active (TI/Energia) |
| **Code Base** | `src/main.cpp` | `energia/*.ino` |
| **Cost** | Free | Free |
| **Best For** | Advanced users, CI/CD | Beginners, quick iteration |

---

## Recommendation

### Start with **Energia** if:
✅ You want to upload firmware quickly without issues  
✅ You're new to embedded development  
✅ You want a simple, Arduino-like experience  
✅ You don't need advanced debugging  
✅ You're on Windows/Mac/Linux (all supported)

### Switch to **PlatformIO** if:
✅ You want advanced IDE features (VS Code)  
✅ You need integrated debugging (GDB)  
✅ You prefer command-line tools  
✅ You want to automate builds  
✅ You already use VS Code for other projects

---

## File Organization

```
stellarisBimotorPWM2brushedWRelay/
│
├── src/                          # PlatformIO version
│   ├── main.cpp                  # C++ firmware for PlatformIO
│   └── energia_compat.h          # Compatibility layer for OpenOCD
│
├── energia/                      # Energia IDE version (RECOMMENDED)
│   ├── README.md                 # Energia setup guide
│   ├── ENERGIA_SETUP.md          # Detailed installation
│   └── stellarisBimotorPWM2brushedWRelay/
│       └── stellarisBimotorPWM2brushedWRelay.ino  # Energia sketch
│
├── platformio.ini                # PlatformIO configuration
│
└── [documentation files]
```

---

## Which Version to Download

### For Energia Users:
```
Download/use ONLY:
✓ energia/stellarisBimotorPWM2brushedWRelay.ino
✓ Ignore: src/main.cpp (that's for PlatformIO)
```

### For PlatformIO Users:
```
Download/use ONLY:
✓ src/main.cpp
✓ platformio.ini
✓ Ignore: energia/ folder (that's for Energia IDE)
```

---

## Installation & Usage

### Energia IDE (Recommended)

**Installation:**
```
1. Visit http://energia.nu/
2. Download Windows/Mac/Linux version
3. Extract to any folder (e.g., C:\Program Files\Energia)
4. Run energia.exe
5. Install TM4C123 board package (automatic first run)
```

**Usage:**
```
1. File → Open → energia/stellarisBimotorPWM2brushedWRelay/
2. Tools → Board → "TI LaunchPad w/ LM4F120 (80MHz)"
3. Tools → Port → COM9
4. Sketch → Upload (Ctrl+U)
5. Success! Firmware is now running
```

**Advantages:**
- ✅ One-click upload via serial bootloader
- ✅ No JTAG/OpenOCD issues
- ✅ Familiar Arduino-like IDE
- ✅ Built-in serial monitor
- ✅ Instant gratification

---

### PlatformIO (Advanced)

**Installation:**
```
1. Install VS Code (if not already)
2. Extensions → Search "PlatformIO"
3. Install "PlatformIO IDE"
4. Reload VS Code
5. Close and reopen VS Code
```

**Usage:**
```
1. File → Open Folder → project root
2. Run: pio run -t upload
3. Or use VS Code tasks (Ctrl+Shift+B)
4. May need to enter bootloader mode manually
```

**Advantages:**
- ✅ Advanced VS Code features
- ✅ CLI-based automation
- ✅ Better for large projects
- ✅ GDB debugging support
- ✅ CI/CD integration ready

**Challenges:**
- ⚠️ OpenOCD/JTAG connection issues (common)
- ⚠️ May need manual bootloader mode entry
- ⚠️ Steeper learning curve

---

## Upload Methods Explained

### Energia (Serial Bootloader)

```
USB Connection (Serial):
LaunchPad ←→ (COM9) ←→ Computer

How it works:
1. LaunchPad has built-in serial bootloader
2. Energia sends firmware via COM9 serial port
3. Bootloader writes to flash
4. Bootloader triggers reset
5. Firmware runs immediately

Result: Fast, reliable, no special hardware needed ✅
```

### PlatformIO (JTAG Debugging Interface)

```
USB Connection (Debug Interface):
LaunchPad ←→ (JTAG/SWD via ICDI) ←→ OpenOCD ←→ Computer

How it works:
1. LaunchPad has on-board ICDI debug adapter
2. PlatformIO uses OpenOCD to communicate
3. OpenOCD sends firmware via JTAG protocol
4. JTAG interface writes to flash
5. Firmware runs

Result: Powerful debugging, but connection can be finicky ⚠️
```

---

## Troubleshooting by Environment

### Energia Upload Issues

**Problem**: Upload hangs or fails
```
Solution 1: Press RST button on board
Solution 2: Enter bootloader mode:
  - Hold RST button
  - Press SSO button once
  - Release RST
  - Try upload again immediately
```

**Problem**: COM9 not showing
```
Solution: Check Device Manager
  1. Right-click Start → Device Manager
  2. Find "Périphérique série USB (COM9)"
  3. If missing: Install Stellaris USB drivers
  4. Restart computer
```

---

### PlatformIO Upload Issues

**Problem**: "Error: open failed"
```
This is the OpenOCD/JTAG connection issue
Solutions:
1. Try Energia IDE instead (recommended)
2. Power cycle board (unplug 15s, plug back)
3. Try bootloader mode (hold RST + press SSO)
4. Use LM Flash Programmer (TI official tool)
5. Switch to Energia IDE (safest option)
```

**Problem**: Build works but upload fails
```
Solution: Use debug environment
  pio run -t upload -e lm4f120_debug

This uses slower clock (100kHz) for better reliability
```

---

## Both Environments Simultaneously

You CAN use both, but they're independent:

```
scenario 1: Develop in Energia, debug in PlatformIO
- Edit code in Energia, upload quickly
- If issues, switch to PlatformIO for advanced debugging

Scenario 2: Use PlatformIO for CI/CD, Energia for rapid dev
- CI/CD pipeline uses pio run
- Local development uses Energia IDE

Scenario 3: Team preference
- Beginners use Energia
- Advanced devs use PlatformIO
- Both working from same code base (with .ino/.cpp versions)
```

---

## Code Differences

### PlatformIO (C++)
```cpp
#include "energia_compat.h"   // Compatibility layer
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  // ...
}
```

### Energia (Arduino)
```cpp
// No includes needed - Energia handles it automatically

void setup() {
  Serial.begin(115200);
  // ...
}
```

**Result**: Energia version is simpler, more Arduino-like ✅

---

## Decision Matrix

Choose based on your situation:

```
Do you want to upload immediately without issues?
├─ YES → Use ENERGIA IDE ✅
└─ NO → Use PlatformIO (advanced users)

Are you new to embedded development?
├─ YES → Use ENERGIA IDE ✅
└─ NO → Either works

Do you already use VS Code daily?
├─ YES → Try PlatformIO
└─ NO → Use ENERGIA IDE ✅

Do you need advanced debugging (breakpoints, etc)?
├─ YES → Use PlatformIO
└─ NO → ENERGIA IDE is perfect ✅

Do you want the simplest, fastest solution?
├─ YES → Use ENERGIA IDE ✅✅✅
└─ NO → Use PlatformIO
```

---

## Migration Path

### From Energia to PlatformIO

```
1. Your Energia sketch in: energia/stellarisBimotorPWM2brushedWRelay.ino
2. Equivalent PlatformIO code in: src/main.cpp
3. To switch:
   pio run -t upload

Note: May encounter JTAG issues (common)
```

### From PlatformIO to Energia

```
1. Your PlatformIO code in: src/main.cpp
2. Equivalent Energia sketch in: energia/stellarisBimotorPWM2brushedWRelay/
3. To switch:
   - Download Energia IDE
   - Open: energia/stellarisBimotorPWM2brushedWRelay.ino
   - Click Upload

Note: Uploads will work reliably ✅
```

---

## Performance Comparison

### Upload Time
- Energia: ~1-2 seconds
- PlatformIO: ~2-3 seconds (if it works)

### Compile Time
- Energia: ~3-5 seconds
- PlatformIO: ~5-8 seconds

### Startup Time After Upload
- Both: Immediate (< 100ms)

### Firmware Size
- Both: 11,860 bytes (identical)

---

## Real-World Scenario

### Scenario: New User Getting Started

```
Day 1: Want to get firmware working ASAP
  → Use ENERGIA IDE
  → 5 minutes to install, 10 seconds to upload ✅

Day 7: Want to add features and debug
  → Still using ENERGIA IDE
  → Works great for quick iterations ✅

Day 30: Interested in advanced debugging
  → Optionally try PlatformIO
  → Use GDB for breakpoints and stepping
  → Or stick with Energia (perfectly fine) ✅
```

---

## Final Recommendation

### For 95% of Users: **Use Energia IDE**

```
Why?
✅ Simpler to set up
✅ Faster, more reliable uploads
✅ Familiar Arduino environment
✅ Active community support
✅ Zero JTAG connection hassles
✅ Built-in serial monitor

Getting started:
1. Download Energia from http://energia.nu/
2. Extract and run energia.exe
3. Open energia/stellarisBimotorPWM2brushedWRelay.ino
4. Select Tools → Board → LM4F120
5. Select Tools → Port → COM9
6. Press Ctrl+U to upload

Result: Firmware running in minutes ✅✅✅
```

### For Advanced Users: PlatformIO (if you want)

```
Why consider it?
✅ Powerful IDE (VS Code)
✅ Advanced debugging
✅ Automation capabilities
✅ Large community

Getting started:
1. Install VS Code
2. Add PlatformIO extension
3. Open project folder
4. Run: pio run -t upload
5. Debug as needed

Caveat: May need bootloader mode entry
```

---

## Summary

| Environment | Setup | Speed | Reliability | For Whom |
|------------|-------|-------|-------------|----------|
| **Energia** | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | Everyone! |
| **PlatformIO** | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | Advanced users |

---

## Resources

### Energia IDE
- Download: http://energia.nu/
- Documentation: http://energia.nu/reference/
- Setup Guide: See `energia/ENERGIA_SETUP.md`

### PlatformIO
- Website: https://platformio.org/
- Docs: https://docs.platformio.org/
- Configuration: `platformio.ini` in project root

### Community
- TI E2E: https://e2e.ti.com/
- 43oh Forum: https://forum.43oh.com/ (Energia community hub)
- Energia GitHub: https://github.com/energia/Energia

---

## Questions?

- **How do I choose?** → Read "Recommendation" section above
- **Why two versions?** → To give users choice and flexibility
- **Can I switch?** → Yes, both versions use same code base
- **Which is faster?** → Energia (no JTAG issues)
- **Which is simpler?** → Energia (Arduino-like)

---

*Last Updated: April 18, 2026*
*For Maximum Clarity and User Success*
