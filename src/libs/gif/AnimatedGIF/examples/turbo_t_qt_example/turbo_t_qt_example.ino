//
// Example of how to use Turbo mode on the LilyGo T-QT
// This code can work equally well with a wide variety of MCUs and displays
//
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
#define GIF_NAME earth_128x128
#include "../test_images/earth_128x128.h"

uint8_t *pTurboBuffer;
uint8_t *pFrameBuffer;

AnimatedGIF gif;
BB_SPI_LCD lcd;
int iOffX, iOffY;
//
// Draw callback from the AnimatedGIF decoder
//
// Called once for each line of the current frame
// MCUs with minimal RAM would have to process "RAW" pixels into "COOKED" here.
// "Cooking" involves testing for disposal methods, merging non-transparent pixels
// and translating the raw pixels through the palette to generate the final output.
// The code for MCUs with enough RAM is much simpler because the AnimatedGIF library can
// generate "cooked" pixels that are ready to send to the display as-is.
//
void GIFDraw(GIFDRAW *pDraw)
{
  if (pDraw->y == 0) { // set the memory window (once per frame) when the first line is rendered
    lcd.setAddrWindow(iOffX + pDraw->iX, iOffY + pDraw->iY, pDraw->iWidth, pDraw->iHeight);
  }
  // For all other lines, just push the pixels to the display. We requested 'COOKED'big-endian RGB565 and
  // the library provides them here. No need to do anything except push them right to the display
  lcd.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth, DRAW_TO_LCD | DRAW_WITH_DMA);
} /* GIFDraw() */

void setup() {
  Serial.begin(115200);

  gif.begin(GIF_PALETTE_RGB565_BE); // Set the cooked output type we want (compatible with SPI LCDs)

  // Take a look in bb_spi_lcd.h for a list of popular products which have a pre-configured display
  // name. Alternatively, you can pass the library the GPIO pin numbers, but I made it easier
  // for well-known IoT products by keeping those details inside my library.
  lcd.begin(DISPLAY_T_QT); // Initialize the display controller of the LilyGo T-QT
  lcd.fillScreen(TFT_BLACK);
} /* setup() */

void loop() {
  long lTime;
  int iFrames, iFPS;

// Allocate a buffer to enable Turbo decoding mode (~50% faster)
// it requires enough space for the full "raw" canvas plus about 32K workspace for the decoder
  pTurboBuffer = (uint8_t *)heap_caps_malloc(TURBO_BUFFER_SIZE + (128*128), MALLOC_CAP_8BIT);
  pFrameBuffer = (uint8_t *)heap_caps_malloc(128*128*sizeof(uint16_t), MALLOC_CAP_8BIT); // this is for the full frame RGB565 pixels
  while (1) { // loop forever
     // The GIFDraw callback is optional if you use Turbo mode (pass NULL to disable it). You can either
     // manage the transparent pixels + palette conversion yourself or provide a framebuffer for the 'cooked'
     // version of the canvas size (setDrawType to GIF_DRAW_FULLFRAME).
      if (gif.open((uint8_t *)GIF_NAME, sizeof(GIF_NAME), GIFDraw)) {
      Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
      gif.setDrawType(GIF_DRAW_COOKED); // We want the library to generate ready-made pixels
      gif.setFrameBuf(pFrameBuffer); // for Turbo+cooked, we need to supply a full sized output framebuffer
      gif.setTurboBuf(pTurboBuffer); // Set this before calling playFrame()
      iOffX = (lcd.width() - gif.getCanvasWidth())/2; // center on the display
      iOffY = (lcd.height() - gif.getCanvasHeight())/2;
      lTime = micros();
      // Decode frames until we hit the end of the file
      // false in the first parameter tells it to return immediately so we can test the decode speed
      // Change to true if you would like the animation to run at the speed programmed into the file
      iFrames = 0;
      while (gif.playFrame(false, NULL)) {
        iFrames++;
      }
      lTime = micros() - lTime;
      iFPS = (iFrames * 10000000) / lTime; // get 10x FPS to make an integer fraction of 1/10th 
      Serial.printf("total decode time for %d frames = %d us, %d.%d FPS\n", iFrames, (int)lTime, iFPS/10, iFPS % 10);
      gif.close(); // You can also use gif.reset() instead of close() to start playing the same file again
    } // if the file opened successfully
  } // while (1)
} /* loop() */
