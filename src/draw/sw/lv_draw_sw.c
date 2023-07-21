/**
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"
#if LV_USE_DRAW_SW

#include "../../core/lv_refr.h"
#include "lv_draw_sw.h"
#include "../../disp/lv_disp_private.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_USE_OS
    static void render_thread_cb(void * ptr);
#endif

static void execute_drawing(lv_draw_sw_unit_t * u);

static int32_t lv_draw_sw_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

static void lv_draw_sw_buffer_copy(lv_layer_t * layer,
                                   void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                   void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area);

static void lv_draw_sw_buffer_convert(lv_layer_t * layer);

static void lv_draw_sw_buffer_clear(lv_layer_t * layer, const lv_area_t * a);

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_init(void)
{

#if LV_DRAW_SW_COMPLEX == 1
    lv_draw_sw_mask_init();
#endif

    uint32_t i;
    for(i = 0; i < LV_DRAW_SW_DRAW_UNIT_CNT; i++) {
        lv_draw_sw_unit_t * draw_sw_unit = lv_draw_create_unit(sizeof(lv_draw_sw_unit_t));
        draw_sw_unit->base_unit.dispatch = lv_draw_sw_dispatch;
        draw_sw_unit->idx = i;

#if LV_USE_OS
        lv_thread_init(&draw_sw_unit->thread, LV_THREAD_PRIO_HIGH, render_thread_cb, 8 * 1024, draw_sw_unit);
#endif
    }
}


lv_layer_t * lv_draw_sw_layer_init(lv_disp_t * disp)
{
    lv_layer_t * layer = lv_malloc(sizeof(lv_layer_t));
    LV_ASSERT_MALLOC(layer);
    lv_memzero(layer, sizeof(lv_layer_t));
    layer->color_format = disp->color_format;
    layer->buffer_copy = lv_draw_sw_buffer_copy;
    layer->buffer_convert = lv_draw_sw_buffer_convert;
    layer->buffer_clear = lv_draw_sw_buffer_clear;

    return layer;
}

void lv_draw_sw_layer_deinit(lv_disp_t * disp, lv_layer_t * layer)
{
    LV_UNUSED(disp);
    lv_memzero(layer, sizeof(lv_layer_t));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t lv_draw_sw_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_sw_unit_t * draw_sw_unit = (lv_draw_sw_unit_t *) draw_unit;

    /*Return immediately if it's busy with draw task*/
    if(draw_sw_unit->task_act) return 0;

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_next_available_task(layer, NULL);
    if(t == NULL) return -1;

    /*If the buffer of the layer is not allocated yet, allocate it now*/
    if(layer->buf == NULL) {
        uint32_t px_size = lv_color_format_get_size(layer->color_format);
        uint32_t layer_size_byte = lv_area_get_size(&layer->buf_area) * px_size;

        uint8_t * buf = lv_malloc(layer_size_byte);
        if(buf == NULL) {
            LV_LOG_WARN("Allocating %"LV_PRIu32" bytes of layer buffer failed. Try later", layer_size_byte);
            return -1;
        }
        LV_ASSERT_MALLOC(buf);
        lv_draw_add_used_layer_size(layer_size_byte < 1024 ? 1 : layer_size_byte >> 10);

        layer->buf = buf;

        if(lv_color_format_has_alpha(layer->color_format)) {
            layer->buffer_clear(layer, &layer->buf_area);
        }
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_sw_unit->base_unit.target_layer = layer;
    draw_sw_unit->base_unit.clip_area = &t->clip_area;
    draw_sw_unit->task_act = t;

#if LV_USE_OS
    /*Let the render thread work*/
    lv_thread_sync_signal(&draw_sw_unit->sync);
#else
    execute_drawing(draw_sw_unit);

    draw_sw_unit->task_act->state = LV_DRAW_TASK_STATE_READY;
    draw_sw_unit->task_act = NULL;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();

#endif
    return 1;
}

