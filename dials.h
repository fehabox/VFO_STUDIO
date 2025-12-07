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


// ---- Glow phase for dial ----
float glowPhase = 0;

// ---- Draw dial ----

void drawDial(long freq) {
    dialSprite.fillSprite(currentTheme.bg);
    int w = dialSprite.width();
    int h = dialSprite.height();
    int center = w/2;
    const long span = 100000;
    const double hzPerPixel = (double)span / (double)w;
    const double startFreq = (double)freq - (double)(w/2)*hzPerPixel;

    float glowFactor = 1.0 + 0.15*sin(glowPhase);
    glowPhase += 0.05;
    if(glowPhase>2*PI) glowPhase-=2*PI;

    // ---- Dial gradient based on theme RGB ----
    auto dialGradient = [w, glowFactor](int x)->uint16_t {
        float factor = 1.0 - fabs((x - w/2.0)/(w/2.0)) * 0.5;
        factor *= glowFactor;
        if(factor>1.0) factor=1.0;
        if(factor<0.5) factor=0.5;
        uint8_t r = currentTheme.dialR * factor;
        uint8_t g = currentTheme.dialG * factor;
        uint8_t b = currentTheme.dialB * factor;
        return tft.color565(r,g,b);
    };

    int topMargin = 24;
    for(int x=0;x<w;x++){
        double f = startFreq + x*hzPerPixel;
        uint16_t col = dialGradient(x);

        if(fmod(f,10000.0)<hzPerPixel){
            int tickLen = (fmod(f,100000.0)<hzPerPixel)?24:16;
            dialSprite.drawFastVLine(x, topMargin, tickLen, col);

            if(fmod(f,100000.0)<hzPerPixel){
                char buf[8];
                sprintf(buf,"%.1f",f/1000000.0);
                dialSprite.setTextColor(col,currentTheme.bg);
                dialSprite.setTextDatum(textdatum_t::top_center);
                dialSprite.setFont(&fonts::Font4);
                dialSprite.drawString(buf,x,topMargin-10);
            }
        }
    }

    int dividerY = topMargin + 28;
for(int x=0;x<w;x++){
    // Horizontal gradient, left/right darker
    float factor = 1.0 - fabs((x - w/2.0)/(w/2.0)) * 0.5; 
    uint16_t col = tft.color565(currentTheme.dialR * factor,
                                currentTheme.dialG * factor,
                                currentTheme.dialB * factor);
    dialSprite.drawFastVLine(x,dividerY,3,col);
}
    long subDialPos = freq%100000;
    const double subHzPerPixel = (double)span/(double)w;
    const double subStart = (double)subDialPos - (double)(w/2)*subHzPerPixel;
    int baseY = dividerY + 10;
    int labelY = h - 5;

    for(int x=0;x<w;x++){
        double f = subStart + x*subHzPerPixel;
        if(f<0) f+=100000.0;
        if(f>=100000.0) f-=100000.0;
        uint16_t col = dialGradient(x);

        if(fmod(f,5000.0)<subHzPerPixel){
            int tickLen = (fmod(f,10000.0)<subHzPerPixel)?18:8;
            dialSprite.drawFastVLine(x,baseY,tickLen,col);

            int label = (int)(f/1000.0);
            if(label%20==0){
                char buf[6];
                sprintf(buf,"%02d",label);
                dialSprite.setTextColor(col,currentTheme.bg);
                dialSprite.setTextDatum(textdatum_t::bottom_center);
                dialSprite.setFont(&fonts::Font2);
                dialSprite.drawString(buf,x,labelY);
            }
        }
    }

    dialSprite.drawFastVLine(center,0,h,currentTheme.marker);

    int dialY = 60 + 5;
    dialSprite.pushSprite(10,dialY);
}


// ===============================================================
// FINAL STABLE ROTATED LABEL RENDERER (NO BG SQUARES, NO BLINK)
// ===============================================================

