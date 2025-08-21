//
// GIF on SPI LCD test program
// Written by Larry Bank
// demo written for the Waveshare 1.3" 240x240 IPS LCD "Hat"
// or the Pimoroni mini display HAT
//
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "bb_spi_lcd.h"
#include "../src/AnimatedGIF.h"
#include "../src/gif.inl"

#define PIMORONI_HAT

#ifdef PIMORONI_HAT
#define DC_PIN 9
#define RESET_PIN -1
#define CS_PIN 7
#define LED_PIN 13
#define LCD_TYPE LCD_ST7789
#else
// Pin definitions for Adafruit PiTFT HAT
// GPIO 25 = Pin 22
#define DC_PIN 22
// GPIO 27 = Pin 13
#define RESET_PIN 13
// GPIO 8 = Pin 24
#define CS_PIN 24
// GPIO 24 = Pin 18
#define LED_PIN 18
#define LCD_TYPE LCD_ST7789_240
#endif

SPILCD lcd;
GIFIMAGE gif;
uint8_t *pStart;

int MilliTime()
{
int iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000*res.tv_sec + res.tv_nsec/1000000;

    return iTime;
} /* MilliTime() */

void GIFDraw(GIFDRAW *pDraw)
{
uint8_t *d;
	d = &pStart[((pDraw->iY + pDraw->y) * pDraw->iWidth + pDraw->iX)*2];
	memcpy(d, pDraw->pPixels, pDraw->iWidth * 2);
}

int main(int argc, char *argv[])
{
int i, iFrame, iTime;
int w, h;

// int spilcdInit(int iLCDType, int bFlipRGB, int bInvert, int bFlipped, int32_t iSPIFreq, int iCSPin, int iDCPin, int iResetPin, int iLEDPin, int iMISOPin, int iMOSIPin, int iCLKPin);
	i = spilcdInit(&lcd, LCD_TYPE, FLAGS_NONE, 125000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN, -1,-1,-1,1);
	if (i == 0)
	{
		spilcdSetOrientation(&lcd, LCD_ORIENTATION_90);
		spilcdFill(&lcd, 0, DRAW_TO_LCD);
		memset(&gif, 0, sizeof(gif));
		GIF_begin(&gif, BIG_ENDIAN_PIXELS);
		i = GIF_openFile(&gif, argv[1], GIFDraw);
		if (i) {
			w = gif.iCanvasWidth;
			h = gif.iCanvasHeight;
			printf("GIF opened; w=%d, h=%d\n", w, h);
			gif.pFrameBuffer = (uint8_t*)malloc(w * h * 3);
                        pStart = &gif.pFrameBuffer[w*h];
			gif.ucDrawType = GIF_DRAW_COOKED;
			gif.pTurboBuffer = (uint8_t*)malloc(TURBO_BUFFER_SIZE + (w*h));
			while (1) {
				iFrame = 0;
				iTime = MilliTime();
			while (GIF_playFrame(&gif, NULL, NULL)) {
				spilcdSetPosition(&lcd, gif.iX,gif.iY,gif.iWidth,gif.iHeight, DRAW_TO_LCD);
				i = MilliTime();
				spilcdWriteDataBlock(&lcd, pStart, gif.iWidth * gif.iHeight * 2, DRAW_TO_LCD);
				i = MilliTime() - i;
	//			printf("WriteDataBlock time = %dms\n", i);
				iFrame++;
			}
			iTime = MilliTime() - iTime;
			printf("%d frames in %d ms\n", iFrame, iTime);
			GIF_reset(&gif); // repeat
			} // while (1)
		}
	}
	else
	{
		printf("Unable to initialize the spi_lcd library\n");
	}
   return 0;
} /* main() */
