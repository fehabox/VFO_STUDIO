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


// ---- S-Meter Sprite ----
lgfx::LGFX_Sprite sMeterSprite(&tft);
float sGlowPhase = 0;      // for needle glow effect
int lastNeedlePos = 0;     // smooth needle movement

// ---- Initialize S-Meter sprite ----
void initSMeter() {
    sMeterSprite.createSprite(dialSprite.width(), dialSprite.height());
    sMeterSprite.setColorDepth(16);
    sMeterSprite.fillSprite(currentTheme.bg);
    sMeterSprite.pushSprite(0, 60 + 5); // same Y as dial container
}

// ---- Draw S-Meter ----
// rssiValue: 0–100 simulated signal strength
// ---- Draw linear horizontal S-Meter ----
// ---- Smoothed S-Meter ----
float smoothedSValue = 0;
const float smoothingFactor = 0.15;

// ---- Full S-Meter Function ----
void drawSMeterLinear(int sValue) {

    // Smooth needle motion
    smoothedSValue += (sValue - smoothedSValue) * smoothingFactor;

    dialSprite.fillSprite(currentTheme.bg);

    int w = dialSprite.width();
    int h = dialSprite.height();
    int topMargin = 20;
    int bottom = h - 6;

    // Geometry
    int leftMargin  = w * 0.04;
    int usableWidth = w - leftMargin * 2;

    // Extract RGB from theme dial color
    uint8_t baseR = currentTheme.dialR;
    uint8_t baseG = currentTheme.dialG;
    uint8_t baseB = currentTheme.dialB;

    // --- 3D Gradient (same as VFO dial) ---
    auto shade = [&](int x)->uint16_t {
        float factor = 1.0 - fabs((x - w/2.0) / (w/2.0)) * 0.50;
        if (factor < 0.50) factor = 0.50;
        uint8_t r = baseR * factor;
        uint8_t g = baseG * factor;
        uint8_t b = baseB * factor;
        return tft.color565(r, g, b);
    };

    // ---- Minor ticks (theme panel text — Atlas = white) ----
    int numMinor = 24;
    for (int i = 0; i <= numMinor; i++) {
        int x = i * (w - 1) / numMinor;
        dialSprite.drawFastVLine(x, bottom - 4, 4, currentTheme.panelText);
    }

    // ---- Major ticks + Labels ----
    const char* labels[] = {
        "S0","S2","S4","S6","S8","S9","+20","+40","+60"
    };
    const int majorCount = 9;

    for (int i = 0; i < majorCount; i++) {

        int x = leftMargin + i * usableWidth / (majorCount - 1);

        // Tick color: S0-S9 uses dial color, +20/+40/+60 uses marker color
        uint16_t tickColor = (i < 6) ? shade(x) : currentTheme.marker;

        dialSprite.drawFastVLine(x, topMargin, 16, tickColor);

        // Label color same logic
        uint16_t labelColor = tickColor;

        dialSprite.setFont(&fonts::Font2);
        dialSprite.setTextDatum(textdatum_t::middle_center);
        dialSprite.setTextColor(labelColor, currentTheme.bg);
        dialSprite.drawString(labels[i], x, bottom - 18);
    }

    // ---- NEEDLE ----
    uint16_t needleBase = currentTheme.marker; // pure needle color

    // Extract RGB for shading
    uint8_t nr = ((needleBase >> 11) & 31) << 3;
    uint8_t ng = ((needleBase >> 5)  & 63) << 2;
    uint8_t nb = ( needleBase        & 31) << 3;

    int needleX = leftMargin + map((int)smoothedSValue, 0, 100, 0, usableWidth);
    //int needleWidth = 4;
// --- NEEDLE WIDTH FROM THEME ---
int needleWidth = currentTheme.needleWidth;
    // Shaded thick needle
    for (int dx = -needleWidth/2; dx <= needleWidth/2; dx++) {

        int nx = needleX + dx;
        if (nx < 0 || nx >= w) continue;

        // Same dial shading function for 3D edge fade
        float factor = 1.0 - fabs((nx - w/2.0) / (w/2.0)) * 0.50;
        if(factor < 0.50) factor = 0.50;

        uint16_t shadedNeedle =
            tft.color565(nr * factor, ng * factor, nb * factor);

        dialSprite.drawLine(nx, topMargin, nx, bottom, shadedNeedle);
    }

    // ---- NEEDLE GLOW ----
    for (int dx = -7; dx <= 7; dx++) {

        int nx = needleX + dx;
        if (nx < 0 || nx >= w) continue;

        float a = 1.0 - fabs(dx) / 7.0;

        uint8_t r = ((needleBase >> 11) & 31) << 3;
        uint8_t g = ((needleBase >> 5)  & 63) << 2;
        uint8_t b = ( needleBase        & 31) << 3;

        dialSprite.drawPixel(
            nx,
            (topMargin + bottom) / 2,
            tft.color565(r * a, g * a, b * a)
        );
    }

    // ---- Push to screen ----
    dialSprite.pushSprite(0, 60 + 5);
}




