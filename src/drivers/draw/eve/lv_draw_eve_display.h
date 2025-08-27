/**
 * @file lv_draw_eve_display.h
 *
 */

#ifndef LV_DRAW_EVE_DISPLAY_H
#define LV_DRAW_EVE_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE

#include "../../../draw/eve/lv_draw_eve_target.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a display for the EVE draw unit.
 * @param params      Pointer to a struct of display parameters. Can be a temporary variable
 * @param op_cb       A callback that will be called to perform pin and SPI IO operations with the EVE chip
 * @param user_data   use `lv_draw_eve_display_get_user_data` to get this pointer inside the `op_cb`
 * @return            the EVE display
 */
lv_display_t * lv_draw_eve_display_create(const lv_draw_eve_parameters_t * params, lv_draw_eve_operation_cb_t op_cb,
                                          void * user_data);

/**
 * Get the `user_data` parameter that was passed to `lv_draw_eve_display_create`. Useful in the operation callback.
 * @param disp      pointer to the lv_draw_eve display
 * @return          the `user_data` pointer
 */
void * lv_draw_eve_display_get_user_data(lv_display_t * disp);

/**
 * Create a touchscreen indev for the EVE display.
 * @param disp        pointer to the lv_draw_eve display
 * @return            the EVE touchscreen indev
 */
lv_indev_t * lv_draw_eve_touch_create(lv_display_t * disp);


/* RAM_G asset pre-upload functions */

/**
 * Upload an image src to RAM_G now instead of as-needed during rendering.
 * @param disp    pointer to the lv_draw_eve display
 * @param src     image src. The value passed to `lv_image_set_src`
 */
void lv_draw_eve_pre_upload_image(lv_display_t * disp, const void * src);

/**
 * Upload font glyphs to RAM_G now instead of as-needed during rendering.
 * Upload all the glyphs in the range of unicode code points (inclusive of the start and end values).
 * It can be called multiple times with different ranges.
 * @param disp                  pointer to the lv_draw_eve display
 * @param font                  the font to upload glyphs from
 * @param unicode_range_start   the first unicode code point in the range of glyphs to upload
 * @param unicode_range_end     the last unicode code point (inclusive) in the range of glyphs to upload
 */
void lv_draw_eve_pre_upload_font_range(lv_display_t * disp, const lv_font_t * font, uint32_t unicode_range_start,
                                       uint32_t unicode_range_end);

/**
 * Upload font glyphs to RAM_G now instead of as-needed during rendering.
 * It will upload all the glyphs needed to render the string `text`.
 * It can be called multiple times with different strings.
 * @param disp    pointer to the lv_draw_eve display
 * @param font    the font to upload glyphs from
 * @param text    the ASCII or UTF-8 string that will be iterated for glyphs to upload
 */
void lv_draw_eve_pre_upload_font_text(lv_display_t * disp, const lv_font_t * font, const char * text);


/* Low-level EVE control functions */

/**
 * Call `EVE_memRead8` for custom low-level control of the display.
 * @param disp    the display returned by `lv_draw_eve_display_create`
 * @param address the EVE address to read from
 * @return        the read value
 */
uint8_t lv_draw_eve_memread8(lv_display_t * disp, uint32_t address);

/**
 * Call `EVE_memRead16` for custom low-level control of the display.
 * @param disp    the display returned by `lv_draw_eve_display_create`
 * @param address the EVE address to read from
 * @return        the read value
 */
uint16_t lv_draw_eve_memread16(lv_display_t * disp, uint32_t address);

/**
 * Call `EVE_memRead32` for custom low-level control of the display.
 * @param disp    the display returned by `lv_draw_eve_display_create`
 * @param address the EVE address to read from
 * @return        the read value
 */
uint32_t lv_draw_eve_memread32(lv_display_t * disp, uint32_t address);

/**
 * Call `EVE_memWrite8` for custom low-level control of the display.
 * @param disp    the display returned by `lv_draw_eve_display_create`
 * @param address the EVE address to write to
 * @param data    the value to write
 */
void lv_draw_eve_memwrite8(lv_display_t * disp, uint32_t address, uint8_t data);

/**
 * Call `EVE_memWrite16` for custom low-level control of the display.
 * @param disp    the display returned by `lv_draw_eve_display_create`
 * @param address the EVE address to write to
 * @param data    the value to write
 */
void lv_draw_eve_memwrite16(lv_display_t * disp, uint32_t address, uint16_t data);

/**
 * Call `EVE_memWrite32` for custom low-level control of the display.
 * @param disp    the display returned by `lv_draw_eve_display_create`
 * @param address the EVE address to write to
 * @param data    the value to write
 */
void lv_draw_eve_memwrite32(lv_display_t * disp, uint32_t address, uint32_t data);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_EVE_DISPLAY_H*/
