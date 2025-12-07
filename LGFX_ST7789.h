/*
    Project: VFO Studio V1.0
    fehabox Software License
    Copyright © fehabox.com
    Author: Femi Hasani (@fehabox), ex YU8CF

    License   : License_Details.txt
    Credits   : credits.txt
    Features  : VFO_Studio_Features.txt
    GitHub    : https://github.com/fehabox
*/



#pragma once
#include <LovyanGFX.hpp>

// ---------------------------------------------
// 1) DEFINE LGFX CLASS FIRST
// ---------------------------------------------
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 panel;
  lgfx::Bus_SPI bus;
  lgfx::Light_PWM backlight;

public:
  LGFX(void) {

    { // SPI Config
      auto cfg = bus.config();
      cfg.spi_host   = VSPI_HOST;
      cfg.spi_mode   = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read  = 16000000;
      cfg.spi_3wire  = false;
      cfg.use_lock   = true;
      cfg.dma_channel = 1;
      cfg.pin_sclk   = 18;
      cfg.pin_mosi   = 23;
      cfg.pin_miso   = -1;
      cfg.pin_dc     = 2;
      bus.config(cfg);
      panel.setBus(&bus);
    }

    { // Panel Config
      auto cfg = panel.config();
      cfg.pin_cs     = 5;
      cfg.pin_rst    = 15;
      cfg.pin_busy   = -1;

      cfg.memory_width  = 240;
      cfg.memory_height = 320;
      cfg.panel_width   = 240;
      cfg.panel_height  = 320;

      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 1;

      cfg.readable   = false;
      cfg.invert     = false;
      cfg.rgb_order  = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;

      panel.config(cfg);
    }

    { // Backlight
      auto cfg = backlight.config();
      cfg.pin_bl = -1;
      cfg.invert = false;
      cfg.freq   = 5000;
      cfg.pwm_channel = 7;
      backlight.config(cfg);
      panel.setLight(&backlight);
    }

    setPanel(&panel);
  }
};

// ---------------------------------------------
// 2) Declare the global tft object AFTER class
// ---------------------------------------------
extern LGFX tft;
