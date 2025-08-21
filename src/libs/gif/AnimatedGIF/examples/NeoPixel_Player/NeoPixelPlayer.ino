//
// LOLRGB GIF player example
//
// Written by Larry Bank
//
// This Arduino example sketch demonstrates how to play animated GIF images
// on the UnexpectedMaker 5x14 RGB LED shield. This code depends on two libraries
// (both are available from the Arduino library manager), my AnimatedGIF library and
// Adafruit's NeoPixel.
//
#include <AnimatedGIF.h>
#include "rainbow_5x14.h"

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        14 // TinyPico + LOLRGB Shield use this GPIO pin

// 5x14 = 70
#define NUMPIXELS 70
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
AnimatedGIF gif; // static instance of the class uses 22.5K of RAM

// Normal color values are too bright on the LED array, so reduce the values
#define BRIGHT_SHIFT 3

//
// GIF decoder callback function
// called once per line as the image is decoded
//
void GIFDraw(GIFDRAW *pDraw)
{
uint8_t r, g, b, *s, *p, *pPal = (uint8_t *)pDraw->pPalette;
int x, y = pDraw->iY + pDraw->y;

    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      p = &pPal[pDraw->ucBackground * 3];
      r = p[0] >> BRIGHT_SHIFT; g = p[1] >> BRIGHT_SHIFT; b = p[2] >> BRIGHT_SHIFT;
      for (x=0; x<pDraw->iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent) {
           pixels.setPixelColor(((4-x)*14)+y, pixels.Color(r, g, b));
        }
      }
      pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      const uint8_t ucTransparent = pDraw->ucTransparent;
      for (x=0; x<pDraw->iWidth; x++)
      {
        if (s[x] != ucTransparent) {
           p = &pPal[s[x] * 3];
           pixels.setPixelColor(((4-x)*14)+y, pixels.Color(p[0]>>BRIGHT_SHIFT, p[1]>>BRIGHT_SHIFT, p[2]>>BRIGHT_SHIFT));
        }
      }
    }
    else // no transparency, just copy them all
    {
      for (x=0; x<pDraw->iWidth; x++)
      {
           p = &pPal[s[x] * 3];
           pixels.setPixelColor(((4-x)*14)+y, pixels.Color(p[0]>>BRIGHT_SHIFT, p[1]>>BRIGHT_SHIFT, p[2]>>BRIGHT_SHIFT));
      }
    }
    if (pDraw->y == pDraw->iHeight-1) // last line has been decoded, display the image
      pixels.show();
} /* GIFDraw() */

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();
  gif.begin(GIF_PALETTE_RGB888); // request 24-bit palette
} /* setup() */

void loop() {
  int rc;
  // This 13K GIF image is compiled with the sketch and read from FLASH memory
  rc = gif.open((uint8_t *)rainbow_5x14, sizeof(rainbow_5x14), GIFDraw);
  if (rc) {
    while (rc) {
      rc = gif.playFrame(true, NULL); // play a frame and pause for the correct amount of time
    }
    gif.close();
  }
} /* loop() */
