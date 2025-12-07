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
// GLOBAL CONFIGURATION (unchanged)
// ==========================================================

static int flapSpeed = 14;        // lower = slower animation
static int flapYOffset = 0;

static const int FIXED_DIGITS = 9;

static int cellW = 30;
static int cellH = 48;

static long lastFreq = 0;

// -------------------------------------------------------------------------
// Uses your existing:
//   freqSprite
//   tft
//   currentTheme
//   MONO_DIGIT_WIDTH / MONO_PUNCT_FACTOR / MONO_ALIGN
//   MonospaceFontRenderer
// -------------------------------------------------------------------------

// Easing for smooth slide
static float easeSlide(float t)
{
    return 1.0f - powf(1.0f - t, 3.0f);  // soft curve
}

static inline bool isPunc(char c)
{
    return (c == '.' || c == ',' || c == ':' || c == ';');
}

// width computation with punctuation factor
static int cellWidthOf(char c)
{
    if (isPunc(c)) {
        int w = int(MONO_DIGIT_WIDTH * MONO_PUNCT_FACTOR + 0.5f);
        return max(1, w);
    }
    return MONO_DIGIT_WIDTH;
}

// Draw one character at absolute coords
static void drawCharAbs(int x, int y, const String &txt)
{
    bool isIFont = (currentTheme.fontType != 0);

    int saveAlign = MONO_ALIGN;
    MONO_ALIGN = 0;

    MonospaceFontRenderer::draw(
        &freqSprite,
        txt,
        MONO_DIGIT_WIDTH,
        currentTheme.fg,
        currentTheme.bg,
        currentTheme.font,
        isIFont,
        x,
        y + flapYOffset
    );

    MONO_ALIGN = saveAlign;
}

// ==========================================================
// Sliding digit animation per cell
// ==========================================================
static void slideDigit(
    char oldC,
    char newC,
    int x,
    int y,
    int w,
    int h,
    int direction   // +1 = upward, -1 = downward
){
    if (isPunc(newC)) {
        // punctuation never slides
        drawCharAbs(x, y, String(newC));
        return;
    }

    const int frames = max(6, 26 - flapSpeed);
    bool up = (direction >= 0);

    for (int f = 0; f <= frames; f++)
    {
        float t = (float)f / frames;
        float e = easeSlide(t);

        int offset = int(e * h);

        freqSprite.setClipRect(0,0,0,0);
        freqSprite.fillSprite(currentTheme.bg);

        // Draw old digit sliding out
        if (up) {
            // old digit moves upward off screen
            drawCharAbs(x, y - offset, String(oldC));
        } else {
            // old digit moves downward
            drawCharAbs(x, y + offset, String(oldC));
        }

        // Draw new digit sliding in
        if (up) {
            drawCharAbs(x, y + (h - offset), String(newC));
        } else {
            drawCharAbs(x, y - (h - offset), String(newC));
        }

        freqSprite.pushSprite(0,0);
    }

    // final fix frame
    drawCharAbs(x, y, String(newC));
}

// ==========================================================
// Frequency drawing using sliding digits
// ==========================================================
void drawSlidingFrequency(long freq)
{
    long mhz = freq / 1000000;
    long khz = (freq / 1000) % 1000;
    long hz  = (freq / 10)  % 100;

    String tmp =
        String(mhz) + "." +
        (khz < 100 ? "0" : "") +
        (khz < 10  ? "0" : "") +
        String(khz) + "." +
        (hz < 10 ? "0" : "") +
        String(hz);

    while (tmp.length() < FIXED_DIGITS)
        tmp = " " + tmp;

    String freqStr = tmp;

    int direction = (freq > lastFreq) ? +1 : -1;
    lastFreq = freq;

    // font
    if (currentTheme.fontType == 0)
        freqSprite.setFont((const GFXfont*)currentTheme.font);
    else
        freqSprite.setFont((const lgfx::IFont*)currentTheme.font);

    freqSprite.setTextSize(1);
    freqSprite.fillSprite(currentTheme.bg);

    // compute widths
    int totalWidth = 0;
    for (int i = 0; i < FIXED_DIGITS; i++)
        totalWidth += cellWidthOf(freqStr[i]);

    int x0 = (freqSprite.width() - totalWidth) / 2;
    int y0 = (freqSprite.height() - (freqSprite.fontHeight() + 12)) / 2;

    cellH = freqSprite.fontHeight() + 12;

    static String lastStr = String(FIXED_DIGITS, ' ');

    // SLIDE ANIMATION —  ANY digit that changed
    const int frames = max(6, 26 - flapSpeed);
    bool animateNeeded = false;

    for (int i = 0; i < FIXED_DIGITS; i++) {
        if (!isPunc(freqStr[i]) && freqStr[i] != lastStr[i])
            animateNeeded = true;
    }

    if (animateNeeded) {

        for (int f = 0; f <= frames; f++) {

            float t = (float)f / frames;
            float e = easeSlide(t);
            int offset = int(e * cellH);

            freqSprite.fillSprite(currentTheme.bg);

            int cx = x0;

            for (int i = 0; i < FIXED_DIGITS; i++) {

                char oldC = lastStr[i];
                char newC = freqStr[i];
                int cw = cellWidthOf(newC);

                if (isPunc(newC)) {
                    drawCharAbs(cx, y0, String(newC));
                }
                else if (newC != oldC) {
                    // sliding digit
                    if (direction >= 0) {
                        // up
                        drawCharAbs(cx, y0 - offset, String(oldC));
                        drawCharAbs(cx, y0 + (cellH - offset), String(newC));
                    } else {
                        // down
                        drawCharAbs(cx, y0 + offset, String(oldC));
                        drawCharAbs(cx, y0 - (cellH - offset), String(newC));
                    }
                }
                else {
                    // unchanged digit
                    drawCharAbs(cx, y0, String(newC));
                }

                cx += cw;
            }

            freqSprite.pushSprite(0,0);
        }
    }

    // FINAL STATIC FRAME
    freqSprite.fillSprite(currentTheme.bg);

    int cx = x0;
    for (int i = 0; i < FIXED_DIGITS; i++) {
        drawCharAbs(cx, y0, String(freqStr[i]));
        cx += cellWidthOf(freqStr[i]);
    }

    lastStr = freqStr;

    freqSprite.pushSprite(0,0);
}
