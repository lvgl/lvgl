//
// Best Practices Sketch
//
// An AnimatedGIF comprehensive example sketch which demonstrates rendering an animated GIF
// to an SPI LCD display as efficiently as possible. In the process, shows how to best use:
// - DMA
// - RAM
// - uSD card file access
// written by Larry Bank (bitbank@pobox.com)
//
// The target platform used for this example is the Adafruit PyPortal. The same code can work on many
// different boards with minor changes. This example will display a single GIF
// file stored on the uSD card. Change line 316 to reflect the name of your file.
//
#include <AnimatedGIF.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <SD.h>
//
// This define can select between having a backing buffer (faster output) or using less memory.
// The backing buffer is defined to be 16-bits per pixel and the size of the output display (320x240 in this case)
// 320x240x2 = 153600 bytes. This quantity of RAM is not available on all Microcontrollers and for this reason
// the sketch provides code to draw GIFs with and without the use of this memory. Without the backing RAM, the
// transparent pixels of each frame must be skipped on the LCD framebuffer through a slower set of commands.
//
#define USE_RAM

// Display settings for Adafruit PyPortal
#define TFT_RESET     24
#define TFT_BACKLIGHT 25
#define TFT_D0        34 // Data bit 0 pin (MUST be on PORT byte boundary)
#define TFT_WR        26 // Write-strobe pin (CCL-inverted timer output)
#define TFT_DC        10 // Data/command pin
#define TFT_CS        11 // Chip-select pin
#define TFT_RST       24 // Reset pin
#define TFT_RD         9 // Read-strobe pin

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

#ifndef BUILTIN_SDCARD
#define BUILTIN_SDCARD 32
#endif

#ifdef USE_RAM
//
// Hold the entire (16-bits per pixel) GIF image in RAM to speed up LCD output of transparent sections
// We could potentially store the GIF image as 8-bits per pixel, but that can create a problem...
// Often the color palette is changed on new frames and transparent pixels' palette entries are reused
// for a different color. If we only kept 8-bits per pixel, those re-used entries would show up as the wrong
// color for frames which have transparent pixels and re-use the values. In other words, if pixel 0,0 was
// written as value 0x01 with color palette entry of 0x00ff00 (pure green), suppose that the next frame the
// pixel was to remain green and a transparent color was written to position (0,0) to keep the color from
// changing, but... palette entry 1 was re-used and set to blue (0xff0000). If we only had 8-bits of info
// for the old version of the frame, we would incorrectly draw pixel 0,0 as blue because of the palette change.
// By holding onto the translated color (green), we would display the frame correctly.
// 
static uint16_t usImage[DISPLAY_WIDTH * DISPLAY_HEIGHT];
#endif // USE_RAM

Adafruit_ILI9341 tft = Adafruit_ILI9341(tft8bitbus, TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST, TFT_RD);
AnimatedGIF gif;
File f;
static int iXOff, iYOff; // centering values

void * GIFOpenFile(const char *fname, int32_t *pSize)
{
  f = SD.open(fname);
  if (f)
  {
    *pSize = f.size();
    return (void *)&f;
  }
  return NULL;
} /* GIFOpenFile() */

void GIFCloseFile(void *pHandle)
{
  File *f = static_cast<File *>(pHandle);
  if (f != NULL)
     f->close();
} /* GIFCloseFile() */

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;
    iBytesRead = iLen;
    File *f = static_cast<File *>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
       return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
} /* GIFReadFile() */

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{ 
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  return pFile->iPos;
} /* GIFSeekFile() */

