#include <AnimatedGIF.h>
#include <bb_spi_lcd.h>
#include "../test_images/badgers.h"
AnimatedGIF gif;
BB_SPI_LCD lcd;
int iXOff, iYOff;
//
// The GIFDraw callback is passed each line of image data one by one
// When using the 'COOKED' output mode, the pixels are translated through the palette
// and ready to draw directly on the output device.
// GIF files can make things complicated due to dynamic palette changes and transparent pixels
// COOKED output can look wrong if only drawing each line and a local palette change has
// occurred on the canvas; parts that are not overwritten by the current frame will have the wrong
// colors.
//
// N.B.
// When using microcontrollers for playing GIF files, it's best to create animations which only use
// a global palette. In order to draw local palette changes, the entire canvas of pixels must be run
// through the palette each time it changes. This would slow down the output considerably and make
// it impractical on SPI LCDs and MCUs with a small amount of RAM
//
void GIFDraw(GIFDRAW *pDraw)
{
    if (pDraw->y == 0) { // first line, set output window on the LCD
      lcd.setAddrWindow(iXOff + pDraw->iX, iYOff + pDraw->iY, pDraw->iWidth, pDraw->iHeight);
    }
    // Push the current line of pixels to the display
    // We opted for 'COOKED' output, so the pixels have been converted for transparency and palette lookup
    lcd.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth);
} /* GIFDraw() */

void setup() {
  lcd.begin(DISPLAY_WS_AMOLED_18); // Waveshare ESP32-S3 AMOLED 1.8" 368x448
  lcd.fillScreen(TFT_BLACK);
  lcd.setFont(FONT_12x16);
  gif.begin(BIG_ENDIAN_PIXELS); // bb_spi_lcd assumes all displays use big-endian RGB565 pixels
}

void loop() {
uint8_t *pFramebuf;
int w, h;

  if (gif.open((uint8_t *)badgers, sizeof(badgers), GIFDraw)) {
    gif.setDrawType(GIF_DRAW_COOKED);
    w = gif.getCanvasWidth();
    h = gif.getCanvasHeight();
    iXOff = (lcd.width() - w)/2; // center it on the LCD
    iYOff = (lcd.height() - h)/2;
    // For cooked (fully prepared) output we need to allocate a buffer the size of the canvas+2 lines
    pFramebuf = (uint8_t *)malloc(w * (h+2));
    if (!pFramebuf) { // out of memory
      lcd.setTextColor(TFT_RED);
      lcd.print("Insufficient memory!");
      while (1) {}; // stop
    }
    gif.setFrameBuf(pFramebuf);
    lcd.setTextColor(TFT_GREEN);
    lcd.println("GIF opened");
    lcd.printf("Canvas: %d x %d", w, h);
    while (1) { // play it looping forever
      while (gif.playFrame(true, NULL)) {  /* nothing needed here */ }
      gif.reset();
    }
  } // GIF openeded successfully
} /* loop() */