// ---- Vi-meter (angled ticks, pivot bottom-center, needle swings left->right) ----
// ---- Vi-meter (angled ticks, pivot bottom-center, needle swings left->right) ----
// ---- Vi-meter (angled ticks, pivot bottom-center, needle swings left->right) ----
void drawVUMeter2(int sValue) {
    // clear area 
    dialSprite.fillSprite(currentTheme.bg);

    int w = dialSprite.width();      // 320
    int h = dialSprite.height();     // 90
    int topLineY = 20;
    int pivotY = h - 8;
    int pivotX = w / 2;

    int leftMargin  = (int)(w * 0.05f);
    int rightMargin = leftMargin;
    int usableWidth = w - leftMargin - rightMargin;

    // major tick labels
    const char* labels[] = { "S0","S2","S4","S6","S8","S9","+20","+40","+60" };
    const int majorCount = 9;

    // dial base color
    uint8_t baseR = currentTheme.dialR;
    uint8_t baseG = currentTheme.dialG;
    uint8_t baseB = currentTheme.dialB;

    // gradient helper
    auto shadeColor = [&](int x)->uint16_t {
        float factor = 1.0f - fabsf((x - w * 0.5f) / (w * 0.5f)) * 0.50f;
        if (factor < 0.5f) factor = 0.5f;
        uint8_t r = (uint8_t)(baseR * factor);
        uint8_t g = (uint8_t)(baseG * factor);
        uint8_t b = (uint8_t)(baseB * factor);
        return tft.color565(r, g, b);
    };

    // subtle top horizontal line
    uint16_t guideCol = tft.color565(
        (uint8_t)(baseR * 0.4f),
        (uint8_t)(baseG * 0.4f),
        (uint8_t)(baseB * 0.4f)
    );
    dialSprite.drawFastHLine(leftMargin, topLineY, usableWidth, guideCol);

    // ---- Angle correction per tick (right side needs forward tilt) ----
    float tickAngleCorrection[majorCount] = {
        0.0f, 0.0f, 0.0f, 0.0f,       // S0..S6 natural
        0.0f,                         // S8 = vertical naturally
        +0.05f, +0.10f, +0.15f        // S9, +20, +40, +60 tilt forward
    };

    // ---- Draw Ticks (perfectly anchored on topLineY) ----
    for (int i = 0; i < majorCount; i++) {

        int x = leftMargin + (i * usableWidth) / (majorCount - 1);

        // Base vector from tick root to pivot
        float dx = (float)(pivotX - x);
        float dy = (float)(pivotY - topLineY);

        // Base angle
        float angle = atan2f(dy, dx);

        // Add correction for right-side ticks
        angle += tickAngleCorrection[i];

        int tickLen = 14;

        // Tick START = EXACTLY on the top line
        float sx = (float)x;
        float sy = (float)topLineY;

        // Tick END = angled outward
        float ex = sx + cosf(angle) * tickLen;
        float ey = sy + sinf(angle) * tickLen;

        // color selection
        uint16_t tickCol = (i < 5) ? shadeColor(x) : currentTheme.marker;

        // draw the tick
        dialSprite.drawLine((int)sx, (int)sy, (int)ex, (int)ey, tickCol);

        // labels
        dialSprite.setFont(&fonts::Font2);
        dialSprite.setTextDatum(textdatum_t::middle_center);
        dialSprite.setTextColor(tickCol, currentTheme.bg);
        dialSprite.drawString(labels[i], x, topLineY + 20);
    }

    // ---- Needle angle mapping ----
    const float maxAngleDeg = 67.0f;
    const float maxAngle = maxAngleDeg * (float)(M_PI / 180.0f);

    float angle = ((float)smoothedSValue / 100.0f) * (2.0f * maxAngle) - maxAngle;

    // rotate 90°: center = UP
    float a = angle - (float)M_PI / 2.0f;

    // Needle geometry
    int needleLen = (int)(usableWidth * 0.60f);
    int needleW   = max(1, (int)currentTheme.needleWidth);
    uint16_t needleBase = currentTheme.marker;

    int tipX = pivotX + (int)(cosf(a) * needleLen);
    int tipY = pivotY + (int)(sinf(a) * needleLen);

    // ---- Needle with clipping: visible ONLY below topLineY ----
    for (int wx = -needleW / 2; wx <= needleW / 2; wx++) {

        float px = -sinf(a) * (float)wx;
        float py =  cosf(a) * (float)wx;

        int sx = pivotX + (int)px;
        int sy = pivotY + (int)py;
        int ex = tipX   + (int)px;
        int ey = tipY   + (int)py;

        if (sy >= topLineY && ey >= topLineY) {
            dialSprite.drawLine(sx, sy, ex, ey, needleBase);
        }
        else if (sy >= topLineY && ey < topLineY) {
            float t = (float)(topLineY - sy) / (float)(ey - sy);
            int cx = (int)(sx + t * (ex - sx));
            dialSprite.drawLine(sx, sy, cx, topLineY, needleBase);
        }
        else if (sy < topLineY && ey >= topLineY) {
            float t = (float)(topLineY - sy) / (float)(ey - sy);
            int cx = (int)(sx + t * (ex - sx));
            dialSprite.drawLine(cx, topLineY, ex, ey, needleBase);
        }
    }

    // ---- Pivot uses needle color ----
    uint16_t pivotCol = currentTheme.marker;
    dialSprite.fillCircle(pivotX, pivotY, 3, pivotCol);
    dialSprite.fillCircle(pivotX, pivotY, 1, pivotCol);

    // push to screen
    dialSprite.pushSprite(0, 60 + 5);

}



