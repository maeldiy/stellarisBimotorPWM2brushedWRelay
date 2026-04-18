# Troubleshooting & Setup Guide

## Initial Setup

### 1. Install PlatformIO IDE in VS Code

```bash
# Open VS Code and press Ctrl+Shift+P
# Type: "Install Extensions"
# Search for "PlatformIO IDE" by PlatformIO
# Click Install
```

### 2. Verify Board Detection

```bash
# In PlatformIO terminal:
pio device list

# Should show COM9 and the board info
```

### 3. Select Project Environment

```bash
# In PlatformIO Home, select:
# - Project: stellarisBimotorPWM2brushedWRelay
# - Environment: lm4f120_launchpad
```

## Common Issues & Solutions

### Issue: "Board not found" or "Unknown board ID"

**Solution:**
1. Update PlatformIO core:
   ```bash
   pio upgrade
   ```

2. Clear cache:
   ```bash
   pio project clean --all
   ```

3. Re-open VS Code

4. Verify platformio.ini has correct board ID:
   ```ini
   board = lm4f120_launchpad
   ```

### Issue: "COM9 not recognized" / Serial Upload Fails

**Solutions:**

1. **Check Windows Device Manager:**
   - Right-click Start → Device Manager
   - Expand "Ports (COM & LPT)"
   - Look for "COM9" using Stellaris ICDI or similar
   - If not listed, driver not installed

