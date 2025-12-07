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

// ==========================================================
//                     THEME STRUCTURE
// ==========================================================
#include "fonts/scoreboard26pt7b.h"
#include "fonts/roboto_mono_latin_100_normal26pt7b.h"
#include "fonts/roboto_mono_latin_100_italic26pt7b.h"
//#include "fonts/Patopian_198626pt7b.h"
#include "fonts/advanced_led_board_726pt7b.h"
#include "fonts/Digital7Mono_Yz9J426pt7b.h"
//#include "fonts/PegHolesRegular_L3lWg26pt7b.h"
#include "fonts/FreeSansBold26pt7b.h"
#include "fonts/Sans_Narrow_BOLD26pt7b.h"
#include "fonts/Seven_Segment26pt7b.h"

#include "fonts/NeonGlow_8VLz26pt7b.h"
#include "fonts/Digital7Monoitalic_8MDLJ26pt7b.h"
// fontType: 0 = GFXfont / RLEfont (Font7, scoreboard...)
//           1 = IFont (Roboto, FreeFonts)

// ==========================================================
//                       THEME STRUCTURE
// ==========================================================
struct Theme {
    uint16_t bg;
    uint16_t fg;
    uint16_t marker;

    uint8_t  dialR, dialG, dialB;

    uint16_t step;
    uint16_t panelBg;
    uint16_t panelText;

    uint8_t  needleWidth;

    float    glowStrength;
    uint8_t  glowSize;

    const void* font;
    uint8_t  fontType;

    uint16_t rotateMask;
    uint8_t  rotateLabels;

    uint16_t labelColor;
};

// ==========================================================
//                       THEMES
// ==========================================================

// ----------------------------------------------------------
// 1) ATLAS CLASSIC — red digits on black
// ----------------------------------------------------------
Theme atlasTheme = {
    TFT_BLACK,
    tft.color565(255,0,0),
    TFT_RED,
    255,255,200,
    tft.color565(255, 255, 200), tft.color565(20,20,20), tft.color565(255, 255, 200),
    0, 0.30f, 2,
    &scoreboard26pt7b, 0,
    TFT_BLACK, 1,
    TFT_WHITE
};

// ---------- ATLAS ----------
Theme atlas7Theme = {
    // Background, foreground, marker
    TFT_BLACK,                          // bg
    tft.color565(255, 255, 200),          // fg
    TFT_RED,                            // marker

    // Dial RGB (uint8_t)
    255, 255, 200,                      // dialR, dialG, dialB

    // Panel
    tft.color565(255, 255, 200),                          // step
    tft.color565(20,20,20),                       // panelBg
    tft.color565(255, 255, 200),                          // panelText

    // Needle and glow
    0,                                  // needleWidth
    3.30f,                              // glowStrength (float MUST end with f)
    2,                                  // glowSize

    // Font
    &Digital7Monoitalic_8MDLJ26pt7b,    // font pointer
    0,                                  // fontType (0 = GFXfont)

    // Rotated text settings
    TFT_BLACK,                          // rotateMask
    1,                                  // rotateLabels

    // Special label color
    TFT_WHITE                           // labelColor
};

// ----------------------------------------------------------
// 2) CASIO GOLD — calculator-style
// ----------------------------------------------------------
Theme casioTheme = {
    tft.color565(210,190,60),
    TFT_BLACK,
    TFT_RED,
    10,10,10,
    TFT_BLACK, tft.color565(180,160,40), TFT_BLACK,
    0, 0.0f, 0,
    &Font7, 0,
    TFT_BLACK, 1,
    tft.color565(50,50,50)
};

// ----------------------------------------------------------
// 3) AMBER — warm analog glow
// ----------------------------------------------------------
Theme amberTheme = {
    TFT_BLACK,
    tft.color565(255,200,80),
    TFT_RED,
    255,160,60,
    TFT_ORANGE, tft.color565(20,20,20), tft.color565(255,200,80),
    0, 2.60f, 3,
    &roboto_mono_latin_100_normal26pt7b, 1,
    TFT_BLACK, 1,
    tft.color565(255,200,80)
};

// ----------------------------------------------------------
// 4) GREEN — classic radio green
// ----------------------------------------------------------
Theme greenTheme = {
    TFT_BLACK,
    tft.color565(144,255,144),
    TFT_GREEN,
    0,255,128,
    tft.color565(0,255,64), tft.color565(20,20,20), tft.color565(144,255,144),
    0, 0, 0,
    &roboto_mono_latin_100_italic26pt7b, 0,
    TFT_BLACK, 1,
    tft.color565(144,255,144)
};

// ----------------------------------------------------------
// 5) WHITE — clean laboratory look
// ----------------------------------------------------------
Theme whiteTheme = {
    TFT_BLACK,
    tft.color565(255, 255, 200),
    TFT_RED,
    255,255,200,
    TFT_WHITE, tft.color565(20,20,20), tft.color565(255, 255, 200),
    0, 0.40f, 2,
    &FreeSansBold26pt7b, 0,
    TFT_BLACK, 1,
    tft.color565(255, 255, 200)
};

// ---------- CASIO ORANGE ----------
Theme casioOrangeTheme = {
    tft.color565(10,10,10), TFT_ORANGE, TFT_ORANGE,
    255,165,0,
    TFT_ORANGE, tft.color565(60,30,0), TFT_ORANGE,
    0, 0.25, 2,
    &Font7, 1,
    TFT_BLACK, 1,
    TFT_ORANGE
};

Theme silverTheme = {
    tft.color565(220,220,220),
    TFT_BLACK,
    TFT_RED,
    10,10,10,
    TFT_BLACK, tft.color565(220,220,220), TFT_BLACK,
    0, 0.0f, 0,
    &Font7, 0,
    TFT_BLACK, 1,
    tft.color565(180,160,40)
};
// ---------- CASIO MONO ----------
Theme casioMonoTheme = {
    tft.color565(210,190,60), TFT_BLACK, TFT_BLACK,
    10,10,10,
    TFT_BLACK, tft.color565(210,190,60), TFT_BLACK,
    0, 0.0, 0,
    &advanced_led_board_726pt7b, 0,
    TFT_BLACK, 1,
    tft.color565(210,190,60)
};



// ---------- NIXIE ----------
Theme nixieTheme = {
    TFT_BLACK, 0xFDA0, TFT_RED,
    0xFE,0xB3,0x47,
    TFT_ORANGE, tft.color565(50,50,50), tft.color565(180,180,180),
    2, 2.0f, 4,
    &roboto_mono_latin_100_italic26pt7b, 1,
    TFT_BLACK, 1,
    0xFDA0
};

Theme currentTheme = atlasTheme;   // default at boot

Theme* themeList[] = {
    &atlasTheme,
    &casioTheme,
    &amberTheme,
    &greenTheme,
    &whiteTheme,
    &atlas7Theme,
    &casioOrangeTheme,
    &silverTheme,
    &casioMonoTheme,
    &nixieTheme,
};

