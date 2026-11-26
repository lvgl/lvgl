/**
 * @file lv_draw_private.h
 *
 */

#ifndef LV_DRAW_PRIVATE_H
#define LV_DRAW_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lvgl_public.h"
#include "../osal/lv_os_private.h"
#include "../misc/cache/lv_cache.h"
#include "../misc/cache/lv_cache_entry.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_mask_t {
    void * user_data;
};

/**
 * The draw callbacks of a draw unit, one per draw descriptor type.
 * NULL for the types the unit doesn't draw.
 * Each `can_draw_*_cb` tells whether the unit can draw the descriptor on the layer. It must not change anything.
 * NULL: the unit can draw every descriptor of the type it has a draw callback for.
 */
typedef struct {
    void (*draw_fill_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                         const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_fill_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
    void (*draw_border_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_border_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc);
    void (*draw_box_shadow_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                               const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_box_shadow_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer,
                                   const lv_draw_box_shadow_dsc_t * dsc);
    void (*draw_letter_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_letter_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_letter_dsc_t * dsc);
    void (*draw_label_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_label_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc);
    void (*draw_image_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_image_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
    void (*draw_layer_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_layer_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
    void (*draw_arc_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_arc_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc);
    void (*draw_line_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                         const lv_draw_line_dsc_t * dsc);
    bool (*can_draw_line_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc);
    void (*draw_triangle_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                             const lv_draw_triangle_dsc_t * dsc);
    bool (*can_draw_triangle_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc);
    void (*draw_mask_rect_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                              const lv_draw_mask_rect_dsc_t * dsc);
    bool (*can_draw_mask_rect_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc);
    void (*draw_blur_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                         const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_blur_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc);
#if LV_USE_VECTOR_GRAPHIC
    void (*draw_vector_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_vector_dsc_t * dsc);
    bool (*can_draw_vector_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_vector_dsc_t * dsc);
#endif
#if LV_USE_3DTEXTURE
    void (*draw_3d_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords);
    bool (*can_draw_3d_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_3d_dsc_t * dsc);
#endif
} lv_draw_unit_draw_cbs_t;

struct _lv_draw_unit_t {
    lv_draw_unit_t * next;

    /**
     * Name and ID of the draw unit, for debugging purposes only.
     */
    const char * name;
    int32_t idx;

    /**
     * The draw callbacks of the unit. Usually a `static const` table.
     */
    const lv_draw_unit_draw_cbs_t * draw_cbs;

    /**
     * The unit to try when this one can't draw a descriptor. NULL: the descriptor isn't drawn.
     * Set by the unit itself, e.g. to `lv_draw_sw_get_draw_unit()`.
     */
    lv_draw_unit_t * fallback;

    /**
     * Not used by LVGL.
     */
    uint8_t id;

    /**
     * Wait until the unit has finished everything it has queued (e.g. a GPU command list),
     * so that the CPU can read or flush the buffers it drew to.
     * NULL if the unit draws before its draw callbacks return.
     * @param draw_unit     pointer to the draw unit
     * @return              not used, return 0
     */
    int32_t (*wait_for_finish_cb)(lv_draw_unit_t * draw_unit);

    /**
     * Called to delete draw unit.
     * @param draw_unit
     * @return
     */
    int32_t (*delete_cb)(lv_draw_unit_t * draw_unit);

    /**
     * Called when an event is sent to the draw unit.
     * @param event pointer to the event descriptor
     */
    void (*event_cb)(lv_event_t * event);
};

typedef struct {
    lv_draw_unit_t * unit_head;
    uint32_t unit_cnt;
    uint32_t used_memory_for_layers; /* measured as bytes */
    lv_mutex_t circle_cache_mutex;
    bool is_sending_event;
} lv_draw_global_info_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Draw a descriptor on a layer with the draw units, sending the draw events around it.
 * Used by the `lv_draw_<type>()` functions after they've prepared the descriptor.
 *
 * The descriptor is modified, so pass a local copy (e.g. on the stack), never the caller's `const` descriptor:
 * - its `type` and `layer` fields are set;
 * - `LV_EVENT_DRAW_DSC_BEGIN` handlers may change it before it's drawn.
 *
 * The copy doesn't need to be allocated: the descriptor, and everything it points to (points, text,
 * image source), is no longer used after this function returns. Drawing may still be in progress
 * when it returns.
 *
 * If no draw unit can draw it, nothing is drawn and a warning is logged.
 *
 * @param dsc       the descriptor to draw, modified as described above
 * @param type      the type of `dsc`, which decides the draw callback that is called
 * @param layer     the layer to draw on; the clip area is read from it once
 * @param coords    the area of the drawing, or its bounding box for line, triangle, arc, vector and mask (absolute coordinates)
 */
void lv_draw_dsc_submit(lv_draw_dsc_t * dsc, lv_draw_dsc_type_t type, lv_layer_t * layer, const lv_area_t * coords);

/**
 * Initialize a layer that draws into a buffer owned by someone else.
 * @param layer         pointer to the layer to initialize
 * @param parent_layer  the parent layer @nullable
 * @param draw_buf      the buffer to draw into, can be set later @nullable
 * @param color_format  the color format of the layer
 * @param area          the area of the layer (absolute coordinates)
 */
void lv_draw_layer_init_with_buf(lv_layer_t * layer, lv_layer_t * parent_layer, lv_draw_buf_t * draw_buf,
                                 lv_color_format_t color_format, const lv_area_t * area);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PRIVATE_H*/
