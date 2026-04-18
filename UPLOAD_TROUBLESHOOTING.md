# Upload Troubleshooting - OpenOCD Connection Issue

## Current Status
- ✅ Build successful (firmware compiled)
- ✅ COM9 detected by Windows
- ❌ OpenOCD cannot connect via JTAG interface
- **Error**: "Error: open failed" in OpenOCD

## Immediate Steps

### Step 1: Hardware Power Cycle (IMPORTANT!)
```
1. Unplug USB cable from LaunchPad
2. Wait 15 seconds (let all power drain)
3. Check that all LEDs on board turn off
4. Plug USB cable back in
5. Wait 10 seconds for drivers to fully load
6. Look for LED indicator on board (should be lit)
```

### Step 2: Device Manager Verification
```
1. Right-click Start Menu → Device Manager
2. Expand "Ports (COM & LPT)"
3. Should see: "Périphérique série USB (COM9)"
4. No yellow warning triangles
5. Right-click → Properties → Driver tab
6. Click "Driver Details" and verify usbser.sys is loaded
```

### Step 3: Try Upload Again
```bash
cd C:\Users\mael\Documents\mael\stellarisBimotorPWM2brushedWRelay
pio run -t upload
```

---

## Manual Upload Method (Alternative)

If OpenOCD continues to fail, use LM Flash Programmer from TI:

### Download LM Flash Programmer
```
1. Visit: https://www.ti.com/tool/LMFLASHPROGRAMMER
2. Download for Windows
3. Install in default location
4. Open LM Flash Programmer
```

### Manual Upload Process
```
1. In LM Flash Programmer:
   - Select "New Project"
   - Device: LM4F120H5QR
   - Load Image: .pio/build/lm4f120_launchpad/firmware.bin
   
2. Connect LaunchPad
3. Click "Program"
4. Wait for completion
```

---

## Advanced Troubleshooting

### Check USB Drivers
```powershell
# Check if driver is correctly installed:
Get-PnpDevice -FriendlyName "*Stellaris*" | Format-List

# Or search for ICDI:
Get-WmiObject Win32_PnPDevice | Where-Object {$_.Name -match "ICDI|Serial"} | Format-List Name, Manufacturer, Status
```

### Reinstall USB Drivers
```
1. Go to Device Manager
2. Find COM9 port
3. Right-click → Uninstall device
4. Check "Delete driver software"
5. Unplug USB
6. Plug back in
7. Windows will auto-reinstall drivers
```

### Check OpenOCD Directly
```bash
# Test OpenOCD connection manually:
cd C:\Users\mael\.platformio\packages\tool-openocd\bin

# Try connecting:
openocd -f interface/ti-icdi.cfg -f target/stellaris.cfg -c "init"

# If this works, the connection is good
# If it fails, the issue is with the board/cable/driver
```

---

## Bootloader Mode Upload

For some board configurations, you may need to enter bootloader mode:

### Enter Bootloader Mode
```
1. Press and hold RST button on LaunchPad
2. While holding RST, press and release SSO button (once)
3. Wait 1 second
4. Release RST button
5. LED should flash indicating bootloader is ready
```

### Then Upload
```bash
pio run -t upload
```

---

## Hardware Diagnostics

### LED Indicators
- **LED lit after power**: ✓ Power delivery OK
- **No LEDs light**: ✗ Power issue or USB cable problem
- **LED blinks on connect**: ✓ Board responding

### USB Cable Test
- **Try different USB port** on computer
- **Try different USB cable** (must be data cable, not charge-only)
- **Try another computer** if available to isolate software vs hardware

### Physical Inspection
- Check for bent pins near USB connector
- Ensure USB connector is fully seated
- Look for any burn marks or damage

---

## Configuration Options

If the above doesn't work, try modifying `platformio.ini`:

### Option A: Try Different Upload Protocol
```ini
[env:lm4f120_launchpad]
upload_protocol = openocd        ; Already using this
upload_options = 
    -c 'transport select swd'     ; Try SWD instead of JTAG
    -c 'adapter_khz 400'          ; Reduce speed to 400kHz
```

### Option B: Add Debug/Verbose Options
```ini
[env:lm4f120_launchpad]
upload_options = 
    -c 'debug_level 2'
    -c 'adapter_nsrst_delay 200'
    -c 'telnet_port disabled'
```

---

## Error Messages Reference

| Error | Cause | Solution |
|-------|-------|----------|
| "open failed" | Cannot connect to debug interface | Unplug/replug USB, check drivers |
| "timeout" | Device not responding | Press RST button, wait 10s, retry |
| "Connection refused" | OpenOCD port in use | Kill other PlatformIO processes |
| "Unknown device" | Driver not installed | Reinstall Stellaris USB drivers |
| "No target detected" | JTAG interface offline | Check USB cable, power cycle board |

---

## Still Not Working?

### Try Serial Bootloader Upload
For some boards, you might be able to use serial bootloader:

```ini
[env:lm4f120_launchpad_serial]
upload_protocol = serial
upload_speed = 115200
```

Then upload with:
```bash
pio run -t upload -e lm4f120_launchpad_serial
```

### Contact Support Resources
- **PlatformIO**: https://community.platformio.org/
- **Energia/TIVA**: https://forum.43oh.com/ (43oh.com TIVA forum)
- **TI Support**: https://e2e.ti.com/ (TI Developer Community)

---

## Success Checklist

After successful upload, you should see:
- ✓ Build successful message
- ✓ Upload progress indicator
- ✓ Board RST and LEDs blink during upload
- ✓ Final status: `[SUCCESS]` in terminal
- ✓ Board resets and runs firmware
- ✓ Serial monitor shows initialization messages

---

## Next Steps

1. **Try hardware power cycle first** - this solves 80% of connection issues
2. **Check Device Manager** for proper driver status
3. **Retry upload** after power cycle
4. **If still failing**: Try LM Flash Programmer as alternative
5. **If that fails**: Contact support with error messages and setup details

---

Generated: April 18, 2026
