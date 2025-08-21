//
// GIF 1-bit per pixel OLED demo
// This example shows how to use the new "COOKED" pixel feature of AnimatedGIF
// to generate ready-made output for various types of displays. In this case,
// the output is tailored for the common 128x64 SSD1306 OLED display.
//
// Copyright (c) 2024 BitBank Software, Inc.
// written by Larry Bank (bitbank@pobox.com)
// Project started March 7, 2024
//
#include <OneBitDisplay.h>
#include <AnimatedGIF.h>
#include "../test_images/bw_wiggler_128x64.h"

ONE_BIT_DISPLAY obd;
AnimatedGIF gif;
// This frame buffer needs to be large enough for the canvas size as 1 byte per pixel
// plus the display framebuffer as 1 bit per pixel
uint8_t ucFrameBuffer[(128*64) + ((128 * 64)/8)]; // holds current canvas as 8-bpp and 1-bit output after that

void setup() {
  Serial.begin(115200);
  obd.I2Cbegin(OLED_128x64); // use the default Wire instance
  obd.setBuffer(&ucFrameBuffer[128*64]); // point to 1-bit display data after the 8-bpp canvas
  obd.fillScreen(OBD_WHITE); // clears the display to back on OLEDs (inverted)
  obd.setFont(FONT_12x16);
  obd.println("OLED GIF");
  obd.println(" Player");
  // When a framebuffer is given to OneBitDisplay, drawing will occur in RAM until
  // explicitly calling the display() method to transmit the RAM buffer to the physical display
  obd.display();
  delay(2000);
} /* setup() */

void loop() {
  int iFrame;
  char szTemp[64];

  gif.begin(GIF_PALETTE_1BPP_OLED); // Choose the "OLED" version of 1-bpp output (vertical bytes, LSB on top)
  // For 1-bit output, we can pass NULL for the GIFDraw method pointer; it won't be needed
  if (gif.open((uint8_t *)bw_wiggler_128x64, sizeof(bw_wiggler_128x64), NULL))
  {
    gif.setFrameBuf(ucFrameBuffer);
    // We want the library to generate ready-made (COOKED) pixels
    // For 1-bit output, the whole frame (8-bit + 1-bit) is written into the buffer we set above
    gif.setDrawType(GIF_DRAW_COOKED);
    sprintf(szTemp, "Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    Serial.print(szTemp);
    while (gif.playFrame(false, NULL)) { // live dangerously; run unthrottled :)
      obd.display(); // Write the current frame to the OLED display
    }
    gif.close();
  }
} /* loop() */