// Final rotated label renderer (no double inversion, no blue shift)
void drawRotatedLabel(const char* txt, int cx, int cy, float ang,
                      const lgfx::IFont* font)
{
    LGFX_Sprite tmp(&tft);

    // Select font
    tmp.setFont(font);
    tmp.setTextDatum(textdatum_t::top_left);

    // Text dimensions
    int32_t tw = tmp.textWidth(txt);
    int32_t th = tmp.fontHeight();
    if (tw <= 0 || th <= 0) return;

    // Add small padding
    int sw = tw + 8;
    int sh = th + 8;

    // Create sprite with swapBytes ON (because your display uses swapped order)
    tmp.createSprite(sw, sh);
    tmp.setSwapBytes(true);

    // Mask background (transparent)
    const uint16_t BG = currentTheme.rotateMask;

    tmp.fillSprite(BG);
    tmp.setTextColor(currentTheme.labelColor, BG);

    // Center text
    tmp.drawString(txt, (sw - tw) / 2, (sh - th) / 2);

    // DIRECT buffer access (correct raw color)
    uint16_t* buf = (uint16_t*)tmp.getBuffer();
    if (!buf) { tmp.deleteSprite(); return; }

    float c = cosf(ang);
    float s = sinf(ang);
    float ox = -sw * 0.5f;
    float oy = -sh * 0.5f;

    int W = dialSprite.width();
    int H = dialSprite.height();

    for (int py = 0; py < sh; py++)
    {
        for (int px = 0; px < sw; px++)
        {
            uint16_t p = buf[py * sw + px];

            // Skip transparency
            if (p == BG) continue;

            // UN-SWAP pixel from sprite (because swapBytes=true stored swapped 565)
            uint16_t col = (uint16_t)((p >> 8) | (p << 8));

            // IMPORTANT: NO SOFTWARE INVERSION HERE
            // Screen is already inverted by tft.invertDisplay(true)

            float rx = px + ox;
            float ry = py + oy;

            int dx = cx + (int)roundf(rx * c - ry * s);
            int dy = cy + (int)roundf(rx * s + ry * c);

            if (dx >= 0 && dx < W && dy >= 0 && dy < H)
                dialSprite.drawPixel(dx, dy, col);
        }
    }

    tmp.deleteSprite();
}


// ===============================================================
//                   FINAL CLEAN ARC DIAL
// ===============================================================
// ==================================
// FONT CONTROL FOR ARC DIAL LABELS
// ==================================
const lgfx::IFont* dialOuterFont = &fonts::Font4;   // for MHz + 50kHz labels
const lgfx::IFont* dialInnerFont = &fonts::Font2;   // for kHz labels

