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
#include "../draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;

/** Design results*/
typedef enum {
    LV_DRAW_RES_OK,          /**< Draw ready*/
    LV_DRAW_RES_COVER,       /**< Returned on `LV_DRAW_COVER_CHK` if the areas is fully covered*/
    LV_DRAW_RES_NOT_COVER,   /**< Returned on `LV_DRAW_COVER_CHK` if the areas is not covered*/
    LV_DRAW_RES_MASKED,      /**< Returned on `LV_DRAW_COVER_CHK` if the areas is masked out (children also not cover)*/
}lv_draw_res_t;


typedef struct
{
    lv_area_t * draw_area;
    const lv_area_t * clip_area;
    lv_draw_rect_dsc_t * rect_dsc;
    lv_draw_label_dsc_t * label_dsc;
    lv_draw_line_dsc_t * line_dsc;
    lv_draw_img_dsc_t *  img_dsc;
    lv_draw_arc_dsc_t *  arc_dsc;
    const lv_point_t * p1;
    const lv_point_t * p2;
    const lv_coord_t * radius;
    char text[16];
    int32_t value;
    uint32_t id;
    uint32_t part   :8;
    const void * sub_part_ptr;
}lv_obj_draw_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a rectangle draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part part of the object. E.g.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc the descriptor the initialize.
 *                 If an `..._opa` filed is set to `LV_OPA_TRANSP` the related properties won't be initialized.
 *                 Should be initialized with `lv_draw_rect_dsc_init(draw_dsc)`.
 * @note Only the relevant fields will be set.
 *       E.g. if `border width == 0` the other border properties won't be evaluated.
 */
void lv_obj_init_draw_rect_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_rect_dsc_t * draw_dsc);

/**
 * Initialize a label draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part part of the object. E.g.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc the descriptor the initialize.
 *                 If the `opa` filed is set to or the property is equal to `LV_OPA_TRANSP` the rest won't be initialized.
 *                 Should be initialized with `lv_draw_label_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_label_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_label_dsc_t * draw_dsc);

/**
 * Initialize an image draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part part of the object. E.g.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc the descriptor the initialize.
 *                 Should be initialized with `lv_draw_image_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_img_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_img_dsc_t * draw_dsc);


/**
 * Initialize a line draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part part of the object. E.g.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc the descriptor the initialize.
 *                 Should be initialized with `lv_draw_line_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_line_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_line_dsc_t * draw_dsc);

/**
 * Initialize an arc draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part part of the object. E.g.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc the descriptor the initialize.
 *                 Should be initialized with `lv_draw_arc_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_arc_dsc(struct _lv_obj_t * obj, uint8_t part, lv_draw_arc_dsc_t * draw_dsc);

/**
 * Get the required extra size (around the object's part) to draw shadow, outline, value etc.
 * @param obj pointer to an object
 * @param part part of the object
 * @return the extra size required around the object
 */
lv_coord_t lv_obj_calculate_ext_draw_size(struct _lv_obj_t * obj, uint8_t part);

/**
 * Initialize a draw descriptor used in events.
 * @param dsc pointer to a descriptor. Later it should be passed as parameter to an `LV_EEVNT_DRAW_PART_BEGIN/END` event.
 * @param clip_area the current clip area of the drawing
 */
void lv_obj_draw_dsc_init(lv_obj_draw_dsc_t * dsc, const lv_area_t * clip_area);

/**
 * Send a 'LV_EVENT_REFR_EXT_DRAW_SIZE' Call the ancestor's event handler to the object to refresh the value of the extended draw size.
 * The result will be saved in `obj`.
 * @param obj pointer to an object
 */
void lv_obj_refresh_ext_draw_size(struct _lv_obj_t * obj);

/**
 * Get the extended draw area of an object.
 * @param obj pointer to an object
 * @return the size extended draw area around the real coordinates
 */
lv_coord_t _lv_obj_get_ext_draw_size(const struct _lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_DRAW_H*/
