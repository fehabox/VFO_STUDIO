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
// PINS
// ==========================================================
#define OPT_ENC_A   35     // optical encoder
#define OPT_ENC_B   34

#define ENC_A       33     // mechanical encoder (menu)
#define ENC_B       32
#define ENC_BTN     27

#define X_BTN       14     // secondary button


// ==========================================================
// INTERNAL STATE
// ==========================================================
volatile int32_t encRaw = 0;

bool lastEncBtn = HIGH;
bool lastXBtn   = HIGH;


// ==========================================================
// MECHANICAL ENCODER ISR
// ==========================================================
volatile int8_t encState = 0;

void IRAM_ATTR encISR_A()
{
    bool a = digitalRead(ENC_A);
    bool b = digitalRead(ENC_B);

    // State machine decoding
    encState = (encState << 2) | (a << 1) | b;

    switch (encState & 0x0F)
    {
        case 0b0001:
        case 0b0111:
        case 0b1110:
        case 0b1000:
            encRaw++;   // CW
            break;

        case 0b0010:
        case 0b0100:
        case 0b1101:
        case 0b1011:
            encRaw--;   // CCW
            break;
    }
}

void IRAM_ATTR encISR_B()
{
    encISR_A();   // both pins decode identical
}



// ==========================================================
// OPTICAL ENCODER ISR  (FIXED!)
// ==========================================================
void IRAM_ATTR encISR_OPT_A() {
    encRaw += (digitalRead(OPT_ENC_A) == digitalRead(OPT_ENC_B)) ? 1 : -1;
}

void IRAM_ATTR encISR_OPT_B() {
    encRaw += (digitalRead(OPT_ENC_A) != digitalRead(OPT_ENC_B)) ? 1 : -1;
}


// ==========================================================
// UNIFIED DELTA
// ==========================================================
int getEncoderDelta()
{
    static int32_t last = 0;
    static unsigned long lastTime = 0;

    int32_t now = encRaw;
    int32_t d = now - last;

    const int MIN_PULSES = 2;
    if (abs(d) < MIN_PULSES)
        return 0;

    if (millis() - lastTime < 10)
        return 0;

    lastTime = millis();
    last = now;

    return (d > 0) ? +1 : -1;
}


inline bool handleEncoderButton()
{
    static bool lastState = HIGH;
    static unsigned long tDown = 0;
    static bool longReported = false;

    bool s = digitalRead(ENC_BTN);

    // ------------------------------
    // BUTTON PRESSED
    // ------------------------------
    if (s == LOW && lastState == HIGH) {
        tDown = millis();
        longReported = false;
    }

    // ------------------------------
    // LONG PRESS
    // ------------------------------
    if (s == LOW && !longReported &&
        (millis() - tDown) > 600)
    {
        longReported = true;

        if (menuActive) {
            const MenuState& st = menuCurrentState();

            if (st.index >= 0) {
                menuSelect();       // ensure correct submenu index
                memorySave();
                menuFlashSave(1500);
                menuActive = false;
                previewActive = false;
                Serial.println(">>> EEPROM SAVED <<<");
            }
        }

        lastState = s;
        return true;
    }

    // ------------------------------
    // SHORT PRESS (on release)
    // ------------------------------
    if (s == HIGH && lastState == LOW && !longReported)
    {
        // ===========================
        // CASE 1 — MENU IS ACTIVE
        // ===========================
        if (menuActive)
        {
            const MenuState& st = menuCurrentState();
            const MenuNode* node = menuCurrentNode();

            // ------------------------------------------------
            // EXIT MENU ONLY IF:
            //    - index == -1 (title)
            //    - AND this is the ROOT MENU
            // ------------------------------------------------
            if (st.index == -1 && node == &menuRoot) {
                menuActive = false;
                tft.fillScreen(TFT_BLACK);
                //vfoDisplayDraw();
                vfoDisplayDraw();
                lastState = s;
                return true;
            }

            // ---------------------------
            // NORMAL MENU ITEM
            // ---------------------------
            menuSelect();
            lastState = s;
            return true;
        }

        // ===========================
        // CASE 2 — VFO MODE → ENTER MENU
        // ===========================
        else {
            menuActive = true;
            menuInit(&menuRoot);
            menuDisplayDraw();

            lastState = s;
            return true;
        }
    }

    lastState = s;
    return false;
}


inline void toggleMode()
{
    if(menuActive)
    return;
    modeIndex++;
    if (modeIndex >= 5) modeIndex = 0;   // wrap through LSB,USB,CW,AM,FM

    Serial.print("Mode toggled to: ");
    Serial.println(modeIndex);
}

