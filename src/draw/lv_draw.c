/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"
#include "../misc/lv_area_private.h"
#include "../misc/lv_event_private.h"
#include "lv_draw_private.h"
#include "lv_draw_image_private.h"
#include "lv_draw_vector_private.h"
#include "../display/lv_display_private.h"
#include "../core/lv_global.h"
#include "../core/lv_refr_private.h"

/*********************
 *      DEFINES
 *********************/
#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_area_t * get_dsc_coords(lv_draw_dsc_t * dsc);
static void apply_coords_change(lv_draw_dsc_t * dsc, const lv_area_t * old_coords, const lv_area_t * new_coords);
static bool unit_can_draw(lv_draw_unit_t * draw_unit, lv_layer_t * layer, lv_draw_dsc_t * dsc);
static bool unit_can_draw_type(lv_draw_unit_t * draw_unit, lv_layer_t * layer, lv_draw_dsc_t * dsc);
static void unit_draw(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      lv_draw_dsc_t * dsc, const lv_area_t * coords);

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO
static inline uint32_t get_layer_size_kb(uint32_t size_byte)
{
    return (size_byte + 1023) >> 10;
}
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_init(void)
{
}

void lv_draw_deinit(void)
{
    lv_draw_unit_t * u = _draw_info.unit_head;
    while(u) {
        lv_draw_unit_t * cur_unit = u;
        u = u->next;

        if(cur_unit->delete_cb) cur_unit->delete_cb(cur_unit);
        lv_free(cur_unit);
    }
    _draw_info.unit_head = NULL;
}

void * lv_draw_create_unit(size_t size)
{
    lv_draw_unit_t * new_unit = lv_malloc_zeroed(size);
    LV_ASSERT_MALLOC(new_unit);
    new_unit->next = _draw_info.unit_head;

    _draw_info.unit_head = new_unit;
    _draw_info.unit_cnt++;

    new_unit->idx = _draw_info.unit_cnt;

    return new_unit;
}

void lv_draw_wait_for_finish(void)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_unit_t * u = _draw_info.unit_head;
    while(u) {
        if(u->wait_for_finish_cb) {
            LV_PROFILER_DRAW_BEGIN_TAG("wait_for_finish_cb");
            LV_PROFILER_DRAW_BEGIN_TAG(u->name);
            u->wait_for_finish_cb(u);
            LV_PROFILER_DRAW_END_TAG(u->name);
            LV_PROFILER_DRAW_END_TAG("wait_for_finish_cb");
        }
        u = u->next;
    }
    LV_PROFILER_DRAW_END;
}

uint32_t lv_draw_get_unit_count(void)
{
    return _draw_info.unit_cnt;
}

uint32_t lv_draw_unit_get_draw_count(const lv_draw_unit_t * unit, lv_draw_dsc_type_t type)
{
    LV_CHECK_ARG(unit != NULL, return 0);
    LV_CHECK_ARG(type < LV_DRAW_DSC_TYPE_COUNT, return 0);
#if LV_USE_DRAW_COUNTERS
    return unit->draw_count[type];
#else
    return 0;
#endif
}

uint32_t lv_draw_get_dropped_count(lv_draw_dsc_type_t type)
{
    LV_CHECK_ARG(type < LV_DRAW_DSC_TYPE_COUNT, return 0);
#if LV_USE_DRAW_COUNTERS
    return _draw_info.dropped_count[type];
#else
    return 0;
#endif
}

void lv_draw_reset_counters(void)
{
#if LV_USE_DRAW_COUNTERS
    lv_draw_unit_t * u = _draw_info.unit_head;
    while(u) {
        lv_memset(u->draw_count, 0, sizeof(u->draw_count));
        u = u->next;
    }

    lv_memset(_draw_info.dropped_count, 0, sizeof(_draw_info.dropped_count));
#endif
}

