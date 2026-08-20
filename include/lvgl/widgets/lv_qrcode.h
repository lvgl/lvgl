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
 * Controls when a property change is turned into a new QR code bitmap.
 * Only the properties that require re-encoding (size, quiet zone) are affected;
 * `lv_qrcode_update()` always encodes right away and the colors are always a
 * palette-only write.
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
 * Set the data of a QR code object and generate the bitmap.
 * A copy of the data is stored, so a later `lv_qrcode_set_size()` or
 * `lv_qrcode_set_quiet_zone()` can re-encode it. The properties may therefore be
 * set before or after the data, in any order.
 * Use `lv_qrcode_render()` to re-encode the stored payload without passing it again.
 * @param obj      pointer to a QR code object
 * @param data     data to display
 * @param data_len length of `data` in bytes
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error
 */
lv_result_t lv_qrcode_update(lv_obj_t * obj, const void * data, uint32_t data_len);

/**
 * Helper function to set the data of a QR code object from a string.
 * The NUL terminator is not part of the encoded payload.
 * @param obj  pointer to a QR code object
 * @param data data to display as a NUL terminated string
 */
void lv_qrcode_set_data(lv_obj_t * obj, const char * data);

/**
 * (Re)generate the QR code bitmap from the payload that is already stored.
 * Unlike `lv_qrcode_update()` this needs no payload, so it is the way to apply property
 * changes made in LV_QRCODE_UPDATE_MODE_DEFERRED: set the size and quiet zone, then call
 * this once to encode them and get the result.
 * The bitmap is regenerated whether or not anything changed.
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
 * Set when a property change is turned into a new QR code bitmap.
 * With LV_QRCODE_UPDATE_MODE_IMMEDIATE (the default) changing the size or the quiet
 * zone re-encodes the stored data right away. With LV_QRCODE_UPDATE_MODE_DEFERRED
 * such a change only marks the bitmap as out of date and several changes are
 * collapsed into a single re-encode on the next redraw.
 * @note In deferred mode you are expected to call `lv_qrcode_render()` yourself once
 *       the properties are set. It encodes right away and returns the result, leaving
 *       the next redraw nothing to do. If it is forgotten, the encode is done by the
 *       redraw instead: the bitmap is still correct, but the work is charged to that
 *       refresh and its result cannot be reported to anyone, so a warning is logged.
 *       Prefer the explicit call.
 * @note Switching back to LV_QRCODE_UPDATE_MODE_IMMEDIATE while the bitmap is out of
 *       date also re-encodes it, but this function returns void, so an encode failure
 *       can only be logged, not reported. A warning is emitted in that case. Call
 *       `lv_qrcode_render()` first and switch the mode afterwards to get the result.
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
 * Get whether the QR code bitmap is missing because the last attempt to generate it
 * failed. Most encodes report their result directly: `lv_qrcode_update()` returns it.
 * The ones that cannot are the re-encodes triggered by a property change - they happen
 * in a void setter or, in LV_QRCODE_UPDATE_MODE_DEFERRED, in the draw pass. Use this to
 * detect those, e.g. after shrinking the object below the size its payload needs.
 * @note A failed encode leaves the bitmap marked as out of date, so it is never reported
 *       as current, and it is not retried on every redraw - only a property change makes
 *       the Widget try again. Encode failures are not logged when the caller can see the
 *       result; the one exception is the re-encode done by the redraw, which has no
 *       caller, so that one is logged.
 * @param obj pointer to a QR code object
 * @return true: the last generation attempt failed, or no data has been set yet;
 *         false: the bitmap holds a valid QR code
 */
bool lv_qrcode_get_render_failed(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_QRCODE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_QRCODE_H*/
