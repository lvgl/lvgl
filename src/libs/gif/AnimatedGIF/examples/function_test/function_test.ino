//
// AnimatedGIF Function Tests
// The full function test suite, including fuzz tests, is only available
// for MacOS/Linux/Windows
// This collection of tests can run on the ESP32
//
#include <AnimatedGIF.h>
#include "../../test_images/earth_128x128.h"
#include "../../test_images/green.h"
AnimatedGIF gif;
uint32_t u32Pixel;

void GIFDraw(GIFDRAW *pDraw)
{
    if (pDraw->y == 0) {
        u32Pixel = *(uint32_t *)pDraw->pPixels; // grab 1 or more pixels to test
    }
} /* GIFDraw() */

//
// Simple logging print
//
void GIFLOG(int line, char *string, const char *result)
{
    Serial.printf("Line: %d: msg: %s%s\n", line, string, result);
} /* GIFLOG() */

void setup()
{
int i, w, h, iFrame, iTotal = 0;
uint8_t *pFuzzData;
char *szTestName;
int iTotalPass, iTotalFail;
uint8_t *pFrameBuffer;
const char *szStart = " - START";

    Serial.begin(115200);
    delay(3000); // wait for USB-CDC to start
    Serial.println("GIF function tests");
    iTotalPass = iTotalFail = iTotal++;
    // Test 1 - Decode a file to completion
    szTestName = (char *)"GIF full file decode";
    iTotal++;
    GIFLOG(__LINE__, szTestName, szStart);
    if (gif.open((uint8_t *)earth_128x128, sizeof(earth_128x128), GIFDraw)) {
        w = gif.getCanvasWidth();
        h = gif.getCanvasHeight();
        pFrameBuffer = (uint8_t *)malloc(w * (h+2)); // 2 extra lines for cooked pixels
        gif.setDrawType(GIF_DRAW_COOKED);
        gif.setFrameBuf(pFrameBuffer);
        // By setting the framebuffer pointer and passing a GIFDraw callback
        // we tell AnimatedGIF that we are only buffering the canvas as 8-bpp
        // and that we need the pixels converted through the palette one line
        // at a time
        iFrame = 0;
        while (gif.playFrame(false, NULL)) {
            iFrame++;
        }
        free(pFrameBuffer);
        if (iFrame == 102) {
            iTotalPass++;
            GIFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            iTotalFail++;
            GIFLOG(__LINE__, szTestName, " - FAILED");
        }
    } else {
        GIFLOG(__LINE__, szTestName, "Error opening GIF file.");
    }
    // Test 2 - Verify bad parameters return an error
    szTestName = (char *)"GIF bad parameter test 1";
    iTotal++;
    GIFLOG(__LINE__, szTestName, szStart);
    if (gif.open((uint8_t *)earth_128x128, sizeof(earth_128x128), NULL)) {
        gif.setDrawType(GIF_DRAW_COOKED); // set cooked pixel type without a framebuffer nor GIFDRAW callback
        gif.setFrameBuf(NULL);
        gif.playFrame(false, NULL);
        if (gif.getLastError() == GIF_INVALID_PARAMETER) {
            iTotalPass++;
            GIFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            iTotalFail++;
            GIFLOG(__LINE__, szTestName, " - FAILED");
        }
    } else {
        GIFLOG(__LINE__, szTestName, "Error opening GIF file.");
    }
    // Test 3 - Verify bad parameters return an error
    szTestName = (char *)"GIF bad parameter test 2";
    iTotal++;
    GIFLOG(__LINE__, szTestName, szStart);
    if (gif.open((uint8_t *)earth_128x128, sizeof(earth_128x128), NULL)) {
        gif.setDrawType(GIF_DRAW_RAW); // set raw pixel type without a GIFDRAW callback
        gif.setFrameBuf(NULL);
        gif.playFrame(false, NULL);
        if (gif.getLastError() == GIF_INVALID_PARAMETER) {
            iTotalPass++;
            GIFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            iTotalFail++;
            GIFLOG(__LINE__, szTestName, " - FAILED");
        }
    } else {
        GIFLOG(__LINE__, szTestName, "Error opening GIF file.");
    }
    // Test 4 - Verify raw pixel format
    szTestName = (char *)"GIF verify raw pixel format";
    iTotal++;
    GIFLOG(__LINE__, szTestName, szStart);
    if (gif.open((uint8_t *)green, sizeof(green), GIFDraw)) {
        u32Pixel = 0xffff;
        gif.setDrawType(GIF_DRAW_RAW); // set cooked pixel type without a framebuffer nor GIFDRAW callback
        gif.setFrameBuf(NULL);
        gif.playFrame(false, NULL);
        if (u32Pixel == 0x0) { // should be 4 pixels of color 0 (green)
            iTotalPass++;
            GIFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            iTotalFail++;
            GIFLOG(__LINE__, szTestName, " - FAILED");
        }
    } else {
        GIFLOG(__LINE__, szTestName, "Error opening GIF file.");
    }
    // Test 5 - Verify cooked pixel format 1
    szTestName = (char *)"GIF verify cooked pixel format 1 (RGB565_LE)";
    iTotal++;
    GIFLOG(__LINE__, szTestName, szStart);
    gif.begin(GIF_PALETTE_RGB565_LE);
    if (gif.open((uint8_t *)green, sizeof(green), GIFDraw)) {
        w = gif.getCanvasWidth();
        h = gif.getCanvasHeight();
        pFrameBuffer = (uint8_t *)malloc(w * (h+2)); // 2 extra lines for cooked pixels
        gif.setFrameBuf(pFrameBuffer);
        gif.setDrawType(GIF_DRAW_COOKED);
        u32Pixel = 0xffff;
        gif.playFrame(false, NULL);
        if (u32Pixel == 0x17491749) { // should be 2 pixels of (green)
            iTotalPass++;
            GIFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            iTotalFail++;
            GIFLOG(__LINE__, szTestName, " - FAILED");
        }
        free(pFrameBuffer);
    } else {
        GIFLOG(__LINE__, szTestName, "Error opening GIF file.");
    }
    // Test 6 - Verify cooked pixel format 2
    szTestName = (char *)"GIF verify cooked pixel format 2 (RGB565_BE)";
    iTotal++;
    GIFLOG(__LINE__, szTestName, szStart);
    gif.begin(GIF_PALETTE_RGB565_BE);
    if (gif.open((uint8_t *)green, sizeof(green), GIFDraw)) {
        w = gif.getCanvasWidth();
        h = gif.getCanvasHeight();
        pFrameBuffer = (uint8_t *)malloc(w * (h+2)); // 2 extra lines for cooked pixels
        gif.setFrameBuf(pFrameBuffer);
        gif.setDrawType(GIF_DRAW_COOKED);
        u32Pixel = 0xffff;
        gif.playFrame(false, NULL);
        if (u32Pixel == 0x49174917) { // should be 2 pixels of (green)
            iTotalPass++;
            GIFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            iTotalFail++;
            GIFLOG(__LINE__, szTestName, " - FAILED");
        }
        free(pFrameBuffer);
    } else {
        GIFLOG(__LINE__, szTestName, "Error opening GIF file.");
    }
    // Test 7 - Verify cooked pixel format 3
    szTestName = (char *)"GIF verify cooked pixel format 3 (RGB888)";
    iTotal++;
    GIFLOG(__LINE__, szTestName, szStart);
    gif.begin(GIF_PALETTE_RGB888);
    if (gif.open((uint8_t *)green, sizeof(green), GIFDraw)) {
        w = gif.getCanvasWidth();
        h = gif.getCanvasHeight();
        pFrameBuffer = (uint8_t *)malloc(w * (h+3)); // 3 extra lines for cooked pixels
        gif.setFrameBuf(pFrameBuffer);
        gif.setDrawType(GIF_DRAW_COOKED);
        u32Pixel = 0xffff;
        gif.playFrame(false, NULL);
        if (u32Pixel == 0x134be913) { // should be 1 1/3 pixels of (green)
            iTotalPass++;
            GIFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            iTotalFail++;
            GIFLOG(__LINE__, szTestName, " - FAILED");
        }
        free(pFrameBuffer);
    } else {
        GIFLOG(__LINE__, szTestName, "Error opening GIF file.");
    }
    // Test 8 - Verify animation delay
    szTestName = (char *)"GIF verify animation delay";
    iTotal++;
    GIFLOG(__LINE__, szTestName, szStart);
    if (gif.open((uint8_t *)earth_128x128, sizeof(earth_128x128), GIFDraw)) {
        gif.setDrawType(GIF_DRAW_RAW);
        gif.setFrameBuf(NULL);
        i = 0;
        gif.playFrame(false, &i);
        if (i == 100) { // should be 10 jiffies (100ms)
            iTotalPass++;
            GIFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            iTotalFail++;
            GIFLOG(__LINE__, szTestName, " - FAILED");
        }
    } else {
        GIFLOG(__LINE__, szTestName, "Error opening GIF file.");
    }

}

void loop()
{

}