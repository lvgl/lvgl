//
// Two Instances
// This example shows how to run two simultaneous instances of the
// AnimatedGIF class to have two animations running at the same time
// written by Larry Bank (bitbank@pobox.com)
// Febrary 23, 2025
//
#include <AnimatedGIF.h>
#include <bb_spi_lcd.h>
BB_SPI_LCD lcd;
AnimatedGIF gif1, gif2; // two instances of the class
#include "../test_images/earth_128x128.h"
#include "../test_images/badgers.h"
static uint8_t *pFrameBuffer1, *pFrameBuffer2;
static uint8_t pDMA[640]; // for ESP32, we need DMA RAM
int iXOff1, iXOff2;
int iYOff = (240-128)/2; // center it vertically
void GIFDraw(GIFDRAW *pDraw)
{
  int xoff = *(int *)pDraw->pUser; // get the horizontal offset
  if (pDraw->y == 0) { // set address window on first line
    lcd.setAddrWindow(pDraw->iX + xoff, pDraw->iY + iYOff, pDraw->iWidth, pDraw->iHeight);
  }
  memcpy(pDMA, pDraw->pPixels, pDraw->iWidth*2); // use DMA memory to not cause the SPI driver to allocate a block for you
  lcd.pushPixels((uint16_t *)pDMA, pDraw->iWidth, DRAW_TO_LCD | DRAW_WITH_DMA);
} /* GIFDraw() */

void setup()
{
  int w, h;
  gif1.begin(GIF_PALETTE_RGB565_BE); // we want big-endian RGB565
  gif2.begin(GIF_PALETTE_RGB565_BE);
  lcd.begin(DISPLAY_TUFTY2040);
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_GREEN);
  lcd.setFont((lcd.width() > 320) ? FONT_12x16 : FONT_8x8);    
  lcd.println("Animated GIF Dual Instance Demo");
  lcd.println("Plays 2 unrelated animations together");
  // Both instances can share the same draw callback
  // We should check return codes :(
  gif1.open((uint8_t *)earth_128x128, sizeof(earth_128x128), GIFDraw);
  gif2.open((uint8_t *)badgers, sizeof(badgers), GIFDraw);
    w = gif1.getCanvasWidth();
    h = gif1.getCanvasHeight();
    iXOff1 = (lcd.width() - w*2)/2; // center on the LCD
    iXOff2 = iXOff1 + 128; // place them side by side
    gif1.setDrawType(GIF_DRAW_COOKED);
    gif2.setDrawType(GIF_DRAW_COOKED);
    pFrameBuffer1 = (uint8_t *)malloc(w * (h+2)); // both animations are the same size
    pFrameBuffer2 = (uint8_t *)malloc(w * (h+2));
    if (!pFrameBuffer1 || !pFrameBuffer2) {
      lcd.setTextColor(TFT_RED);
      lcd.println("Mem alloc failed!");
      while (1) {}
    }
    gif1.setFrameBuf(pFrameBuffer1);
    gif2.setFrameBuf(pFrameBuffer2);
    while (1) { // play forever
    // This is a very simple example that doesn't account for differences in playback
    // rate between the two animations. It is simply to show that multiple instances of AnimatedGIF
    // can work independently, yet share the GIFDraw callback
      if (!gif1.playFrame(false, NULL, (void *)&iXOff1)) {
        gif1.reset(); // start over
      }
      if (!gif2.playFrame(false, NULL, (void *)&iXOff2)) {
        gif2.reset(); // start over
      }
    } // while (1)
} /* setup() */

void loop()
{
}

