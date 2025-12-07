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

#define EEPROM_SIZE 32
#define EE_BAND     0
#define EE_MODE     1
#define EE_STEP     2
#define EE_LAYOUT     5
#define EE_THEME     4
#define EE_BRIGHT   3

inline void memoryLoad()
{
    EEPROM.begin(EEPROM_SIZE);

    bandIndex   = EEPROM.read(EE_BAND);
    modeIndex   = EEPROM.read(EE_MODE);
    stepIndex   = EEPROM.read(EE_STEP);
    layoutIndex = EEPROM.read(EE_LAYOUT);
    themeIndex  = EEPROM.read(EE_THEME);
    brightIndex = EEPROM.read(EE_BRIGHT);

    if (bandIndex   < 0 || bandIndex   >= bandCount) bandIndex   = 0;
    if (modeIndex   < 0 || modeIndex   >= 5)         modeIndex   = MODE_USB;
    if (stepIndex   < 0 || stepIndex   >= 5)         stepIndex   = 1;
    if (layoutIndex   < 0 || layoutIndex   >= 5)      layoutIndex   = 0;
    if (themeIndex  < 0 || themeIndex  >= 10)         themeIndex  = 0;
    if (brightIndex < 0 || brightIndex >= 5)         brightIndex = 2;
    
}


inline void memorySave()
{
    EEPROM.write(EE_BAND,   bandIndex);
    EEPROM.write(EE_MODE,   modeIndex);
    EEPROM.write(EE_STEP,   stepIndex);
    EEPROM.write(EE_LAYOUT,   layoutIndex);
    EEPROM.write(EE_THEME,   themeIndex);
    EEPROM.write(EE_BRIGHT, brightIndex);
    EEPROM.commit();
}
