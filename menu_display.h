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

// These menu nodes exist in menu_data.h and bands_data.h
extern const MenuNode menuBands;
extern const MenuNode menuMode;
extern const MenuNode menuStep;
extern const MenuNode menuLayout;
extern const MenuNode menuTheme;
extern const MenuNode menuBright;

// ======================================================
//  SETTINGS
// ======================================================
static const int MENU_X      = 10;
static const int MENU_Y0     = 40;
static const int MENU_ROW_H  = 32;
static const int MENU_GAP    = 4;
static const int MENU_W      = 300;

static const int MENU_VISIBLE_ROWS = 5;

static int menuScrollOffset = 0;

// ======================================================
// COLORS
// ======================================================
#define COLOR_SELECTED_BG   TFT_WHITE
#define COLOR_SELECTED_FG   TFT_BLACK

#define COLOR_SAVED_BG      TFT_DARKGRAY   // dark gray
#define COLOR_SAVED_FG      TFT_WHITE

#define COLOR_NORMAL_BG     TFT_BLACK
#define COLOR_NORMAL_FG     TFT_WHITE


// ======================================================
//  SPRITE DRAW FUNCTION (uses bg + fg arguments)
// ======================================================
void drawMenuRowSprite(int x, int y, int w, int h,
                       uint16_t bg, uint16_t fg,
                       const char* text)
{
    static lgfx::LGFX_Sprite row(&tft);

    row.createSprite(w, h);

    row.fillRect(0, 0, w, h, bg);

    row.setFont(&fonts::Font4);
    row.setTextSize(1);
    row.setTextColor(fg, bg);

    row.setCursor(12, 8);
    row.print(text);

    row.pushSprite(x, y);
    row.deleteSprite();
}



// ======================================================
//  MAIN DRAW FUNCTION
// ======================================================
void menuDisplayDraw()
{
    const MenuState& st = menuCurrentState();
    const MenuNode* node = menuCurrentNode();

    tft.fillScreen(TFT_BLACK);

    // --------------------------------
    // COUNT VISIBLE ITEMS
    // --------------------------------
    int count = (node->subMenuCount > 0)
                ? node->subMenuCount
                : node->itemCount;

    // --------------------------------
    // SCROLL ADJUSTMENT
    // --------------------------------
    if (st.index >= 0)
    {
        if (st.index < menuScrollOffset)
            menuScrollOffset = st.index;

        if (st.index >= menuScrollOffset + MENU_VISIBLE_ROWS)
            menuScrollOffset = st.index - (MENU_VISIBLE_ROWS - 1);
    }

    if (menuScrollOffset < 0) menuScrollOffset = 0;
    if (menuScrollOffset > max(0, count - MENU_VISIBLE_ROWS))
        menuScrollOffset = max(0, count - MENU_VISIBLE_ROWS);


    // =====================================================
    // TITLE ROW ("< MenuName")
    // =====================================================
    bool selTitle = (st.index == -1);

    char titleText[64];
    snprintf(titleText, sizeof(titleText), "< %s", node->label);

    uint16_t titleBG = selTitle ? COLOR_SELECTED_BG : COLOR_NORMAL_BG;
    uint16_t titleFG = selTitle ? COLOR_SELECTED_FG : COLOR_NORMAL_FG;

    drawMenuRowSprite(
        MENU_X,
        MENU_Y0 - (MENU_ROW_H + MENU_GAP),
        MENU_W,
        MENU_ROW_H,
        titleBG, titleFG,
        titleText
    );


    // =====================================================
    // DRAW MENU ITEMS
    // =====================================================
    for (int r = 0; r < MENU_VISIBLE_ROWS; r++)
    {
        int itemIndex = menuScrollOffset + r;
        if (itemIndex >= count) break;

        int y = MENU_Y0 + r * (MENU_ROW_H + MENU_GAP);

        const char* text =
            (node->subMenuCount > 0)
            ? node->subMenus[itemIndex].label
            : node->items[itemIndex];

        // -----------------------------
        // Determine highlight state
        // -----------------------------
        bool isSelected = (itemIndex == st.index);
        bool isSaved    = false;

        // Highlight saved values in dark gray
        switch (node->menuID)
{
    case 1: isSaved = (itemIndex == bandIndex);   break;   // Band menu
    case 2: isSaved = (itemIndex == modeIndex);   break;   // Mode menu
    case 3: isSaved = (itemIndex == stepIndex);   break;   // Step menu
    case 4: isSaved = (itemIndex == layoutIndex);   break;   // Theme menu
    case 5: isSaved = (itemIndex == themeIndex);   break;   // Theme menu
    case 6: isSaved = (itemIndex == brightIndex); break;   // Bright menu
}


        uint16_t bg, fg;

        if (isSelected) {
            bg = COLOR_SELECTED_BG;
            fg = COLOR_SELECTED_FG;
        }
        else if (isSaved) {
            bg = COLOR_SAVED_BG;
            fg = COLOR_SAVED_FG;
        }
        else {
            bg = COLOR_NORMAL_BG;
            fg = COLOR_NORMAL_FG;
        }

        drawMenuRowSprite(
            MENU_X,
            y,
            MENU_W,
            MENU_ROW_H,
            bg, fg,
            text
        );
    }
}



// ======================================================
//  GLOBAL "Saved" POPUP (always on top)
// ======================================================
// ======================================================
//  GLOBAL "Saved" POPUP (centered text)
// ======================================================
inline void menuFlashSave(uint16_t flashTimeMs = 1500)
{
    const int pw = 320;
    const int ph = 40;

    int px = (tft.width()  - pw) / 2;  // centered horizontally
    int py = 0;                         // top popup

    // Draw popup background
    tft.fillRect(px, py, pw, ph, TFT_GREEN);

    // Use Font4 (same as your menu)
    tft.setFont(&fonts::Font4);
    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_GREEN);

    // ---- ENABLE TRUE CENTERING ----
    tft.setTextDatum(middle_center);

    // Popup message
    const char* msg = "Saved";

    // Center inside 320×50 box
    int cx = px + pw/2;
    int cy = py + ph/2;     // <- THIS centers vertically

    tft.drawString(msg, cx, cy);

    delay(flashTimeMs);

    menuDisplayDraw();
}
