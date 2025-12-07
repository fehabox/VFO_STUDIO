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


// =====================================================================
//   fehabox Splash Screen (perfect vertical centering)
// =====================================================================

void fehaboxSplash(const char* projectName)
{
    tft.fillScreen(TFT_BLACK);
    tft.setColorDepth(16);

    // Use center alignment
    tft.setTextDatum(textdatum_t::middle_center);
    tft.setFont(&fonts::Font4);

    // ---------------------------------------------------------
    // Prepare text lines
    // ---------------------------------------------------------
    const char* line1 = projectName;        // Dynamic
    const char* line2 = "Tools in the box.";
    const char* line3 = "Ideas out of the box.";
    const char* line4 = "by @fehabox";
    const char* line5 = "fehabox.com";

    const char* lines[] = { line1, line2, line3, line4, line5 };
    const int lineCount = 5;

    // ---------------------------------------------------------
    // Compute total text height (Font4 height is constant)
    // ---------------------------------------------------------
    int lh = tft.fontHeight();  // height of one line
    int totalH = lh * lineCount + 8 * (lineCount - 1);  // 8px spacing

    // Compute start Y so the whole block is vertically centered
    int cy = (tft.height() - totalH) / 2 + lh / 2;

    const int cx = tft.width() / 2;

    // ---------------------------------------------------------
    // Draw each line
    // ---------------------------------------------------------

    for (int i = 0; i < lineCount; i++)
    {
        int y = cy + i * (lh + 8);

        switch (i)
        {
            case 0: tft.setTextColor(TFT_WHITE,      TFT_BLACK); break; // project name
            case 1: tft.setTextColor(TFT_LIGHTGREY,  TFT_BLACK); break;
            case 2: tft.setTextColor(TFT_LIGHTGREY,  TFT_BLACK); break;
            case 3: tft.setTextColor(TFT_CYAN,       TFT_BLACK); break;
            case 4: tft.setTextColor(TFT_LIGHTGREY,  TFT_BLACK); break;
        }

        tft.drawString(lines[i], cx, y);
    }

    //delay(3000);
}
