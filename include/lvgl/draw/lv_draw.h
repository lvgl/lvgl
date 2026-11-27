/**
 * @file lv_draw.h
 *
 */

/**
 * Modified by NXP in 2024
 */

#ifndef LV_DRAW_H
#define LV_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../config/lv_conf_internal.h"

#include "../lv_types.h"
#include "../core/lv_style.h"
#include "../font/lv_text.h"
#include "../debugging/profiler/lv_profiler.h"
#include "../core/lv_matrix.h"
#include "../core/lv_event.h"
#include "lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/
#if LV_DRAW_TRANSFORM_USE_MATRIX
#if !LV_USE_MATRIX
#error "LV_DRAW_TRANSFORM_USE_MATRIX requires LV_USE_MATRIX = 1"
#endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DRAW_DSC_TYPE_NONE = 0,
    LV_DRAW_DSC_TYPE_FILL,
    LV_DRAW_DSC_TYPE_BORDER,
    LV_DRAW_DSC_TYPE_BOX_SHADOW,
    LV_DRAW_DSC_TYPE_LETTER,
    LV_DRAW_DSC_TYPE_LABEL,
    LV_DRAW_DSC_TYPE_IMAGE,
    LV_DRAW_DSC_TYPE_LAYER,
    LV_DRAW_DSC_TYPE_LINE,
    LV_DRAW_DSC_TYPE_ARC,
    LV_DRAW_DSC_TYPE_TRIANGLE,
    LV_DRAW_DSC_TYPE_MASK_RECTANGLE,
    LV_DRAW_DSC_TYPE_BLUR,
#if LV_USE_VECTOR_GRAPHIC
    LV_DRAW_DSC_TYPE_VECTOR,
#endif
#if LV_USE_3DTEXTURE
    LV_DRAW_DSC_TYPE_3D,
#endif
    LV_DRAW_DSC_TYPE_COUNT
} lv_draw_dsc_type_t;

struct _lv_layer_t  {
    /** Target draw buffer of the layer */
    lv_draw_buf_t * draw_buf;

    /** The display assigned to this layer*/
    lv_display_t * display;

    /** Parent layer */
    lv_layer_t * parent;

    /** Next layer */
    lv_layer_t * next;

    /** User data */
    void * user_data;

    /** The absolute coordinates of the buffer */
    lv_area_t buf_area;

    /** The physical clipping area relative to the display */
    lv_area_t phy_clip_area;

    /**
     * The current clip area with absolute coordinates, always the same or smaller than `buf_area`.
     * The draw functions read it when they're called. Draw units use the `clip_area` they get instead.
     */
    lv_area_t _clip_area;

#if LV_DRAW_TRANSFORM_USE_MATRIX
    /** Transform matrix to be applied when rendering the layer */
    lv_matrix_t matrix;
#endif

    /** Partial y offset */
    int32_t partial_y_offset;

    /** Recolor of the layer */
    lv_color32_t recolor;

    /** The color format of the layer. LV_COLOR_FORMAT_... */
    lv_color_format_t color_format;

    /** Opacity of the layer */
    lv_opa_t opa;

    /** true if LVGL is responsible for freeing `draw_buf`*/
    bool buffer_owned;
};

typedef struct {
    /**The widget for which draw descriptor was created */
    lv_obj_t * obj;

    /**The widget part for which draw descriptor was created */
    uint32_t part;

    /**A widget type specific ID (e.g. table row index). See the docs of the given widget.*/
    uint32_t id1;

    /**A widget type specific ID (e.g. table column index). See the docs of the given widget.*/
    uint32_t id2;

    /**The target layer */
    lv_layer_t * layer;

    /**The type of the draw descriptor*/
    lv_draw_dsc_type_t type;

    /*Drop shadow is part of every draw dsc as anything can have drop shadow*/

    /**Drop shadow offset in X*/
    int16_t drop_shadow_ofs_x;

    /**Drop shadow offset in Y*/
    int16_t drop_shadow_ofs_y;

    /**Drop shadow color*/
    lv_color_t drop_shadow_color;

    /**Drop shadow opacity*/
    lv_opa_t drop_shadow_opa;

    /**Drop shadow blur radius*/
    int32_t drop_shadow_blur_radius: 20;

    /**Drop shadow blur quality*/
    lv_blur_quality_t drop_shadow_quality : 3;

    /**Size of the specific draw descriptor into which this base descriptor is embedded*/
    size_t dsc_size;

    /**Any custom user data*/
    void * user_data;
} lv_draw_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Used internally to initialize the drawing module
 */
void lv_draw_init(void);

/**
 * Deinitialize the drawing module
 */
void lv_draw_deinit(void);

/**
 * Allocate a new draw unit with the given size and appends it to the list of draw units.
 * It becomes the default draw unit and falls back to the software draw unit.
 * @param size      the size to allocate. E.g. `sizeof(my_draw_unit_t)`,
 *                  where the first element of `my_draw_unit_t` is `lv_draw_unit_t`.
 */
