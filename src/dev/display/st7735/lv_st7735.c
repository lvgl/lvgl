/**
 * @file lv_st7735.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_st7735.h"

#if LV_USE_ST7735

/*********************
 *      DEFINES
 *********************/

#define CMD_GAMSET      0x26

#define CMD_FRMCTR1     0xB1
#define CMD_FRMCTR2     0xB2
#define CMD_FRMCTR3     0xB3
#define CMD_INVCTR      0xB4
#define CMD_DISSET5     0xB6

#define CMD_PWCTR1      0xC0
#define CMD_PWCTR2      0xC1
#define CMD_PWCTR3      0xC2
#define CMD_PWCTR4      0xC3
#define CMD_PWCTR5      0xC4
#define CMD_VMCTR1      0xC5
#define CMD_VMOFCTR     0xC7

#define CMD_NVFCTR1     0xD9

#define CMD_GMCTRP1     0xE0
#define CMD_GMCTRN1     0xE1

#define CMD_PWCTR6      0xFC

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC CONSTANTS
 **********************/

/* init commands for buydisplay.com ER-TFTM018-3 */
static const uint8_t init_cmd_list[] = {
    0xB1,       3,  0x05, 0x3C, 0x3C,
    0xB2,       3,  0x05, 0x3C, 0x3C,
    0xB3,       6,  0x05, 0x3C, 0x3C, 0x05, 0x3C, 0x3C,
    0xB4,       1,  0x03,
    0xC0,       3,  0x28, 0x08, 0x04,
    0xC1,       1,  0XC0,
    0xC2,       2,  0x0D, 0x00,
    0xC3,       2,  0x8D, 0x2A,
    0xC4,       2,  0x8D, 0xEE,
    0xC5,       1,  0x10,
    0xE0,       16, 0x04, 0x22, 0x07, 0x0A, 0x2E, 0x30, 0x25, 0x2A, 0x28, 0x26, 0x2E, 0x3A, 0x00, 0x01, 0x03, 0x13,
    0xE1,       16, 0x04, 0x16, 0x06, 0x0D, 0x2D, 0x26, 0x23, 0x27, 0x27, 0x25, 0x2D, 0x3B, 0x00, 0x01, 0x04, 0x13,
    LV_LCD_CMD_DELAY_MS, LV_LCD_CMD_EOF
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_st7735_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                lv_st7735_send_cmd_cb_t send_cmd_cb, lv_st7735_send_color_cb_t send_color_cb)
{
    lv_display_t * disp = lv_lcd_generic_mipi_create(hor_res, ver_res, flags, send_cmd_cb, send_color_cb);
    lv_lcd_generic_mipi_send_cmd_list(disp, init_cmd_list);
    return disp;
}

void lv_st7735_set_gap(lv_display_t * disp, uint16_t x, uint16_t y)
{
    lv_lcd_generic_mipi_set_gap(disp, x, y);
}

void lv_st7735_set_invert(lv_display_t * disp, bool invert)
{
    lv_lcd_generic_mipi_set_invert(disp, invert);
}

void lv_st7735_set_gamma_curve(lv_display_t * disp, uint8_t gamma)
{
    lv_lcd_generic_mipi_set_gamma_curve(disp, gamma);
}

void lv_st7735_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list)
{
    lv_lcd_generic_mipi_send_cmd_list(disp, cmd_list);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif  /*LV_USE_ST7735*/