// ---- S-meter VU-meter style (tall arc, pivot lower, natural analog geometry, exact tick alignment) ----
void drawVUMeter(int sValue) {
    // clear area 
    dialSprite.fillSprite(currentTheme.bg);

    int w = dialSprite.width();      // 320

    // --- Taller container ---
    int h = 160;
    int topLineY = 20;

    // --- Deeper pivot for natural analog arc ---
    int pivotY = h - 20;
    int pivotX = w / 2;

    // geometry
    int leftMargin  = (int)(w * 0.05f);
    int rightMargin = leftMargin;
    int usableWidth = w - leftMargin - rightMargin;

    // major tick labels
    const char* labels[] = { "S0","S2","S4","S6","S8","S9","+20","+40","+60" };
    const int majorCount = 9;

    // dial base color
    uint8_t baseR = currentTheme.dialR;
    uint8_t baseG = currentTheme.dialG;
    uint8_t baseB = currentTheme.dialB;

    // gradient helper
    auto shadeColor = [&](int x)->uint16_t {
        float factor = 1.0f - fabsf((x - w * 0.5f) / (w * 0.5f)) * 0.50f;
        if (factor < 0.5f) factor = 0.5f;
        uint8_t r = (uint8_t)(baseR * factor);
        uint8_t g = (uint8_t)(baseG * factor);
        uint8_t b = (uint8_t)(baseB * factor);
        return tft.color565(r, g, b);
    };

    // subtle top horizontal line
    uint16_t guideCol = tft.color565(
        (uint8_t)(baseR * 0.4f),
        (uint8_t)(baseG * 0.4f),
        (uint8_t)(baseB * 0.4f)
    );
    dialSprite.drawFastHLine(leftMargin, topLineY, usableWidth, guideCol);

    // ---- Draw Ticks (exact radial alignment: angle from tick root to pivot) ----
    for (int i = 0; i < majorCount; i++) {

        int x = leftMargin + (i * usableWidth) / (majorCount - 1);

        // EXACT angle from the tick root (x, topLineY) to the pivot
        float dx = (float)(pivotX - x);
        float dy = (float)(pivotY - topLineY);
        float angle = atan2f(dy, dx);

        // snap center tick (S8) to exactly vertical if near center
        if (abs(x - pivotX) <= 1) {
            angle = (float)M_PI / 2.0f; // perfectly downwards from top line
        }

        // longer ticks for perspective
        int tickLen = 18;

        // anchored on top horizontal line
        float sx = (float)x;
        float sy = (float)topLineY;

        float ex = sx + cosf(angle) * tickLen;
        float ey = sy + sinf(angle) * tickLen;

        uint16_t tickCol = (i < 5) ? shadeColor(x) : currentTheme.marker;

        // draw the tick
        dialSprite.drawLine((int)sx, (int)sy, (int)ex, (int)ey, tickCol);

        // labels (slightly lower for better balance)
        dialSprite.setFont(&fonts::Font2);
        dialSprite.setTextDatum(textdatum_t::middle_center);
        dialSprite.setTextColor(tickCol, currentTheme.bg);
        dialSprite.drawString(labels[i], x, topLineY + 28);
    }

    // ---- Needle angle mapping ----
    const float maxAngleDeg = 50.0f;       // 50° sweep left/right
    const float maxAngle = maxAngleDeg * (float)(M_PI / 180.0f);

    float angle = ((float)smoothedSValue / 100.0f) * (2.0f * maxAngle) - maxAngle;

    // rotate 90° for "UP" center
    float a = angle - (float)M_PI / 2.0f;

    // shorter needle for realism
    int needleLen = (int)(usableWidth * 0.68f);
    int needleW   = max(1, (int)currentTheme.needleWidth);
    uint16_t needleBase = currentTheme.marker;

    int tipX = pivotX + (int)(cosf(a) * needleLen);
    int tipY = pivotY + (int)(sinf(a) * needleLen);

    // ---- Needle with clipping (never above topLineY) ----
    for (int wx = -needleW / 2; wx <= needleW / 2; wx++) {

        float px = -sinf(a) * (float)wx;
        float py =  cosf(a) * (float)wx;

        int sx = pivotX + (int)px;
        int sy = pivotY + (int)py;
        int ex = tipX   + (int)px;
        int ey = tipY   + (int)py;

        if (sy >= topLineY && ey >= topLineY) {
            dialSprite.drawLine(sx, sy, ex, ey, needleBase);
        }
        else if (sy >= topLineY && ey < topLineY) {
            float t = (float)(topLineY - sy) / (float)(ey - sy);
            int cx = (int)(sx + t * (ex - sx));
            dialSprite.drawLine(sx, sy, cx, topLineY, needleBase);
        }
        else if (sy < topLineY && ey >= topLineY) {
            float t = (float)(topLineY - sy) / (float)(ey - sy);
            int cx = (int)(sx + t * (ex - sx));
            dialSprite.drawLine(cx, topLineY, ex, ey, needleBase);
        }
    }

    // ---- Pivot uses needle color ----
    uint16_t pivotCol = currentTheme.marker;
    dialSprite.fillCircle(pivotX, pivotY, 3, pivotCol);
    dialSprite.fillCircle(pivotX, pivotY, 1, pivotCol);

    // draw sprite on screen (adjust Y offset if you moved panel)
    dialSprite.pushSprite(0, 60);
}




