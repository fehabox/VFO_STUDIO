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

// Global for special dial background
uint16_t dialBGColor = TFT_BLACK;  // change to any color for dial container
// --- Global needle thickness (1–6 recommended) ---
int needleWidth = 4;   // global S-meter
float freqGlowPhase = 3;        // animated glow phase
float freqGlowStrength = 1.7;  // glow opacity (0.0–1.0)
// ---- Main display + sprites ----
//LGFX tft;

extern lgfx::LGFX_Sprite freqSprite(&tft);
extern lgfx::LGFX_Sprite dialSprite(&tft);
extern lgfx::LGFX_Sprite panelSprite(&tft);

//#include <LovyanGFX.hpp>
#include "extra/MonospaceFontRenderer.h"
#include "themes.h"
#include "panels/counters.h"   // contains drawFrequency(), drawFrequencyGlow(), drawFastCounter()
#include "panels/dials.h"      // contains drawDial(), drawArcDial()
#include "panels/meters.h" 
//#include "panels/intro.h" 

extern uint32_t vfoFrequency;
extern Theme currentTheme;
//extern lgfx::LGFX tft;



// ---- Initialize display ----
void initVFODesign() {
    tft.init();
    tft.setColorDepth(16);
    tft.setRotation(0);
    tft.invertDisplay(false);
    tft.fillScreen(currentTheme.bg);

    // Use the global sprites (already constructed with &tft)
    freqSprite.setColorDepth(16);
    dialSprite.setColorDepth(16);
    panelSprite.setColorDepth(16);

    freqSprite.createSprite(320, 70);
    dialSprite.createSprite(320, 115);
    panelSprite.createSprite(320, 50);
}

inline const char* formatStepText(uint32_t stepHz)
{
    switch (stepHz)
    {
        case 10:     return "10 Hz";
        case 100:    return "100 Hz";
        case 1000:   return "1 kHz";
        case 10000:  return "10 kHz";
        default:     return "?";
    }
}

// =====================================================
// 3) SPRITE-BASED STATUS BAR (BOTTOM AREA) — THEMED
// =====================================================
inline void drawStatus()
{
    int y0 = TOP_H + MID_H;

    panelSprite.createSprite(320, BOT_H);

    // PANEL BACKGROUND (theme-based)
    panelSprite.fillScreen(currentTheme.panelBg);

    const Band& b = bands[bandIndex];
    const char* modeText[] = {"LSB","USB","CW","AM","FM"};

    // COLUMN POSITIONS
    int colBand = 20;
    int colMode = 120;
    int colStep = 220;

    // --------------------------------------
    // LABELS (small text)
    // --------------------------------------
    panelSprite.setFont(&fonts::Font2);
    panelSprite.setTextSize(1);

    panelSprite.setTextColor(
        currentTheme.panelText,      // themed label color
        currentTheme.panelBg         // background
    );

    panelSprite.setCursor(colBand, 10); panelSprite.print("BAND");
    panelSprite.setCursor(colMode, 10); panelSprite.print("MODE");
    panelSprite.setCursor(colStep, 10); panelSprite.print("STEP");

    // --------------------------------------
    // VALUES (highlight items)
    // --------------------------------------
    panelSprite.setTextColor(
        currentTheme.step,           // themed highlight color
        currentTheme.panelBg
    );

    panelSprite.setCursor(colBand, 32);
    panelSprite.print(b.name);

    panelSprite.setCursor(colMode, 32);
    panelSprite.print(modeText[modeIndex]);

    // STEP DISPLAY
    panelSprite.setCursor(colStep, 32);

if (stepIndex == STEP_AUTO)
{
    panelSprite.print("AUTO");
}
else
{
    const char* txt = formatStepText(vfoGetStep());
    panelSprite.print(txt);
}


    // PUSH ONLY BOTTOM AREA
    panelSprite.pushSprite(0, y0);
    panelSprite.deleteSprite();
}

// ==========================================================
//  EXTRA DISPLAY ENTRY POINT
//  You can freely choose which visuals to use
// ==========================================================
inline void vfoDisplayDraw()
{

    switch(layoutIndex)
    {
        // ---------------------------------------------------
        // LAYOUT 0: Classic
        // ---------------------------------------------------
        case 0:
            drawFrequency(vfoFrequency);
            drawDial(vfoFrequency);
            drawStatus();
            break;

        // ---------------------------------------------------
        // LAYOUT 1: ArcDial only
        // ---------------------------------------------------
        case 1:
             drawFrequency(vfoFrequency);
            drawArcDial(vfoFrequency);
            drawStatus();
            break;

        // ---------------------------------------------------
        // LAYOUT 2: Minimal (frequency only)
        // ---------------------------------------------------
        case 2:
            drawFrequency(vfoFrequency);
            drawXmeter(60,0);
            drawStatus();
            break;

        // ---------------------------------------------------
        // LAYOUT 3: Full display (everything)
        // ---------------------------------------------------
        case 3:
            drawFrequency(vfoFrequency);
            drawArcDial(vfoFrequency);
            drawStatus();
            break;

        // Fallback
        default:
       drawFrequency(vfoFrequency);
            drawDial(vfoFrequency);
            drawStatus();
            break;
    }


}
