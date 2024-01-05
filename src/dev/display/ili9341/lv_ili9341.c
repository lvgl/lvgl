/**
 * @file lv_ili9341.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_ili9341.h"

/*********************
 *      DEFINES
 *********************/

#define CMD_FRMCTR1 	0xB1
#define CMD_FRMCTR2		0xB2
#define CMD_FRMCTR3		0xB3
#define CMD_INVCTR		0xB4
#define CMD_DFUNCTR		0xB6
#define CMD_PWCTR1		0xC0
#define CMD_PWCTR2		0xC1
#define CMD_PWCTR3		0xC2
#define CMD_PWCTR4		0xC3
#define CMD_PWCTR5		0xC4
#define CMD_VMCTR1		0xC5
#define CMD_VMCTR2		0xC7
#define CMD_GMCTRP1		0xE0	/* Positive Gamma Correction (E0h) */
#define CMD_GMCTRN1		0xE1	/* Negative Gamma Correction (E1h) */
#define CMD_RDINDEX		0xD9	/* ili9341 */
#define CMD_IDXRD		0xDD	/* ILI9341 only, indexed control register read */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void send_cmd(lv_lcd_generic_mipi_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size);
static lv_lcd_generic_mipi_driver_t *get_driver(lv_display_t *disp);

/**********************
 *  STATIC CONSTANTS
 **********************/

// init commands based on LovyanGFX
static const uint8_t init_cmd_list[] = {
	0xEF, 3, 0x03, 0x80, 0x02,
	0xCF, 3, 0x00, 0xC1, 0x30,
	0xED, 4, 0x64, 0x03, 0x12, 0x81,
	0xE8, 3, 0x85, 0x00, 0x78,
	0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
	0xF7, 1, 0x20,
	0xEA, 2, 0x00, 0x00,
	CMD_PWCTR1, 1, 0x23,
	CMD_PWCTR2, 1, 0x10,
	CMD_VMCTR1, 2, 0x3e, 0x28,
	CMD_VMCTR2, 1, 0x86,
	CMD_FRMCTR1, 2, 0x00, 0x13,
	0xF2, 1, 0x00,
	LV_LCD_CMD_SET_GAMMA_CURVE, 1, 0x01,
	CMD_GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
	CMD_GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
	CMD_DFUNCTR, 3, 0x08, 0xC2, 0x27,
	LV_LCD_CMD_SPECIAL, LV_LCD_CMD_EOF
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

lv_display_t *lv_ili9341_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags, lv_ili9341_send_cmd_cb_t send_cmd_cb, lv_ili9341_send_cmd_cb_t send_color_cb)
{
	lv_display_t *disp = lv_lcd_generic_mipi_create(hor_res, ver_res, flags, send_cmd_cb, send_color_cb);
	lv_lcd_generic_mipi_driver_t *drv = get_driver(disp);
    lv_lcd_generic_mipi_send_cmd_list(drv, init_cmd_list);
    return disp;
}

void lv_ili9341_set_gap(lv_display_t *disp, uint16_t x, uint16_t y)
{
	lv_lcd_generic_mipi_set_gap(disp, x, y);
}

void lv_ili9341_set_invert(lv_display_t *disp, bool invert)
{
	lv_lcd_generic_mipi_set_invert(disp, invert);
}

void lv_ili9341_set_gamma_curve(lv_display_t *disp, uint8_t gamma)
{
	lv_lcd_generic_mipi_set_gamma_curve(disp, gamma);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void send_cmd(lv_lcd_generic_mipi_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size)
{
    uint8_t cmdbuf = cmd;		/* MIPI uses 8 bit commands */
    drv->send_cmd(drv->disp, &cmdbuf, 1, param, param_size);
}

static lv_lcd_generic_mipi_driver_t *get_driver(lv_display_t *disp)
{
	return (lv_lcd_generic_mipi_driver_t *)lv_display_get_driver_data(disp);
}

