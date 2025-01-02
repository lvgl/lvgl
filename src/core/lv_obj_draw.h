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
#include "../misc/lv_types.h"
#include "../draw/lv_draw_rect.h"
#include "../draw/lv_draw_label.h"
#include "../draw/lv_draw_image.h"
#include "../draw/lv_draw_line.h"
#include "../draw/lv_draw_arc.h"
#include "../draw/lv_draw_triangle.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Store the type of layer required to render a widget.*/
typedef enum {
    /**No layer is needed. */
    LV_LAYER_TYPE_NONE,

    /**Simple layer means that the layer can be rendered in chunks.
     * For example with opa_layered = 140 it's possible to render only 10 lines
     * from the layer. When it's ready go the the next 10 lines.
     * It avoids large memory allocations for the layer buffer.
     * The buffer size for a chunk can be set by `LV_DRAW_LAYER_SIMPLE_BUF_SIZE` in lv_conf.h.*/
    LV_LAYER_TYPE_SIMPLE,

    /**The widget is transformed and cannot be rendered in chunks.
     * It's because - due to the transformations -  pixel outside of
     * a given area will also contribute to the final image.
     * In this case there is no limitation on the buffer size.
     * LVGL will allocate as large buffer as needed to render the transformed area.*/
    LV_LAYER_TYPE_TRANSFORM,
} lv_layer_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a rectangle draw descriptor from an object's styles in its current state
 * @param obj       pointer to an object
 * @param part      part of the object, e.g. `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc  the descriptor to initialize.
 *                  If an `..._opa` field is set to `LV_OPA_TRANSP` the related properties won't be initialized.
 *                  Should be initialized with `lv_draw_rect_dsc_init(draw_dsc)`.
 * @note Only the relevant fields will be set.
 *       E.g. if `border width == 0` the other border properties won't be evaluated.
 */
void lv_obj_init_draw_rect_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_rect_dsc_t * draw_dsc);

/**
 * Initialize a label draw descriptor from an object's styles in its current state
 * @param obj       pointer to an object
 * @param part      part of the object, e.g. `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc  the descriptor to initialize.
 *                  If the `opa` field is set to or the property is equal to `LV_OPA_TRANSP` the rest won't be initialized.
 *                  Should be initialized with `lv_draw_label_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_label_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_label_dsc_t * draw_dsc);

/**
 * Initialize an image draw descriptor from an object's styles in its current state
 * @param obj       pointer to an object
 * @param part      part of the object, e.g. `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc  the descriptor to initialize.
 *                  Should be initialized with `lv_draw_image_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_image_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_image_dsc_t * draw_dsc);

/**
 * Initialize a line draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param part      part of the object, e.g. `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc  the descriptor to initialize.
 *                  Should be initialized with `lv_draw_line_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_line_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_line_dsc_t * draw_dsc);

/**
 * Initialize an arc draw descriptor from an object's styles in its current state
 * @param obj       pointer to an object
 * @param part      part of the object, e.g. `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_KNOB`, etc
 * @param draw_dsc  the descriptor to initialize.
 *                  Should be initialized with `lv_draw_arc_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_arc_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_arc_dsc_t * draw_dsc);

/**
 * Get the required extra size (around the object's part) to draw shadow, outline, value etc.
 * @param obj       pointer to an object
 * @param part      part of the object
 * @return          the extra size required around the object
 */
int32_t lv_obj_calculate_ext_draw_size(lv_obj_t * obj, lv_part_t part);

/**
 * Send a 'LV_EVENT_REFR_EXT_DRAW_SIZE' Call the ancestor's event handler to the object to refresh the value of the extended draw size.
 * The result will be saved in `obj`.
 * @param obj       pointer to an object
 */
void lv_obj_refresh_ext_draw_size(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_DRAW_H*/
