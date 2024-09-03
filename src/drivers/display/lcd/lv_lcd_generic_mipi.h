/**
 * @file lv_lcd_generic_mipi.h
 *
 * Generic driver for controllers adhering to the MIPI DBI/DCS specification
 *
 * Works with:
 *
 * ST7735
 * ST7789
 * ST7796
 * ILI9341
 * ILI9488 (NOTE: in SPI mode ILI9488 only supports RGB666 mode, which is currently not supported)
 *
 * any probably many more
 *
 */

#ifndef LV_LCD_GENERIC_MIPI_H
#define LV_LCD_GENERIC_MIPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../display/lv_display.h"

#if LV_USE_GENERIC_MIPI

/*********************
 *      DEFINES
 *********************/

/* MIPI DCS (Display Command Set) v1.02.00 User Command Set */
#define LV_LCD_CMD_NOP                      0x00    /* No Operation */
#define LV_LCD_CMD_SOFT_RESET               0x01    /* Software Reset */
#define LV_LCD_CMD_GET_POWER_MODE           0x0A    /* Get the current power mode */
#define LV_LCD_CMD_GET_ADDRESS_MODE         0x0B    /* Get the data order for transfers from the Host to the display module and from the frame memory to the display device */
#define LV_LCD_CMD_GET_PIXEL_FORMAT         0x0C    /* Get the current pixel format */
#define LV_LCD_CMD_GET_DISPLAY_MODE         0x0D    /* Get the current display mode from the peripheral */
#define LV_LCD_CMD_GET_SIGNAL_MODE          0x0E    /* Get display module signaling mode */
#define LV_LCD_CMD_GET_DIAGNOSTIC_RESULT    0x0F    /* Get Peripheral Self-Diagnostic Result */
#define LV_LCD_CMD_ENTER_SLEEP_MODE         0x10    /* Power for the display panel is off */
#define LV_LCD_CMD_EXIT_SLEEP_MODE          0x11    /* Power for the display panel is on */
#define LV_LCD_CMD_ENTER_PARTIAL_MODE       0x12    /* Part of the display area is used for image display */
#define LV_LCD_CMD_ENTER_NORMAL_MODE        0x13    /* The whole display area is used for image display */
#define LV_LCD_CMD_EXIT_INVERT_MODE         0x20    /* Displayed image colors are not inverted */
#define LV_LCD_CMD_ENTER_INVERT_MODE        0x21    /* Displayed image colors are inverted */
#define LV_LCD_CMD_SET_GAMMA_CURVE          0x26    /* Selects the gamma curve used by the display device */
#define LV_LCD_CMD_SET_DISPLAY_OFF          0x28    /* Blanks the display device */
#define LV_LCD_CMD_SET_DISPLAY_ON           0x29    /* Show the image on the display device */
#define LV_LCD_CMD_SET_COLUMN_ADDRESS       0x2A    /* Set the column extent */
#define LV_LCD_CMD_SET_PAGE_ADDRESS         0x2B    /* Set the page extent */
#define LV_LCD_CMD_WRITE_MEMORY_START       0x2C    /* Transfer image data from the Host Processor to the peripheral starting at the location provided by set_column_address and set_page_address */
#define LV_LCD_CMD_READ_MEMORY_START        0x2E    /* Transfer image data from the peripheral to the Host Processor interface starting at the location provided by set_column_address and set_page_address */
#define LV_LCD_CMD_SET_PARTIAL_ROWS         0x30    /* Defines the number of rows in the partial display area on the display device */
#define LV_LCD_CMD_SET_PARTIAL_COLUMNS      0x31    /* Defines the number of columns in the partial display area on the display device */
#define LV_LCD_CMD_SET_SCROLL_AREA          0x33    /* Defines the vertical scrolling and fixed area on display device */
#define LV_LCD_CMD_SET_TEAR_OFF             0x34    /* Synchronization information is not sent from the display module to the host processor */
#define LV_LCD_CMD_SET_TEAR_ON              0x35    /* Synchronization information is sent from the display module to the host processor at the start of VFP */
#define LV_LCD_CMD_SET_ADDRESS_MODE         0x36    /* Set the data order for transfers from the Host to the display module and from the frame memory to the display device */
#define LV_LCD_CMD_SET_SCROLL_START         0x37    /* Defines the vertical scrolling starting point */
#define LV_LCD_CMD_EXIT_IDLE_MODE           0x38    /* Full color depth is used on the display panel */
#define LV_LCD_CMD_ENTER_IDLE_MODE          0x39    /* Reduced color depth is used on the display panel */
#define LV_LCD_CMD_SET_PIXEL_FORMAT         0x3A    /* Defines how many bits per pixel are used in the interface */
#define LV_LCD_CMD_WRITE_MEMORY_CONTINUE    0x3C    /* Transfer image information from the Host Processor interface to the peripheral from the last written location */
#define LV_LCD_CMD_READ_MEMORY_CONTINUE     0x3E    /* Read image data from the peripheral continuing after the last read_memory_continue or read_memory_start */
#define LV_LCD_CMD_SET_TEAR_SCANLINE        0x44    /* Synchronization information is sent from the display module to the host processor when the display device refresh reaches the provided scanline */
#define LV_LCD_CMD_GET_SCANLINE             0x45    /* Get the current scanline */
#define LV_LCD_CMD_READ_DDB_CONTINUE        0xA8    /* Continue reading the DDB from the last read location */
#define LV_LCD_CMD_READ_DDB_START           0xA1    /* Read the DDB from the provided location */

