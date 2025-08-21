//
// EZ GIF Player
// Uses the (new) GIFPlayer Class to make it easier
// to play animations without having to re-invent the wheel each time
// Written by Larry Bank (bitbank@pobox.com)
// The GIFPlayer class requires PSRAM to work
//
#include <GIFPlayer.h>
#include <bb_spi_lcd.h>
#include <SPI.h>
#include <SD.h>
//#include "homer_music.h"
BB_SPI_LCD lcd;
GIFPlayer gifplayer;
SPIClass SD_SPI;

// GPIO of SD card on the Waveshare AMOLED touch 2.41
#define SD_CS 2
#define SD_SCK 4
#define SD_MOSI 5
#define SD_MISO 6
// GPIO of SD card on the JC4827W543
//#define SD_CS 10
//#define SD_SCK 12
//#define SD_MOSI 11
//#define SD_MISO 13

void setup()
{
  int w, h;
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting...");
  lcd.begin(/*DISPLAY_CYD_543);*/ DISPLAY_WS_AMOLED_241);
  lcd.fillScreen(TFT_BLACK);
  lcd.setFont(FONT_12x16);
  lcd.setTextColor(TFT_GREEN);
  lcd.setCursor((lcd.width() - 192)/2, 0);
  lcd.println("GIF from SD Demo");
  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  while (!SD.begin(SD_CS, SD_SPI,10000000)) {
     Serial.println("Unable to access SD card");
     delay(1000);
  }
  Serial.println("SD card success");
  // Choose the source of your GIF animation - SD card, LittleFS or compiled into your sketch (FLASH)
  if (gifplayer.openSD(&lcd, "/this_is_fine_240x240.gif") != GIF_SUCCESS) {
//  if (gifplayer.openLFS(&lcd, "/homer_test.gif") != GIF_SUCCESS) {
//  if (gifplayer.openData(&lcd, homer_music, sizeof(homer_music)) != GIF_SUCCESS) {
    lcd.setTextColor(TFT_RED);
    lcd.print("open() failed!");
    while (1) {
      vTaskDelay(1);
    }
  }
  Serial.println("gifplayer.open succeeded");
  gifplayer.getInfo(&w, &h);
  lcd.setCursor((lcd.width() - 200)/2, lcd.height()-16);
  lcd.printf("Opened: %d x %d\n", w, h);

} /* setup() */

void loop()
{  // Play the animation forever (each call plays 1 frame)
   gifplayer.play(GIF_CENTER, GIF_CENTER, true); // center on display and use the frame delay
} /* loop() */