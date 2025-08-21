//
//  main.cpp
//  frame buffer GIF demo
//
//  Created by Laurence Bank on 12/2/21.
//  Copyright © 2021 Laurence Bank. All rights reserved.
//
#include "../src/AnimatedGIF.h"
#include "../src/gif.inl"

#include "../test_images/badgers.h"
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <string.h>

GIFIMAGE gif;
int iGIFWidth, iGIFHeight;
uint8_t *pGIFBuf;

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
uint8_t *fbp; // start of frame buffer pointer
int iPitch; // bytes per line on the display
int iScreenWidth, iScreenHeight;
volatile int iStop = 0;

// CTRL-C handler
void my_handler(int signal)
{
   iStop = 1;
} /* my_handler() */

//
// Display a GIF frame on the framebuffer
//
void ShowFrame(int bFullscreen)
{
uint32_t ulPixel=0, *pul;
uint8_t *s, *pPal;
int pixel, x, y;

	if (bFullscreen) { // stretch to fit
	int iFracX, iFracY; // x/y stretch fractions
	int newpixel, iAccumX, iAccumY;
	iFracX = (iGIFWidth * 256) / iScreenWidth;
	iFracY = (iGIFHeight * 256) / iScreenHeight;
	iAccumY = 0;
	for (y=0; y<iScreenHeight; y++) {
		pul = (uint32_t *)&fbp[iPitch * y];
		s = &pGIFBuf[(iAccumY>>8) * iGIFWidth];
		iAccumY += iFracY;
		iAccumX = 0;
		pixel = -1;
		for (x=0; x<iScreenWidth; x++) {
			newpixel = s[(iAccumX >> 8)];
			if (newpixel != pixel) {
				pixel = newpixel;
				pPal = (uint8_t *)&gif.pPalette;
				pPal += pixel * 3;
				ulPixel = 0xff000000 | (pPal[0] << 16) | (pPal[1] << 8) | pPal[2];
			}
			*pul++ = ulPixel;
			iAccumX += iFracX;
		}
	} // for y
        } else { // draw 1:1
	for (y=0; y<iGIFHeight; y++) {
		pul = (uint32_t *)&fbp[iPitch * y];
		s = &pGIFBuf[y * iGIFWidth];
		for (x=0; x<iGIFWidth; x++) {
			pixel = *s++;
			pPal = (uint8_t *)&gif.pPalette;
			pPal += pixel * 3;
			ulPixel = 0xff000000 | (pPal[0] << 16) | (pPal[1] << 8) | pPal[2];
			*pul++ = ulPixel;
		}
	} // for y
	} // 1:1
} /* ShowFrame() */
//
// Callback from GIF library for each line decoded
//
void GIFDraw(GIFDRAW *pDraw)
{
uint8_t *s, *d;
int x, y;

    y = pDraw->iY + pDraw->y; // current line
    s = pDraw->pPixels;
    d = &pGIFBuf[pDraw->iX + (y * iGIFWidth)];
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      for (x=0; x<pDraw->iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent)
           s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t c, ucTransparent = pDraw->ucTransparent;
      for (x=0; x < pDraw->iWidth; x++)
      {
          c = *s++;
          if (c != ucTransparent)
          {
            d[x] = c; 
          }
      }
    }
    else
    {
      s = pDraw->pPixels;
      memcpy(d, s, pDraw->iWidth);
    }
} /* GIFDraw() */

int main(int argc, const char * argv[]) {
int screensize, fbfd = 0, rc;
struct sigaction sigIntHandler;

// Set CTRL-C signal handler
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    printf("Animated GIF Linux Demo\n");
    printf("Run with no parameters to test in-memory decoding\n");
    printf("Or pass a filename on the command line\n\n");
    // Access the framebuffer
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
	printf("Error opening framebuffer device; try disabling the VC4 overlay\n");
	return -1;
    }
    // Get the fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
       printf("Error reading the framebuffer fixed information.\n");
       close(fbfd);
       return -1;
    }
      // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
       printf("Error reading the framebuffer variable information.\n");
       close(fbfd);
       return -1;
    }
    printf("%dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
    // Map framebuffer to user memory
    iScreenWidth = vinfo.xres;
    iScreenHeight = vinfo.yres;
    iPitch = (iScreenWidth * vinfo.bits_per_pixel)/8;
    screensize = finfo.smem_len;
    fbp = (uint8_t*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

    if ((int)fbp == -1) {
       printf("Failed to mmap the framebuffer.\n");
       close(fbfd);
       return -1;
    }

    GIF_begin(&gif, BIG_ENDIAN_PIXELS);
    printf("Starting GIF decoder...\n");
    if (argc == 2) // use filename
        rc = GIF_openFile(&gif, argv[1], GIFDraw);
    else
        rc = GIF_openRAM(&gif, (uint8_t *)ucBadgers, sizeof(ucBadgers), GIFDraw);
    if (rc)
    {
        printf("Successfully opened GIF\n");
	iGIFWidth = GIF_getCanvasWidth(&gif);
	iGIFHeight = GIF_getCanvasHeight(&gif);
        printf("Image size: %d x %d\n", iGIFWidth, iGIFHeight);
	gif.ucDrawType = GIF_DRAW_RAW; // we want the original 8-bit pixels
	gif.ucPaletteType = GIF_PALETTE_RGB888;
	// Allocate a buffer to hold the current GIF frame
	pGIFBuf = malloc(iGIFWidth * iGIFHeight);
        while (!iStop) {
	    int iDelay;
	    while (GIF_playFrame(&gif, &iDelay, NULL)) {
		    ShowFrame(1);
		    // usleep(iDelay * 1000);
            }
	} // waiting for CTRL-C
        GIF_close(&gif);
    }
    // Cleanup
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
