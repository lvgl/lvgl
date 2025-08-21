//
// Animated GIF digit demo
// written by Larry Bank
// May 30, 2021
//
#ifdef ESP_PLATFORM
#include <M5Core2.h>
#define tft M5.Lcd
#endif

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include <AnimatedGIF.h>
// animated digit images
#include "digit_0.h"
#include "digit_1.h"
#include "digit_2.h"
#include "digit_3.h"
#include "digit_4.h"
#include "digit_5.h"
#include "digit_6.h"
#include "digit_7.h"
#include "digit_8.h"
#include "digit_9.h"

// Display settings for Adafruit PyPortal
#define TFT_RESET     24
#define TFT_BACKLIGHT 25
#define TFT_D0        34 // Data bit 0 pin (MUST be on PORT byte boundary)
#define TFT_WR        26 // Write-strobe pin (CCL-inverted timer output)
#define TFT_DC        10 // Data/command pin
#define TFT_CS        11 // Chip-select pin
#define TFT_RST       24 // Reset pin
#define TFT_RD         9 // Read-strobe pin
#define TFT_BACKLIGHT 25

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#ifndef ESP_PLATFORM
Adafruit_ILI9341 tft = Adafruit_ILI9341(tft8bitbus, TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST, TFT_RD);
#endif
AnimatedGIF gif[4]; // we need 4 independent instances of the class to animate the 4 digits simultaneously
typedef struct my_private_struct
{
  int xoff, yoff; // corner offset
} PRIVATE;

// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;
    PRIVATE *pPriv = (PRIVATE *)pDraw->pUser;
    
    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > DISPLAY_WIDTH)
       iWidth = DISPLAY_WIDTH - pDraw->iX;
    usPalette = pDraw->pPalette;
    y = pPriv->yoff + pDraw->iY + pDraw->y; // current line
    if (y >= DISPLAY_HEIGHT || pDraw->iX >= DISPLAY_WIDTH || iWidth < 1)
       return; 
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      for (x=0; x<iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent)
           s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }

    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
      int x, iCount;
      pEnd = s + iWidth;
      x = 0;
      iCount = 0; // count non-transparent pixels
      while(x < iWidth)
      {
        c = ucTransparent-1;
        d = usTemp;
        while (c != ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent) // done, stop
          {
            s--; // back up to treat it like transparent
          }
          else // opaque
          {
             *d++ = usPalette[c];
             iCount++;
          }
        } // while looking for opaque pixels
        if (iCount) // any opaque pixels?
        {
          tft.startWrite();
          tft.setAddrWindow(pPriv->xoff + pDraw->iX + x, y, iCount, 1);
#ifdef ESP_PLATFORM
           tft.pushColors(usTemp, iCount, true);
#else
          tft.writePixels(usTemp, iCount, false, false);
#endif
          tft.endWrite();
          x += iCount;
          iCount = 0;
        }
        // no, look for a run of transparent pixels
        c = ucTransparent;
        while (c == ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent)
             iCount++;
          else
             s--; 
        }
        if (iCount)
        {
          x += iCount; // skip these
          iCount = 0;
        }
      }
    }
    else
    {
      s = pDraw->pPixels;
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x=0; x<iWidth; x++)
        usTemp[x] = usPalette[*s++];
      tft.startWrite();
      tft.setAddrWindow(pPriv->xoff + pDraw->iX, y, iWidth, 1);
#ifdef ESP_PLATFORM
      tft.pushColors(usTemp, iWidth, true);
#else
      tft.writePixels(usTemp, iWidth, false, false);
#endif
      tft.endWrite();
    }
} /* GIFDraw() */

const uint8_t * digits[10] = {digit_0, digit_1, digit_2, digit_3, digit_4, digit_5, digit_6, digit_7, digit_8, digit_9};
const size_t lengths[10] = {sizeof(digit_0), sizeof(digit_1), sizeof(digit_2), sizeof(digit_3), sizeof(digit_4), sizeof(digit_5),
                            sizeof(digit_6), sizeof(digit_7), sizeof(digit_8), sizeof(digit_9)};
//
// Display 4 animated digits (80 pixels wide each)
// only animate the digits which change from one iteration to the next
//
void ShowDigits(int iValue, int iOldValue)
{
int i, rc, iBusy;
PRIVATE priv;
int jn, jo, t0, t1;
long lTime;

  priv.yoff = 72; // center digits vertically  
  iBusy = 0;
  // mark digits which need to change with a single bit flag
  jn = iValue; jo = iOldValue;
  for (i=3; i>=0; i--) { // compare old and new values
    t0 = jn % 10; t1 = jo % 10;
    if (t0 != t1) {
      iBusy |= (1 << i);
      gif[i].open((uint8_t *)digits[t0], lengths[t0], GIFDraw); // prepare the right digit animated file
    }
    jn /= 10;
    jo /= 10; // next digit
  }
  while (iBusy) {
    // Draw each frame of each changing digit together so that they animate together
    lTime = millis() + 40; // play the frames at a rate of 25fps (40ms per frame)
    for (i=0; i<4; i++) {
       if (iBusy & (1 << i)) {
         // Advance this animation one frame
         priv.xoff = 80 * i; // each digit is 80 pixels wide
         rc = gif[i].playFrame(false, NULL, (void *)&priv); // draw it and return immediately
         if (!rc) { // animation has ended
            iBusy &= ~(1<<i); // clear the bit indicating this digit is busy
            gif[i].close();
         }
       }
    } // for each digit
    delay(25);
//    while ((millis() - lTime) < 0)
//    {
//      delay(1);
//    };
  } // while digits still changing
} /* ShowDigits() */

void setup() {
  Serial.begin(115200);
//  while (!Serial);

#ifdef ESP_PLATFORM
  M5.begin(true, true, true, true);
//  if (!SD.begin()) {
//    M5.Lcd.println("Card failed, or not present");
//    while (1);
//  }
#else
  // put your setup code here, to run once:
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  pinMode(TFT_RESET, OUTPUT);
  digitalWrite(TFT_RESET, HIGH);
  delay(10);
  digitalWrite(TFT_RESET, LOW);
  delay(10);
  digitalWrite(TFT_RESET, HIGH);
  delay(10);
  tft.begin();
  tft.setRotation(1);
 #endif
  tft.fillScreen(ILI9341_WHITE);
//  tft.setTextColor(ILI9341_YELLOW);
//  tft.setTextSize(2);
  for (int i=0; i<4; i++)
     gif[i].begin(GIF_PALETTE_RGB565_LE);
} /* setup() */

void loop() {
int i, iOld;
  iOld = 9999; // force every digit to be drawn the first time
  for (i=0; i<9999; i++) {
    ShowDigits(i, iOld);
    iOld = i;
    delay(250);
  }
} /* loop() */
