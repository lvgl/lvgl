/**
 * @file lv_ili9341.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_ili9341.h"

#if LV_USE_ILI9341

/*********************
 *      DEFINES
 *********************/

#define CMD_FRMCTR1     0xB1    /* Frame Rate Control (In Normal Mode/Full Colors) */
#define CMD_FRMCTR2     0xB2    /* Frame Rate Control (In Idle Mode/8 colors) */
#define CMD_FRMCTR3     0xB3    /* Frame Rate control (In Partial Mode/Full Colors) */
#define CMD_INVCTR      0xB4    /* Display Inversion Control */
#define CMD_DFUNCTR     0xB6    /* Display Function Control */
#define CMD_PWCTR1      0xC0    /* Power Control 1 */
#define CMD_PWCTR2      0xC1    /* Power Control 2 */
#define CMD_VMCTR1      0xC5    /* VCOM Control 1 */
#define CMD_VMCTR2      0xC7    /* VCOM Control 2 */
#define CMD_PWCTRA      0xCB    /* Power Control A */
#define CMD_PWCTRB      0xCF    /* Power Control B */
#define CMD_GMCTRP1     0xE0    /* Positive Gamma Correction */
#define CMD_GMCTRN1     0xE1    /* Negative Gamma Correction */
#define CMD_DTCTRA      0xE8    /* Driver timing control A */
#define CMD_DTCTRB      0xEA    /* Driver timing control B */
#define CMD_PONSEQ      0xED    /* Power On Sequence */
#define CMD_RDINDEX     0xD9    /* ili9341 */
#define CMD_IDXRD       0xDD    /* ILI9341 only, indexed control register read */
#define CMD_ENA3G       0xF2    /* Enable 3 Gamma control */
#define CMD_IFCTR       0xF6    /* Interface Control */
#define CMD_PRCTR       0xF7    /* Pump ratio control */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC CONSTANTS
 **********************/

/* init commands based on LovyanGFX ILI9341 driver */
static const uint8_t init_cmd_list[] = {
    CMD_PWCTRB,     3,  0x00, 0xC1, 0x30,
    CMD_PONSEQ,     4,  0x64, 0x03, 0x12, 0x81,
    CMD_DTCTRA,     3,  0x85, 0x00, 0x78,
    CMD_PWCTRA,     5,  0x39, 0x2C, 0x00, 0x34, 0x02,
    CMD_PRCTR,      1,  0x20,
    CMD_DTCTRB,     2,  0x00, 0x00,
    CMD_PWCTR1,     1,  0x23,
    CMD_PWCTR2,     1,  0x10,
    CMD_VMCTR1,     2,  0x3e, 0x28,
    CMD_VMCTR2,     1,  0x86,
    CMD_FRMCTR1,    2,  0x00, 0x13,
    CMD_DFUNCTR,    2,  0x0A, 0xA2,
    CMD_IFCTR,      3,  0x09, 0x30, 0x00,
    CMD_ENA3G,      1,  0x00,
    LV_LCD_CMD_SET_GAMMA_CURVE, 1, 0x01,
    CMD_GMCTRP1,    15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    CMD_GMCTRN1,    15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
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

lv_display_t * lv_ili9341_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                 lv_ili9341_send_cmd_cb_t send_cmd_cb, lv_ili9341_send_color_cb_t send_color_cb)
{
    lv_display_t * disp = lv_lcd_generic_mipi_create(hor_res, ver_res, flags, send_cmd_cb, send_color_cb);
    lv_lcd_generic_mipi_send_cmd_list(disp, init_cmd_list);
    return disp;
}

void lv_ili9341_set_gap(lv_display_t * disp, uint16_t x, uint16_t y)
{
    lv_lcd_generic_mipi_set_gap(disp, x, y);
}

void lv_ili9341_set_invert(lv_display_t * disp, bool invert)
{
    lv_lcd_generic_mipi_set_invert(disp, invert);
}

void lv_ili9341_set_gamma_curve(lv_display_t * disp, uint8_t gamma)
{
    lv_lcd_generic_mipi_set_gamma_curve(disp, gamma);
}

void lv_ili9341_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list)
{
    lv_lcd_generic_mipi_send_cmd_list(disp, cmd_list);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_ILI9341*/