void lv_draw_dsc_submit(lv_draw_dsc_t * dsc, lv_draw_dsc_type_t type, lv_layer_t * layer, const lv_area_t * coords)
{
    LV_ASSERT(dsc != NULL);
    LV_ASSERT(layer != NULL);
    LV_ASSERT(coords != NULL);
    dsc->type = type;
    dsc->layer = layer;
    const lv_area_t clip = layer->_clip_area;

    lv_area_t * dsc_coords = get_dsc_coords(dsc);
    if(dsc_coords) {
        *dsc_coords = *coords;
    }

    bool send_events = !_draw_info.is_sending_event && dsc->obj && lv_obj_is_send_draw_dsc_events(dsc->obj);
    if(send_events) {
        _draw_info.is_sending_event = true;
        lv_result_t res = lv_obj_send_event(dsc->obj, LV_EVENT_DRAW_DSC_BEGIN, dsc);
        _draw_info.is_sending_event = false;
        if(res != LV_RESULT_OK) {
            /* object deleted during event*/
            return;
        }
    }

    const lv_area_t * draw_coords = coords;
    if(dsc_coords) {
        apply_coords_change(dsc, coords, dsc_coords);
        draw_coords = dsc_coords;
    }

    lv_draw_unit_t * main = _draw_info.unit_head;
    if(!main) {
        LV_LOG_ERROR("No draw unit registered");
        return;
    }

    /* we try to render with the main draw unit*/
    if(unit_can_draw(main, layer, dsc)) {
        unit_draw(main, layer, &clip, dsc, draw_coords);
    }
    else if(main->fallback && unit_can_draw(main->fallback, layer, dsc)) {
        /* if we can't, check with its fallback */

        /* before the fallback draws, ensure the pixels have been actually
         * written in case the main draw unit queues instead of drawing immediately */
        if(main->wait_for_finish_cb) {
            LV_PROFILER_DRAW_BEGIN_TAG("wait_for_finish_cb");
            LV_PROFILER_DRAW_BEGIN_TAG(main->name);
            main->wait_for_finish_cb(main);
            LV_PROFILER_DRAW_END_TAG(main->name);
            LV_PROFILER_DRAW_END_TAG("wait_for_finish_cb");
        }
        unit_draw(main->fallback, layer, &clip, dsc, draw_coords);
    }
    else {
        LV_LOG_WARN("Draw dropped (type %d): no draw unit can draw it", (int)type);
#if LV_USE_DRAW_COUNTERS
        _draw_info.dropped_count[type]++;
#endif /* LV_USE_DRAW_COUNTERS */
    }

    if(send_events) {
        _draw_info.is_sending_event = true;
        lv_obj_send_event(dsc->obj, LV_EVENT_DRAW_DSC_END, dsc);
        _draw_info.is_sending_event = false;
    }
}

lv_draw_unit_t * lv_draw_unit_get_next(lv_draw_unit_t * unit)
{
    if(!unit) {
        return _draw_info.unit_head;
    }
    return unit->next;
}

const char * lv_draw_unit_get_name(const lv_draw_unit_t * unit)
{
    LV_CHECK_ARG(unit != NULL, return NULL);
    return unit->name;
}

void lv_draw_unit_send_event(const char * name, lv_event_code_t code, void * param)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_event_t event = { 0 };
    event.code = code;
    event.param = param;
    lv_draw_unit_t * u = _draw_info.unit_head;
    while(u) {
        if(u->event_cb && (!name || lv_strcmp(name, u->name) == 0)) {
            event.current_target = event.original_target = u;
            LV_PROFILER_DRAW_BEGIN_TAG("event_cb");
            LV_PROFILER_DRAW_BEGIN_TAG(u->name);
            u->event_cb(&event);
            LV_PROFILER_DRAW_END_TAG(u->name);
            LV_PROFILER_DRAW_END_TAG("event_cb");
        }
        u = u->next;
    }

    LV_PROFILER_DRAW_END;
}

void lv_layer_init(lv_layer_t * layer)
{
    LV_CHECK_ARG(layer != NULL, return);
    lv_memzero(layer, sizeof(lv_layer_t));
    lv_layer_reset(layer);
}

void lv_layer_reset(lv_layer_t * layer)
{
    LV_CHECK_ARG(layer != NULL, return);
#if LV_DRAW_TRANSFORM_USE_MATRIX
    lv_matrix_identity(&layer->matrix);
#endif
    layer->opa = LV_OPA_COVER;
    layer->recolor = lv_color32_make(0, 0, 0, 0);
}

lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area)
{
    LV_CHECK_ARG(area != NULL, return NULL);

    LV_PROFILER_DRAW_BEGIN;
    lv_layer_t * new_layer = lv_malloc_zeroed(sizeof(lv_layer_t));
    LV_ASSERT_MALLOC(new_layer);
    if(new_layer == NULL) {
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    lv_result_t res = lv_draw_layer_init(new_layer, parent_layer, color_format, area);
    if(res != LV_RESULT_OK) {
        lv_free(new_layer);
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    /*Inherits transparency from parent*/
    if(parent_layer) {
        new_layer->opa = parent_layer->opa;
        new_layer->recolor = parent_layer->recolor;
    }

    LV_PROFILER_DRAW_END;
    return new_layer;
}

lv_result_t lv_draw_layer_init(lv_layer_t * layer, lv_layer_t * parent_layer, lv_color_format_t color_format,
                               const lv_area_t * area)
{
    LV_CHECK_ARG(layer != NULL, return LV_RESULT_INVALID);
    LV_CHECK_ARG(area != NULL, return LV_RESULT_INVALID);

    LV_PROFILER_DRAW_BEGIN;
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    const uint32_t layer_size_byte = h * lv_draw_buf_width_to_stride(w, color_format);

#if LV_DRAW_LAYER_MAX_MEMORY > 0
    /* Do not allocate the layer if the sum of allocated layer sizes
     * would exceed `LV_DRAW_LAYER_MAX_MEMORY` */
    if((_draw_info.used_memory_for_layers + layer_size_byte) > LV_DRAW_LAYER_MAX_MEMORY) {
        LV_LOG_WARN("LV_DRAW_LAYER_MAX_MEMORY was reached when allocating the layer.");
        LV_PROFILER_DRAW_END;
        return LV_RESULT_INVALID;
    }
#endif

    lv_draw_buf_t * draw_buf = lv_draw_buf_create(w, h, color_format, LV_STRIDE_AUTO);
    if(draw_buf == NULL) {
        LV_LOG_WARN("Allocating the layer buffer failed");
        LV_PROFILER_DRAW_END;
        return LV_RESULT_INVALID;
    }

    if(lv_color_format_has_alpha(color_format)) {
        lv_draw_buf_clear(draw_buf, NULL);
    }

    lv_draw_layer_init_with_buf(layer, parent_layer, draw_buf, color_format, area);
    layer->buffer_owned = true;

    _draw_info.used_memory_for_layers += layer_size_byte;
    LV_LOG_INFO("Layer memory used: %" LV_PRIu32 " kB", get_layer_size_kb(_draw_info.used_memory_for_layers));

    LV_PROFILER_DRAW_END;
    return LV_RESULT_OK;
}

void lv_draw_layer_init_with_buf(lv_layer_t * layer, lv_layer_t * parent_layer, lv_draw_buf_t * draw_buf,
                                 lv_color_format_t color_format, const lv_area_t * area)
{
    LV_CHECK_ARG(layer != NULL, return);
    LV_CHECK_ARG(area != NULL, return);

    lv_layer_init(layer);
    lv_display_t * disp = parent_layer ? parent_layer->display : lv_refr_get_disp_refreshing();

    layer->parent = parent_layer;
    layer->_clip_area = *area;
    layer->buf_area = *area;
    layer->phy_clip_area = *area;
    layer->color_format = color_format;
    layer->display = disp;
    layer->draw_buf = draw_buf;
    layer->buffer_owned = false;

    lv_draw_unit_send_event(NULL, LV_EVENT_CHILD_CREATED, layer);

    if(disp->layer_head) {
        lv_layer_t * tail = disp->layer_head;
        while(tail->next) tail = tail->next;
        tail->next = layer;
    }
    else {
        disp->layer_head = layer;
    }
}

void * lv_draw_layer_go_to_xy(lv_layer_t * layer, int32_t x, int32_t y)
{
    LV_CHECK_ARG(layer != NULL, return NULL);

    return lv_draw_buf_goto_xy(layer->draw_buf, x, y);
}

lv_draw_dsc_type_t lv_draw_dsc_get_type(const lv_draw_dsc_t * dsc)
{
    LV_CHECK_ARG(dsc != NULL, return LV_DRAW_DSC_TYPE_NONE);

    return dsc->type;
}


lv_layer_t * lv_draw_layer_create_drop_shadow(lv_layer_t * parent_layer, const lv_draw_dsc_t * base,
                                              const lv_area_t * area)
{
    LV_CHECK_ARG(parent_layer != NULL, return NULL);
    LV_CHECK_ARG(base != NULL, return NULL);
    LV_CHECK_ARG(area != NULL, return NULL);

    lv_area_t drop_shadow_area = *area;
    int32_t blur_radius = base->drop_shadow_blur_radius;

    /* x2 to have some extra space for cleaner blurring */
    lv_area_increase(&drop_shadow_area, blur_radius * 2, blur_radius * 2);

    lv_layer_t * ds_layer = lv_draw_layer_create(parent_layer, LV_COLOR_FORMAT_A8, &drop_shadow_area);
    if(ds_layer == NULL) {
        LV_LOG_WARN("Failed to create a layer for the drop shadow");
    }
    return ds_layer;
}

void lv_draw_layer_finish_drop_shadow(lv_layer_t * drop_shadow_layer, const lv_draw_dsc_t * base)
{
    LV_CHECK_ARG(drop_shadow_layer != NULL, return);
    LV_CHECK_ARG(base != NULL, return);

    lv_area_t drop_shadow_area = drop_shadow_layer->buf_area;
    lv_draw_blur_dsc_t blur_dsc;
    lv_draw_blur_dsc_init(&blur_dsc);
    blur_dsc.blur_radius = base->drop_shadow_blur_radius;
    blur_dsc.quality = base->drop_shadow_quality;
    /* Forward the shadow color so backends that bake the recolor into the
     * blur pass (e.g. NanoVG, where the subsequent layer composite cannot
     * easily apply recolor to a pre-existing FBO image) can pick it up. */
    blur_dsc.base.drop_shadow_color = base->drop_shadow_color;
    blur_dsc.base.drop_shadow_opa   = base->drop_shadow_opa;
    lv_draw_blur(drop_shadow_layer, &blur_dsc, &drop_shadow_layer->buf_area);

    lv_area_move(&drop_shadow_area, base->drop_shadow_ofs_x, base->drop_shadow_ofs_y);

    lv_draw_image_dsc_t layer_draw_dsc;
    lv_draw_image_dsc_init(&layer_draw_dsc);
    layer_draw_dsc.src = drop_shadow_layer;
    layer_draw_dsc.recolor = base->drop_shadow_color;
    layer_draw_dsc.opa = base->drop_shadow_opa;
    lv_draw_layer(drop_shadow_layer->parent, &layer_draw_dsc, &drop_shadow_area);
}

void lv_draw_layer_deinit(lv_layer_t * layer)
{
    if(!layer) {
        return;
    }

    if(layer->display) {
        lv_layer_t * l = layer->display->layer_head;
        while(l) {
            if(l->next == layer) {
                l->next = layer->next;
                break;
            }
            l = l->next;
        }
    }

    lv_draw_unit_send_event(NULL, LV_EVENT_CHILD_DELETED, layer);

    lv_draw_layer_dealloc_buf(layer);
}

void lv_draw_layer_delete(lv_layer_t * layer)
{
    if(!layer) {
        return;
    }
    lv_draw_layer_deinit(layer);
    lv_free(layer);
}

void lv_draw_layer_dealloc_buf(lv_layer_t * layer)
{
    LV_CHECK_ARG(layer != NULL, return);
    if(layer->draw_buf == NULL || !layer->buffer_owned) return;

    uint32_t layer_size_byte = lv_area_get_height(&layer->buf_area) * layer->draw_buf->header.stride;
    if(_draw_info.used_memory_for_layers >= layer_size_byte) {
        _draw_info.used_memory_for_layers -= layer_size_byte;
    }
    else {
        _draw_info.used_memory_for_layers = 0;
        LV_LOG_WARN("More layers were freed than allocated");
    }
    LV_LOG_INFO("Layer memory used: %" LV_PRIu32 " kB", get_layer_size_kb(_draw_info.used_memory_for_layers));

    lv_draw_buf_destroy(layer->draw_buf);
    layer->draw_buf = NULL;
    layer->buffer_owned = false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Keep the image area in line with the coords when they're changed (e.g. in `LV_EVENT_DRAW_DSC_BEGIN`):
 * if it was the same as the old coords, it becomes the new coords.
 */
static void apply_coords_change(lv_draw_dsc_t * dsc, const lv_area_t * old_coords, const lv_area_t * new_coords)
{
    if(lv_area_is_equal(old_coords, new_coords)) return;

    if(dsc->type == LV_DRAW_DSC_TYPE_IMAGE || dsc->type == LV_DRAW_DSC_TYPE_LAYER) {
        lv_draw_image_dsc_t * image_dsc = (lv_draw_image_dsc_t *)dsc;
        if(lv_area_is_equal(&image_dsc->image_area, old_coords)) {
            image_dsc->image_area = *new_coords;
        }
    }
}

/**
 * Get the area of a draw descriptor, for the types that are drawn to an area.
 * @param dsc       the draw descriptor, with its `type` set
 * @return          the `coords` of the draw descriptor, or NULL if its geometry is in other fields (e.g. lines)
 */
static lv_area_t * get_dsc_coords(lv_draw_dsc_t * dsc)
{
    LV_ASSERT(dsc != NULL);
    switch(dsc->type) {
        case LV_DRAW_DSC_TYPE_FILL:
            return &((lv_draw_fill_dsc_t *)dsc)->coords;
        case LV_DRAW_DSC_TYPE_BORDER:
            return &((lv_draw_border_dsc_t *)dsc)->coords;
        case LV_DRAW_DSC_TYPE_BOX_SHADOW:
            return &((lv_draw_box_shadow_dsc_t *)dsc)->coords;
        case LV_DRAW_DSC_TYPE_LETTER:
            return &((lv_draw_letter_dsc_t *)dsc)->coords;
        case LV_DRAW_DSC_TYPE_LABEL:
            return &((lv_draw_label_dsc_t *)dsc)->coords;
        case LV_DRAW_DSC_TYPE_IMAGE:
        case LV_DRAW_DSC_TYPE_LAYER:
            return &((lv_draw_image_dsc_t *)dsc)->coords;
        case LV_DRAW_DSC_TYPE_BLUR:
            return &((lv_draw_blur_dsc_t *)dsc)->coords;
#if LV_USE_3DTEXTURE
        case LV_DRAW_DSC_TYPE_3D:
            return &((lv_draw_3d_dsc_t *)dsc)->coords;
#endif
        case LV_DRAW_DSC_TYPE_NONE:
        case LV_DRAW_DSC_TYPE_COUNT:
        case LV_DRAW_DSC_TYPE_LINE:
        case LV_DRAW_DSC_TYPE_ARC:
        case LV_DRAW_DSC_TYPE_TRIANGLE:
        case LV_DRAW_DSC_TYPE_MASK_RECTANGLE:
#if LV_USE_VECTOR_GRAPHIC
        case LV_DRAW_DSC_TYPE_VECTOR:
#endif
            return NULL;
    }
    return NULL;
}

static bool unit_can_draw(lv_draw_unit_t * draw_unit, lv_layer_t * layer, lv_draw_dsc_t * dsc)
{
    LV_PROFILER_DRAW_BEGIN_TAG("can_draw");
    LV_PROFILER_DRAW_BEGIN_TAG(draw_unit->name);
    bool can_draw = unit_can_draw_type(draw_unit, layer, dsc);
    LV_PROFILER_DRAW_END_TAG(draw_unit->name);
    LV_PROFILER_DRAW_END_TAG("can_draw");
    return can_draw;
}

static bool unit_can_draw_type(lv_draw_unit_t * draw_unit, lv_layer_t * layer, lv_draw_dsc_t * dsc)
{
    const lv_draw_unit_draw_cbs_t * cbs = draw_unit->draw_cbs;
    if(cbs == NULL) return false;

    switch(dsc->type) {
        case LV_DRAW_DSC_TYPE_NONE:
        case LV_DRAW_DSC_TYPE_COUNT:
            return false;
        case LV_DRAW_DSC_TYPE_FILL:
            if(cbs->can_draw_fill_cb) return cbs->can_draw_fill_cb(draw_unit, layer, (lv_draw_fill_dsc_t *)dsc);
            return cbs->draw_fill_cb != NULL;
        case LV_DRAW_DSC_TYPE_BORDER:
            if(cbs->can_draw_border_cb) return cbs->can_draw_border_cb(draw_unit, layer, (lv_draw_border_dsc_t *)dsc);
            return cbs->draw_border_cb != NULL;
        case LV_DRAW_DSC_TYPE_BOX_SHADOW:
            if(cbs->can_draw_box_shadow_cb) return cbs->can_draw_box_shadow_cb(draw_unit, layer, (lv_draw_box_shadow_dsc_t *)dsc);
            return cbs->draw_box_shadow_cb != NULL;
        case LV_DRAW_DSC_TYPE_LETTER:
            if(cbs->can_draw_letter_cb) return cbs->can_draw_letter_cb(draw_unit, layer, (lv_draw_letter_dsc_t *)dsc);
            return cbs->draw_letter_cb != NULL;
        case LV_DRAW_DSC_TYPE_LABEL:
            if(cbs->can_draw_label_cb) return cbs->can_draw_label_cb(draw_unit, layer, (lv_draw_label_dsc_t *)dsc);
            return cbs->draw_label_cb != NULL;
        case LV_DRAW_DSC_TYPE_IMAGE:
            if(cbs->can_draw_image_cb) return cbs->can_draw_image_cb(draw_unit, layer, (lv_draw_image_dsc_t *)dsc);
            return cbs->draw_image_cb != NULL;
        case LV_DRAW_DSC_TYPE_LAYER:
            if(cbs->can_draw_layer_cb) return cbs->can_draw_layer_cb(draw_unit, layer, (lv_draw_image_dsc_t *)dsc);
            return cbs->draw_layer_cb != NULL;
        case LV_DRAW_DSC_TYPE_LINE:
            if(cbs->can_draw_line_cb) return cbs->can_draw_line_cb(draw_unit, layer, (lv_draw_line_dsc_t *)dsc);
            return cbs->draw_line_cb != NULL;
        case LV_DRAW_DSC_TYPE_ARC:
            if(cbs->can_draw_arc_cb) return cbs->can_draw_arc_cb(draw_unit, layer, (lv_draw_arc_dsc_t *)dsc);
            return cbs->draw_arc_cb != NULL;
        case LV_DRAW_DSC_TYPE_TRIANGLE:
            if(cbs->can_draw_triangle_cb) return cbs->can_draw_triangle_cb(draw_unit, layer, (lv_draw_triangle_dsc_t *)dsc);
            return cbs->draw_triangle_cb != NULL;
        case LV_DRAW_DSC_TYPE_MASK_RECTANGLE:
            if(cbs->can_draw_mask_rect_cb) return cbs->can_draw_mask_rect_cb(draw_unit, layer, (lv_draw_mask_rect_dsc_t *)dsc);
            return cbs->draw_mask_rect_cb != NULL;
        case LV_DRAW_DSC_TYPE_BLUR:
            if(cbs->can_draw_blur_cb) return cbs->can_draw_blur_cb(draw_unit, layer, (lv_draw_blur_dsc_t *)dsc);
            return cbs->draw_blur_cb != NULL;
#if LV_USE_VECTOR_GRAPHIC
        case LV_DRAW_DSC_TYPE_VECTOR:
            if(cbs->can_draw_vector_cb) return cbs->can_draw_vector_cb(draw_unit, layer, (lv_draw_vector_dsc_t *)dsc);
            return cbs->draw_vector_cb != NULL;
#endif
#if LV_USE_3DTEXTURE
        case LV_DRAW_DSC_TYPE_3D:
            if(cbs->can_draw_3d_cb) return cbs->can_draw_3d_cb(draw_unit, layer, (lv_draw_3d_dsc_t *) dsc);
            return cbs->draw_3d_cb != NULL;
#endif
    }

    return false;
}


static void unit_draw(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      lv_draw_dsc_t * dsc, const lv_area_t * coords)
{
    LV_ASSERT(draw_unit != NULL);
    const lv_draw_unit_draw_cbs_t * cbs = draw_unit->draw_cbs;
    LV_ASSERT(cbs != NULL);

#if LV_USE_DRAW_COUNTERS
    draw_unit->draw_count[dsc->type]++;
#endif /* LV_USE_DRAW_COUNTERS */

    LV_PROFILER_DRAW_BEGIN_TAG(draw_unit->name);
    switch(dsc->type) {
        case LV_DRAW_DSC_TYPE_NONE:
        case LV_DRAW_DSC_TYPE_COUNT:
            break;
        case LV_DRAW_DSC_TYPE_FILL:
            LV_ASSERT(cbs->draw_fill_cb != NULL);
            cbs->draw_fill_cb(draw_unit, layer, clip_area, (lv_draw_fill_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_BORDER:
            LV_ASSERT(cbs->draw_border_cb != NULL);
            cbs->draw_border_cb(draw_unit, layer, clip_area, (lv_draw_border_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_BOX_SHADOW:
            LV_ASSERT(cbs->draw_box_shadow_cb != NULL);
            cbs->draw_box_shadow_cb(draw_unit, layer, clip_area, (lv_draw_box_shadow_dsc_t *)dsc,
                                    coords);
            break;
        case LV_DRAW_DSC_TYPE_LETTER:
            LV_ASSERT(cbs->draw_letter_cb != NULL);
            cbs->draw_letter_cb(draw_unit, layer, clip_area, (lv_draw_letter_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_LABEL:
            LV_ASSERT(cbs->draw_label_cb != NULL);
            cbs->draw_label_cb(draw_unit, layer, clip_area, (lv_draw_label_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_IMAGE:
            LV_ASSERT(cbs->draw_image_cb != NULL);
            cbs->draw_image_cb(draw_unit, layer, clip_area, (lv_draw_image_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_LAYER:
            LV_ASSERT(cbs->draw_layer_cb != NULL);
            cbs->draw_layer_cb(draw_unit, layer, clip_area, (lv_draw_image_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_LINE:
            LV_ASSERT(cbs->draw_line_cb != NULL);
            cbs->draw_line_cb(draw_unit, layer, clip_area, (lv_draw_line_dsc_t *)dsc);
            break;
        case LV_DRAW_DSC_TYPE_ARC:
            LV_ASSERT(cbs->draw_arc_cb != NULL);
            cbs->draw_arc_cb(draw_unit, layer, clip_area, (lv_draw_arc_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_TRIANGLE:
            LV_ASSERT(cbs->draw_triangle_cb != NULL);
            cbs->draw_triangle_cb(draw_unit, layer, clip_area, (lv_draw_triangle_dsc_t *)dsc);
            break;
        case LV_DRAW_DSC_TYPE_MASK_RECTANGLE:
            LV_ASSERT(cbs->draw_mask_rect_cb != NULL);
            cbs->draw_mask_rect_cb(draw_unit, layer, clip_area, (lv_draw_mask_rect_dsc_t *)dsc);
            break;
        case LV_DRAW_DSC_TYPE_BLUR:
            LV_ASSERT(cbs->draw_blur_cb != NULL);
            cbs->draw_blur_cb(draw_unit, layer, clip_area, (lv_draw_blur_dsc_t *)dsc, coords);
            break;
#if LV_USE_VECTOR_GRAPHIC
        case LV_DRAW_DSC_TYPE_VECTOR:
            LV_ASSERT(cbs->draw_vector_cb != NULL);
            cbs->draw_vector_cb(draw_unit, layer, clip_area, (lv_draw_vector_dsc_t *)dsc);
            break;
#endif
#if LV_USE_3DTEXTURE
        case LV_DRAW_DSC_TYPE_3D:
            LV_ASSERT(cbs->draw_3d_cb != NULL);
            cbs->draw_3d_cb(draw_unit, layer, clip_area, (lv_draw_3d_dsc_t *)dsc, coords);
            break;
#endif
    }
    LV_PROFILER_DRAW_END_TAG(draw_unit->name);
}
