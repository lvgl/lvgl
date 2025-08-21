//
// This example shows how to use "cooked" pixel mode on the CYD (Cheap Yellow Display - an ESP32 + SPI LCD)
// With cooked pixels, we can send each line of pixels to the display without "jumping over" transparent pixels
// This almost always results in much higher display performance because switching between command and data mode
// on SPI displays can cause significant slowdowns.
//
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
// Set the name of the example animation
#define GIF_NAME thisisfine_240x179
// Use any of the provided examples
#include "../test_images/thisisfine_240x179.h"

uint8_t *pFrameBuffer;

AnimatedGIF gif;
BB_SPI_LCD lcd;
int iOffX, iOffY;
//
// Draw callback from GIF decoder
//
// called once for each line of the current frame
// MCUs with very little RAM would have to test for disposal methods, transparent pixels
// and translate the 8-bit pixels through the palette to generate the final output.
// The code for MCUs with enough RAM is much simpler because the AnimatedGIF library can
// generate "cooked" pixels that are ready to send to the display
//
void GIFDraw(GIFDRAW *pDraw)
{
  if (pDraw->y == 0) { // set the memory window when the first line is rendered
    lcd.setAddrWindow(iOffX + pDraw->iX, iOffY + pDraw->iY, pDraw->iWidth, pDraw->iHeight);
  }
  // For all other lines, just push the pixels to the display
  lcd.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth, DRAW_TO_LCD | DRAW_WITH_DMA);
} /* GIFDraw() */

void setup() {
  Serial.begin(115200);

  lcd.begin(DISPLAY_CYD);
  lcd.setRotation(0);
  lcd.fillScreen(TFT_BLACK);
} /* setup() */

void loop() {
  long lTime;
  int iLoop, w, h, iFrames, iFPS;

gif.begin(GIF_PALETTE_RGB565_BE); // the SPI display's native pixel format is RGB565 big endian
if (gif.open((uint8_t *)GIF_NAME, sizeof(GIF_NAME), GIFDraw)) {
      w = gif.getCanvasWidth();
      h = gif.getCanvasHeight();
      lcd.setTextColor(TFT_GREEN, TFT_BLACK);
      lcd.setFont(FONT_12x16);
      lcd.printf("GIF Canvas %dx%d\n", w, h);
      // We need fast SRAM the size of the GIF canvas as 8-bit pixels + extra space for 1 line of cooked pixels (canvas width * sizeof(uint16_t))
      pFrameBuffer = (uint8_t *)heap_caps_malloc(w * (h+2), MALLOC_CAP_8BIT);
      if (pFrameBuffer == NULL) {
        lcd.setTextColor(TFT_RED, TFT_BLACK);
        lcd.println("Memory Error!");
        lcd.printf("Failed to allocate %d bytes\n", w*(h+2));
        lcd.println("Halted");
        while (1) {}; // not enough memory to continue
      }
      iOffX = (lcd.width() - w)/2;
      iOffY = (lcd.height() - h)/2;
      iLoop = 0;
  while (1) {
      gif.setDrawType(GIF_DRAW_COOKED); // we want the library to generate ready-made pixels
      gif.setFrameBuf(pFrameBuffer); // pass it the buffer to hold the canvas as 8-bit pixels (it merges new opaque pixels for each frame rendered)
      lTime = micros();
      iFrames = 0;
      while (gif.playFrame(iLoop != 0, NULL)) { // Use the first loop to see the unthrottled library speed
        iFrames++;
      }
      if (iLoop == 0) { // show the maximum possible render speed the first time through the animation loop
        lTime = micros() - lTime;
        iFPS = (iFrames * 10000000) / lTime;
        lcd.setCursor(0, 304); // show speed at the bottom
        lcd.printf("Max Speed: %d.%d FPS", iFPS/10, iFPS % 10);
      }
      gif.reset(); // don't close() the file here, just reset to frame 0 to repeat the sequence forever
      iLoop++;
    } // while (1)
  } // if GIF opened successfully
} /* loop() */
