/**
 * @file lv_st7789.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_st7789.h"

#if LV_USE_ST7789

/*********************
 *      DEFINES
 *********************/

#define CMD_RAMCTRL     0xB0
#define CMD_PORCTRL     0xB2    /* Porch control */
#define CMD_GCTRL       0xB7    /* Gate control */
#define CMD_VCOMS       0xBB    /* VCOMS setting */
#define CMD_LCMCTRL     0xC0    /* LCM control */
#define CMD_VDVVRHEN    0xC2    /* VDV and VRH command enable */
#define CMD_VRHS        0xC3    /* VRH set */
#define CMD_VDVSET      0xC4    /* VDV setting */
#define CMD_FRCTR2      0xC6    /* FR Control 2 */
#define CMD_PWCTRL1     0xD0    /* Power control 1 */
#define CMD_PVGAMCTRL   0xE0    /* Positive Gamma Correction */
#define CMD_NVGAMCTRL   0xE1    /* Negative Gamma Correction */
#define CMD_GATESEL     0xD6    /* Gate Output Selection in Sleep In Mode */

#ifndef LV_ST_7789_GAMCTRL
    #define LV_ST_7789_GAMCTRL -1
#endif

#define MAKE_GAMMA( \
    p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, \
    n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14  \
)   \
    CMD_PVGAMCTRL, 14, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, \
    CMD_NVGAMCTRL, 14, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14


#if   LV_ST_7789_GAMCTRL == 0   // Default from this library
    #define GAMMA MAKE_GAMMA( \
        0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0E, 0x12, 0x14, 0x17, \
        0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31, 0x54, 0x47, 0x0E, 0x1C, 0x17, 0x1B, 0x1E)
#elif LV_ST_7789_GAMCTRL == 1   // Waveshare 1.54" (IPS)
    #define GAMMA MAKE_GAMMA( \
        0xF0, 0x09, 0x13, 0x12, 0x0B, 0x11, 0x07, 0x31, 0x33, 0x42, 0x05, 0x0C, 0x0A, 0x28, \
        0xF0, 0x09, 0x13, 0x0C, 0x0D, 0x27, 0x3F, 0x42, 0x39, 0x12, 0x0E, 0x14, 0x31, 0x30)
#elif LV_ST_7789_GAMCTRL == 2   // Waveshare ESP32-S3-Touch-LCD-2.8
    #define GAMMA MAKE_GAMMA( \
        0xD0, 0x0D, 0x14, 0x0D, 0x0D, 0x09, 0x38, 0x44, 0x4E, 0x3A, 0x17, 0x18, 0x2F, 0x30, \
        0xD0, 0x09, 0x0F, 0x08, 0x07, 0x14, 0x37, 0x44, 0x4D, 0x38, 0x15, 0x16, 0x2C, 0x2E)
#elif LV_ST_7789_GAMCTRL == 3   // JD-T9350 (IPS)
    #define GAMMA MAKE_GAMMA( \
        0xD0, 0x08, 0x11, 0x0B, 0x0A, 0x1F, 0x43, 0x53, 0x53, 0x31, 0x13, 0x07, 0x11, 0x10, \
        0xD0, 0x08, 0x10, 0x0B, 0x0A, 0x06, 0x3B, 0x44, 0x44, 0x1F, 0x15, 0x0F, 0x31, 0x35)
#elif LV_ST_7789_GAMCTRL == 4   // Nintendo Switch Lite (IPS)
    #define GAMMA MAKE_GAMMA( \
        0xE0, 0x1F, 0x29, 0x26, 0x0C, 0x0E, 0x09, 0x4B, 0xCD, 0x3C, 0x09, 0x13, 0x05, 0x22, \
        0xE1, 0x20, 0x2A, 0x27, 0x0D, 0x0F, 0x0A, 0x4C, 0xCE, 0x3D, 0x0A, 0x14, 0x06, 0x23)
#elif LV_ST_7789_GAMCTRL == 5   // Acuamarine
    #define GAMMA MAKE_GAMMA( \
        0xE0, 0xD0, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x54, 0x46, 0x18, 0x12, 0x0A, 0x17, 0x1D, \
        0xE1, 0xD0, 0x06, 0x0D, 0x14, 0x0C, 0x35, 0x53, 0x45, 0x17, 0x13, 0x0B, 0x18, 0x1E)
#elif LV_ST_7789_GAMCTRL == 6   // High contrast
    #define GAMMA MAKE_GAMMA( \
        0xE0, 0xF0, 0x09, 0x13, 0x12, 0x18, 0x2F, 0x3F, 0x3F, 0x1F, 0x1F, 0x1F, 0x2F, 0xFF, \
        0xE1, 0xF0, 0x09, 0x00, 0x00, 0x18, 0x2F, 0x3F, 0x3F, 0x1F, 0x1F, 0x1F, 0x3F, 0x3F)
