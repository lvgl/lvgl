//
// An example clock application using
// a looping GIF animation as the
// background image
//
// written by Larry Bank (bitbank@pobox.com)
//
// This example is for the Waveshare Touch LCD 1.69 280x240, but can be easily changed for any ESP32
// PSRAM needs to be enabled for this to work
//
#include <bb_spi_lcd.h>  // https://github.com/bitbank2/bb_spi_lcd
#include <AnimatedGIF.h> // https://github.com/bitbank2/AnimatedGIF
#include <bb_rtc.h>      // https://github.com/bitbank2/bb_rtc
#include "loop_clouds_280x240.h"
#include "Roboto_Black_70.h"
BB_SPI_LCD lcd, sprite;
AnimatedGIF gif;
BBRTC rtc;
struct tm myTime;
static uint8_t *pFrameBuffer;
#define SDA_PIN 11
#define SCL_PIN 10

void setup()
{
  int w, h;

    lcd.begin(DISPLAY_WS_LCD_169);
    lcd.setRotation(270); // this display is natively 240x280, so rotate it
    rtc.init(SDA_PIN, SCL_PIN); // init + auto-detect RTC type
    // It's up to you how you want to set the correct time on your RTC device
    // This sketch will simply read the time that's currently set
    gif.begin(GIF_PALETTE_RGB565_BE); // We want the GIF library to produce RGB565 big-endian pixels
    // Open the GIF animation from FLASH memory. Make sure to choose the "Huge App" partition scheme so
    // that it will fit on 4MB FLASH devices
    if (gif.open((uint8_t *)loop_clouds_280x240, sizeof(loop_clouds_280x240), NULL)) {
      // Query the canvas size of the animation (in this case it's 280x240)
      w = gif.getCanvasWidth();
      h = gif.getCanvasHeight();
      // The memory layout of AnimatedGIF "COOKED" pixel mode is a 1-byte per pixel area for the original
      // data and either 1 line or a full Canvas sized frame for the RGB565 processed frame
      // If you don't provide a GIFDraw callback function, the library assumes that you've provided enough
      // memory in the framebuffer to hold both the 1-byte decoded indexed pixels PLUS the full framebuffer
      // of the pixel type you've asked for (immediately after the first area). This is why we're allocating
      // w * h * 3 (w * h) + (w * h * sizeof(uint16_t))
      pFrameBuffer = (uint8_t *)ps_malloc(w * h * 3); // 8-bit and 16-bit output areas for full frame decoding
      if (pFrameBuffer == NULL) { // trouble, we can't allocate 40K of RAM
          lcd.setTextColor(TFT_RED);
          lcd.printf("Unable to allocate %d bytes!", w * h * 3);
          while (1) {};
      }
      // COOKED pixels means we want the library to update the full canvas (of 8-bit indexed pixels)
      // and translate them through the palette each frame. In other words, fully prepare them for display
      gif.setDrawType(GIF_DRAW_COOKED);
      gif.setFrameBuf(pFrameBuffer);
      // Create another instance of the bb_spi_lcd class so that we can use it for compositing the
      // GIF frames and the text on top. Instead of allocating a new buffer for it, we're telling
      // the library to use the 'cooked' pixel area of the buffer we already allocated
      sprite.createVirtual(280, 240, &pFrameBuffer[280*240]); // point to RGB565 output frame (after the indexed pixels)
      sprite.setFreeFont(&Roboto_Black_70); // large font for the time display
      sprite.setTextColor(TFT_BLACK, -1); // black with transparent background
    } // if opened successfully
} /* setup() */

void loop()
{
  int x;
  int16_t x1, y1;
  uint16_t w, h;

  // Center the time horizontally
  sprite.getTextBounds("12:34", 0, 299, &x1, &y1, &w, &h); // Measure the size of the text we want to draw
  x = (lcd.width() - w)/2; // center it on the display
  while (1) { // loop forever
    while (gif.playFrame(true, NULL)) { // ask AnimatedGIF to respect the frame delay time
      // draw the time on top of the GIF frame (each frame will be decoded onto the buffer we provided)
      sprite.setCursor(x, lcd.height()-20); // place the text with the baseline 20 pixels above the bottom
      rtc.getTime(&myTime); // get the current time
      sprite.printf("%02d:%02d", myTime.tm_hour, myTime.tm_min); // display it in the nice looking font
      // Draw the entire screen full of pixels at 1.0 scale
      lcd.drawSprite(0,0, &sprite, 1.0f);
    }
    gif.reset(); // When we reach the last frame of the animation, start over without having to re-open the file
  } // while (1)
} /* loop() */
