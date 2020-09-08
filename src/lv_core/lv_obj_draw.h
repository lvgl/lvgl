/**
 * @file lv_obj_draw.h
 *
 */

#ifndef LV_OBJ_DRAW_H
#define LV_OBJ_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a rectangle draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_OBJ_PART_MAIN`, `LV_BTN_PART_REL` or `LV_SLIDER_PART_KNOB`
 * @param draw_dsc the descriptor the initialize.
 *                 If an `*_opa` filed is set to `LV_OPA_TRANSP` the related properties won't be initialized.
 *                 Should be initialized with `lv_draw_rect_dsc_init(draw_dsc)`.
 * @note Only the relevant fields will be set.
 *       E.g. if `border width == 0` the other border properties won't be evaluated.
 */
void lv_obj_init_draw_rect_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_rect_dsc_t * draw_dsc);

/**
 * Initialize a label draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_OBJ_PART_MAIN`, `LV_BTN_PART_REL` or `LV_SLIDER_PART_KNOB`
 * @param draw_dsc the descriptor the initialize.
 *                 If the `opa` filed is set to or the property is equal to `LV_OPA_TRANSP` the rest won't be initialized.
 *                 Should be initialized with `lv_draw_label_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_label_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_label_dsc_t * draw_dsc);

/**
 * Initialize an image draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_IMG_PART_MAIN`
 * @param draw_dsc the descriptor the initialize.
 *                 If the `opa` filed is set to or the property is equal to `LV_OPA_TRANSP` the rest won't be initialized.
 *                 Should be initialized with `lv_draw_image_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_img_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_img_dsc_t * draw_dsc);

/**
 * Initialize a line draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_LINE_PART_MAIN`
 * @param draw_dsc the descriptor the initialize.
 *                 If the `opa` filed is set to or the property is equal to `LV_OPA_TRANSP` the rest won't be initialized.
 *                 Should be initialized with `lv_draw_line_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_line_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_line_dsc_t * draw_dsc);

/**
 * Get the required extra size (around the object's part) to draw shadow, outline, value etc.
 * @param obj pointer to an object
 * @param part part of the object
 * @return the extra size required around the object
 */
lv_coord_t _lv_obj_get_draw_rect_ext_pad_size(struct _lv_obj_t * obj, uint8_t part);

/**
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object to refresh the extended draw area.
 * The result will be written into `obj->ext_draw_pad`
 * @param obj pointer to an object
 */
void _lv_obj_refresh_ext_draw_pad(struct _lv_obj_t * obj);

/**
 * Draw scrollbars on an object is required
 * @param obj pointer to an object
 * @param clip_area the clip area coming from the design function
 */
void _lv_obj_draw_scrollbar(struct _lv_obj_t * obj, const lv_area_t * clip_area);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_DRAW_H*/