2. **Install Stellaris USB Drivers:**
   - Download from TI website: [Stellaris ICDI Drivers](http://www.ti.com/tool/lm4f120_launchpad)
   - Unzip and run installer
   - Restart computer

3. **Update platformio.ini if port changed:**
   ```ini
   monitor_port = COM9
   upload_port = COM9
   ```

4. **Try different USB cable:**
   - Use a quality USB cable with data pins
   - Avoid cheap cables (charge-only)

5. **Reset board:**
   - Press RST button on LaunchPad
   - Wait 2 seconds
   - Try upload again

### Issue: "Build Errors" or "Unknown Pin Definitions"

**Solutions:**

1. **Verify Energia framework is installed:**
   ```bash
   pio platform show titiva
   ```

2. **Check C/C++ IntelliSense:**
   - Press Ctrl+Shift+P
   - Type: "C/C++: Select IntelliSense Engine"
   - Choose "cpptools" (default)

3. **Update includes in src/main.cpp:**
   - Add at top:
   ```cpp
   #include <Arduino.h>  // Energia equivalent
   ```

4. **Clear build cache:**
   ```bash
   pio run -t clean
   pio run
   ```

### Issue: "PWM Not Working" / Motors Not Spinning

**Diagnostics:**

1. **Check Serial Output:**
   ```bash
   pio device monitor  # Baud: 115200
   ```
   - Should show signal readings every 500ms
   - Look for "Motor1 - Signal: XXXX µs"

2. **Verify PWM Signal Input:**
   - Use oscilloscope to check PB6 and PB7
   - Expected: ~1500µs pulse width at 50Hz
   - Check signal amplitude: 3.3V peak

3. **Test PWM Output with LED:**
   - Connect LED to PE4 with 470Ω resistor
   - Upload code
   - Check if LED brightness varies

4. **Verify Direction Pins:**
   ```bash
   # In serial monitor, manually set:
   # digitalWrite(PD0, HIGH);  // Forward
   # Check with multimeter: should show 3.3V
   ```

5. **Check Motor Controller Power:**
   - Verify 5V or 12V power to motor controller
   - Use multimeter to check voltage
   - Verify ground connection

### Issue: "Serial Monitor Shows No Output"

**Solutions:**

1. **Check baud rate:**
   - platformio.ini should have: `monitor_speed = 115200`
   - Serial monitor must match (115200)

2. **Restart serial monitor:**
   ```bash
   # Close any open serial monitors
   pio device monitor --baud 115200
   ```

3. **Reset board after upload:**
   - Press RST button
   - Output should appear

4. **Check USB connection:**
   - Try different USB port
   - Check Device Manager for COM port

### Issue: "Interrupt Errors" / "digitalPinToInterrupt not defined"

**Solution:**

Ensure main.cpp has proper Energia headers. Add to top of file:
```cpp
#include <Arduino.h>
#include <ENERGIA_ARCH_TITIVA>

// Then use:
attachInterrupt(digitalPinToInterrupt(PROP_IN_PIN1), interruptHandler1, CHANGE);
```

### Issue: "Upload timeout" or "Failed to upload"

**Solutions:**

1. **Verify upload speed matches:**
   ```ini
   upload_speed = 115200  ; Must match board DFU speed
   ```

2. **Put board in bootloader mode:**
   - Press and hold RST button
   - Press SSO button once
   - Count to 3
   - Release RST button
   - LED should flash

3. **Try slower upload speed:**
   ```ini
   upload_speed = 57600
   ```

4. **Alternative upload method:**
   ```bash
   # Use LM Flash Programmer from TI
   # Download: http://www.ti.com/tool/lmflash
   ```

## Building & Uploading

### Standard Build Process

```bash
# Build only (no upload)
pio run

# Build and upload
pio run -t upload

# Clean and rebuild
pio run -t clean
pio run -t upload

# Monitor output
pio device monitor --baud 115200
```

### Using VS Code Tasks

1. **Open Command Palette:** Ctrl+Shift+P
2. **Type:** "Tasks: Run Task"
3. **Select:**
   - "Build Project" - compile only
   - "Upload to Board" - compile and upload
   - "Monitor Serial Port" - view serial output
   - "Build and Upload" - full process

### Viewing Detailed Build Output

```bash
# Verbose mode
pio run -v

# Check build directory
cd .pio/build/lm4f120_launchpad
```

## Testing the Installation

### Quick Test

1. **Compile without upload:**
   ```bash
   pio run
   ```
   - Should complete without errors
   - Creates .elf and .hex files

2. **Upload to board:**
   ```bash
   pio run -t upload
   ```
   - LEDs should blink during upload
   - Should complete in <10 seconds

3. **Verify serial output:**
   ```bash
   pio device monitor
   ```
   - Press Ctrl+Shift+M in VS Code
   - Should show:
   ```
   ========================================
   Stellaris LM4F120 Bi-Motor PWM Controller
   ========================================
   
   Initialization complete.
   Waiting for PWM signals...
   ```

## Hardware Verification

### Pin Configuration Check

Use multimeter to verify:

1. **Power Pins:**
   - VCC: 3.3V
   - GND: 0V

2. **Input Signal (PB6, PB7):**
   - Should see pulses around 1500µs width
   - Frequency should be ~50Hz

3. **Output Pins After Upload:**
   - PE4, PE5: Should vary 0-3.3V with PWM
   - PD0, PD1: Should be HIGH/LOW as controlled

4. **Status LED (PF1):**
   - Should flash during upload
   - Should stay lit or flash based on program state

## Performance Tuning

### Increase PWM Frequency

If motors buzz at low frequency, increase PWM frequency in code:

```cpp
// In setup() add:
analogWriteFrequency(1000);  // 1kHz (default)
// or
analogWriteFrequency(10000); // 10kHz (higher frequency, less audible)
```

### Adjust Signal Dead Zone

Edit in main.cpp:
```cpp
#define DEAD_ZONE 900         // Threshold for motion
#define FORWARD_THRESHOLD 1400  // Threshold for high-speed
```

## Getting Help

1. **Check Serial Output** - most issues visible here
2. **Use oscilloscope** - verify signal timing
3. **Check pin connections** - verify soldering/breadboard contacts
4. **Review Official Docs:**
   - [TM4C123G Datasheet](http://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf)
   - [Energia Reference](http://energia.nu/reference.html)
   - [LaunchPad Manual](http://www.ti.com/lit/ug/spmu289c/spmu289c.pdf)

5. **PlatformIO Documentation:**
   - https://docs.platformio.org/
   - https://docs.platformio.org/en/latest/boards/titiva/lm4f120_launchpad.html

## Emergency Recovery

### Board Won't Respond to Uploads

1. **Disconnect USB**
2. **Hold RST button**
3. **Connect USB (still holding RST)**
4. **Wait 3 seconds**
5. **Release RST**
6. **Try upload immediately**

### Hard Reset via Command Line

```bash
# Erase all flash memory
pio run -t erase

# Then upload fresh
pio run -t upload
```

### Factory Reset Using LM Flash Programmer

- Download from [TI website](http://www.ti.com/tool/lmflash)
- Load latest firmware image
- Erase device
- Re-upload via PlatformIO
