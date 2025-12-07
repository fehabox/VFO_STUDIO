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
//  EXTERNAL VARIABLES PROVIDED BY YOUR PROJECT
// ==========================================================
// These already exist in your system (from settings + bands)
extern uint32_t vfoFrequency;
extern int bandIndex;
extern int modeIndex;
extern int stepIndex;

extern const Band bands[];     // from bands_data.h
extern const int bandCount;

// ==========================================================
//  STEP TABLE
// ==========================================================
#define STEP_10HZ     0
#define STEP_100HZ    1
#define STEP_1KHZ     2
#define STEP_10KHZ    3
#define STEP_AUTO     4        // adaptive tuning velocity

inline const uint32_t stepTable[5] = {
    10,    // 10 Hz
    100,   // 100 Hz
    1000,  // 1 kHz
    10000, // 10 kHz
    0      // AUTO placeholder
};

// ==========================================================
//  FORMAT FREQUENCY  →  "7.045.12"
// ==========================================================
inline void vfoFormatFrequency(uint32_t f, char* out)
{
    uint32_t MHz =  f / 1000000UL;
    uint32_t kHz = (f / 1000UL) % 1000UL;
    uint32_t Hz  = (f / 10UL) % 100UL;

    snprintf(out, 16, "%lu.%03lu.%02lu", MHz, kHz, Hz);
}

// ==========================================================
//  CLAMP FREQUENCY TO CURRENT BAND
// ==========================================================
inline void vfoClampToBand()
{
    const Band& b = bands[bandIndex];

    if (vfoFrequency < b.startFreq) vfoFrequency = b.startFreq;
    if (vfoFrequency > b.endFreq)   vfoFrequency = b.endFreq;
}

// ==========================================================
//  ENCODER RAW COUNTER (from control.h)
// ==========================================================
extern volatile int32_t encRaw;

// ==========================================================
//  GET CURRENT STEP (for MANUAL modes / display)
// ==========================================================
inline uint32_t vfoGetStep()
{
    if (stepIndex == STEP_AUTO)
        return 10;                  // base fine step used in AUTO

    return stepTable[stepIndex];    // 10 / 100 / 1k / 10k
}
// ==========================================================
//  BASIC VFO DELTA (mechanical encoder → ±1 per detent)
// ==========================================================
inline int getVfoDelta()
{
    static int32_t last = 0;
    int32_t now = encRaw;
    int32_t d = now - last;
    last = now;

    if (d > 0) return 1;
    if (d < 0) return -1;
    return 0;
}
// ==========================================================
//  MAIN TUNING FUNCTION
//
//  dir = encoder delta from your VFO loop (usually ±1)
//  - MANUAL: uses fixed steps (10/100/1k/10k)
//  - AUTO:   uses GOLD acceleration based on encRaw + time
// ==========================================================
inline void vfoTune(int dir)
{
    // ------------------------------------------------------
    // MANUAL MODE (10 / 100 / 1k / 10k)
    // ------------------------------------------------------
    if (stepIndex != STEP_AUTO)
    {
        if (dir == 0) return;

        uint32_t step = stepTable[stepIndex];

        if (dir > 0) vfoFrequency += step;
        else         vfoFrequency -= step;

        vfoClampToBand();
        return;
    }

    // ------------------------------------------------------
    // AUTO MODE (GOLD + 10 kHz turbo)
    // ------------------------------------------------------
    static int32_t lastCount = 0;
    int32_t delta = encRaw - lastCount;
    lastCount = encRaw;

    if (delta == 0) return;

    // Base steps
    const long minStep = 10;        // 10 Hz fine
    const long midStep = 1000;      // normal max = 1 kHz
    const long maxStep = 10000;     // turbo max = 10 kHz

    long effectiveStep = minStep;

    static unsigned long lastMoveTime = 0;
    unsigned long now = millis();
    double dt = now - lastMoveTime;
    lastMoveTime = now;

    if (dt <= 0.0) dt = 1.0;

    // ------------------------------------------------------
    // GOLD SPEED CURVE → natural acceleration
    // ------------------------------------------------------
    if (dt > 100.0)
    {
        // Very slow turning → 10 Hz
        effectiveStep = minStep;
    }
    else
    {
        // First stage acceleration (up to 1 kHz)
        double factor = pow(100.0 / dt, 1.25);
        long step1k = (long)(minStep * factor);

        if (step1k < minStep) step1k = minStep;
        if (step1k > midStep) step1k = midStep;

        effectiveStep = step1k;

        // ------------------------------------------------------
        // TURBO MODE → unlock 10 kHz only on VERY fast turning
        // ------------------------------------------------------
        if (dt < 20.0)   // threshold for turbo
        {
            double turboFactor = pow(20.0 / dt, 1.40);
            long turboStep = (long)(midStep * turboFactor);

            if (turboStep > maxStep) turboStep = maxStep;
            if (turboStep > effectiveStep)
                effectiveStep = turboStep;
        }
    }

    // ------------------------------------------------------
    // Apply tuning
    // ------------------------------------------------------
    long long newFreq =
        (long long)vfoFrequency +
        (long long)delta * (long long)effectiveStep;

    // Clamp to band
    const Band& b = bands[bandIndex];
    if (newFreq < (long long)b.startFreq) newFreq = b.startFreq;
    if (newFreq > (long long)b.endFreq)   newFreq = b.endFreq;

    vfoFrequency = (uint32_t)newFreq;

    // ------------------------------------------------------
    // SNAP TO GRID
    // ------------------------------------------------------
    if (effectiveStep >= 10000)
        vfoFrequency = (vfoFrequency / 10000UL) * 10000UL;   // snap 10 kHz
    else if (effectiveStep >= 1000)
        vfoFrequency = (vfoFrequency / 1000UL) * 1000UL;     // snap 1 kHz
    else
        vfoFrequency = (vfoFrequency / 10UL) * 10UL;         // snap 10 Hz
}


// ==========================================================
//  CHANGE BAND → frequency = band start
// ==========================================================
inline void vfoSetBand(int idx)
{
    if (idx < 0 || idx >= bandCount) return;

    bandIndex = idx;
    vfoFrequency = bands[idx].startFreq;
   
    vfoClampToBand();
}

// ==========================================================
//  DIRECT FREQUENCY SET (with clamp)
// ==========================================================
inline void vfoSetFrequency(uint32_t f)
{
    vfoFrequency = f;
    vfoClampToBand();
}

// ==========================================================
//  NEXT / PREV BAND
// ==========================================================
inline void vfoNextBand()
{
    int n = bandIndex + 1;
    if (n >= bandCount) n = 0;
    vfoSetBand(n);
}

inline void vfoPrevBand()
{
    int n = bandIndex - 1;
    if (n < 0) n = bandCount - 1;
    vfoSetBand(n);
}

// ==========================================================
//  GET BAND WIDTH
// ==========================================================
inline uint32_t vfoBandWidth()
{
    const Band& b = bands[bandIndex];
    return b.endFreq - b.startFreq;
}
