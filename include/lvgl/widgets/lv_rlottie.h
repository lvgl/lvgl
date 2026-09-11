/**
 * @file lv_rlottie.h
 *
 */

#ifndef LV_RLOTTIE_H
#define LV_RLOTTIE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../config/lv_conf_internal.h"
#include "../lv_types.h"
#if LV_USE_RLOTTIE

/*********************
 *      DEFINES
 *********************/

/**
 * @deprecated The rlottie player is deprecated and kept only for backward
 * compatibility. Use the `lv_lottie` widget instead. See the Lottie widget docs.
 */
#define LV_RLOTTIE_DEPRECATED_MSG \
    "rlottie is deprecated; use the lv_lottie widget instead."

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_RLOTTIE_CTRL_FORWARD  = 0,
    LV_RLOTTIE_CTRL_BACKWARD = 1,
    LV_RLOTTIE_CTRL_PAUSE    = 2,
    LV_RLOTTIE_CTRL_PLAY     = 0, /* Yes, play = 0 is the default mode */
    LV_RLOTTIE_CTRL_LOOP     = 8,
} lv_rlottie_ctrl_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_rlottie_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an rlottie animation from a JSON file.
 * The animation starts playing forward in a loop right away.
 *
 * rlottie doesn't use LVGL's File System API.
 *
 * @param parent    pointer to a parent widget @nullable. When NULL, the widget
 *                  is created as a screen on the active display.
 * @param width     width of the animation and its render buffer in pixels
 * @param height    height of the animation and its render buffer in pixels
 * @param path      path to a json file, e.g. "path/to/file.json"
 * @return          pointer to the created rlottie widget, or NULL if `path` is NULL
 * @deprecated rlottie is deprecated. Use the `lv_lottie` widget instead.
 */
LV_DEPRECATED(LV_RLOTTIE_DEPRECATED_MSG)
lv_obj_t * lv_rlottie_create_from_file(lv_obj_t * parent, int32_t width, int32_t height, const char * path);

/**
 * Create an rlottie animation from a raw JSON description.
 * The animation starts playing forward in a loop right away.
 *
 * @param parent        pointer to a parent widget @nullable. When NULL, the widget
 *                      is created as a screen on the active display.
 * @param width         width of the animation and its render buffer in pixels
 * @param height        height of the animation and its render buffer in pixels
 * @param rlottie_desc  the lottie animation as a nul terminated string
 * @return              pointer to the created rlottie widget, or NULL if
 *                      `rlottie_desc` is NULL
 * @deprecated rlottie is deprecated. Use the `lv_lottie` widget instead.
 */
LV_DEPRECATED(LV_RLOTTIE_DEPRECATED_MSG)
lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, int32_t width, int32_t height,
                                      const char * rlottie_desc);

/**
 * Set how the animation is played.
 *
 * @param rlottie   pointer to an rlottie widget
 * @param ctrl      OR-ed values of `lv_rlottie_ctrl_t`, e.g.
 *                  `LV_RLOTTIE_CTRL_BACKWARD | LV_RLOTTIE_CTRL_LOOP`.
 *                  Without `LV_RLOTTIE_CTRL_LOOP` an `LV_EVENT_READY` event is
 *                  sent when the last frame is reached.
 * @deprecated rlottie is deprecated. Use the `lv_lottie` widget instead.
 */
LV_DEPRECATED(LV_RLOTTIE_DEPRECATED_MSG)
void lv_rlottie_set_play_mode(lv_obj_t * rlottie, const lv_rlottie_ctrl_t ctrl);

/**
 * Jump to a given frame of the animation.
 *
 * @param rlottie       pointer to an rlottie widget
 * @param goto_frame    zero based index of the frame to show. Values not smaller
 *                      than the total frame count select the last frame.
 * @deprecated rlottie is deprecated. Use the `lv_lottie` widget instead.
 */
LV_DEPRECATED(LV_RLOTTIE_DEPRECATED_MSG)
void lv_rlottie_set_current_frame(lv_obj_t * rlottie, const size_t goto_frame);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLOTTIE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_RLOTTIE_H*/
