//
// Big memory demo
//
// Demonstrates how to get better draw performance and simpler code when your
// MCU has enough RAM to hold the entire GIF image (canvas)
//
// The 'pattern' GIF used in this demo has a lot of small runs of transparent pixels
// This creates a performance problem when drawing on a SPI LCD since the
// LCD must be told to 'skip' those pixels when drawing each line. If your MCU has
// enough RAM (additional 16K in this case) to hold the entire GIF image in memory
// then the transparent pixels can be managed in memory and the draw function can
// just write continuous runs of pixels to the display. For this particular image
// running on my Nano33 BLE test rig, the 'big memory' way of drawing it runs 3x faster
// than the 'small memory' way.
//
// The pin numbers referenced below are for my custom Nano33 BLE test rig
//
#include <AnimatedGIF.h>
#include <bb_spi_lcd.h>
#include "../test_images/pattern.h"

#define LED_PIN       -1
#define CS_PIN        -1
#define RESET_PIN      A2
#define DC_PIN         A3
#define MOSI_PIN -1
#define SCK_PIN -1
#define MISO_PIN -1

#define DISPLAY_WIDTH 320
AnimatedGIF gif;
#ifndef HAL_ESP32_HAL_H_
uint8_t ucTXBuf[1024];
#endif
SPILCD lcd;

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
     spilcdSetPosition(&lcd, pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight, DRAW_TO_LCD);
  }
  // For all other lines, just push the pixels to the display
  spilcdWriteDataBlock(&lcd, (uint8_t *)pDraw->pPixels, pDraw->iWidth*2, DRAW_TO_LCD);
} /* GIFDraw() */

//
// The memory management functions are needed to keep operating system
// dependencies out of the core library code
//
// memory allocation callback function
void * GIFAlloc(uint32_t u32Size)
{
  return malloc(u32Size);
} /* GIFAlloc() */
// memory free callback function
void GIFFree(void *p)
{
  free(p);
} /* GIFFree() */

void setup() {
  Serial.begin(115200);
//  while (!Serial);

#ifndef HAL_ESP32_HAL_H_
  spilcdSetTXBuffer(ucTXBuf, sizeof(ucTXBuf));
#endif
  spilcdInit(&lcd, LCD_ILI9341, FLAGS_NONE, 8000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN, MISO_PIN, MOSI_PIN, SCK_PIN); // custom ESP32 rig
  spilcdSetOrientation(&lcd, LCD_ORIENTATION_90);
  spilcdFill(&lcd, 0,DRAW_TO_LCD);

  gif.begin(BIG_ENDIAN_PIXELS);
} /* setup() */

void loop() {
  long lTime;
  int iFrames;
  if (gif.open((uint8_t *)ucPattern, sizeof(ucPattern), GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    // Allocate an internal buffer to hold the full canvas size
    // In this case, 128x128 = 16k of RAM
    if (gif.allocFrameBuf(GIFAlloc) == GIF_SUCCESS)
    {
      gif.setDrawType(GIF_DRAW_COOKED); // we want the library to generate ready-made pixels
      lTime = micros();
      iFrames = 0;
      while (gif.playFrame(true, NULL))
      {
        iFrames++;
      }
      lTime = micros() - lTime;
      Serial.printf("total decode time for %d frames = %d us\n", iFrames, (int)lTime);
      gif.freeFrameBuf(GIFFree);
    }
    else
    {
      Serial.println("Insufficient memory!");
    }
    gif.close();
  }
} /* loop() */
