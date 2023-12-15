/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/* Common LCD panel commands */
#define LCD_CMD_NOP          0x00 // This command is empty command
#define LCD_CMD_SWRESET      0x01 // Software reset registers (the built-in frame buffer is not affected)
#define LCD_CMD_RDDID        0x04 // Read 24-bit display ID
#define LCD_CMD_RDDST        0x09 // Read display status
#define LCD_CMD_RDDPM        0x0A // Read display power mode
#define LCD_CMD_RDD_MADCTL   0x0B // Read display MADCTL
#define LCD_CMD_RDD_COLMOD   0x0C // Read display pixel format
#define LCD_CMD_RDDIM        0x0D // Read display image mode
#define LCD_CMD_RDDSM        0x0E // Read display signal mode
#define LCD_CMD_RDDSR        0x0F // Read display self-diagnostic result
#define LCD_CMD_SLPIN        0x10 // Go into sleep mode (DC/DC, oscillator, scanning stopped, but memory keeps content)
#define LCD_CMD_SLPOUT       0x11 // Exit sleep mode
#define LCD_CMD_PTLON        0x12 // Turns on partial display mode
#define LCD_CMD_NORON        0x13 // Turns on normal display mode
#define LCD_CMD_INVOFF       0x20 // Recover from display inversion mode
#define LCD_CMD_INVON        0x21 // Go into display inversion mode
#define LCD_CMD_GAMSET       0x26 // Select Gamma curve for current display
#define LCD_CMD_DISPOFF      0x28 // Display off (disable frame buffer output)
#define LCD_CMD_DISPON       0x29 // Display on (enable frame buffer output)
#define LCD_CMD_CASET        0x2A // Set column address
#define LCD_CMD_RASET        0x2B // Set row address
#define LCD_CMD_RAMWR        0x2C // Write frame memory
#define LCD_CMD_RAMRD        0x2E // Read frame memory
#define LCD_CMD_PTLAR        0x30 // Define the partial area
#define LCD_CMD_VSCRDEF      0x33 // Vertical scrolling definition
#define LCD_CMD_TEOFF        0x34 // Turns off tearing effect
#define LCD_CMD_TEON         0x35 // Turns on tearing effect

#define LCD_CMD_MADCTL       0x36     // Memory data access control
#define LCD_CMD_MH_BIT       (1 << 2) // Display data latch order, 0: refresh left to right, 1: refresh right to left
#define LCD_CMD_BGR_BIT      (1 << 3) // RGB/BGR order, 0: RGB, 1: BGR
#define LCD_CMD_ML_BIT       (1 << 4) // Line address order, 0: refresh top to bottom, 1: refresh bottom to top
#define LCD_CMD_MV_BIT       (1 << 5) // Row/Column order, 0: normal mode, 1: reverse mode
#define LCD_CMD_MX_BIT       (1 << 6) // Column address order, 0: left to right, 1: right to left
#define LCD_CMD_MY_BIT       (1 << 7) // Row address order, 0: top to bottom, 1: bottom to top

#define LCD_CMD_VSCSAD       0x37 // Vertical scroll start address
#define LCD_CMD_IDMOFF       0x38 // Recover from IDLE mode
#define LCD_CMD_IDMON        0x39 // Fall into IDLE mode (8 color depth is displayed)
#define LCD_CMD_COLMOD       0x3A // Defines the format of RGB picture data
#define LCD_CMD_RAMWRC       0x3C // Memory write continue
#define LCD_CMD_RAMRDC       0x3E // Memory read continue
#define LCD_CMD_STE          0x44 // Set tear scan line, tearing effect output signal when display module reaches line N
#define LCD_CMD_GDCAN        0x45 // Get scan line
#define LCD_CMD_WRDISBV      0x51 // Write display brightness
#define LCD_CMD_RDDISBV      0x52 // Read display brightness value
