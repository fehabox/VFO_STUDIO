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
//#include <LovyanGFX.hpp>
// MonospaceFontRenderer.h
// -------------------------------------------------
// GLOBAL SETTINGS (you can tweak at runtime)
// -------------------------------------------------
int MONO_DIGIT_WIDTH    = 30;     // default width for digits
float MONO_PUNCT_FACTOR = 0.25f;  // punctuation width multiplier

// ALIGNMENT MODES
// 0 = left, 1 = center, 2 = right
int MONO_ALIGN = 0;

// Globals (declare extern in .h if needed)
extern int MONO_DIGIT_WIDTH;
extern float MONO_PUNCT_FACTOR;
extern int MONO_ALIGN;

class MonospaceFontRenderer {
public:

    static void draw(
        LGFX_Sprite* spr,
        const String& text,
        int cellWidth,            // ignored
        uint16_t textColor,
        uint16_t bgColor,
        const void* font,
        bool isIFont = false,
        int xOffset = 0,          // <–– now optional scroll offset
        int yStart = 0
    ) {
        // =======================================
        // Compute total width (rounded)
        // =======================================
        int totalW = totalWidth(text, 0);

        int cursorX = 0;

        // =======================================
        // Alignment inside SPRITE
        // =======================================
        if (MONO_ALIGN == 0) {             // LEFT
            cursorX = 0 + xOffset;
        }
        else if (MONO_ALIGN == 1) {        // CENTER
            cursorX = (spr->width() - totalW) / 2 + xOffset;
        }
        else if (MONO_ALIGN == 2) {        // RIGHT
            cursorX = (spr->width() - totalW-1) + xOffset;
        }


        // =======================================
        // Draw characters
        // =======================================
        for (int i = 0; i < text.length(); i++) {

            char c = text[i];

            int thisCellWidth;
            if (c == '.' || c == ',' || c == ':' || c == ';') {
                thisCellWidth = int(MONO_DIGIT_WIDTH * MONO_PUNCT_FACTOR + 0.5f);
                if (thisCellWidth < 1) thisCellWidth = 1;
            } else {
                thisCellWidth = MONO_DIGIT_WIDTH;
            }

            LGFX_Sprite cell(spr->getParent());
            cell.setColorDepth(16);
            cell.setFont((isIFont) ? (const lgfx::IFont*)font
                                   : (const GFXfont*)font);

            cell.setTextColor(textColor, bgColor);
            cell.setTextDatum(textdatum_t::middle_center);

            int cellH = spr->fontHeight();
            if (cellH < 8) cellH = 8;

            cell.createSprite(thisCellWidth, cellH);
            cell.fillSprite(bgColor);

            cell.drawString(String(c), thisCellWidth / 2, cellH / 2);

            spr->pushImage(cursorX, yStart, thisCellWidth, cellH,
                           (uint16_t*)cell.getBuffer());

            cursorX += thisCellWidth;
            cell.deleteSprite();
        }
    }

    static int totalWidth(const String& text, int cellWidthUnused) {
        int w = 0;
        for (int i = 0; i < text.length(); i++) {
            char c = text[i];
            if (c == '.' || c == ',' || c == ':' || c == ';') {
                int punctW = int(MONO_DIGIT_WIDTH * MONO_PUNCT_FACTOR + 0.5f);
                if (punctW < 1) punctW = 1;
                w += punctW;
            }
            else {
                w += MONO_DIGIT_WIDTH;
            }
        }
        return w;
    }
};
