/**
 * @file lv_draw_eve_target.h
 *
 */

#ifndef LV_DRAW_EVE_TARGET_H
#define LV_DRAW_EVE_TARGET_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE

#include LV_STDBOOL_INCLUDE
#include LV_STDINT_INCLUDE

/* lv_draw_eve_target_parameters.h should be in the include
 * paths of your build. I.e., `-I /path/to/dir/that/has/the/file/`
 *
 * It should define these defines:
 *     #define EVE_HSIZE          ...   active display width
 *     #define EVE_VSIZE          ...   active display height
 *     #define EVE_VSYNC0         ...   start of vertical sync pulse
 *     #define EVE_VSYNC1         ...   end of vertical sync pulse
 *     #define EVE_VOFFSET        ...   start of active screen
 *     #define EVE_VCYCLE         ...   total number of lines per screen, including pre/post
 *     #define EVE_HSYNC0         ...   start of horizontal sync pulse
 *     #define EVE_HSYNC1         ...   end of horizontal sync pulse
 *     #define EVE_HOFFSET        ...   start of active line
 *     #define EVE_HCYCLE         ...   total number of clocks per line, incl front/back porch
 *     #define EVE_PCLK           ...   60MHz / EVE_PCLK = PCLK frequency
 *     #define EVE_PCLKPOL        ...   LCD data is clocked in on this PCLK edge
 *     #define EVE_SWIZZLE        ...   FT8xx output to LCD - pin order
 *     #define EVE_CSPREAD        ...   helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1
 *     #define EVE_HAS_CRYSTAL    (`#define EVE_HAS_CRYSTAL` for yes, `#undef EVE_HAS_CRYSTAL` for no)
 *     #define EVE_HAS_GT911      (`#define EVE_HAS_GT911` for yes, `#undef EVE_HAS_GT911` for no)
 *     #define EVE_GEN            ...   2, 3, or 4
 *     #define EVE_BACKLIGHT_PWM  ...   (optional, 128 default) backlight PWM duty cycle. 0 = off, 128 = max
 *     #define EVE_BACKLIGHT_FREQ ...   (optional, 4000 default) backlight PWM frequency
 */
#include "lv_draw_eve_target_parameters.h" /* see comment above */

/*********************
 *      DEFINES
 *********************/

#ifndef EVE_BACKLIGHT_PWM
#define EVE_BACKLIGHT_PWM 0x80
#endif

#ifndef EVE_BACKLIGHT_FREQ
#define EVE_BACKLIGHT_FREQ (4000U)
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * The function that will be called to control the EVE IC's "PD_N" pin.
 * @param powerdown false: set the pin high (powerup)
 *                  true: set the pin low (powerdown)
 */
void lv_draw_eve_target_powerdown(bool powerdown);

/**
 * The function that will be called to control the EVE IC's SPI "CS_N" pin.
 * @param selected  false: set the pin high (deselected)
 *                  true: set the pin low (selected)
 */
void lv_draw_eve_target_spi_cs(bool selected);

/**
 * The function that will be called to transmit SPI data to the EVE IC.
 * @param src  Pointer to the buffer of bytes that should be transmitted.
 * @param len  The number of bytes to be transmitted.
 */
void lv_draw_eve_target_spi_transmit(uint8_t * src, uint32_t len);

/**
 * The function that will be called to receive SPI data from the EVE IC.
 * @param src  Pointer to the buffer that `len` bytes should be received into.
 * @param len  The number of bytes to receive.
 */
void lv_draw_eve_target_spi_receive(uint8_t * dst, uint32_t len);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_EVE_TARGET_H*/