/* address mode flag masks */
#define LV_LCD_MASK_FLIP_VERTICAL                       (1 << 0)    /* This bit flips the image shown on the display device top to bottom. No change is made to the frame memory */
#define LV_LCD_MASK_FLIP_HORIZONTAL                     (1 << 1)    /* This bit flips the image shown on the display device left to right. No change is made to the frame memory */
#define LV_LCD_MASK_DATA_LATCH_DATA_ORDER               (1 << 2)    /* Display Data Latch Order */
#define LV_LCD_MASK_RGB_ORDER                           (1 << 3)    /* RGB/BGR Order */
#define LV_LCD_MASK_LINE_ADDRESS_ORDER                  (1 << 4)    /* Line Address Order */
#define LV_LCD_MASK_PAGE_COLUMN_ORDER                   (1 << 5)    /* Page/Column Order */
#define LV_LCD_MASK_COLUMN_ADDRESS_ORDER                (1 << 6)    /* Column Address Order */
#define LV_LCD_MASK_PAGE_ADDRESS_ORDER                  (1 << 7)    /* Page Address Order */

#define LV_LCD_BIT_FLIP_VERTICAL__NOT_FLIPPED           0
#define LV_LCD_BIT_FLIP_VERTICAL__FLIPPED               LV_LCD_MASK_FLIP_VERTICAL           /* This bit flips the image shown on the display device top to bottom. No change is made to the frame memory */
#define LV_LCD_BIT_FLIP_HORIZONTAL__NOT_FLIPPED         0
#define LV_LCD_BIT_FLIP_HORIZONTAL__FLIPPED             LV_LCD_MASK_FLIP_HORIZONTAL         /* This bit flips the image shown on the display device left to right. No change is made to the frame memory */
#define LV_LCD_BIT_DATA_LATCH_DATA_ORDER__LTOR          0                                   /* Display Data Latch Order: LCD Refresh Left to Right */
#define LV_LCD_BIT_DATA_LATCH_DATA_ORDER__RTOL          LV_LCD_MASK_DATA_LATCH_DATA_ORDER   /* Display Data Latch Order: LCD Refresh Right to Left */
#define LV_LCD_BIT_RGB_ORDER__RGB                       0                                   /* RGB/BGR Order: RGB */
#define LV_LCD_BIT_RGB_ORDER__BGR                       LV_LCD_MASK_RGB_ORDER               /* RGB/BGR Order: BGR */
#define LV_LCD_BIT_LINE_ADDRESS_ORDER__TTOB             0                                   /* Line Address Order: LCD Refresh Top to Bottom */
#define LV_LCD_BIT_LINE_ADDRESS_ORDER__BTOT             LV_LCD_MASK_LINE_ADDRESS_ORDER      /* Line Address Order: LCD Refresh Bottom to Top */
#define LV_LCD_BIT_PAGE_COLUMN_ORDER__NORMAL            0                                   /* Page/Column Order: Normal Mode */
#define LV_LCD_BIT_PAGE_COLUMN_ORDER__REVERSE           LV_LCD_MASK_PAGE_COLUMN_ORDER       /* Page/Column Order: Reverse Mode */
#define LV_LCD_BIT_COLUMN_ADDRESS_ORDER__LTOR           0                                   /* Column Address Order: Left to Right */
#define LV_LCD_BIT_COLUMN_ADDRESS_ORDER__RTOL           LV_LCD_MASK_COLUMN_ADDRESS_ORDER    /* Column Address Order: Right to Left */
#define LV_LCD_BIT_PAGE_ADDRESS_ORDER__TTOB             0                                   /* Page Address Order: Top to Bottom */
#define LV_LCD_BIT_PAGE_ADDRESS_ORDER__BTOT             LV_LCD_MASK_PAGE_ADDRESS_ORDER      /* Page Address Order: Bottom to Top */

/* predefined gamma curves */
#define LV_LCD_GAMMA_2_2                                0x01    /* 2.2 */
#define LV_LCD_GAMMA_1_8                                0x02    /* 1.8 */
#define LV_LCD_GAMMA_2_5                                0x04    /* 2.5 */
#define LV_LCD_GAMMA_1_0                                0x08    /* 1.0 */