static void lv_draw_sw_buffer_copy(lv_layer_t * layer,
                                   void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                   void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    LV_UNUSED(layer);

    uint8_t px_size = lv_color_format_get_size(layer->color_format);
    uint8_t * dest_bufc =  dest_buf;
    uint8_t * src_bufc =  src_buf;

    /*Got the first pixel of each buffer*/
    dest_bufc += dest_stride * px_size * dest_area->y1;
    dest_bufc += dest_area->x1 * px_size;

    src_bufc += src_stride * px_size * src_area->y1;
    src_bufc += src_area->x1 * px_size;

    src_stride *= px_size;
    dest_stride *= px_size;

    uint32_t line_length = lv_area_get_width(dest_area) * px_size;
    lv_coord_t y;
    for(y = dest_area->y1; y <= dest_area->y2; y++) {
        lv_memcpy(dest_bufc, src_bufc, line_length);
        dest_bufc += dest_stride;
        src_bufc += src_stride;
    }
}

static void lv_draw_sw_buffer_convert(lv_layer_t * layer)
{
    LV_UNUSED(layer);
}

uint8_t * buf_tmp;

static void lv_draw_sw_buffer_clear(lv_layer_t * layer, const lv_area_t * a)
{
    uint8_t px_size = lv_color_format_get_size(layer->color_format);
    uint8_t * buf =  layer->buf;
    uint32_t dest_stride = lv_area_get_width(&layer->buf_area) * px_size;

    /*Go to the first pixel of each buffer*/
    buf += dest_stride * (a->y1 - layer->buf_area.y1);
    buf += (a->x1 - layer->buf_area.x1) * px_size;

    uint32_t line_length = lv_area_get_width(a) * px_size;
    lv_coord_t y;
    for(y = a->y1; y <= a->y2; y++) {
        lv_memzero(buf, line_length);
        buf += dest_stride;
    }
}

#if LV_USE_OS
static void render_thread_cb(void * ptr)
{
    lv_draw_sw_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);

    while(1) {
        while(u->task_act == NULL) {
            lv_thread_sync_wait(&u->sync);
        }

        execute_drawing(u);

        /*Cleaup*/
        u->task_act->state = LV_DRAW_TASK_STATE_READY;
        u->task_act = NULL;

        /*The draw unit is free now. Request a new dispatching as it can get a new task*/
        lv_draw_dispatch_request();
    }
}
#endif

static void execute_drawing(lv_draw_sw_unit_t * u)
{
    /*Render the draw task*/
    lv_draw_task_t * t = u->task_act;
    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_sw_fill((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_sw_border((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
            lv_draw_sw_box_shadow((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BG_IMG:
            lv_draw_sw_bg_img((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_sw_label((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_sw_img((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_sw_arc((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_sw_line((lv_draw_unit_t *)u, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_sw_triangle((lv_draw_unit_t *)u, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_sw_layer((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
            lv_draw_sw_mask_rect((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }

#if LV_USE_PARALLEL_DRAW_DEBUG
    /*Layers manage it for themselves*/
    if(t->type != LV_DRAW_TASK_TYPE_LAYER) {
        lv_area_t draw_area;
        if(!_lv_area_intersect(&draw_area, &t->area, u->base_unit.clip_area)) return;

        int32_t idx = 0;
        lv_disp_t * disp = _lv_refr_get_disp_refreshing();
        lv_draw_unit_t * draw_unit_tmp = disp->draw_unit_head;
        while(draw_unit_tmp != (lv_draw_unit_t *)u) {
            draw_unit_tmp = draw_unit_tmp->next;
            idx++;
        }
        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_palette_main(idx % _LV_PALETTE_LAST);
        rect_dsc.border_color = rect_dsc.bg_color;
        rect_dsc.bg_opa = LV_OPA_10;
        rect_dsc.border_opa = LV_OPA_80;
        rect_dsc.border_width = 1;
        lv_draw_sw_fill((lv_draw_unit_t *)u, &rect_dsc, &draw_area);

        lv_point_t txt_size;
        lv_txt_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

        lv_area_t txt_area;
        txt_area.x1 = draw_area.x1;
        txt_area.y1 = draw_area.y1;
        txt_area.x2 = draw_area.x1 + txt_size.x - 1;
        txt_area.y2 = draw_area.y1 + txt_size.y - 1;

        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_color_white();
        lv_draw_sw_fill((lv_draw_unit_t *)u, &rect_dsc, &txt_area);

        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d", idx);
        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_black();
        label_dsc.text = buf;
        lv_draw_sw_label((lv_draw_unit_t *)u, &label_dsc, &txt_area);
    }
#endif

}

#endif /*LV_USE_DRAW_SW*/