#elif LV_ST_7789_GAMCTRL == 7   // Cinema mode, DCI-P3 color space
    #define GAMMA MAKE_GAMMA( \
        0xE0, 0x70, 0x11, 0x1A, 0x0D, 0x1B, 0x06, 0x4C, 0x97, 0x39, 0x0A, 0x13, 0x03, 0x43, \
        0xE1, 0x70, 0x10, 0x19, 0x0C, 0x1C, 0x06, 0x4D, 0x97, 0x39, 0x0A, 0x13, 0x05, 0x44)
#elif LV_ST_7789_GAMCTRL == 8   // HDR
    #define GAMMA MAKE_GAMMA( \
        0xE0, 0xFF, 0x1A, 0x00, 0x0A, 0x02, 0x3C, 0xBB, 0x36, 0x0A, 0x0F, 0x01, 0x35, 0x33, \
        0xE1, 0xFF, 0x21, 0x00, 0x0B, 0x02, 0x3A, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x20, 0x24)
#elif LV_ST_7789_GAMCTRL == 9   // Anbernic RG353V
    #define GAMMA MAKE_GAMMA( \
        0xE0, 0xF0, 0x09, 0x0B, 0x06, 0x04, 0x15, 0x2F, 0x54, 0x44, 0x3C, 0x17, 0x14, 0x18, \
        0xE1, 0xF0, 0x09, 0x0B, 0x06, 0x06, 0x01, 0x2F, 0x54, 0x43, 0x3C, 0x17, 0x14, 0x18)
#elif LV_ST_7789_GAMCTRL == 10  // Miyoo Mini+
    #define GAMMA MAKE_GAMMA( \
        0xE0, 0x20, 0x1A, 0x0A, 0x0B, 0x06, 0x4A, 0xB8, 0x37, 0x0A, 0x0F, 0x06, 0x32, 0x33, \
        0xE1, 0x20, 0x1C, 0x09, 0x0B, 0x2B, 0x3B, 0x55, 0x4D, 0x0F, 0x0E, 0x12, 0x16, 0x1D)
#else
    #define GAMMA 0x00, 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC CONSTANTS
 **********************/

/* init commands based on LovyanGFX ST7789 driver */
static const uint8_t init_cmd_list[] = {
    CMD_GCTRL,      1,  0x53,       /* GCTRL -- panel dependent */
    CMD_VCOMS,      1,  0x1A,       /* VCOMS -- panel dependent */
    CMD_VRHS,       1,  0x13,       /* VRHS - panel dependent */
    CMD_PWCTRL1,    2,  0xA4, 0xA1,
    CMD_RAMCTRL,    2,  0x00, 0xEC, /* controls mapping of RGB565 to RGB666 */
    CMD_GATESEL,    1,  0xA1,       /* D4 -> “0”: Gate output is GND in sleep in mode */
    GAMMA,
    LV_LCD_CMD_SET_GAMMA_CURVE, 1, 0x01,
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

lv_display_t * lv_st7789_create_ex(
    uint32_t hor_res, 
    uint32_t ver_res, 
    lv_lcd_flag_t flags,
    lv_st7789_send_cmd_cb_t send_cmd_cb,
    lv_st7789_send_color_cb_t send_color_cb,
    void *user_data)
{
    lv_display_t * disp = lv_lcd_generic_mipi_create_ex(hor_res, ver_res, flags, send_cmd_cb, send_color_cb, user_data);
    lv_lcd_generic_mipi_send_cmd_list(disp, init_cmd_list);
    return disp;
}

lv_display_t * lv_st7789_create(
    uint32_t hor_res, 
    uint32_t ver_res, 
    lv_lcd_flag_t flags,
    lv_st7789_send_cmd_cb_t send_cmd_cb,
    lv_st7789_send_color_cb_t send_color_cb)
{
    return lv_st7789_create_ex(hor_res, ver_res, flags, send_cmd_cb, send_color_cb, NULL);
}

void lv_st7789_set_gap(lv_display_t * disp, uint16_t x, uint16_t y)
{
    lv_lcd_generic_mipi_set_gap(disp, x, y);
}

void lv_st7789_set_invert(lv_display_t * disp, bool invert)
{
    lv_lcd_generic_mipi_set_invert(disp, invert);
}

void lv_st7789_set_gamma_curve(lv_display_t * disp, uint8_t gamma)
{
    lv_lcd_generic_mipi_set_gamma_curve(disp, gamma);
}

void lv_st7789_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list)
{
    lv_lcd_generic_mipi_send_cmd_list(disp, cmd_list);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_ST7789*/
