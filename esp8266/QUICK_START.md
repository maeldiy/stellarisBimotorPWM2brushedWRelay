# ESP8266 Quick Start (5-minute version)

## Installation

```
1. Download Arduino IDE: https://www.arduino.cc/en/software
2. Install normally
3. File → Preferences → Add Board URL:
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
4. Tools → Board Manager → Search "esp8266" → Install
5. Done! ✅
```

## First Upload

```
1. Plug ESP8266 (NodeMCU) into USB
2. Tools → Board → NodeMCU 1.0 (ESP-12E Module)
3. Tools → Port → COM# (your board)
4. File → Open → esp8266_bimotorPWM.ino
5. Sketch → Upload
6. Done! ✅
```

## Check It Works

```
Tools → Serial Monitor (set to 115200 baud)
Should show: "✓ Initialization complete!"
```

## Wiring

```
ArduPilot CH1 → ESP8266 D5
ArduPilot CH2 → ESP8266 D6
Motor PWM 1  ← ESP8266 D7
Motor PWM 2  ← ESP8266 D8
Direction 1  ← ESP8266 D0
Direction 2  ← ESP8266 D1
```

## Signal Thresholds

- **<700µs**: Error (motor off, red LED)
- **700-900µs**: Dead zone (motor neutral)
- **900-1400µs**: Forward (proportional speed)
- **>1400µs**: High-speed (full throttle)

## Common Issues

| Problem | Solution |
|---------|----------|
| No COM port | Install CH340 driver from Device Manager |
| Upload fails | Hold GPIO0 to GND during upload |
| Garbage in serial | Set baud to 115200 |
| Compilation error | Check Tools → Board is NodeMCU |

## That's It!

Done. Works immediately. No driver headaches. ✅

---

**Why ESP8266 > Stellaris for this:**
- No driver issues ✅
- Arduino IDE native ✅
- $5-15 board cost ✅
- WiFi bonus ✅
- Active community ✅
- Works out of the box ✅
