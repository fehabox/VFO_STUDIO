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
//  BAND STRUCTURE
// ==========================================================
struct Band {
    const char* name;
    uint32_t startFreq;
    uint32_t endFreq;
    uint8_t  lpfCode;
    uint8_t  modeDefault;
    uint32_t cwStart;   // NEW
    uint32_t cwEnd;     // NEW
};

// ==========================================================
//  BAND TABLE
// ==========================================================
inline const Band bands[] = {
    { "ALL 1-30 MHz",   1000000, 30000000, 0, 1,     0, 0 },
    { "160m 1.8 MHz",   1800000, 2000000,  1, 0,     1830000, 1900000 },
    { "80m 3.5 MHz",    3500000, 4000000,  2, 0,     3500000, 3560000 },
    { "40m 7.0 MHz",    7000000, 7300000,  3, 0,     7000000, 7040000 },
    { "30m 10.1 MHz",  10100000,10150000,  4, 2,     10100000,10150000 },
    { "20m 14.0 MHz",  14000000,14350000,  5, 1,     14000000,14060000 },
    { "17m 18.1 MHz",  18100000,18168000,  6, 1,     18100000,18130000 },
    { "15m 21.0 MHz",  21000000,21450000,  7, 1,     21000000,21070000 },
    { "12m 24.9 MHz",  24900000,24990000,  8, 1,     24900000,24950000 },
    { "10m 28.0 MHz",  28000000,29700000,  9, 1,     28000000,28120000 },
    { "10 MHz TEST",  10000000,10500000,  10, 2,     10000000,10500000 }
};

inline const int bandCount = sizeof(bands) / sizeof(bands[0]);

// ==========================================================
//  BAND NAME ARRAY FOR MENU
// ==========================================================
inline const char* bandNames[bandCount] = {
    "ALL 1-30 MHz",
    "160m 1.8 MHz",
    "80m 3.5 MHz",
    "40m 7.0 MHz",
    "30m 10.1 MHz",
    "20m 14.0 MHz",
    "17m 18.1 MHz",
    "15m 21.0 MHz",
    "12m 24.9 MHz",
    "10m 28.0 MHz"
};

// External globals from settings.h
extern int bandIndex;
extern uint32_t vfoFrequency;
extern uint32_t cwStart;
extern uint32_t cwEnd;

// ==========================================================
//  CALLBACK: APPLY SELECTED BAND
// ==========================================================
inline void onBandSelect(int i)
{
    bandIndex = i;

    // Auto mode from band table
    modeIndex = bands[bandIndex].modeDefault;

    Serial.print("Band changed: ");
    Serial.print(bands[bandIndex].name);
    Serial.print(" | Auto mode=");
    Serial.println(modeIndex);
    // Optional: move VFO to start of band
    vfoFrequency = bands[bandIndex].startFreq;
    cwStart = bands[bandIndex].cwStart;
    cwEnd = bands[bandIndex].cwEnd;
  
}



// ==========================================================
//  MENU NODE FOR BANDS
// ==========================================================
inline const MenuNode menuBands = {
    "Band",
    bandNames,
    bandCount,
    onBandSelect,
    nullptr,
    0,1
};
