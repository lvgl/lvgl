/**
 * @file lv_qrcode.h
 *
 */

#ifndef LV_QRCODE_H
#define LV_QRCODE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../config/lv_conf_internal.h"
#if LV_USE_QRCODE

#include "../draw/lv_color.h"
#include "../lv_types.h"
#include "lv_canvas.h"
#include LV_STDBOOL_INCLUDE
#include LV_STDINT_INCLUDE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * When a change of the size or the quiet zone re-encodes the QR code.
 * The payload setters always encode immediately. A color change only writes the palette.
 */
typedef enum {
    LV_QRCODE_UPDATE_MODE_IMMEDIATE = 0,    /**< Re-encode as soon as a property changes (default) */
    LV_QRCODE_UPDATE_MODE_DEFERRED,         /**< Only mark the bitmap out of date and re-encode once, on the next redraw */
} lv_qrcode_update_mode_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_qrcode_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an empty QR code (an `lv_canvas`) object.
 * @param parent    pointer to a parent widget @nullable. When NULL, the widget
 *                  is created as a screen on the active display.
 * @return          pointer to the created QR code object
 */
lv_obj_t * lv_qrcode_create(lv_obj_t * parent);

/**
 * Set QR code size.
 * @param obj pointer to a QR code object
 * @param size width and height of the QR code
 */
void lv_qrcode_set_size(lv_obj_t * obj, int32_t size);

/**
 * Set QR code dark color.
 * @param obj pointer to a QR code object
 * @param color dark color of the QR code
 */
void lv_qrcode_set_dark_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set QR code light color.
 * @param obj pointer to a QR code object
 * @param color light color of the QR code
 */
void lv_qrcode_set_light_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set a binary payload and generate the bitmap. All `data_len` bytes are encoded.
 * The Widget keeps a copy, so `lv_qrcode_set_size()` and `lv_qrcode_set_quiet_zone()`
 * can re-encode it. You can call them before or after this function.
 * Use `lv_qrcode_set_text()` for a NUL terminated string.
 * @param obj      pointer to a QR code object
 * @param data     payload to encode
 * @param data_len length of `data` in bytes
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error
 */
lv_result_t lv_qrcode_set_data(lv_obj_t * obj, const void * data, uint32_t data_len);

/**
 * Set a text payload and generate the bitmap. The NUL terminator is not encoded.
 * Use `lv_qrcode_set_data()` for binary data.
 * @param obj  pointer to a QR code object
 * @param text payload to encode, as a NUL terminated string
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error
 */
lv_result_t lv_qrcode_set_text(lv_obj_t * obj, const char * text);

/**
 * Get the payload set with `lv_qrcode_set_text()`.
 * Use `lv_qrcode_get_data()` for a payload set with `lv_qrcode_set_data()`.
 * @param obj pointer to a QR code object
 * @return the text, or NULL if the payload is binary or no payload is set.
 *         The QR code object owns the string.
 */
const char * lv_qrcode_get_text(lv_obj_t * obj);

/**
 * Copy the payload into a buffer. Works for text and binary payloads.
 * The NUL terminator of a text payload is not copied.
 * Pass `buf == NULL` or `buf_size == 0` to get only the length.
 * @param obj      pointer to a QR code object
 * @param buf      buffer for the payload @nullable
 * @param buf_size size of `buf` in bytes
 * @return the full length of the payload in bytes. If it is larger than `buf_size`,
 *         only `buf_size` bytes were copied.
 */
uint32_t lv_qrcode_get_data(lv_obj_t * obj, void * buf, uint32_t buf_size);

/**
 * Encode the stored payload again. The bitmap is always regenerated.
 * In LV_QRCODE_UPDATE_MODE_DEFERRED, call this after you set the size and the quiet zone.
 * @param obj pointer to a QR code object
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error (e.g. no data set, or
 *         the payload does not fit the current size)
 */
lv_result_t lv_qrcode_render(lv_obj_t * obj);

/**
 * Enable or disable quiet zone.
 * Quiet zone is the area around the QR code where no data is encoded.
 * @param obj pointer to a QR code object
 * @param enable true: enable quiet zone; false: disable quiet zone
 */
void lv_qrcode_set_quiet_zone(lv_obj_t * obj, bool enable);

/**
 * Set when a change of the size or the quiet zone re-encodes the QR code.
 * LV_QRCODE_UPDATE_MODE_IMMEDIATE (the default) re-encodes in the setter.
 * LV_QRCODE_UPDATE_MODE_DEFERRED only marks the bitmap as out of date.
 * @note In deferred mode, call `lv_qrcode_render()` after you set the properties.
 *       If you do not, the next redraw encodes the bitmap and logs a warning. A failure
 *       in the redraw is only logged.
 * @note A switch to LV_QRCODE_UPDATE_MODE_IMMEDIATE encodes a bitmap that is out of
 *       date and logs a warning. This function cannot return the result. To get it,
 *       call `lv_qrcode_render()` before you switch the mode.
 * @param obj  pointer to a QR code object
 * @param mode the mode to use
 */
void lv_qrcode_set_update_mode(lv_obj_t * obj, lv_qrcode_update_mode_t mode);

/**
 * Get when a property change is turned into a new QR code bitmap.
 * @param obj pointer to a QR code object
 * @return the update mode currently in use
 */
lv_qrcode_update_mode_t lv_qrcode_get_update_mode(lv_obj_t * obj);

/**
 * Check if the last encode failed. Use this after a change of the size or the quiet
 * zone, or after a redraw in LV_QRCODE_UPDATE_MODE_DEFERRED. These encodes cannot
 * return a result. The payload setters and `lv_qrcode_render()` return their result.
 * @note The Widget does not retry a failed encode on each redraw. The next change of
 *       the size, the quiet zone or the payload starts a new attempt.
 * @param obj pointer to a QR code object
 * @return true: no encode failed since the last change of the size, the quiet zone or
 *               the payload.
 *               This is also true while a deferred encode is pending;
 *         false: the last encode failed, or no payload is set
 */
bool lv_qrcode_is_render_valid(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_QRCODE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_QRCODE_H*/
