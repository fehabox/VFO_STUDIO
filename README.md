# VFO_STUDIO
VFO 

VFO STUDIO V1.0 - MAIN FEATURES
--------------------------------

- [x] (Smooth VFO tuning with adaptive auto-step)
- [x] (High-speed rotary encoder response (no lag))
- [x] (SI5351 dual-clock control (VFO + BFO)))
- [x] (Mode-dependent BFO offsets (LSB, USB, CW, AM)))
- [x] (Automatic CW mode switching inside band CW segments))
- [x] (Full band system with start/end limits and default mode)
- [x] (Multiple display layouts (frequency, dial, arc dial, full, minimal))
- [x] (Theme system with runtime selection and live preview)
- [x] (Custom fonts per theme (GFX + IFont supported))
- [x] (Flicker-free rendering using LovyanGFX sprites)
- [x] (Bottom status panel showing band, mode, and tuning step)
- X-button system:
    - [x] (Single click = next band)
    - [x] (Double click = next mode (temporary))
    - [x] (Long press = save settings)
- [x] (Save confirmation popup (non-blocking, centered))
- [x] (EEPROM storage of band, mode, step, theme, brightness)
- [x] (Startup splash screen (project name + signature))

**Planned for V2:**
- [ ] (Clean modular code structure (dials, counters, themes, menu, memory))
- [ ] (Compatible with gesture tuning modules)
- [ ] (Support for future S-meter, SWR, power meter, and texture backgrounds)
- [ ] (Support for future optical encoder, frequency tuning)

**Connection diagram**

```
ESP32 DevKit
│
├── ST7789 Display
│   ├── VCC  → 3.3V
│   ├── GND  → GND
│   ├── SCK  → GPIO18
│   ├── MOSI → GPIO23
│   ├── DC   → GPIO2
│   ├── RST  → GPIO4
│   └── CS   → GPIO5
│
├── Rotary Encoder (EC11)
│   ├── A    → GPIO34
│   ├── B    → GPIO35
│   └── SW   → GPIO27
│
└── Si5351 I2C Synth
    ├── SDA  → GPIO21
    ├── SCL  → GPIO22
    ├── VIN  → 3.3V / 5V
    └── GND  → GND
```


**Demo**

<a href="http://www.youtube.com/watch?feature=player_embedded&v=5rs3k1yIMuM" target="_blank">
 <img src="https://img.youtube.com/vi/5rs3k1yIMuM/mqdefault.jpg" alt="Watch the video" width="640" height="480" border="10" />
</a>


