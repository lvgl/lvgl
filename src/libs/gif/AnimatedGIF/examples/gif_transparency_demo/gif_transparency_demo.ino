//
// GIF transparency demo
//
// written by Larry Bank
// Demonstrates how to draw a single GIF image with transparency
// on an LCD in various positions and rotation angles
// Uses a 120x100 5-bpp palette image
//
#include "octocat_4bpp.h"
#include <AnimatedGIF.h>
#include <bb_spi_lcd.h>

AnimatedGIF gif;
SPILCD lcd;
static uint8_t ucTXBuf[1024];
#define TFT_CS 10
#define TFT_RST -1
#define TFT_DC 9
#define TFT_CLK 13
#define TFT_MOSI 11
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

typedef struct my_private_struct
{
  int xoff, yoff; // corner offset
  int angle; // rotation angle (0,1,2,3 = 0, 90, 180, 270)
} PRIVATE;
//
// GIFDraw callback
//
// Called for every scan line of the image as it decodes
// The pixels delivered are the 8-bit native GIF output
// The palette is either RGB565 or the original 24-bit RGB values
// depending on the pixel type selected with gif.begin()
//
void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, iWidth;
    int startx, starty;
    int bRotate;
    PRIVATE *pPriv = (PRIVATE *)pDraw->pUser;

    switch (pPriv->angle) {
      case 0:
        bRotate = 0;
        startx = pPriv->xoff + pDraw->iX;
        starty = pPriv->yoff + pDraw->iY + pDraw->y;
      break;
      case 1: // 90
        bRotate = 1;
        startx = pPriv->xoff + pDraw->iX + pDraw->y;
        starty = pPriv->yoff + pDraw->iY;
      break;
      case 2: // 180
        bRotate = 0;
        startx = pPriv->xoff + pDraw->iX;
        starty = pPriv->yoff + pDraw->iHeight - pDraw->y;
      break;
      case 3: // 270
        bRotate = 1;
        startx = pPriv->xoff + pDraw->iX + (pDraw->iHeight - pDraw->y);
        starty = pPriv->yoff + pDraw->iY;
      break;
    }
    usPalette = pDraw->pPalette;
    iWidth = pDraw->iWidth;
    if (iWidth > DISPLAY_WIDTH)
       iWidth = DISPLAY_WIDTH;
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
          if (bRotate)
            spilcdSetPosition(&lcd, startx, starty + x, 1, iCount, DRAW_TO_LCD);
          else
            spilcdSetPosition(&lcd, startx+x, starty, iCount, 1, DRAW_TO_LCD);
          spilcdWriteDataBlock(&lcd, (uint8_t *)usTemp, iCount*2, DRAW_TO_LCD);
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
      if (bRotate)
        spilcdSetPosition(&lcd, startx, starty, 1, iWidth, DRAW_TO_LCD);
      else
        spilcdSetPosition(&lcd, startx, starty, iWidth, 1, DRAW_TO_LCD);
      spilcdWriteDataBlock(&lcd, (uint8_t *)usTemp, iWidth*2, DRAW_TO_LCD);
    }
} /* GIFDraw() */

void setup() {
  spilcdSetTXBuffer(ucTXBuf, sizeof(ucTXBuf));
  spilcdInit(&lcd, LCD_ILI9341, FLAGS_NONE, 40000000, TFT_CS, TFT_DC, TFT_RST, -1, -1, TFT_MOSI, TFT_CLK);
  spilcdSetOrientation(&lcd, LCD_ORIENTATION_270);
  spilcdFill(&lcd, 0xf81f, DRAW_TO_LCD);
  spilcdWriteString(&lcd, 34, 0, (char *)"GIF Transparency Demo", 0xffff, 0xf81f, FONT_12x16, DRAW_TO_LCD);
  spilcdWriteString(&lcd, 34, 16, (char *)"Draws image at random", 0xffff, 0xf81f, FONT_12x16, DRAW_TO_LCD);
  spilcdWriteString(&lcd, 52, 32, (char *)"locations & angles", 0xffff, 0xf81f, FONT_12x16, DRAW_TO_LCD);
  Serial.begin(115200);
  delay(2000);
  gif.begin(GIF_PALETTE_RGB565_BE); // big endian pixels
}

void loop() {
  int rc, i = 0;
  PRIVATE priv;

  spilcdFill(&lcd, 0xf81f, DRAW_TO_LCD);
  rc = gif.open((uint8_t *)octocat_4bpp, sizeof(octocat_4bpp), GIFDraw);
  if (rc) {
      Serial.printf("image specs: (%d x %d)\n", gif.getCanvasWidth(), gif.getCanvasHeight());
      for (int i=0; i<1000; i++) { // loop forever
        priv.xoff = rand() & 0xff;
        priv.yoff = rand() % 160;
        priv.angle = rand() & 3; // note 180 & 270 degrees are not mirrored correctly
        rc = gif.playFrame(false, NULL, &priv);
        gif.reset(); // Allow decoding to occur again without re-opening the file
//        delay(1000);
      }
      gif.close();
  }
}