#ifdef USE_RAM
//
// Callback function to receive each line of the image as it decodes.
// This implementation uses a backing buffer to allow more efficient
// handling of transparent pixels. With a RAM copy of the last frame
// transparent pixels can just be skipped. With using system RAM
// we would need to "jump over" the transparent pixels on the LCD's
// frame buffer and this slows things down quite a bit with SPI LCDs
//
void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette;
    int x, iWidth;

    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > DISPLAY_WIDTH)
       iWidth = DISPLAY_WIDTH - pDraw->iX;
    usPalette = pDraw->pPalette;
    if (pDraw->iY + pDraw->y >= DISPLAY_HEIGHT || pDraw->iX >= DISPLAY_WIDTH || iWidth < 1)
       return;
    if (pDraw->y == 0) { // start of frame, set address window on LCD
       tft.dmaWait(); // wait for previous writes to complete before trying to access the LCD
       tft.setAddrWindow(iXOff + pDraw->iX, iYOff + pDraw->iY, pDraw->iWidth, pDraw->iHeight);
       // By setting the address window to the size of the current GIF frame, we can just write
       // continuously over the whole frame without having to set the address window again
    }
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
    d = &usImage[pDraw->iWidth * pDraw->y];
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t c, ucTransparent = pDraw->ucTransparent;
      int x;
      for (x=0; x < iWidth; x++)
      {
        c = *s++;
        if (c != ucTransparent)
           d[x] = usPalette[c];
      }
    }
    else
    {
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x=0; x<iWidth; x++) {
        d[x] = usPalette[s[x]];
      }
    }
    tft.dmaWait(); // wait for last write to complete (the last scan line)
    // We write with block set to FALSE (3rd param) so that we can be decoding the next
    // line while the DMA hardware continues to write data to the LCD controller
    tft.writePixels(d, iWidth, false, false);
} /* GIFDraw() */
#else
//
// This version of the GIFDRAW callback function relies 100% on the RAM of the LCD itself
// to manage transparent pixels. In this case, we need to move the LCD's current write pointer
// to skip over transparent pixels in the current frame. This causes major delays because we have
// to switch from data mode to command mode and send new positioning commands each time there is
// a group of transparent pixels to skip.
//
// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;

    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > DISPLAY_WIDTH)
       iWidth = DISPLAY_WIDTH - pDraw->iX;
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line
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
    //
    // Apply the new pixels to the main image
    // We need to search for runs of transparent pixels and runs of opaque pixels
    // to efficiently send them to the SPI LCD.
    // Transparent pixels will be skipped since they will retain the previous value,
    // and opaque pixels will be transmitted. The performance is negatively affected
    // by transparent pixels because the LCD memory write pointer must be moved to
    // skip over the transparent pixels. If we didn't search for runs of each pixel
    // type and wrote them individually, it would be excruciatingly slow.
    //
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
          tft.dmaWait(); // wait for last write to complete
          tft.setAddrWindow(iXOff + pDraw->iX + x, iYOff + y, iCount, 1);
          tft.writePixels(usTemp, iCount, false, false);
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
    else // There aren't any transparent pixels, so just convert and send the line in one pass
    {
      s = pDraw->pPixels;
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x=0; x<iWidth; x++)
        usTemp[x] = usPalette[*s++];
      tft.dmaWait(); // wait for last write to complete
      // We have to write 1 line at a time (without backing RAM) because each line can contain transparent pixels 
      tft.setAddrWindow(iXOff + pDraw->iX, iYOff + y, iWidth, 1);
      // We write with block set to FALSE (3rd param) so that we can be decoding the next
      // line while the DMA hardware continues to write data to the LCD controller
      tft.writePixels(usTemp, iWidth, false, false);
    }
} /* GIFDraw() */

#endif // USE_RAM

void setup() {
  Serial.begin(115200);
  while (!Serial);

// Note - some systems (ESP32?) require an SPI.begin() before calling SD.begin()
// this code was tested on a Teensy 4.1 board

  if(!SD.begin(BUILTIN_SDCARD))
  {
    Serial.println("SD Card mount failed!");
    return;
  }
  else
  {
    Serial.println("SD Card mount succeeded!");
  }

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
  tft.fillScreen(ILI9341_BLACK);

  // Set LE pixels (the RGB565 palette passed to GIFDRAW)
  gif.begin(LITTLE_ENDIAN_PIXELS);
}

void loop() {
//  Serial.println("About to call gif.open");
  if (gif.open("/HOMER2.GIF", GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw))
  {
    GIFINFO gi;
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    
    // The getInfo() method can be slow since it walks through the entire GIF file to count the frames
    // and gather info about total play time. Comment out this section if you don't need this info
    if (gif.getInfo(&gi)) {
      Serial.printf("frame count: %d\n", gi.iFrameCount);
      Serial.printf("duration: %d ms\n", gi.iDuration);
      Serial.printf("max delay: %d ms\n", gi.iMaxDelay);
      Serial.printf("min delay: %d ms\n", gi.iMinDelay);
    }
    // Center the image on the display
    iXOff = (DISPLAY_WIDTH - gif.getCanvasWidth())/2;
    if (iXOff < 0) iXOff = 0;
    iYOff = (DISPLAY_HEIGHT - gif.getCanvasHeight())/2;
    if (iYOff < 0) iYOff = 0;
    
    tft.startWrite(); // We assume the LCD has exclusive use of the SPI bus (on PyPortal it does)
    while (gif.playFrame(false, NULL))
    {
    }
    gif.close();
    tft.endWrite();
  }
  else
  {
    Serial.printf("Error opening file = %d\n", gif.getLastError());
    while (1)
    {};
  }
}
