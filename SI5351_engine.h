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
//#include <si5351.h>

// GLOBAL SI5351 CALIBRATION (in Hz)
// Positive = shift output UP
// Negative = shift output DOWN
inline long siCalOffsetHz = -1550;   // adjust until counter is exact

//extern Si5351 si5351;

// -----------------------------
//  CONFIGURATION
// -----------------------------
inline bool siUseIF = true;              // true = VFO uses IF shift
inline long siIF_Offset = 9000000;       // 9 MHz IF example (positive or negative)

inline long lastSiFreq = 0;

inline long bfoFrequency = 0;            // actual BFO frequency (Hz)
inline long lastBfoFreq = 0;             // last applied value
inline bool bfoEnable = true;            // ON/OFF if needed later


// -----------------------------
//  INITIALIZATION
// -----------------------------
inline void si5351_init()
{
    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
    si5351.set_correction(0, SI5351_PLL_INPUT_XO);   // leave zero, we use Hz offset

    // VFO (CLK0)
    si5351.output_enable(SI5351_CLK0, 1);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);

    // BFO (CLK1)
    si5351.output_enable(SI5351_CLK1, 1);
    si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);

    // AUX (CLK2)
    si5351.output_enable(SI5351_CLK2, 0);
    si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);
}


// -----------------------------
//  APPLY VFO FREQUENCY  (CLK0)
// -----------------------------
inline void si5351_setVFO(long vfoFreq)
{
    long outFreq = vfoFreq;

    if (siUseIF)
        outFreq = vfoFreq + siIF_Offset;

    // APPLY GLOBAL CALIBRATION SHIFT
    outFreq += siCalOffsetHz;

    if (outFreq == lastSiFreq) return;
    if (outFreq < 100000) return;

    uint64_t f100 = (uint64_t)outFreq * 100ULL;

    si5351.set_freq(f100, SI5351_CLK0);
    lastSiFreq = outFreq;
}


// -----------------------------
//  APPLY BFO FREQUENCY (CLK1)
// -----------------------------
inline void si5351_setBFO(long freq)
{
    if (!bfoEnable) return;
    if (freq < 100000) return;
    
    long outFreq = freq + siCalOffsetHz;   // APPLY CALIBRATION HERE TOO

    if (outFreq == lastBfoFreq) return;

    uint64_t f100 = (uint64_t)outFreq * 100ULL;

    si5351.set_freq(f100, SI5351_CLK1);
    si5351.output_enable(SI5351_CLK1, 1);
    si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);

    lastBfoFreq = outFreq;
}


// -----------------------------
//  BFO UPDATE BASED ON MODE
// -----------------------------
inline void si5351_updateBFOforMode(uint8_t mode)
{
    long newBfo;

    switch (mode)
    {
        case 0: newBfo = siIF_Offset - 1500; break;   // LSB
        case 1: newBfo = siIF_Offset + 1500; break;   // USB
        case 2: newBfo = siIF_Offset + 600;  break;   // CW
        default:
            return;
    }

    si5351_setBFO(newBfo);
}


// -----------------------------
//  AUX OUTPUT (CLK2)
// -----------------------------
inline void si5351_setAux(long freqHz)
{
    if (freqHz < 1000) return;

    long outFreq = freqHz + siCalOffsetHz;   // ALSO APPLY CALIBRATION IF NEEDED

    uint64_t f100 = (uint64_t)outFreq * 100ULL;
    si5351.set_freq(f100, SI5351_CLK2);
}
