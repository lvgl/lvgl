//
// perf_explained
// Written by Larry Bank (bitbank@pobox.com)
// created February 9, 2025
//
// Example which demonstrates the tradeoffs between memory usage and speed
// Three separate techniques are used to play an animated sequence. The
// animation contains local color palette changes which shows the a limitation
// of one of the techniques used.
// This example requires an ESP32 with PSRAM because the third example
// uses more than 100K of RAM which is not possible to allocate as a single
// block in ESP32 SRAM
//
// N.B. Set the ESP32 FLASH partition scheme to "HUGE APP" or this project won't fit
//
// Cause a compilation error if the target is an ESP32 and PSRAM is not enabled
#if defined(ARDUINO_ARCH_ESP32) && !defined(BOARD_HAS_PSRAM)
#error "Please enable PSRAM support"
#endif

#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
#include "matrix_small.h"
#include "../../test_images/thisisfine_240x179.h"

AnimatedGIF gif;
BB_SPI_LCD lcd;
uint8_t *pFrameBuffer;
int iXOff, iYOff;
bool bDMA = false;
static uint8_t *pDMA;
//
// This version of GIFDraw manages the transparent pixels and palette
// changes by depending on the display (external) framebuffer to hold
// the 'cooked' pixels. This allows the MCU to use a minimum of memory
// but slows down the display of animations which make heavy use of
// transparent pixels. This reason it is slower is because the LCD
// 'write pointer' must be moved over runs of transparent pixels to
// leave the old data on the display. Each time the display switches
// between commands and data takes a lot of time. It's much faster to
// just write continuous runs of pixels without stopping to move the
// write pointer.
//
void GIFDrawSlow(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;

    iWidth = pDraw->iWidth;
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line
    if (y+iYOff >= lcd.height() || pDraw->iX+iXOff >= lcd.width() || iWidth < 1)
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
          lcd.setAddrWindow(pDraw->iX+x+iXOff, y+iYOff, iCount, 1);
          lcd.pushPixels(usTemp, iCount);
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
      lcd.setAddrWindow(pDraw->iX+iXOff, y+iYOff, iWidth, 1);
      lcd.pushPixels(usTemp, iWidth);
    }
} /* GIFDrawSlow() */
//
// This version of GIFDraw sends cooked (fully prepared) rows
// of pixels to the display in one transaction
// No processing of pixel or palette data is needed.
// Each frame can be as big as the full canvas or smaller
// We can set the LCD address window size when getting row 0 of a frame
// since we're not moving the write pointer within each row.
//
void GIFDraw(GIFDRAW *pDraw)
{
  if (pDraw->y == 0) { // set address window on first line
    lcd.setAddrWindow(pDraw->iX + iXOff, pDraw->iY + iYOff, pDraw->iWidth, pDraw->iHeight);
  }
  if (bDMA) {
      memcpy(pDMA, pDraw->pPixels, pDraw->iWidth*2); // use DMA memory to not cause the SPI driver to allocate a block for you
      lcd.pushPixels((uint16_t *)pDMA, pDraw->iWidth, DRAW_TO_LCD | DRAW_WITH_DMA);
  } else {
      lcd.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth);
  }
} /* GIFDraw() */

