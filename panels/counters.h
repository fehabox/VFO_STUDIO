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

void drawFrequency(long freq)
{
    freqSprite.fillSprite(currentTheme.bg);

    // ---------------------------------------
    // Split frequency
    // ---------------------------------------
    long mhz = freq / 1000000;
    long khz = (freq % 1000000) / 1000;
    long hz  = (freq % 1000) / 10;

    String freqStr =
        String(mhz) + "." +
        (khz < 100 ? "0" : "") +
        (khz < 10  ? "0" : "") +
        String(khz) + "." +
        (hz < 10 ? "0" : "") +
        String(hz);

    // ---------------------------------------
    // Select font
    // ---------------------------------------
    bool isIFont = (currentTheme.fontType != 0);

    if (!isIFont)
        freqSprite.setFont((const GFXfont*)currentTheme.font);
    else
        freqSprite.setFont((const lgfx::IFont*)currentTheme.font);

    freqSprite.setTextSize(1);

    // ---------------------------------------
    // Layout
    // ---------------------------------------
    const int cellWidth  = 30;
    const int cellHeight = freqSprite.fontHeight();
    const int totalWidth = freqStr.length() * cellWidth;

    const int x0 = (freqSprite.width()  - totalWidth) / 2;
    const int y0 = (freqSprite.height() - cellHeight) / 2;

    uint16_t fg = currentTheme.fg;

    // ---------------------------------------
    // SHARP MAIN TEXT (no glow)
    // ---------------------------------------
    MonospaceFontRenderer::draw(
        &freqSprite,
        freqStr,
        cellWidth,
        fg,                  // theme text color
        currentTheme.bg,
        currentTheme.font,
        isIFont,
        x0,
        y0
    );

    // ---------------------------------------
    // STATIC "MHz" LABEL (unchanged)
    // ---------------------------------------
    freqSprite.setFont(&Font4);
    freqSprite.setTextSize(1);

    const char* unitLabel = "MHz";
    int labelWidth  = freqSprite.textWidth(unitLabel);
    int labelHeight = freqSprite.fontHeight();
    const int BASE = 3;

    int freqBaseline = y0 + (cellHeight - BASE);

    int lx = freqSprite.width() - labelWidth - 6;
    int ly = freqBaseline - labelHeight + BASE;

    freqSprite.setTextColor(fg, currentTheme.bg);
    freqSprite.drawString(unitLabel, lx, ly);

    // ---------------------------------------
    // PUSH TO SCREEN
    // ---------------------------------------
    freqSprite.pushSprite(0, 0);
}

