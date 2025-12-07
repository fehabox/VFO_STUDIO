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


// ==========================================================
//  MODES (shared everywhere)
// ==========================================================
enum {
    MODE_LSB = 0,
    MODE_USB,
    MODE_CW,
    MODE_AM,
    MODE_FM
};

// ================================
// Screen Regions (240 px tall)
// ================================
static const int TOP_H = 60;
static const int MID_H = 120;
static const int BOT_H = 60;
// Persistent sprites
lgfx::LGFX_Sprite spriteFreq(&tft);
lgfx::LGFX_Sprite spriteStatus(&tft);

// ==========================================================
//  GLOBAL USER SETTINGS (EEPROM TARGETS)
// ==========================================================

inline int bandIndex;
inline int modeIndex;
inline int stepIndex;
inline int themeIndex;
inline int layoutIndex;
inline int brightIndex;
inline int aboutIndex;
inline bool aboutActive = false;
// Frequency is optional but included for completeness
inline uint32_t vfoFrequency = 0;
inline uint32_t cwStart;
inline uint32_t cwEnd;
volatile int32_t encoderCount = 0;
//inline char* ProjectName = "VFO Studio V1.0";
inline bool autoCWActive = false;
inline uint8_t savedSSBmode = MODE_CW;

