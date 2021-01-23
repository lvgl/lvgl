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

/** Design results */
typedef enum {
    LV_DRAW_RES_OK,          /**< Draw ready */
    LV_DRAW_RES_COVER,       /**< Returned on `LV_DRAW_COVER_CHK` if the areas is fully covered*/
    LV_DRAW_RES_NOT_COVER,   /**< Returned on `LV_DRAW_COVER_CHK` if the areas is not covered*/
    LV_DRAW_RES_MASKED,      /**< Returned on `LV_DRAW_COVER_CHK` if the areas is masked out (children also not cover)*/
}lv_draw_res_t;


typedef struct
{
    lv_draw_rect_dsc_t * rect_dsc;
    lv_draw_label_dsc_t * label_dsc;
    lv_draw_line_dsc_t * line_dsc;
    lv_draw_img_dsc_t *  img_dsc;
    lv_draw_arc_dsc_t *  arc_dsc;
    char text[16];
    const lv_area_t * draw_area;
    const lv_point_t * p1;
    const lv_point_t * p2;
    const lv_coord_t * radius;
    const lv_area_t * clip_area;
    uint32_t id;
    uint8_t part;
}lv_obj_draw_hook_dsc_t;

/** Design modes */
enum {
    LV_DRAW_MODE_COVER_CHECK,      /**< Check if the object fully covers the 'mask_p' area */
    LV_DRAW_MODE_MAIN_DRAW,            /**< Draw the main portion of the object */
    LV_DRAW_MODE_POST_DRAW,            /**< Draw extras on the object */
};
typedef uint8_t lv_draw_mode_t;

/**
 * The draw callback is used to draw the object on the screen.
 * It accepts the object, a mask area, and the mode in which to draw the object.
 */
typedef lv_draw_res_t (*lv_draw_cb_t)(struct _lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
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

void lv_obj_init_draw_arc_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_arc_dsc_t * draw_dsc);

bool lv_obj_draw_has_custom(const struct _lv_obj_t * obj, uint8_t part);

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
void lv_obj_draw_hook_dsc_init(lv_obj_draw_hook_dsc_t * hook_dsc, const lv_area_t * clip_area);

/**
 * Draw scrollbars on an object is required
 * @param obj pointer to an object
 * @param clip_area the clip area coming from the draw function
 */
void _lv_obj_draw_scrollbar(struct _lv_obj_t * obj, const lv_area_t * clip_area);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_DRAW_H*/
