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

/** Cover check results.*/
typedef enum {
    LV_COVER_RES_COVER      = 0,
    LV_COVER_RES_NOT_COVER  = 1,
    LV_COVER_RES_MASKED     = 2,
}lv_cover_res_t;

typedef struct
{
    const lv_area_t * clip_area;        /**< The current clip area, required if you need to draw something in the event*/
    lv_area_t * draw_area;              /**< The area of the part being drawn*/
    lv_draw_rect_dsc_t * rect_dsc;      /**< A draw descriptor that can be modified to changed what LVGL will draw. Set only for rectangle-like parts*/
    lv_draw_label_dsc_t * label_dsc;    /**< A draw descriptor that can be modified to changed what LVGL will draw. Set only for text-like parts*/
    lv_draw_line_dsc_t * line_dsc;      /**< A draw descriptor that can be modified to changed what LVGL will draw. Set only for line-like parts*/
    lv_draw_img_dsc_t *  img_dsc;       /**< A draw descriptor that can be modified to changed what LVGL will draw. Set only for image-like parts*/
    lv_draw_arc_dsc_t *  arc_dsc;       /**< A draw descriptor that can be modified to changed what LVGL will draw. Set only for arc-like parts*/
    const lv_point_t * p1;              /**< A point calculated during drawing. E.g. a point of chart or the center of an arc.*/
    const lv_point_t * p2;              /**< A point calculated during drawing. E.g. a point of chart.*/
    char text[16];                      /**< A text calculated during drawing. Can be modified. E.g. tick labels on a chart axis.*/
    uint32_t part;                      /**< The current part for which the event is sent*/
    uint32_t id;                        /**< The index of the part. E.g. a button's index on button matrix or table cell index.*/
    lv_coord_t radius;                  /**< E.g. the radius of an arc (not the corner radius).*/
    int32_t value;                      /**< A value calculated during drawing. E.g. Chart's tick line value.*/
    const void * sub_part_ptr;          /**< A pointer the identifies something in the part. E.g. chart series. */
}lv_obj_draw_part_dsc_t;

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
void lv_obj_init_draw_rect_dsc(struct _lv_obj_t * obj, uint32_t part, lv_draw_rect_dsc_t * draw_dsc);

/**
 * Initialize a label draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part part of the object. E.g.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc the descriptor the initialize.
 *                 If the `opa` filed is set to or the property is equal to `LV_OPA_TRANSP` the rest won't be initialized.
 *                 Should be initialized with `lv_draw_label_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_label_dsc(struct _lv_obj_t * obj, uint32_t part, lv_draw_label_dsc_t * draw_dsc);

/**
 * Initialize an image draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part part of the object. E.g.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc the descriptor the initialize.
 *                 Should be initialized with `lv_draw_image_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_img_dsc(struct _lv_obj_t * obj, uint32_t part, lv_draw_img_dsc_t * draw_dsc);


/**
 * Initialize a line draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part part of the object. E.g.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc the descriptor the initialize.
 *                 Should be initialized with `lv_draw_line_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_line_dsc(struct _lv_obj_t * obj, uint32_t part, lv_draw_line_dsc_t * draw_dsc);

/**
 * Initialize an arc draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part part of the object. E.g.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc the descriptor the initialize.
 *                 Should be initialized with `lv_draw_arc_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_arc_dsc(struct _lv_obj_t * obj, uint32_t part, lv_draw_arc_dsc_t * draw_dsc);

/**
 * Get the required extra size (around the object's part) to draw shadow, outline, value etc.
 * @param obj pointer to an object
 * @param part part of the object
 * @return the extra size required around the object
 */
lv_coord_t lv_obj_calculate_ext_draw_size(struct _lv_obj_t * obj, uint32_t part);

/**
 * Initialize a draw descriptor used in events.
 * @param dsc pointer to a descriptor. Later it should be passed as parameter to an `LV_EEVNT_DRAW_PART_BEGIN/END` event.
 * @param clip_area the current clip area of the drawing
 */
void lv_obj_draw_dsc_init(lv_obj_draw_part_dsc_t * dsc, const lv_area_t * clip_area);

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
