//
// Embedded CPU benchmark test
// tests a real-world job (GIF animation)
// on various CPU types and speeds
//

#include <AnimatedGIF.h>
#include "../test_images/badgers.h"
//
// Use this option if your MCU has enough RAM (additional 48K + the 8-bit uncompressed canvas size)
// It will usually result in a nearly 2X speedup compared to the traditional decoding method
// It accomplishes this by using the output frame as the diction (with some additional pointer tables)
//
// Be mindful on ESP32 targets - using malloc may give you a pointer into PSRAM depending on your setup.
// This can cause it to run much slower than the older decoding method.
//
#define TURBO_MODE

AnimatedGIF gif;
#ifdef TURBO_MODE
uint8_t pTurboBuffer[TURBO_BUFFER_SIZE + 256 + (160 * 120)];
#endif

// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw)
{
 // don't do anything here because we're measuring CPU work
 // not display update time
} /* GIFDraw() */

void setup() {
  Serial.begin(115200);
  while (!Serial);
  gif.begin(BIG_ENDIAN_PIXELS);
}

void loop() {
long lTime;
int iFrames = 0;

  Serial.println("GIF CPU speed benchmark");
  if (gif.open((uint8_t *)ucBadgers, sizeof(ucBadgers), GIFDraw))
  {
    Serial.println("Successfully opened GIF, starting test...");
#ifdef TURBO_MODE
    gif.setTurboBuf(pTurboBuffer);
#endif
    lTime = micros();
    while (gif.playFrame(false, NULL))
    {
      iFrames++;
    }
    gif.close();
    lTime = micros() - lTime;
#ifdef TURBO_MODE
    Serial.print("Turbo mode: Decoded ");
#else
    Serial.print("Normal mode: Decoded ");
#endif
    Serial.print(iFrames, DEC);
    Serial.print(" frames in ");
    Serial.print((int)lTime, DEC);
    Serial.println(" microseconds");
  }
  delay(10000);
}