void setup()
{
  int h, w, iFrame;
  long lTime;
  lcd.begin(DISPLAY_M5STACK_CORES3);
//  lcd.begin(DISPLAY_CYD_543); // 4.3" 480x270 ESP32-S3 W/PSRAM
  lcd.fillScreen(TFT_BLACK);
  gif.begin(GIF_PALETTE_RGB565_BE); // we want big-endian RGB565
  lcd.setTextColor(TFT_GREEN);
  lcd.setFont((lcd.width() > 320) ? FONT_12x16 : FONT_8x8);

  lcd.println("RAW pixel demo (<20K RAM needed)");
  lcd.println("Draws 1 line at a time");
  lcd.println("(GIF has local color palettes)");
  if (gif.open((uint8_t *)matrix_small, sizeof(matrix_small), GIFDrawSlow)) {
    w = gif.getCanvasWidth();
    h = gif.getCanvasHeight();
    iXOff = (lcd.width() - w)/2; // center on the LCD
    iYOff = (lcd.height() - h)/2;
    gif.setDrawType(GIF_DRAW_RAW);
    iFrame = 0;
    lTime = millis();
    while (gif.playFrame(false, NULL)) {
        iFrame++;
    } // play unthrottled to measure the speed
    gif.close();
    lTime = millis() - lTime;
  }
  lcd.setCursor(0, lcd.height() - 16);
  lcd.printf("%d frames in %d ms", iFrame, (int)lTime);
  delay(5000);

  lcd.fillScreen(TFT_BLACK);
  lcd.printf("COOKED pixel demo (%d K needed)\n", 20 + ((w * (h+2))/1024));
  lcd.println("Without buffering the full output");
  lcd.println("local palette changes create errors");
  if (gif.open((uint8_t *)matrix_small, sizeof(matrix_small), GIFDraw)) {
    w = gif.getCanvasWidth();
    h = gif.getCanvasHeight();
    iXOff = (lcd.width() - w)/2; // center on the LCD
    iYOff = (lcd.height() - h)/2;
    pFrameBuffer = (uint8_t *)malloc(w * (h+2)); // 2 extra lines for cooked pixels
    if (pFrameBuffer == NULL) { // trouble, we can't allocate 40K of RAM
      lcd.setTextColor(TFT_RED);
      lcd.printf("Unable to allocate %d bytes!", w * (h+2));
      while (1) {};
    }
    gif.setDrawType(GIF_DRAW_COOKED);
    gif.setFrameBuf(pFrameBuffer);
    // By setting the framebuffer pointer and passing a GIFDraw callback
    // we tell AnimatedGIF that we are only buffering the canvas as 8-bpp
    // and that we need the pixels converted through the palette one line
    // at a time
    iFrame = 0;
    lTime = millis();
    while (gif.playFrame(false, NULL)) {
      iFrame++;
     }
    gif.close();
    lTime = millis() - lTime;
    free(pFrameBuffer);
  }
  lcd.setCursor(0, lcd.height() - 16);
  lcd.printf("%d frames in %d ms", iFrame, (int)lTime);
  delay(5000);

  lcd.fillScreen(TFT_BLACK);
  lcd.println("If we keep a full canvas");
  lcd.printf("of COOKED pixels in memory (%d K)\n", 20 + ((w*h*3)/1024));
  lcd.println("We can handle local palettes");
  pFrameBuffer = (uint8_t *)ps_malloc(w * h * 3);
  if (pFrameBuffer == NULL) { // trouble
      lcd.setTextColor(TFT_RED);
      lcd.setCursor(0,0);;
      lcd.printf("Unable to allocate %d in PSRAM!", w * h * 3);
      while (1) {};
  }
  if (gif.open((uint8_t *)matrix_small, sizeof(matrix_small), NULL)) {
    gif.setDrawType(GIF_DRAW_COOKED);
    gif.setFrameBuf(pFrameBuffer);
    // By setting the framebuffer pointer and passing NULL for GIFDraw
    // we tell AnimatedGIF that we have a large enough framebuffer to hold
    // both the 8-bit indexed canvas pixels AND the RGB565 cooked pixels
    iFrame = 0;
    lTime = millis();
    while (gif.playFrame(false, NULL)) {
        uint16_t *pPixels;
        int y, iX, iY, fW, fH;
        iFrame++;
        // Each frame is fully decoded to RGB565 before we get access
        // to the pixels. The COOKED pixels are placed right after the
        // 8-bit indexed pixels of the canvas
        // We could just send the whole canvas to the LCD each frame, but
        // it's likely that only a portion of it is changing each frame
        iX = gif.getFrameXOff();
        iY = gif.getFrameYOff();
        fW = gif.getFrameWidth();
        fH = gif.getFrameHeight();
        lcd.setAddrWindow(iXOff + iX, iYOff + iY, fW, fH);
        pPixels = (uint16_t *)&pFrameBuffer[w * h]; // cooked pixels start here
        pPixels += iX + (iY * w);
        // the frame is a sub-region of the canvas, so we can't push
        // the pixels in one shot
        for (y = 0; y < fH; y++) {
          lcd.pushPixels(pPixels, fW);
          pPixels += w; // canvas width to the next line
        } // for y
    }
    lTime = millis() - lTime;
    gif.close();
    free(pFrameBuffer);
    lcd.setCursor(0, lcd.height() - 16);
    lcd.printf("%d frames in %d ms", iFrame, (int)lTime);
  }
  delay(5000);
  lcd.fillScreen(TFT_BLACK);
  lcd.println("This demo has no local palette");
  lcd.println("and shows the benefit of DMA");
  lcd.println("(first pass - no DMA)");
  bDMA = false;
  if (gif.open((uint8_t *)thisisfine_240x179, sizeof(thisisfine_240x179), GIFDraw)) {
    w = gif.getCanvasWidth();
    h = gif.getCanvasHeight();
    iXOff = (lcd.width() - w)/2; // center on the LCD
    iYOff = (lcd.height() - h)/2;
    pFrameBuffer = (uint8_t *)ps_malloc(w * (h+2)); // 2 extra lines for cooked pixels
    if (pFrameBuffer == NULL) { // trouble, we can't allocate 40K of RAM
      lcd.setTextColor(TFT_RED);
      lcd.printf("Unable to allocate %d bytes!", w * (h+2));
      while (1) {};
    }
    gif.setDrawType(GIF_DRAW_COOKED);
    gif.setFrameBuf(pFrameBuffer);
    iFrame = 0;
    lTime = millis();
    while (gif.playFrame(false, NULL)) {
      iFrame++;
     }
    gif.close();
    lTime = millis() - lTime;
    free(pFrameBuffer);
  }
  lcd.setCursor(0, lcd.height() - 16);
  lcd.printf("%d frames in %d ms", iFrame, (int)lTime);
  delay(5000);
//
// This demo enables Direct-Memory-Access when sending pixels to the LCD
// Translation - the MCU can send the data to the display while the CPU goes back
// to work on decoding more pixels
// This can effectively reduce the I/O time to 0 if the CPU work takes longer than
// sending the data to the display
//
  lcd.fillScreen(TFT_BLACK);
  lcd.println("Now with DMA");
  bDMA = true;
  // We need to allocate memory that's usable with DMA, otherwise the Espressif SPI driver
  // will allocate (and leak!) a buffer for each transaction
  pDMA = (uint8_t *)heap_caps_malloc(320*2, MALLOC_CAP_DMA);
  if (gif.open((uint8_t *)thisisfine_240x179, sizeof(thisisfine_240x179), GIFDraw)) {
    w = gif.getCanvasWidth();
    h = gif.getCanvasHeight();
    iXOff = (lcd.width() - w)/2; // center on the LCD
    iYOff = (lcd.height() - h)/2;
    pFrameBuffer = (uint8_t *)malloc(w * (h+2)); // 2 extra lines for cooked pixels
    if (pFrameBuffer == NULL) { // trouble, we can't allocate 40K of RAM
      lcd.setTextColor(TFT_RED);
      lcd.printf("Unable to allocate %d bytes!", w * (h+2));
      while (1) {};
    }
    gif.setDrawType(GIF_DRAW_COOKED);
    gif.setFrameBuf(pFrameBuffer);
    iFrame = 0;
    lTime = millis();
    while (gif.playFrame(false, NULL)) {
      iFrame++;
     }
    gif.close();
    lTime = millis() - lTime;
    free(pFrameBuffer);
  }
  free(pDMA);
  lcd.setCursor(0, lcd.height() - 16);
  lcd.printf("%d frames in %d ms", iFrame, (int)lTime);
} /* setup() */

void loop()
{
 // nobody home
} /* loop() */

