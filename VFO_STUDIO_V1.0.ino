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


#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include <EEPROM.h>
#include <stdint.h>
#include "si5351.h"

// --- Display class ---
#include "LGFX_ST7789.h"
LGFX tft;  // THE one and only global object
           //
char* ProjectName = "VFO Studio V1.0";
#include "panels/intro.h"
#include "settings.h"
bool menuActive = false;
Si5351 si5351;

#include "SI5351_engine.h"
#include "menu_engine.h"
#include "bands_data.h"
#include "menu_data.h"
#include "menu_display.h"  // includes menu_engine.h and uses extern LGFX tf
#include "vfo_engine.h"
//#include "vfo_display.h"
#include "vfo_display_extra.h"
#include "memory.h"
#include "control.h"  // encoder + button -> menuNavigate/menuSelect

//#include "debug.h"



// =============================================================
//   SETUP
// =============================================================
void setup() {
  Serial.begin(9600);

  tft.init();
  tft.setRotation(0);
  tft.setColorDepth(16);


  // 1) LOAD SETTINGS
  memoryLoad();
  currentTheme = *themeList[themeIndex];


  // 2) APPLY BAND (this sets vfoFrequency correctly)
  vfoSetBand(bandIndex);
  // SI5351

  modeIndex = bands[bandIndex].modeDefault;
  si5351_init();
  si5351_setVFO(vfoFrequency);
  si5351_updateBFOforMode(modeIndex);
  autoModeCW(vfoFrequency);
  // 3) INIT MENU
  menuInit(&menuRoot);

  // 4) INIT CONTROLS
  controlsInit();
  initVFODesign();
  fehaboxSplash(ProjectName);
  delay(3000);
  // 5) DRAW VFO SCREEN
  //vfoDisplayDraw();
  vfoDisplayDraw();
}




// =============================================================
//   LOOP
// =============================================================
void loop() {
  bool changed = false;
  //autoModeCW(vfoFrequency);
  // =====================================
  //  VFO MODE  (menu inactive)
  // =====================================
  if (!menuActive && !previewActive) {
    // --- tuning ---
    int d = getVfoDelta();
    if (d != 0)

      vfoTune(d);
    autoModeCW(vfoFrequency);
    si5351_setVFO(vfoFrequency);
    //si5351_setAux(14050000);//Test only CLK2
   si5351_setAux(vfoFrequency);//CLK2 exetrnal frequency metter.
    handleEncoderButton();
  }
  if (!menuActive) {  // --- display update @ 33 FPS ---
    static uint32_t lastDraw = 0;
    if (millis() - lastDraw >= 33) {
      currentTheme = *themeList[themeIndex];
      vfoDisplayDraw();
      lastDraw = millis();
    }
  }
  // ====================================================
  // PREVIEW MODE inside menu (live theme/layout preview)
  // ====================================================
  if (menuActive && previewActive) {
    // live theme preview
    currentTheme = *themeList[themeIndex];

    // live layout preview
    vfoDisplayDraw();

    previewActive = false;
  }

  if (menuActive && aboutActive) {
  
    fehaboxSplash(ProjectName);

    aboutActive = false;
  }
  // =====================================
  //  MENU MODE
  // =====================================

  // Encoder button handles:
  //  - menuSelect()
  //  - exit on title
  //  - EEPROM save (long press)
  if (handleEncoderButton())
    changed = true;

  // Encoder rotation
  if (controlUpdateMenu())
    changed = true;

  // Redraw menu when needed
  if (changed && menuActive)
    menuDisplayDraw();
}