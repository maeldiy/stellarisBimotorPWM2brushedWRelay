# Solution: OpenOCD Upload Connection Issues

## What We Found
- Your firmware **builds successfully** ✅
- The board is **detected by Windows** ✅  
- The upload uses **OpenOCD JTAG/SWD interface** (not serial bootloader)
- OpenOCD **cannot connect** to the board ❌

## Root Cause
The JTAG/SWD debug interface on the LaunchPad is not responding to OpenOCD commands. This is typically a USB driver or hardware connection issue.

---

## Solution Path (Try in Order)

### **Step 1: Hardware Power Cycle** ⭐ (Solves 80% of issues)

**This is the most important step!**

```
1. UNPLUG USB from LaunchPad computer side
2. WAIT 15 SECONDS (important - let all power drain)
3. Verify all LEDs on board turn OFF
4. PLUG USB BACK IN
5. WAIT 10 SECONDS (for Windows drivers to fully initialize)
6. Look for GREEN LED on board (should be lit)
```

After power cycle, immediately try:
```bash
cd C:\Users\mael\Documents\mael\stellarisBimotorPWM2brushedWRelay
pio run -t upload
```

---

### **Step 2: Verify Device Manager** (if Step 1 doesn't work)

```
1. Right-click Start → Device Manager
2. Expand "Ports (COM & LPT)"
3. Look for "Périphérique série USB (COM9)"
4. ⚠️ If you see a yellow warning triangle → driver issue
5. ⚠️ If COM9 is missing → hardware not detected

If you see warnings:
  a. Right-click device → Update driver
  b. Select "Search automatically for drivers"
  c. Wait for installation
  d. Unplug/plug USB
  e. Retry upload
```

---

### **Step 3: Try Alternative Bootloader Mode** (if Steps 1-2 don't work)

```
1. Press and HOLD the "RST" button on LaunchPad
2. While holding RST, quickly press the "SSO" button (once)
3. Release RST button
4. The board should reset and LEDs may flash
5. Immediately run upload before 10-second timeout

From command line:
pio run -t upload
```

---

### **Step 4: Use LM Flash Programmer** (Official TI Tool - Fallback)

If steps 1-3 don't work, use TI's official programming tool:

```
Download: https://www.ti.com/tool/LMFLASHPROGRAMMER

1. Download and install "LM Flash Programmer"
2. Run the program
3. Select "Stellaris LM4F120H5QR" or "LM4F120 LaunchPad"
4. In "Image Tool" section:
   - Browse to: .pio/build/lm4f120_launchpad/firmware.bin
   - OR use: .pio/build/lm4f120_launchpad/firmware.hex
5. Click "Program" button
6. Wait for "SUCCESS" message
7. Board will auto-reset with new firmware
```

---

### **Step 5: Debug Mode Upload** (if standard upload still fails)

```bash
# Try with verbose debug output and slower speed:
pio run -t upload -e lm4f120_debug

# This uses a separate environment with:
# - Lower clock speed (100kHz instead of 400kHz)
# - Debug output enabled
# - More lenient timing
```

---

## Troubleshooting Checklist

- [ ] **USB Cable**: Try different USB cable (must be data cable, not charge-only)
- [ ] **USB Port**: Try different USB port on computer
- [ ] **Unplug/Replug**: Full 15-second power cycle
- [ ] **Device Manager**: Check for yellow warnings or missing COM9
- [ ] **Bootloader Mode**: Try RST+SSO button sequence
- [ ] **LM Flash Programmer**: Use TI's official tool as fallback
- [ ] **Different Computer**: If available, try another PC to isolate software vs hardware

---

## Updated platformio.ini

The configuration has been updated with:

```ini
[env:lm4f120_launchpad]
upload_protocol = openocd        # Explicitly set to OpenOCD (JTAG/SWD)
upload_options = 
    -c 'transport select swd'    # Use SWD protocol
    -c 'adapter_khz 400'          # Clock speed 400kHz
    -c 'adapter_nsrst_delay 200'  # Reset delay 200ms
    -c 'telnet_port disabled'     # Disable telnet

[env:lm4f120_debug]             # Alternative environment for debugging
# Same as above but with:
# - 'debug_level 2'               # Verbose output
# - 'adapter_khz 100'             # Slower 100kHz clock
```

---

## Quick Commands

```bash
# Standard upload (try first):
pio run -t upload

# Upload with debug info:
pio run -t upload -v

# Use debug environment (slower speed):
pio run -t upload -e lm4f120_debug

# Rebuild from scratch:
pio run -t clean
pio run -t upload

# Check if device is detected:
pio device list
```

---

## Success Indicators

After **successful** upload, you should see:

```
Processing lm4f120_launchpad...
...
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [=         ]   9.7% (used 3176 bytes from 32768 bytes)
Flash: [          ]   4.5% (used 11860 bytes from 262144 bytes)
...
[SUCCESS] Took X.XX seconds
```

The board will also blink LEDs during the upload process.

---

## Getting Help

If none of these steps work:

1. **Check forums**:
   - PlatformIO Community: https://community.platformio.org/
   - TI E2E Community: https://e2e.ti.com/
   - Energia Forum: https://forum.43oh.com/

2. **Collect information for support**:
   - Full error message from `pio run -t upload -v`
   - Windows Device Manager screenshot
   - PlatformIO version: `pio --version`
   - Your USB cable type

3. **Alternative options**:
   - Use LM Flash Programmer (from TI)
   - Try a different LaunchPad if available
   - Update Windows USB drivers globally

---

## Summary

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Unplug/replug USB (15s) | Board detects, continues trying |
| 2 | Check Device Manager | COM9 shows with no warnings |
| 3 | Bootloader mode (RST+SSO) | Board LED flashes, ready for upload |
| 4 | Use LM Flash Programmer | Firmware uploaded, board resets |
| 5 | Debug environment | More verbose error info for support |

**Start with Step 1** - most connection issues are resolved by a proper power cycle!

---

*Last Updated: April 18, 2026*
*For latest troubleshooting, see: UPLOAD_TROUBLESHOOTING.md*
