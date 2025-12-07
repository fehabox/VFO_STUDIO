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
#include <LovyanGFX.hpp>

class MonoGlowRenderer {
public:

    int cellWidth = 32;      // width of each monospaced cell
    int padding   = 6;       // extra space around glyph (for glow)
    uint8_t glowRadius = 0;  // glow radius (0 = off)
    float glowStrength = 0;  // glow strength
    int align = 1;           // 0=left, 1=center, 2=right

    MonoGlowRenderer() {}

    void draw(
        LGFX_Sprite* spr,
        const String& text,
        uint16_t textColor,
        uint16_t bgColor,
        const void* font,
        bool isIFont,
        int yStart = 0
    )
    {
        int cellH = spr->fontHeight();
        if (cellH < 10) cellH = 10;

        int totalW = text.length() * cellWidth;

        int startX = 0;
        if (align == 1)
            startX = (spr->width() - totalW) / 2;
        else if (align == 2)
            startX = spr->width() - totalW;

        // Mini-sprite for each glyph
        LGFX_Sprite glyph(spr->getParent());
        glyph.setColorDepth(16);
        glyph.setFont(isIFont ? (const lgfx::IFont*)font
                              : (const GFXfont*)font);

        int gW = cellWidth + padding * 2;
        int gH = cellH     + padding * 2;

        glyph.createSprite(gW, gH);

        // Decode RGB for glow
        uint8_t br = ((textColor >> 11) & 0x1F) << 3;
        uint8_t bgc= ((textColor >> 5 ) & 0x3F) << 2;
        uint8_t bb =  (textColor        & 0x1F) << 3;

        bool doGlow = (glowRadius > 0 && glowStrength > 0.01f);

        for (int i = 0; i < text.length(); i++)
        {
            char c = text[i];

            glyph.fillSprite(bgColor);

            // ----------- Glow -------------
            if (doGlow)
            {
                for (int dx = -glowRadius; dx <= glowRadius; dx++)
                {
                    for (int dy = -glowRadius; dy <= glowRadius; dy++)
                    {
                        float dist = sqrtf(dx*dx + dy*dy);
                        if (dist > glowRadius) continue;

                        float fade = (1.0f - dist / glowRadius) * glowStrength;
                        if (fade < 0.02f) continue;

                        uint16_t gcol = glyph.color565(
                            br * fade,
                            bgc * fade,
                            bb * fade
                        );

                        glyph.setTextColor(gcol, bgColor);
                        glyph.setTextDatum(textdatum_t::middle_center);
                        glyph.drawString(String(c),
                            gW/2 + dx,
                            gH/2 + dy
                        );
                    }
                }
            }

            // ----------- Main character -------------
            glyph.setTextColor(textColor, bgColor);
            glyph.setTextDatum(textdatum_t::middle_center);
            glyph.drawString(String(c),
                gW/2,
                gH/2
            );

            // Blit to target
            int px = startX + i*cellWidth - padding;
            int py = yStart - padding;

            spr->pushImage(px, py, gW, gH, (uint16_t*)glyph.getBuffer());
        }

        glyph.deleteSprite();
    }
};