void * lv_draw_create_unit(size_t size);

/**
 * Wait until every draw unit has finished the drawing it has queued.
 */
void lv_draw_wait_for_finish(void);

/**
 * Get the total number of draw units.
  */
uint32_t lv_draw_get_unit_count(void);


/**
 * Send an event to the draw units
 * @param name              the name of the draw unit to send the event to @nullable.
 *                          When NULL the event is sent to all draw units
 * @param code              the event code
 * @param param             the event parameter @nullable
 */
void lv_draw_unit_send_event(const char * name, lv_event_code_t code, void * param);

/**
 * Initialize a layer
 * @param layer pointer to a layer to initialize
 */
void lv_layer_init(lv_layer_t * layer);

/**
 * Reset the layer to a drawable state
 * @param layer pointer to a layer to reset
 */
void lv_layer_reset(lv_layer_t * layer);

/**
 * Create (allocate) a new layer on a parent layer
 * @param parent_layer      the parent layer to which the layer will be merged when it's rendered @nullable
 * @param color_format      the color format of the layer
 * @param area              the areas of the layer (absolute coordinates)
 * @return                  the new target_layer or NULL on error
 */
lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area);

/**
 * Initialize a layer which is allocated by the user
 * @param layer             pointer the layer to initialize (its lifetime needs to be managed by the user)
 * @param parent_layer      the parent layer to which the layer will be merged when it's rendered @nullable
 * @param color_format      the color format of the layer
 * @param area              the areas of the layer (absolute coordinates)
 * @return                  LV_RESULT_OK or LV_RESULT_INVALID on error
 */
lv_result_t lv_draw_layer_init(lv_layer_t * layer, lv_layer_t * parent_layer, lv_color_format_t color_format,
                               const lv_area_t * area);

/**
 * Got to a pixel at X and Y coordinate on a layer
 * @param layer             pointer to a layer
 * @param x                 the target X coordinate
 * @param y                 the target X coordinate
 * @return                  `buf` offset to point to the given X and Y coordinate
 */
void * lv_draw_layer_go_to_xy(lv_layer_t * layer, int32_t x, int32_t y);

/**
 * Get the type of a draw descriptor
 * @param dsc    the draw descriptor
 * @return       the type of the draw descriptor
 */
lv_draw_dsc_type_t lv_draw_dsc_get_type(const lv_draw_dsc_t * dsc);



lv_layer_t * lv_draw_layer_create_drop_shadow(lv_layer_t * parent_layer, const lv_draw_dsc_t * base,
                                              const lv_area_t * area);

void lv_draw_layer_finish_drop_shadow(lv_layer_t * drop_shadow_layer, const lv_draw_dsc_t * base);



/**
 * Deallocate a buffer attached to this layer.
 * @param layer             pointer to a layer
 */
void lv_draw_layer_dealloc_buf(lv_layer_t * layer);

/**
 * Deinitialize a layer, reverse operation of `lv_draw_layer_init`
 * @param layer             pointer the layer to deinitialize @nullable
 */
void lv_draw_layer_deinit(lv_layer_t * layer);

/**
 * Delete a layer, reverse operation of `lv_draw_layer_create`
 * @param layer             pointer the layer to delete @nullable
 */
void lv_draw_layer_delete(lv_layer_t * layer);

/**
 * Get the next draw unit.
 * @param unit  a draw unit, or NULL to get the first one, which is the main unit   @nullable
 * @return      the draw unit after `unit`, or NULL after the last one
 */
lv_draw_unit_t * lv_draw_unit_get_next(lv_draw_unit_t * unit);

/**
 * Get the name of a draw unit, e.g. "SW".
 * @param unit  pointer to a draw unit
 * @return      the name of the draw unit
 */
const char * lv_draw_unit_get_name(const lv_draw_unit_t * unit);

/**
 * Get how many descriptors of a type a draw unit has drawn since the last reset,
 * including those it drew as the fallback of another unit.
 *
 * Always 0 if LV_USE_DRAW_COUNTERS is disabled
 *
 * @param unit  pointer to a draw unit
 * @param type  the draw descriptor type
 * @return      the number of descriptors drawn
 */
uint32_t lv_draw_unit_get_draw_count(const lv_draw_unit_t * unit, lv_draw_dsc_type_t type);

/**
 * Get how many descriptors of a type no draw unit could draw since the last reset.
 *
 * Always 0 if LV_USE_DRAW_COUNTERS is disabled
 *
 * @param type  the draw descriptor type
 * @return      the number of descriptors dropped
 */
uint32_t lv_draw_get_dropped_count(lv_draw_dsc_type_t type);

/**
 * Set the draw counts of every draw unit and the dropped counts to 0.
 */
void lv_draw_reset_counters(void);

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_H*/
