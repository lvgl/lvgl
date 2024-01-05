/**
 * @file lv_st7735.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_st7735.h"

/*********************
 *      DEFINES
 *********************/

#define CMD_GAMSET		0x26

#define CMD_FRMCTR1		0xB1
#define CMD_FRMCTR2		0xB2
#define CMD_FRMCTR3		0xB3
#define CMD_INVCTR		0xB4
#define CMD_DISSET5		0xB6

#define CMD_PWCTR1		0xC0
#define CMD_PWCTR2		0xC1
#define CMD_PWCTR3		0xC2
#define CMD_PWCTR4		0xC3
#define CMD_PWCTR5		0xC4
#define CMD_VMCTR1		0xC5
#define CMD_VMOFCTR		0xC7

#define CMD_NVFCTR1		0xD9

#define CMD_GMCTRP1		0xE0
#define CMD_GMCTRN1		0xE1

#define CMD_PWCTR6		0xFC

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

static const uint8_t init_cmd_list[] = {
#if 0
	// init commands based on LovyanGFX
	CMD_FRMCTR1, 3,				/* Frame rate control */
					0x00,		/* fastest refresh */
					0x06,		/* 6 lines front porch */
					0x03,		/* 3 lines back porch */
	LV_LCD_CMD_SPECIAL, 1,		/* 10 ms delay */
	CMD_DISSET5, 2,				/* Display settings #5 */
					0x15,		/* 1 clk cycle nonoverlap, 2 cycle gate rise, 3 cycle osc equalize */
					0x02,		/* Fix on VTL */
#if 0
	CMD_INVCTR, 1,				/* Display inversion control */
					0x0,		/* Line inversion */
#endif
	CMD_PWCTR1, 2,				/* 8: Power control */
					0x02,		/* GVDD = 4.7V */
					0x70,		/* 1.0uA */
	LV_LCD_CMD_SPECIAL, 1,		/* 10 ms delay */
	CMD_PWCTR2, 1,				/* Power control */
					0x05,		/* VGH = 14.7V, VGL = -7.35V */
	CMD_PWCTR3, 2,				/* Power control */
					0x01,		/* Opamp current small */
					0x02,		/* Boost frequency */
	CMD_VMCTR1, 2,				/* 11: Power control */
					0x3C,		/* VCOMH = 4V */
					0x38,		/* VCOML = -1.1V */
	LV_LCD_CMD_SPECIAL, 1,		/* 10 ms delay */
	CMD_PWCTR6, 2,				/* Power control */
					0x11, 0x15,
	CMD_GMCTRP1, 16,			/* Positive Gamma */
					0x09, 0x16, 0x09, 0x20,
					0x21, 0x1B, 0x13, 0x19,
					0x17, 0x15, 0x1E, 0x2B,
					0x04, 0x05, 0x02, 0x0E,
	CMD_GMCTRN1, 16,			/* Negative Gamma */
					0x0B, 0x14, 0x08, 0x1E,
					0x22, 0x1D, 0x18, 0x1E,
					0x1B, 0x1A, 0x24, 0x2B,
					0x06, 0x06, 0x02, 0x0F,
#else
	// init commands based on BuyDisplay.com
	CMD_FRMCTR1, 3,	0x00, 0b111111, 0b111111,
	CMD_FRMCTR2, 3,	0b1111, 0x01, 0x01,
	CMD_FRMCTR3, 6, 0x05, 0x3c, 0x3c, 0x05, 0x3c, 0x3c,
	CMD_INVCTR, 1, 0x03,
	CMD_PWCTR1, 3, 0b11111100, 0x08, 0b10, /* power control */
	CMD_PWCTR2, 1, 0xc0,
	CMD_PWCTR3, 2, 0x0d, 0x00,
	CMD_PWCTR4, 2, 0x8d, 0x2a,
	CMD_PWCTR5, 2, 0x8d, 0xee,			/* partial */

	/* display brightness and gamma */
	CMD_PWCTR6, 1, 0b11011000,			/* auto gate pump freq, max power save */
	CMD_NVFCTR1, 1, 0b01000000,			/* automatic adjust gate pumping clock for saving power consumption */
	CMD_VMCTR1, 1, 0b001111,			/* VCOM voltage setting */
	CMD_VMOFCTR, 1, 0b10000,			/* lightness of black color 0-0x1f */
	CMD_GAMSET, 1, 0x02,				/* gamma 1, 2, 4, 8 */

	CMD_GMCTRP1, 16, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2c,
                0x29, 0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10,
	CMD_GMCTRN1, 16, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2c,
                0x2e, 0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10,
#endif
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

lv_display_t *lv_st7735_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags, lv_st7735_send_cmd_cb_t send_cmd_cb, lv_st7735_send_cmd_cb_t send_color_cb)
{
	lv_display_t *disp = lv_lcd_generic_mipi_create(hor_res, ver_res, flags, send_cmd_cb, send_color_cb);
	lv_lcd_generic_mipi_driver_t *drv = get_driver(disp);
    lv_lcd_generic_mipi_send_cmd_list(drv, init_cmd_list);
    return disp;
}

void lv_st7735_set_gap(lv_display_t *disp, uint16_t x, uint16_t y)
{
	lv_lcd_generic_mipi_set_gap(disp, x, y);
}

void lv_st7735_set_invert(lv_display_t *disp, bool invert)
{
	lv_lcd_generic_mipi_set_invert(disp, invert);
}

void lv_st7735_set_gamma_curve(lv_display_t *disp, uint8_t gamma)
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