void drawArcDial(long freq)
{
    const int w = dialSprite.width();
    const int h = dialSprite.height();
    const int centerX = w / 2;

    const float sweepDeg = 147.0f;
    const float sweepRad = sweepDeg * (M_PI / 180.0f);
    const float halfSweep = sweepRad * 0.5f;

    const int outerR   = 187;
    const int dividerR = outerR - 14;
    const int innerR   = dividerR - 35;

    const int centerY = outerR + 10;

    dialSprite.fillSprite(currentTheme.bg);

    const long span = 100000;
    const double hzPerPixel = (double)span / (double)w;
    const double startFreq  = (double)freq - (double)(w/2)*hzPerPixel;

    auto angleForX = [&](int x)->float {
        float rel = ((float)x - centerX) / (float)(w/2);
        if (rel < -1) rel = -1;
        if (rel >  1) rel =  1;
        return -PI/2 + rel * halfSweep;
    };

    auto shadeArc = [&](float ang)->uint16_t {
        float off = fabs(ang + PI/2) / halfSweep;
        if (off > 1) off = 1;
        float f = 1.0f - off * 0.5f;
        return tft.color565(
            currentTheme.dialR * f,
            currentTheme.dialG * f,
            currentTheme.dialB * f
        );
    };

    auto dialGradient = [&](int x)->uint16_t {
        float f = 1.0f - fabs((x - w/2.0f)/(w/2.0f))*0.5f;
        if (f < 0.5f) f = 0.5f;
        if (f > 1.0f) f = 1.0f;
        return tft.color565(
            currentTheme.dialR * f,
            currentTheme.dialG * f,
            currentTheme.dialB * f
        );
    };

    // ---- OUTER BORDER ----
    for (float ang = -halfSweep - PI/2;
         ang <= halfSweep - PI/2;
         ang += 0.0032f)
    {
        uint16_t col = shadeArc(ang);
        int x = centerX + cosf(ang)*outerR;
        int y = centerY + sinf(ang)*outerR;
        dialSprite.drawPixel(x, y, col);
    }

    // ---- OUTER TICKS / LABELS ----
    for (int x = 0; x < w; x++)
    {
        double f = startFreq + x * hzPerPixel;
        float  ang = angleForX(x);

        uint16_t tickCol = shadeArc(ang);
        uint16_t labelCol = currentTheme.labelColor;

        if (fmod(f,10000.0) < hzPerPixel)
        {
            bool is100k = (fmod(f,100000.0) < hzPerPixel);
            int tickLen = is100k ? 22 : 14;

            int bx = centerX + cosf(ang)*(outerR - 2);
            int by = centerY + sinf(ang)*(outerR - 2);
            int tx = centerX + cosf(ang)*(outerR - 2 - tickLen);
            int ty = centerY + sinf(ang)*(outerR - 2 - tickLen);

            dialSprite.drawLine(bx, by, tx, ty, tickCol);

            float labelAngle = ang + PI/2;

            if (is100k)
            {
                char buf[16];
                sprintf(buf, "%.1f", f / 1000000.0);

                int lx = centerX + cosf(ang)*(outerR - 2 - tickLen - 4);
                int ly = centerY + sinf(ang)*(outerR - 2 - tickLen - 4);

                drawRotatedLabel(buf, lx, ly, labelAngle, dialOuterFont);
            }
            else if (fabs(fmod(f,100000.0) - 50000.0) < hzPerPixel)
            {
                char buf[16];
                sprintf(buf, "%.2f", f / 1000000.0);

                int lx = centerX + cosf(ang)*(outerR - 2 - tickLen - 10);
                int ly = centerY + sinf(ang)*(outerR - 2 - tickLen - 10);

                drawRotatedLabel(buf, lx, ly, labelAngle, dialOuterFont);
            }
        }

        // micro ticks
        if (fabs(fmod(f,10000.0) - 5000.0) < hzPerPixel)
        {
            int micro = 4;
            int bx = centerX + cosf(ang)*(outerR - 2);
            int by = centerY + sinf(ang)*(outerR - 2);
            int tx = centerX + cosf(ang)*(outerR - 2 - micro);
            int ty = centerY + sinf(ang)*(outerR - 2 - micro);
            dialSprite.drawLine(bx, by, tx, ty, tickCol);
        }
    }

    // ---- INNER TICKS + LABELS ----
    long subDialPos = freq % 100000;
    double subHz = (double)span / (double)w;
    double subStart = subDialPos - (double)(w/2) * subHz;

    for (int x = 0; x < w; x++)
    {
        double f = subStart + x * subHz;
        if (f < 0) f += 100000.0;
        if (f >= 100000.0) f -= 100000.0;

        if (fmod(f,5000.0) < subHz)
        {
            float ang = angleForX(x);
            uint16_t tickCol = shadeArc(ang);

            int tickLen = (fmod(f,10000.0)<subHz) ? 16 : 8;

            int bx = centerX + cosf(ang)*innerR;
            int by = centerY + sinf(ang)*innerR;
            int tx = centerX + cosf(ang)*(innerR - tickLen);
            int ty = centerY + sinf(ang)*(innerR - tickLen);

            dialSprite.drawLine(bx, by, tx, ty, tickCol);

            int label = (int)(f / 1000.0);

            if (label % 20 == 0)
            {
                char buf[8];
                sprintf(buf, "%02d", label);

                float labelAngle = ang + PI/2;

                int lx = centerX + cosf(ang)*(innerR - tickLen - 10);
                int ly = centerY + sinf(ang)*(innerR - tickLen - 10);

                drawRotatedLabel(buf, lx, ly, labelAngle, dialInnerFont);
            }
        }
    }

    dialSprite.drawFastVLine(centerX, 0, h, currentTheme.marker);
    dialSprite.pushSprite(0, 65);
}