inline void toggleBand()
{
    if(menuActive)
    return;
    bandIndex++;
    if (bandIndex >= bandCount) 
        bandIndex = 0;   // wrap around

    Serial.print("Band toggled to: ");
    Serial.println(bandIndex);

    // Apply band defaults (freq + mode)
    const Band& b = bands[bandIndex];

    // Set VFO to band start
    vfoFrequency = b.startFreq;

    // Apply default mode for that band
    modeIndex = b.modeDefault;

    // Update SI5351
    si5351_setVFO(vfoFrequency);
    si5351_updateBFOforMode(modeIndex);
}

inline uint8_t handleXButton()
{
    static bool lastState = HIGH;
    static unsigned long tDown = 0;
    static bool longReported = false;

    static unsigned long lastClickTime = 0;
    static bool singleWaiting = false;

    const unsigned long dblClickGap   = 350;
    const unsigned long longPressTime = 800;

    bool s = digitalRead(X_BTN);

    // -----------------------------------------
    // PRESS DOWN
    // -----------------------------------------
    if (s == LOW && lastState == HIGH)
    {
        tDown = millis();
        longReported = false;
    }

    // -----------------------------------------
    // LONG PRESS DETECT
    // -----------------------------------------
    if (s == LOW && !longReported)
    {
        if ((millis() - tDown) > longPressTime)
        {
            longReported = true;
            singleWaiting = false;   // prevent single click!
            memorySave();
            menuFlashSave(1500);
            menuActive = false;
            lastState = s;
            return 3;  // LONG PRESS
        }
    }

    // -----------------------------------------
    // RELEASE
    // -----------------------------------------
    if (s == HIGH && lastState == LOW)
    {
        unsigned long now = millis();

        // If long press already processed
        if (longReported)
        {
            lastState = s;
            return 0;
        }

        // DOUBLE CLICK?
        if (singleWaiting && (now - lastClickTime) < dblClickGap)
        {
            singleWaiting = false;
            toggleMode();
           si5351_updateBFOforMode(modeIndex);
            lastState = s;
            return 2; // DOUBLE
        }

        // Otherwise start waiting for single
        singleWaiting = true;
        lastClickTime = now;
    }

    // -----------------------------------------
    // CHECK SINGLE CLICK TIMEOUT
    // -----------------------------------------
    if (singleWaiting && (millis() - lastClickTime) >= dblClickGap)
    {
        singleWaiting = false;
        toggleBand();
        return 1; // SINGLE
    }

    lastState = s;
    return 0;
}


// ==========================================================
// INITIALIZE
// ==========================================================
void controlsInit()
{
    pinMode(OPT_ENC_A, INPUT);
    pinMode(OPT_ENC_B, INPUT);

    pinMode(ENC_A, INPUT_PULLUP);
    pinMode(ENC_B, INPUT_PULLUP);
    pinMode(ENC_BTN, INPUT_PULLUP);
    pinMode(X_BTN, INPUT_PULLUP);

    attachInterrupt(ENC_A,     encISR_A,     CHANGE);
    attachInterrupt(ENC_B,     encISR_B,     CHANGE);
    attachInterrupt(OPT_ENC_A, encISR_OPT_A, CHANGE);
    attachInterrupt(OPT_ENC_B, encISR_OPT_B, CHANGE);
}


// ==========================================================
// MENU CONTROL LOOP
// ==========================================================
bool controlUpdateMenu()
{
    // X button provides no menu control yet → ignore result
    uint8_t x = handleXButton();

    if (handleEncoderButton()) {
        return true;
    }

    int d = getEncoderDelta();
    if (d != 0) {
        menuNavigate(d);
        return true;
    }

    return false;
}


void autoModeCW(uint32_t freq)
{
    const Band& b = bands[bandIndex];

    // Check if current frequency is inside CW region
    bool inCW = (freq >= b.cwStart && freq <= b.cwEnd);

    // Keeps state between calls
    static bool autoCWActive = false;

    // --------------------------------------------
    // ENTER CW REGION
    // --------------------------------------------
    if (inCW)
    {
        autoCWActive = true;
        modeIndex = MODE_CW;
        si5351_updateBFOforMode(MODE_CW);
        return;
    }

    // --------------------------------------------
    // EXIT CW REGION → restore default band mode
    // --------------------------------------------
    if (!inCW && autoCWActive)
    {
        autoCWActive = false;
        modeIndex = b.modeDefault;
        si5351_updateBFOforMode(modeIndex);
        return;
    }
}