/* common pixel formats */
#define LV_LCD_PIXEL_FORMAT_RGB565                      0x55    /* bus: 16 bits, pixel: 16 bits */
#define LV_LCD_PIXEL_FORMAT_RGB666                      0x66    /* bus: 18 bits, pixel: 18 bits */

/* flags for lv_lcd_xxx_create() */
#define LV_LCD_FLAG_NONE                                0x00000000UL
#define LV_LCD_FLAG_MIRROR_X                            0x00000001UL
#define LV_LCD_FLAG_MIRROR_Y                            0x00000002UL
#define LV_LCD_FLAG_BGR                                 0x00000008UL
#define LV_LCD_FLAG_RGB666                              0x00000010UL

/* command list */
#define LV_LCD_CMD_DELAY_MS     0xff
#define LV_LCD_CMD_EOF          0xff

/**********************
 *      TYPEDEFS
 **********************/

/**
 *  Configuration flags for lv_lcd_xxx_create()
 *
 */
typedef uint32_t lv_lcd_flag_t;

/**
 * Prototype of a platform-dependent callback to transfer commands and data to the LCD controller.
 * @param disp          display object
 * @param cmd           command buffer (can handle 16 bit commands as well)
 * @param cmd_size      number of bytes of the command
 * @param param         parameter buffer
 * @param param_size    number of bytes of the parameters
 */
typedef void (*lv_lcd_send_cmd_cb_t)(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, const uint8_t * param,
                                     size_t param_size);

/**
 * Prototype of a platform-dependent callback to transfer pixel data to the LCD controller.
 * @param disp          display object
 * @param cmd           command buffer (can handle 16 bit commands as well)
 * @param cmd_size      number of bytes of the command
 * @param param         parameter buffer
 * @param param_size    number of bytes of the parameters
 */
typedef void (*lv_lcd_send_color_cb_t)(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, uint8_t * param,
                                       size_t param_size);

/**
 * Generic MIPI compatible LCD driver
 */
typedef struct {
    lv_display_t      *     disp;          /* the associated LVGL display object */
    lv_lcd_send_cmd_cb_t    send_cmd;       /* platform-specific implementation to send a command to the LCD controller */
    lv_lcd_send_color_cb_t  send_color;     /* platform-specific implementation to send pixel data to the LCD controller */
    uint16_t                x_gap;          /* x offset of the (0,0) pixel in VRAM */
    uint16_t                y_gap;          /* y offset of the (0,0) pixel in VRAM */
    uint8_t                 madctl_reg;     /* current value of MADCTL register */
    uint8_t                 colmod_reg;     /* current value of COLMOD register */
    bool                    mirror_x;
    bool                    mirror_y;
    bool                    swap_xy;
} lv_lcd_generic_mipi_driver_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Create a MIPI DCS compatible LCD display
 * @param hor_res       horizontal resolution
 * @param ver_res       vertical resolution
 * @param flags         default configuration settings (mirror, RGB ordering, etc.)
 * @param send_cmd      platform-dependent function to send a command to the LCD controller (usually uses polling transfer)
 * @param send_color    platform-dependent function to send pixel data to the LCD controller (usually uses DMA transfer: must implement a 'ready' callback)
 * @return              pointer to the created display
 */
lv_display_t * lv_lcd_generic_mipi_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                          lv_lcd_send_cmd_cb_t send_cmd_cb, lv_lcd_send_color_cb_t send_color_cb);

/**
 * Set gap, i.e., the offset of the (0,0) pixel in the VRAM
 * @param disp          display object
 * @param x             x offset
 * @param y             y offset
 */
void lv_lcd_generic_mipi_set_gap(lv_display_t * disp, uint16_t x, uint16_t y);

/**
 * Set color inversion
 * @param disp          display object
 * @param invert        false: normal, true: invert
 */
void lv_lcd_generic_mipi_set_invert(lv_display_t * disp, bool invert);

/**
 * Set address mode
 * @param disp          display object
 * @param mirror_x      horizontal mirror (false: normal, true: mirrored)
 * @param mirror_y      vertical mirror (false: normal, true: mirrored)
 * @param swap_xy       swap axes (false: normal, true: swap)
 * @param bgr           RGB/BGR order (false: RGB, true: BGR)
 */
void lv_lcd_generic_mipi_set_address_mode(lv_display_t * disp, bool mirror_x, bool mirror_y, bool swap_xy, bool bgr);

/**
 * Set gamma curve
 * @param disp          display object
 * @param gamma         gamma curve
 */
void lv_lcd_generic_mipi_set_gamma_curve(lv_display_t * disp, uint8_t gamma);

/**
 * Send list of commands.
 * @param disp          display object
 * @param cmd_list      controller and panel-specific commands
 */
void lv_lcd_generic_mipi_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list);

/**********************
 *      OTHERS
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_GENERIC_MIPI*/

#endif /* LV_LCD_GENERIC_MIPI_H */
