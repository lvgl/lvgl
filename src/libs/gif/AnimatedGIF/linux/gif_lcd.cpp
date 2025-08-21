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
#include "../src/AnimatedGIF.cpp"

//#define PIMORONI_HAT

#ifdef PIMORONI_HAT
#define DC_PIN 9
#define RESET_PIN -1
#define CS_PIN 7
#define LED_PIN 13
#define LCD_TYPE LCD_ST7789
#else
// Pin definitions for Adafruit PiTFT HAT
// GPIO 25 = Pin 22
#define DC_PIN 25
// GPIO 27 = Pin 13
#define RESET_PIN -1
// GPIO 8 = Pin 24
#define CS_PIN -1
// GPIO 24 = Pin 18
#define LED_PIN 24
#define MOSI_PIN 0
#define MISO_PIN 0
#define SCK_PIN -1
#define LCD_TYPE LCD_ILI9341
#endif

BB_SPI_LCD lcd;
AnimatedGIF gif;
uint8_t *pStart;

int MilliTime()
{
int iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000*res.tv_sec + res.tv_nsec/1000000;

    return iTime;
} /* MilliTime() */

int main(int argc, char *argv[])
{
int i, iFrame, iTime;
int w, h;

// int spilcdInit(int iLCDType, int bFlipRGB, int bInvert, int bFlipped, int32_t iSPIFreq, int iCSPin, int iDCPin, int iResetPin, int iLEDPin, int iMISOPin, int iMOSIPin, int iCLKPin);
//	i = spilcdInit(&lcd, LCD_TYPE, FLAGS_NONE, 125000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN, -1,-1,-1,1);
        i = lcd.begin(LCD_TYPE, FLAGS_NONE, 80000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN, MISO_PIN, MOSI_PIN, SCK_PIN);
	if (i == 0)
	{
		lcd.setRotation(90);
		lcd.fillScreen(TFT_BLACK);
		lcd.setTextColor(TFT_GREEN, TFT_BLACK);
		lcd.drawStringFast("GIF Demo", 0,0,FONT_12x16);
		gif.begin(BIG_ENDIAN_PIXELS);
		i = gif.open(argv[1], openFile, closeFile, readFile, seekFile, NULL);
		if (i) {
			w = gif.getCanvasWidth();
			h = gif.getCanvasHeight();
			printf("GIF opened; w=%d, h=%d\n", w, h);
			gif.setFrameBuf((uint8_t*)malloc(w * h * 3));
                        pStart = gif.getFrameBuf();
			pStart += (w*h);
			gif.setDrawType(GIF_DRAW_COOKED);
//			gif.setTurboBuf((uint8_t*)malloc(TURBO_BUFFER_SIZE + (w*h)));
			while (1) {
				iFrame = 0;
				iTime = MilliTime();
			while (gif.playFrame(false, NULL, NULL)) {
				lcd.setAddrWindow(0,0,w,h);
				i = MilliTime();
				lcd.pushPixels((uint16_t *)pStart, w * h);
				i = MilliTime() - i;
	//			printf("WriteDataBlock time = %dms\n", i);
				iFrame++;
			}
			iTime = MilliTime() - iTime;
			printf("%d frames in %d ms\n", iFrame, iTime);
			gif.reset(); // repeat
			} // while (1)
		}
	}
	else
	{
		printf("Unable to initialize the spi_lcd library\n");
	}
   return 0;
} /* main() */