// Exact ICOM-style 3-scale meter rendering with outward-tilted ticks
// Use your existing global:
// float ICOM_TILT_DEG = 28.0f;

// Make sure you have this global somewhere:
// float ICOM_TILT_DEG = 28.0f; // outward tilt degrees (editable)

// Global outward tick tilt (degrees)
// Global outward tick tilt
// GLOBAL (keep or set once near top)
float ICOM_TILT_DEG = 50.0f;

// helper
static inline bool _hasText(const char* s){
    return (s && s[0] && !(s[0]==' ' && s[1]=='\0'));
}

// FINAL polished ICOM meter (2 scales, inline labels, accent bars)
void drawXmeter(int sValue, int mode)
{
    const int W = 320;
    const int H = 160;

    // ----------------------------------
    // Insets and baselines
    // ----------------------------------
    const int LEFTM  = 25;
    const int RIGHTM = 25;
    const int USEW   = W - LEFTM - RIGHTM;

    const int BASE_TOP    = 38;               // S-scale baseline
    const int BASE_BOTTOM = BASE_TOP + 38;    // reduced gap (C)

    // Mode row
    const int MODE_Y = BASE_BOTTOM + 32;

    // pivot
    const int PIVOT_X = W/2;
    const int PIVOT_Y = H - 12;

    // needle sweep
    const float MAX_SWEEP_DEG = 50.0f;
    const float MAX_SWEEP_RAD = MAX_SWEEP_DEG * (M_PI/180.0f);
    const float TICK_TILT_RAD = ICOM_TILT_DEG * (M_PI/180.0f);

    // smoothing
    static float smoothed = 0.0f;
    smoothed += ((float)constrain(sValue,0,100) - smoothed) * 0.18f;
    float sn = smoothed / 100.0f;

    // clear
    dialSprite.fillSprite(currentTheme.bg);

    // colors
    uint8_t r0=currentTheme.dialR, g0=currentTheme.dialG, b0=currentTheme.dialB;
    uint16_t needleCol = currentTheme.marker;

    auto shade=[&](int x)->uint16_t{
        float f = 1.0f - fabsf((x - W*0.5f)/(W*0.5f)) * 0.5f;
        if (f < 0.5f) f = 0.5f;
        return tft.color565((uint8_t)(r0*f),(uint8_t)(g0*f),(uint8_t)(b0*f));
    };

    // guide line for S-scale
    uint16_t guideCol = tft.color565((uint8_t)(r0*0.38f),(uint8_t)(g0*0.38f),(uint8_t)(b0*0.38f));
    dialSprite.drawFastHLine(LEFTM, BASE_TOP, USEW, guideCol);

    // S-scale major positions
    const int MC = 9;
    const float majorNorm[MC] = {0.00f,0.12f,0.27f,0.42f,0.55f,0.65f,0.78f,0.90f,1.00f};
    const int minorCounts[MC-1] = {1,1,1,1,1,1,1,1};

    // Labels (no +40)
    const char* labelsTop[MC] =  { "S","1","3","5","7","9","+20"," ","+60" };
    const char* labelsRF[MC]  =  { "0","1","10","25","50","100"," "," "," " };
    const char* labelsSWR[MC] =  { " "," ","1","1.5","2","3"," "," "," " };

    // Convert norm to X pixel
    auto normToX=[&](float n)->int{ return LEFTM + (int)(n*USEW + 0.5f); };

    // tick lengths
    const int majorLen = 20;
    const int midLen   = 14;
    const int minorLen = 10;
    const int bottomShort = 5;

    // Top scale label row fixed inline
    const int LABEL_Y_TOP = BASE_TOP - (majorLen + 8);

    // ----------------------------------
    // Draw TOP scale
    // ----------------------------------
    auto drawTop=[&](){
        uint16_t baseCol=tft.color565(r0*0.06f,g0*0.06f,b0*0.06f);
        dialSprite.fillRect(LEFTM, BASE_TOP-2, USEW, 4, baseCol);

        for(int i=0;i<MC;i++){
            int x = normToX(majorNorm[i]);
            float rel = (float)(x-PIVOT_X)/(USEW*0.5f);
            rel = constrain(rel, -1.0f, 1.0f);

            float ang = -M_PI_2 + rel*TICK_TILT_RAD;
            int tlen = (i==0||i==MC-1||i==MC/2) ? majorLen : midLen;

            float sx=x, sy=BASE_TOP;
            float ex=sx+cosf(ang)*tlen;
            float ey=sy+sinf(ang)*tlen;
            uint16_t c=shade(x);

            dialSprite.drawLine((int)sx,(int)sy,(int)ex,(int)ey,c);

            // inline label row
            if (_hasText(labelsTop[i])){
                dialSprite.setFont(&fonts::Font2);
                dialSprite.setTextDatum(textdatum_t::middle_center);
                dialSprite.setTextColor(c, currentTheme.bg);
                dialSprite.drawString(labelsTop[i], x, LABEL_Y_TOP);
            }

            // minor tick (midpoint)
            if(i<MC-1){
                int nx = normToX(majorNorm[i+1]);
                float fx = (x + nx)*0.5f;

                float relm = (fx-PIVOT_X)/(USEW*0.5f);
                relm = constrain(relm, -1.0f, 1.0f);

                float a2 = -M_PI_2 + relm*TICK_TILT_RAD;
                float msx=fx, msy=BASE_TOP;
                float mex=msx+cosf(a2)*minorLen;
                float mey=msy+sinf(a2)*minorLen;

                uint16_t mc=shade((int)fx);
                dialSprite.drawLine((int)msx,(int)msy,(int)mex,(int)mey,mc);
            }
        }

        // Accent bar S9 → +60
        int x1 = normToX(majorNorm[5]); // 9
        int x2 = normToX(majorNorm[8]); // +60
        const int barT = 3;
        dialSprite.fillRect(x1, BASE_TOP - barT - 1, x2 - x1, barT, needleCol);
    };

    // ----------------------------------
    // Draw BOTTOM scale
    // ----------------------------------
    auto drawBottom=[&](){
        uint16_t baseCol=tft.color565(r0*0.06f,g0*0.06f,b0*0.06f);
        dialSprite.fillRect(LEFTM, BASE_BOTTOM-2, USEW, 4, baseCol);

        for(int i=0;i<MC;i++){
            int x = normToX(majorNorm[i]);
            float rel = (float)(x-PIVOT_X)/(USEW*0.5f);
            rel = constrain(rel, -1.0f, 1.0f);

            float ang = -M_PI_2 + rel*TICK_TILT_RAD;

            float sx=x, sy=BASE_BOTTOM;
            float ex=sx+cosf(ang)*bottomShort;
            float ey=sy+sinf(ang)*bottomShort;
            uint16_t c=shade(x);

            dialSprite.drawLine((int)sx,(int)sy,(int)ex,(int)ey,c);

            // RF labels (above)
            if(_hasText(labelsRF[i])){
                dialSprite.setFont(&fonts::Font2);
                dialSprite.setTextDatum(textdatum_t::middle_center);
                dialSprite.setTextColor(c,currentTheme.bg);
                dialSprite.drawString(labelsRF[i], x, BASE_BOTTOM - 14);
            }

            // SWR labels (below)
            if(_hasText(labelsSWR[i])){
                dialSprite.setFont(&fonts::Font2);
                dialSprite.setTextDatum(textdatum_t::middle_center);
                dialSprite.setTextColor(c,currentTheme.bg);
                dialSprite.drawString(labelsSWR[i], x, BASE_BOTTOM + 14);
            }
        }

        // Accent bar SWR 3 → end
        int bx1 = normToX(majorNorm[5]); // SWR 3
        int bx2 = normToX(majorNorm[8]); // far end
        const int barT = 3;
        dialSprite.fillRect(bx1, BASE_BOTTOM - barT - 1, bx2 - bx1, barT, needleCol);
    };

    drawTop();
    drawBottom();

    // ----------------------------------
    // Draw MODE SELECTOR ROW
    // ----------------------------------
    const char* modes[5] = {"S","ALC","SWR","RF","SET"};
    const int modeCount = 5;

    int totalWidth = 0;
    dialSprite.setFont(&fonts::Font2);

    // measure total width
    for(int i=0;i<modeCount;i++)
        totalWidth += dialSprite.textWidth(modes[i]) + 18;

    int startX = (W - totalWidth)/2 + 10;
    int cx = startX;

    for(int i=0;i<modeCount;i++){
        const char* m = modes[i];
        int w = dialSprite.textWidth(m);

        if(i == mode){
            // HIGHLIGHT STYLE D: bold + needle color
            dialSprite.setTextColor(needleCol,currentTheme.bg);
            dialSprite.setFont(&fonts::Font2);   // bold variant
        }
        else{
            dialSprite.setFont(&fonts::Font2);
            dialSprite.setTextColor(
                tft.color565(r0*0.7f, g0*0.7f, b0*0.7f),
                currentTheme.bg
            );
        }

        dialSprite.setTextDatum(textdatum_t::middle_left);
        dialSprite.drawString(m, cx, MODE_Y);
        cx += w + 18;
    }

    // ----------------------------------
    // Needle rendering
    // ----------------------------------
    float ang = sn * (2.0f*MAX_SWEEP_RAD) - MAX_SWEEP_RAD;
    float a = ang - M_PI_2;

    int needleLen=(int)(USEW*0.68f);
    int needleBack=12;
    int needleW=max(2,(int)currentTheme.needleWidth);

    int tipX=PIVOT_X + (int)(cosf(a)*needleLen);
    int tipY=PIVOT_Y + (int)(sinf(a)*needleLen);
    int backX=PIVOT_X - (int)(cosf(a)*needleBack);
    int backY=PIVOT_Y - (int)(sinf(a)*needleBack);

    float nx=-sinf(a), ny=cosf(a);

    int bx1 = backX + (int)(nx*(needleW*0.6f));
    int by1 = backY + (int)(ny*(needleW*0.6f));
    int bx2 = backX - (int)(nx*(needleW*0.6f));
    int by2 = backY - (int)(ny*(needleW*0.6f));

    int tx1 = tipX + (int)(nx*(needleW*0.3f));
    int ty1 = tipY + (int)(ny*(needleW*0.3f));
    int tx2 = tipX - (int)(nx*(needleW*0.3f));
    int ty2 = tipY - (int)(ny*(needleW*0.3f));

    dialSprite.fillTriangle(bx1,by1,bx2,by2,tipX,tipY,needleCol);
    dialSprite.fillTriangle(tx1,ty1,tx2,ty2,backX,backY,needleCol);

    dialSprite.fillCircle(PIVOT_X,PIVOT_Y,4,needleCol);
    dialSprite.fillCircle(PIVOT_X,PIVOT_Y,1,currentTheme.bg);

    // present
    dialSprite.pushSprite(0,60);
}
