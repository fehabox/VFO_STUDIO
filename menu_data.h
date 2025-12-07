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
//  GLOBAL VARIABLES (declared in settings.h)
// ==========================================================
/*
extern int modeIndex;
extern int stepIndex;
extern int brightIndex;

*/

inline bool previewActive = false;
// ==========================================================
//  MODE CALLBACK
// ==========================================================
inline void onModeSelect(int i) {
    modeIndex = i;
    Serial.print("Mode changed to: ");
    Serial.println(modeIndex);
}

// ==========================================================
//  STEP CALLBACK
// ==========================================================
inline void onStepSelect(int i) {
    stepIndex = i;
    Serial.print("Step changed to: ");
    Serial.println(stepIndex);
}
// ==========================================================
//  LAYOUT CALLBACK
// ==========================================================
inline void onLayoutSelect(int i)
{
    layoutIndex = i;
 previewActive = true;
    Serial.print("Layout changed to: ");
    Serial.println(layoutIndex);

    // No immediate drawing here, it will be applied on next vfoDisplayDraw()
}
// ==========================================================
//  THEME CALLBACK
// ==========================================================
inline void onThemeSelect(int i) {
    themeIndex = i;
    previewActive = true;          // tell system we are previewing
    Serial.print("Theme changed to: ");
    Serial.println(themeIndex);
    
}

// ==========================================================
//  BRIGHTNESS CALLBACK
// ==========================================================
inline void onBrightSelect(int i) {
    brightIndex = i;
    Serial.print("Brightness changed to: ");
    Serial.println(brightIndex);
}

// ==========================================================
//  
// ==========================================================
inline void onAboutSelect(int i) {
    aboutIndex = i;
    aboutActive = true;   
 
}
// ==========================================================
//  MODE MENU
// ==========================================================
inline const char* modeItems[] = {
    "LSB","USB","CW","AM","FM"
};

inline const MenuNode menuMode = {
    "Mode",
    modeItems, 5,
    onModeSelect,
    nullptr, 0,2
};

// ==========================================================
//  STEP MENU
// ==========================================================
inline const char* stepItems[] = {
    "10 Hz","100 Hz","1 kHz","10 kHz","Auto"
};

inline const MenuNode menuStep = {
    "Step",
    stepItems, 5,
    onStepSelect,
    nullptr, 0,3
};

// ==========================================================
//  Layout menu
// ==========================================================
const char* layoutItems[] = {
    "Linear Dial",
    "Arc Dial",
    "X-meter (Demo)",
    "Future"
};

inline const MenuNode menuLayout = {
    "Layout",
    layoutItems,
    4,
    onLayoutSelect,
    nullptr,
    0, 4
};
// ==========================================================
//  THEMES MENU
// ==========================================================
inline const char* themeItems[] = {
    "Atlas",
    "Casio",
    "Amber",
    "Green",
    "White",
    "Atlas 7",
    "Orange",
    "Silver",
    "Mono",
    "Nixie"
};

inline const MenuNode menuTheme = {
    "Themes",
    themeItems, 10,
    onThemeSelect,
    nullptr, 0,5
};

// ==========================================================
//  BRIGHTNESS MENU
// ==========================================================
inline const char* brightItems[] = {
    "Very Low","Low","Medium","High","Max"
};

inline const MenuNode menuBright = {
    "Brightness",
    brightItems, 5,
    onBrightSelect,
    nullptr, 0,6
};

// ==========================================================
//  ABOUT (static screen)
// ==========================================================
inline const char* aboutItems[] = {
    "VFO Studio V1.0"
};
inline const MenuNode menuAbout = {
    "About",
    aboutItems, 1,
    onAboutSelect,
    nullptr, 0,7
};

// ==========================================================
//  MAIN MENU ROOT
// ==========================================================
inline const MenuNode mainMenuSubmenus[] = {
    menuBands,
   // menuMode,
    menuStep,
    menuLayout,
    menuTheme,
  //  menuBright,
    menuAbout
};

inline const MenuNode menuRoot = {
    "Main Menu",
    nullptr, 0,
    nullptr,
    mainMenuSubmenus,
    sizeof(mainMenuSubmenus) / sizeof(mainMenuSubmenus[0])
};
