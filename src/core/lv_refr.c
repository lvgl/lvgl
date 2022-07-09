/**
 * @file lv_refr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_refr.h"
#include "lv_disp.h"
#include "../hal/lv_hal_tick.h"
#include "../hal/lv_hal_disp.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_mem.h"
#include "../misc/lv_math.h"
#include "../misc/lv_gc.h"
#include "../draw/lv_draw.h"
#include "../font/lv_font_fmt_txt.h"
#include "../extra/others/snapshot/lv_snapshot.h"

#if LV_USE_PERF_MONITOR || LV_USE_MEM_MONITOR
    #include "../widgets/lv_label.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint32_t    perf_last_time;
    uint32_t    elaps_sum;
    uint32_t    frame_cnt;
    uint32_t    fps_sum_cnt;
    uint32_t    fps_sum_all;
#if LV_USE_LABEL
    lv_obj_t  * perf_label;
#endif
} perf_monitor_t;

typedef struct {
    uint32_t     mem_last_time;
#if LV_USE_LABEL
    lv_obj_t  *  mem_label;
#endif
} mem_monitor_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_refr_join_area(void);
static void refr_invalid_areas(void);
static void refr_area(const lv_area_t * area_p);
static void refr_area_part(lv_draw_ctx_t * draw_ctx);
static lv_obj_t * lv_refr_get_top_obj(const lv_area_t * area_p, lv_obj_t * obj);
static void refr_obj_and_children(lv_draw_ctx_t * draw_ctx, lv_obj_t * top_obj);
static void refr_obj(lv_draw_ctx_t * draw_ctx, lv_obj_t * obj);
static uint32_t get_max_row(lv_disp_t * disp, lv_coord_t area_w, lv_coord_t area_h);
static void draw_buf_flush(lv_disp_t * disp);
static void call_flush_cb(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

#if LV_USE_PERF_MONITOR
    static void perf_monitor_init(perf_monitor_t * perf_monitor);
#endif
#if LV_USE_MEM_MONITOR
    static void mem_monitor_init(mem_monitor_t * mem_monitor);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t px_num;
static lv_disp_t * disp_refr; /*Display being refreshed*/

#if LV_USE_PERF_MONITOR
    static perf_monitor_t   perf_monitor;
#endif

#if LV_USE_MEM_MONITOR
    static mem_monitor_t    mem_monitor;
#endif

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_DISP_REFR
    #define REFR_TRACE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define REFR_TRACE(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the screen refresh subsystem
 */
void _lv_refr_init(void)
{
#if LV_USE_PERF_MONITOR
    perf_monitor_init(&perf_monitor);
#endif
#if LV_USE_MEM_MONITOR
    mem_monitor_init(&mem_monitor);
#endif
}

void lv_refr_now(lv_disp_t * disp)
{
    lv_anim_refr_now();

    if(disp) {
        if(disp->refr_timer) _lv_disp_refr_timer(disp->refr_timer);
    }
    else {
        lv_disp_t * d;
        d = lv_disp_get_next(NULL);
        while(d) {
            if(d->refr_timer) _lv_disp_refr_timer(d->refr_timer);
            d = lv_disp_get_next(d);
        }
    }
}

void lv_obj_redraw(lv_draw_ctx_t * draw_ctx, lv_obj_t * obj)
{
    const lv_area_t * clip_area_ori = draw_ctx->clip_area;
    lv_area_t clip_coords_for_obj;

    /*Truncate the clip area to `obj size + ext size` area*/
    lv_area_t obj_coords_ext;
    lv_obj_get_coords(obj, &obj_coords_ext);
    lv_coord_t ext_draw_size = _lv_obj_get_ext_draw_size(obj);
    lv_area_increase(&obj_coords_ext, ext_draw_size, ext_draw_size);
    bool com_clip_res = _lv_area_intersect(&clip_coords_for_obj, clip_area_ori, &obj_coords_ext);
    /*If the object is visible on the current clip area OR has overflow visible draw it.
     *With overflow visible drawing should happen to apply the masks which might affect children */
    bool should_draw = com_clip_res || lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    if(should_draw) {
        draw_ctx->clip_area = &clip_coords_for_obj;

        lv_event_send(obj, LV_EVENT_DRAW_MAIN_BEGIN, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_MAIN, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_MAIN_END, draw_ctx);
#if LV_USE_REFR_DEBUG
        lv_color_t debug_color = lv_color_make(lv_rand(0, 0xFF), lv_rand(0, 0xFF), lv_rand(0, 0xFF));
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_color.full = debug_color.full;
        draw_dsc.bg_opa = LV_OPA_20;
        draw_dsc.border_width = 1;
        draw_dsc.border_opa = LV_OPA_30;
        draw_dsc.border_color = debug_color;
        lv_draw_rect(draw_ctx, &draw_dsc, &obj_coords_ext);
#endif
    }

    /*With overflow visible keep the previous clip area to let the children visible out of this object too
     *With not overflow visible limit the clip are to the object's coordinates to clip the children*/
    lv_area_t clip_coords_for_children;
    bool refr_children = true;
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE)) {
        clip_coords_for_children  = *clip_area_ori;
    }
    else {
        if(!_lv_area_intersect(&clip_coords_for_children, clip_area_ori, &obj->coords)) {
            refr_children = false;
        }
    }

    if(refr_children) {
        draw_ctx->clip_area = &clip_coords_for_children;
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            refr_obj(draw_ctx, child);
        }
    }

    /*If the object was visible on the clip area call the post draw events too*/
    if(should_draw) {
        draw_ctx->clip_area = &clip_coords_for_obj;

        /*If all the children are redrawn make 'post draw' draw*/
        lv_event_send(obj, LV_EVENT_DRAW_POST_BEGIN, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_POST, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_POST_END, draw_ctx);
    }

    draw_ctx->clip_area = clip_area_ori;
}


/**
 * Invalidate an area on display to redraw it
 * @param area_p pointer to area which should be invalidated (NULL: delete the invalidated areas)
 * @param disp pointer to display where the area should be invalidated (NULL can be used if there is
 * only one display)
 */
void _lv_inv_area(lv_disp_t * disp, const lv_area_t * area_p)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) return;
    if(!lv_disp_is_invalidation_enabled(disp)) return;

    if(disp->rendering_in_progress) {
        LV_LOG_ERROR("detected modifying dirty areas in render");
        return;
    }

    /*Clear the invalidate buffer if the parameter is NULL*/
    if(area_p == NULL) {
        disp->inv_p = 0;
        return;
    }

    lv_area_t scr_area;
    scr_area.x1 = 0;
    scr_area.y1 = 0;
    scr_area.x2 = lv_disp_get_hor_res(disp) - 1;
    scr_area.y2 = lv_disp_get_ver_res(disp) - 1;

    lv_area_t com_area;
    bool suc;

    suc = _lv_area_intersect(&com_area, area_p, &scr_area);
    if(suc == false)  return; /*Out of the screen*/

    /*If there were at least 1 invalid area in full refresh mode, redraw the whole screen*/
    if(disp->driver->full_refresh) {
        disp->inv_areas[0] = scr_area;
        disp->inv_p = 1;
        if(disp->refr_timer) lv_timer_resume(disp->refr_timer);
        return;
    }

    if(disp->driver->rounder_cb) disp->driver->rounder_cb(disp->driver, &com_area);

    /*Save only if this area is not in one of the saved areas*/
    uint16_t i;
    for(i = 0; i < disp->inv_p; i++) {
        if(_lv_area_is_in(&com_area, &disp->inv_areas[i], 0) != false) return;
    }

    /*Save the area*/
    if(disp->inv_p < LV_INV_BUF_SIZE) {
        lv_area_copy(&disp->inv_areas[disp->inv_p], &com_area);
    }
    else {   /*If no place for the area add the screen*/
        disp->inv_p = 0;
        lv_area_copy(&disp->inv_areas[disp->inv_p], &scr_area);
    }
    disp->inv_p++;
    if(disp->refr_timer) lv_timer_resume(disp->refr_timer);
}

/**
 * Get the display which is being refreshed
 * @return the display being refreshed
 */
lv_disp_t * _lv_refr_get_disp_refreshing(void)
{
    return disp_refr;
}

/**
 * Set the display which is being refreshed.
 * It shouldn't be used directly by the user.
 * It can be used to trick the drawing functions about there is an active display.
 * @param the display being refreshed
 */
void _lv_refr_set_disp_refreshing(lv_disp_t * disp)
{
    disp_refr = disp;
}

/**
 * Called periodically to handle the refreshing
 * @param tmr pointer to the timer itself
 */
void _lv_disp_refr_timer(lv_timer_t * tmr)
{
    REFR_TRACE("begin");

    uint32_t start = lv_tick_get();
    volatile uint32_t elaps = 0;

    if(tmr) {
        disp_refr = tmr->user_data;
#if LV_USE_PERF_MONITOR == 0 && LV_USE_MEM_MONITOR == 0
        /**
         * Ensure the timer does not run again automatically.
         * This is done before refreshing in case refreshing invalidates something else.
         */
        lv_timer_pause(tmr);
#endif
    }
    else {
        disp_refr = lv_disp_get_default();
    }

    /*Refresh the screen's layout if required*/
    lv_obj_update_layout(disp_refr->act_scr);
    if(disp_refr->prev_scr) lv_obj_update_layout(disp_refr->prev_scr);

    lv_obj_update_layout(disp_refr->top_layer);
    lv_obj_update_layout(disp_refr->sys_layer);

    /*Do nothing if there is no active screen*/
    if(disp_refr->act_scr == NULL) {
        disp_refr->inv_p = 0;
        LV_LOG_WARN("there is no active screen");
        REFR_TRACE("finished");
        return;
    }

    lv_refr_join_area();

    refr_invalid_areas();


    /*If refresh happened ...*/
    if(disp_refr->inv_p != 0) {
        if(disp_refr->driver->full_refresh) {
            lv_area_t disp_area;
            lv_area_set(&disp_area, 0, 0, lv_disp_get_hor_res(disp_refr) - 1, lv_disp_get_ver_res(disp_refr) - 1);
            disp_refr->driver->draw_ctx->buf_area = &disp_area;
            draw_buf_flush(disp_refr);
        }

        /*Clean up*/
        lv_memset_00(disp_refr->inv_areas, sizeof(disp_refr->inv_areas));
        lv_memset_00(disp_refr->inv_area_joined, sizeof(disp_refr->inv_area_joined));
        disp_refr->inv_p = 0;

        elaps = lv_tick_elaps(start);

        /*Call monitor cb if present*/
        if(disp_refr->driver->monitor_cb) {
            disp_refr->driver->monitor_cb(disp_refr->driver, elaps, px_num);
        }
    }

    lv_mem_buf_free_all();
    _lv_font_clean_up_fmt_txt();

#if LV_DRAW_COMPLEX
    _lv_draw_mask_cleanup();
#endif

#if LV_USE_PERF_MONITOR && LV_USE_LABEL
    lv_obj_t * perf_label = perf_monitor.perf_label;
    if(perf_label == NULL) {
        perf_label = lv_label_create(lv_layer_sys());
        lv_obj_set_style_bg_opa(perf_label, LV_OPA_50, 0);
        lv_obj_set_style_bg_color(perf_label, lv_color_black(), 0);
        lv_obj_set_style_text_color(perf_label, lv_color_white(), 0);
        lv_obj_set_style_pad_top(perf_label, 3, 0);
        lv_obj_set_style_pad_bottom(perf_label, 3, 0);
        lv_obj_set_style_pad_left(perf_label, 3, 0);
        lv_obj_set_style_pad_right(perf_label, 3, 0);
        lv_obj_set_style_text_align(perf_label, LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_set_text(perf_label, "?");
        lv_obj_align(perf_label, LV_USE_PERF_MONITOR_POS, 0, 0);
        perf_monitor.perf_label = perf_label;
    }

    if(lv_tick_elaps(perf_monitor.perf_last_time) < 300) {
        if(px_num > 5000) {
            perf_monitor.elaps_sum += elaps;
            perf_monitor.frame_cnt ++;
        }
    }
    else {
        perf_monitor.perf_last_time = lv_tick_get();
        uint32_t fps_limit;
        uint32_t fps;

        if(disp_refr->refr_timer) {
            fps_limit = 1000 / disp_refr->refr_timer->period;
        }
        else {
            fps_limit = 1000 / LV_DISP_DEF_REFR_PERIOD;
        }

        if(perf_monitor.elaps_sum == 0) {
            perf_monitor.elaps_sum = 1;
        }
        if(perf_monitor.frame_cnt == 0) {
            fps = fps_limit;
        }
        else {
            fps = (1000 * perf_monitor.frame_cnt) / perf_monitor.elaps_sum;
        }
        perf_monitor.elaps_sum = 0;
        perf_monitor.frame_cnt = 0;
        if(fps > fps_limit) {
            fps = fps_limit;
        }

        perf_monitor.fps_sum_all += fps;
        perf_monitor.fps_sum_cnt ++;
        uint32_t cpu = 100 - lv_timer_get_idle();
        lv_label_set_text_fmt(perf_label, "%"LV_PRIu32" FPS\n%"LV_PRIu32"%% CPU", fps, cpu);
    }
#endif

#if LV_USE_MEM_MONITOR && LV_MEM_CUSTOM == 0 && LV_USE_LABEL
    lv_obj_t * mem_label = mem_monitor.mem_label;
    if(mem_label == NULL) {
        mem_label = lv_label_create(lv_layer_sys());
        lv_obj_set_style_bg_opa(mem_label, LV_OPA_50, 0);
        lv_obj_set_style_bg_color(mem_label, lv_color_black(), 0);
        lv_obj_set_style_text_color(mem_label, lv_color_white(), 0);
        lv_obj_set_style_pad_top(mem_label, 3, 0);
        lv_obj_set_style_pad_bottom(mem_label, 3, 0);
        lv_obj_set_style_pad_left(mem_label, 3, 0);
        lv_obj_set_style_pad_right(mem_label, 3, 0);
        lv_label_set_text(mem_label, "?");
        lv_obj_align(mem_label, LV_USE_MEM_MONITOR_POS, 0, 0);
        mem_monitor.mem_label = mem_label;
    }

    if(lv_tick_elaps(mem_monitor.mem_last_time) > 300) {
        mem_monitor.mem_last_time = lv_tick_get();
        lv_mem_monitor_t mon;
        lv_mem_monitor(&mon);
        uint32_t used_size = mon.total_size - mon.free_size;;
        uint32_t used_kb = used_size / 1024;
        uint32_t used_kb_tenth = (used_size - (used_kb * 1024)) / 102;
        lv_label_set_text_fmt(mem_label,
                              "%"LV_PRIu32 ".%"LV_PRIu32 " kB used (%d %%)\n"
                              "%d%% frag.",
                              used_kb, used_kb_tenth, mon.used_pct,
                              mon.frag_pct);
    }
#endif

    REFR_TRACE("finished");
}

#if LV_USE_PERF_MONITOR
void lv_refr_reset_fps_counter(void)
{
    perf_monitor.fps_sum_all = 0;
    perf_monitor.fps_sum_cnt = 0;
}

uint32_t lv_refr_get_fps_avg(void)
{
    if(perf_monitor.fps_sum_cnt == 0) {
        return 0;
    }
    return perf_monitor.fps_sum_all / perf_monitor.fps_sum_cnt;
}
#endif


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Join the areas which has got common parts
 */
static void lv_refr_join_area(void)
{
    uint32_t join_from;
    uint32_t join_in;
    lv_area_t joined_area;
    for(join_in = 0; join_in < disp_refr->inv_p; join_in++) {
        if(disp_refr->inv_area_joined[join_in] != 0) continue;

        /*Check all areas to join them in 'join_in'*/
        for(join_from = 0; join_from < disp_refr->inv_p; join_from++) {
            /*Handle only unjoined areas and ignore itself*/
            if(disp_refr->inv_area_joined[join_from] != 0 || join_in == join_from) {
                continue;
            }

            /*Check if the areas are on each other*/
            if(_lv_area_is_on(&disp_refr->inv_areas[join_in], &disp_refr->inv_areas[join_from]) == false) {
                continue;
            }

            _lv_area_join(&joined_area, &disp_refr->inv_areas[join_in], &disp_refr->inv_areas[join_from]);

            /*Join two area only if the joined area size is smaller*/
            if(lv_area_get_size(&joined_area) < (lv_area_get_size(&disp_refr->inv_areas[join_in]) +
                                                 lv_area_get_size(&disp_refr->inv_areas[join_from]))) {
                lv_area_copy(&disp_refr->inv_areas[join_in], &joined_area);

                /*Mark 'join_form' is joined into 'join_in'*/
                disp_refr->inv_area_joined[join_from] = 1;
            }
        }
    }
}

/**
 * Refresh the joined areas
 */
static void refr_invalid_areas(void)
{
    px_num = 0;

    if(disp_refr->inv_p == 0) return;

    /*Find the last area which will be drawn*/
    int32_t i;
    int32_t last_i = 0;
    for(i = disp_refr->inv_p - 1; i >= 0; i--) {
        if(disp_refr->inv_area_joined[i] == 0) {
            last_i = i;
            break;
        }
    }

    /*Notify the display driven rendering has started*/
    if(disp_refr->driver->render_start_cb) {
        disp_refr->driver->render_start_cb(disp_refr->driver);
    }

    disp_refr->driver->draw_buf->last_area = 0;
    disp_refr->driver->draw_buf->last_part = 0;
    disp_refr->rendering_in_progress = true;

    for(i = 0; i < disp_refr->inv_p; i++) {
        /*Refresh the unjoined areas*/
        if(disp_refr->inv_area_joined[i] == 0) {

            if(i == last_i) disp_refr->driver->draw_buf->last_area = 1;
            disp_refr->driver->draw_buf->last_part = 0;
            refr_area(&disp_refr->inv_areas[i]);

            px_num += lv_area_get_size(&disp_refr->inv_areas[i]);
        }
    }

    disp_refr->rendering_in_progress = false;
}

/**
 * Refresh an area if there is Virtual Display Buffer
 * @param area_p  pointer to an area to refresh
 */
static void refr_area(const lv_area_t * area_p)
{
    lv_draw_ctx_t * draw_ctx = disp_refr->driver->draw_ctx;
    draw_ctx->buf = disp_refr->driver->draw_buf->buf_act;

    /*With full refresh just redraw directly into the buffer*/
    /*In direct mode draw directly on the absolute coordinates of the buffer*/
    if(disp_refr->driver->full_refresh || disp_refr->driver->direct_mode) {
        lv_area_t disp_area;
        lv_area_set(&disp_area, 0, 0, lv_disp_get_hor_res(disp_refr) - 1, lv_disp_get_ver_res(disp_refr) - 1);
        draw_ctx->buf_area = &disp_area;

        if(disp_refr->driver->full_refresh) {
            disp_refr->driver->draw_buf->last_part = 1;
            draw_ctx->clip_area = &disp_area;
            refr_area_part(draw_ctx);
        }
        else {
            disp_refr->driver->draw_buf->last_part = disp_refr->driver->draw_buf->last_area;
            draw_ctx->clip_area = area_p;
            refr_area_part(draw_ctx);
        }
        return;
    }

    /*Normal refresh: draw the area in parts*/
    /*Calculate the max row num*/
    lv_coord_t w = lv_area_get_width(area_p);
    lv_coord_t h = lv_area_get_height(area_p);
    lv_coord_t y2 = area_p->y2 >= lv_disp_get_ver_res(disp_refr) ?
                    lv_disp_get_ver_res(disp_refr) - 1 : area_p->y2;

    int32_t max_row = get_max_row(disp_refr, w, h);

    lv_coord_t row;
    lv_coord_t row_last = 0;
    lv_area_t sub_area;
    for(row = area_p->y1; row + max_row - 1 <= y2; row += max_row) {
        /*Calc. the next y coordinates of draw_buf*/
        sub_area.x1 = area_p->x1;
        sub_area.x2 = area_p->x2;
        sub_area.y1 = row;
        sub_area.y2 = row + max_row - 1;
        draw_ctx->buf_area = &sub_area;
        draw_ctx->clip_area = &sub_area;
        draw_ctx->buf = disp_refr->driver->draw_buf->buf_act;
        if(sub_area.y2 > y2) sub_area.y2 = y2;
        row_last = sub_area.y2;
        if(y2 == row_last) disp_refr->driver->draw_buf->last_part = 1;
        refr_area_part(draw_ctx);
    }

    /*If the last y coordinates are not handled yet ...*/
    if(y2 != row_last) {
        /*Calc. the next y coordinates of draw_buf*/
        sub_area.x1 = area_p->x1;
        sub_area.x2 = area_p->x2;
        sub_area.y1 = row;
        sub_area.y2 = y2;
        draw_ctx->buf_area = &sub_area;
        draw_ctx->clip_area = &sub_area;
        draw_ctx->buf = disp_refr->driver->draw_buf->buf_act;
        disp_refr->driver->draw_buf->last_part = 1;
        refr_area_part(draw_ctx);
    }
}

static void refr_area_part(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp_refr);

    /* Below the `area_p` area will be redrawn into the draw buffer.
     * In single buffered mode wait here until the buffer is freed.*/
    if(draw_buf->buf1 && !draw_buf->buf2) {
        while(draw_buf->flushing) {
            if(disp_refr->driver->wait_cb) disp_refr->driver->wait_cb(disp_refr->driver);
        }

        /*If the screen is transparent initialize it when the flushing is ready*/
#if LV_COLOR_SCREEN_TRANSP
        if(disp_refr->driver->screen_transp) {
            if(disp_refr->driver->clear_cb) {
                disp_refr->driver->clear_cb(disp_refr->driver, disp_refr->driver->draw_buf->buf_act, disp_refr->driver->draw_buf->size);
            }
            else {
                lv_memset_00(disp_refr->driver->draw_buf->buf_act, disp_refr->driver->draw_buf->size * LV_IMG_PX_SIZE_ALPHA_BYTE);
            }
        }
#endif
    }

    lv_obj_t * top_act_scr = NULL;
    lv_obj_t * top_prev_scr = NULL;

    /*Get the most top object which is not covered by others*/
    top_act_scr = lv_refr_get_top_obj(draw_ctx->buf_area, lv_disp_get_scr_act(disp_refr));
    if(disp_refr->prev_scr) {
        top_prev_scr = lv_refr_get_top_obj(draw_ctx->buf_area, disp_refr->prev_scr);
    }

    /*Draw a display background if there is no top object*/
    if(top_act_scr == NULL && top_prev_scr == NULL) {
        lv_area_t a;
        lv_area_set(&a, 0, 0,
                    lv_disp_get_hor_res(disp_refr) - 1, lv_disp_get_ver_res(disp_refr) - 1);
        if(draw_ctx->draw_bg) {
            lv_draw_rect_dsc_t dsc;
            lv_draw_rect_dsc_init(&dsc);
            dsc.bg_img_src = disp_refr->bg_img;
            dsc.bg_img_opa = disp_refr->bg_opa;
            dsc.bg_color = disp_refr->bg_color;
            dsc.bg_opa = disp_refr->bg_opa;
            draw_ctx->draw_bg(draw_ctx, &dsc, &a);
        }
        else if(disp_refr->bg_img) {
            lv_img_header_t header;
            lv_res_t res = lv_img_decoder_get_info(disp_refr->bg_img, &header);
            if(res == LV_RES_OK) {
                lv_draw_img_dsc_t dsc;
                lv_draw_img_dsc_init(&dsc);
                dsc.opa = disp_refr->bg_opa;
                lv_draw_img(draw_ctx, &dsc, &a, disp_refr->bg_img);
            }
            else {
                LV_LOG_WARN("Can't draw the background image");
            }
        }
        else {
            lv_draw_rect_dsc_t dsc;
            lv_draw_rect_dsc_init(&dsc);
            dsc.bg_color = disp_refr->bg_color;
            dsc.bg_opa = disp_refr->bg_opa;
            lv_draw_rect(draw_ctx, &dsc, draw_ctx->buf_area);
        }
    }

    if(disp_refr->draw_prev_over_act) {
        if(top_act_scr == NULL) top_act_scr = disp_refr->act_scr;
        refr_obj_and_children(draw_ctx, top_act_scr);

        /*Refresh the previous screen if any*/
        if(disp_refr->prev_scr) {
            if(top_prev_scr == NULL) top_prev_scr = disp_refr->prev_scr;
            refr_obj_and_children(draw_ctx, top_prev_scr);
        }
    }
    else {
        /*Refresh the previous screen if any*/
        if(disp_refr->prev_scr) {
            if(top_prev_scr == NULL) top_prev_scr = disp_refr->prev_scr;
            refr_obj_and_children(draw_ctx, top_prev_scr);
        }

        if(top_act_scr == NULL) top_act_scr = disp_refr->act_scr;
        refr_obj_and_children(draw_ctx, top_act_scr);
    }

    /*Also refresh top and sys layer unconditionally*/
    refr_obj_and_children(draw_ctx, lv_disp_get_layer_top(disp_refr));
    refr_obj_and_children(draw_ctx, lv_disp_get_layer_sys(disp_refr));

    /*In true double buffered mode flush only once when all areas were rendered.
     *In normal mode flush after every area*/
    if(disp_refr->driver->full_refresh == false) {
        draw_buf_flush(disp_refr);
    }
}

/**
 * Search the most top object which fully covers an area
 * @param area_p pointer to an area
 * @param obj the first object to start the searching (typically a screen)
 * @return
 */
static lv_obj_t * lv_refr_get_top_obj(const lv_area_t * area_p, lv_obj_t * obj)
{
    lv_obj_t * found_p = NULL;

    if(_lv_area_is_in(area_p, &obj->coords, 0) == false) return NULL;
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return NULL;
    if(_lv_obj_get_layer_type(obj) != LV_LAYER_TYPE_NONE) return NULL;

    /*If this object is fully cover the draw area then check the children too*/
    lv_cover_check_info_t info;
    info.res = LV_COVER_RES_COVER;
    info.area = area_p;
    lv_event_send(obj, LV_EVENT_COVER_CHECK, &info);
    if(info.res == LV_COVER_RES_MASKED) return NULL;

    int32_t i;
    int32_t child_cnt = lv_obj_get_child_cnt(obj);
    for(i = child_cnt - 1; i >= 0; i--) {
        lv_obj_t * child = obj->spec_attr->children[i];
        found_p = lv_refr_get_top_obj(area_p, child);

        /*If a children is ok then break*/
        if(found_p != NULL) {
            break;
        }
    }

    /*If no better children use this object*/
    if(found_p == NULL && info.res == LV_COVER_RES_COVER) {
        found_p = obj;
    }

    return found_p;
}

/**
 * Make the refreshing from an object. Draw all its children and the youngers too.
 * @param top_p pointer to an objects. Start the drawing from it.
 * @param mask_p pointer to an area, the objects will be drawn only here
 */
static void refr_obj_and_children(lv_draw_ctx_t * draw_ctx, lv_obj_t * top_obj)
{
    /*Normally always will be a top_obj (at least the screen)
     *but in special cases (e.g. if the screen has alpha) it won't.
     *In this case use the screen directly*/
    if(top_obj == NULL) top_obj = lv_disp_get_scr_act(disp_refr);
    if(top_obj == NULL) return;  /*Shouldn't happen*/

    /*Refresh the top object and its children*/
    refr_obj(draw_ctx, top_obj);

    /*Draw the 'younger' sibling objects because they can be on top_obj*/
    lv_obj_t * parent;
    lv_obj_t * border_p = top_obj;

    parent = lv_obj_get_parent(top_obj);

    /*Do until not reach the screen*/
    while(parent != NULL) {
        bool go = false;
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(parent);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = parent->spec_attr->children[i];
            if(!go) {
                if(child == border_p) go = true;
            }
            else {
                /*Refresh the objects*/
                refr_obj(draw_ctx, child);
            }
        }

        /*Call the post draw draw function of the parents of the to object*/
        lv_event_send(parent, LV_EVENT_DRAW_POST_BEGIN, (void *)draw_ctx);
        lv_event_send(parent, LV_EVENT_DRAW_POST, (void *)draw_ctx);
        lv_event_send(parent, LV_EVENT_DRAW_POST_END, (void *)draw_ctx);

        /*The new border will be the last parents,
         *so the 'younger' brothers of parent will be refreshed*/
        border_p = parent;
        /*Go a level deeper*/
        parent = lv_obj_get_parent(parent);
    }
}


static lv_res_t layer_get_area(lv_draw_ctx_t * draw_ctx, lv_obj_t * obj, lv_layer_type_t layer_type,
                               lv_area_t * layer_area_out)
{
    lv_coord_t ext_draw_size = _lv_obj_get_ext_draw_size(obj);
    lv_area_t obj_coords_ext;
    lv_obj_get_coords(obj, &obj_coords_ext);
    lv_area_increase(&obj_coords_ext, ext_draw_size, ext_draw_size);

    if(layer_type == LV_LAYER_TYPE_TRANSFORM) {
        /*Get the transformed area and clip it to the current clip area.
         *This area needs to be updated on the screen.*/
        lv_area_t clip_coords_for_obj;
        lv_area_t tranf_coords = obj_coords_ext;
        lv_obj_get_transformed_area(obj, &tranf_coords, false, false);
        if(!_lv_area_intersect(&clip_coords_for_obj, draw_ctx->clip_area, &tranf_coords)) {
            return LV_RES_INV;
        }

        /*Transform back (inverse) the transformed area.
         *It will tell which area of the non-transformed widget needs to be redrawn
         *in order to cover transformed area after transformation.*/
        lv_area_t inverse_clip_coords_for_obj = clip_coords_for_obj;
        lv_obj_get_transformed_area(obj, &inverse_clip_coords_for_obj, false, true);
        if(!_lv_area_intersect(&inverse_clip_coords_for_obj, &inverse_clip_coords_for_obj, &obj_coords_ext)) {
            return LV_RES_INV;
        }

        *layer_area_out = inverse_clip_coords_for_obj;
    }
    else if(layer_type == LV_LAYER_TYPE_SIMPLE) {
        lv_area_t clip_coords_for_obj;
        if(!_lv_area_intersect(&clip_coords_for_obj, draw_ctx->clip_area, &obj_coords_ext)) {
            return LV_RES_INV;
        }
        *layer_area_out = clip_coords_for_obj;
    }
    else {
        LV_LOG_WARN("Unhandled intermediate layer type");
        return LV_RES_INV;
    }

    return LV_RES_OK;
}

static void layer_alpha_test(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * layer_ctx,
                             lv_draw_layer_flags_t flags)
{
    bool has_alpha;
    /*If globally the layer has alpha maybe this smaller section has not (e.g. not on a rounded corner)
     *If turns out that this section has no alpha renderer can choose faster algorithms*/
    if(flags & LV_DRAW_LAYER_FLAG_HAS_ALPHA) {
        /*Test for alpha by assuming there is no alpha. If it fails, fall back to rendering with alpha*/
        has_alpha = true;
        if(_lv_area_is_in(&layer_ctx->area_act, &obj->coords, 0)) {
            lv_cover_check_info_t info;
            info.res = LV_COVER_RES_COVER;
            info.area = &layer_ctx->area_act;
            lv_event_send(obj, LV_EVENT_COVER_CHECK, &info);
            if(info.res == LV_COVER_RES_COVER) has_alpha = false;
        }

        if(has_alpha) {
            layer_ctx->area_act.y2 = layer_ctx->area_act.y1 + layer_ctx->max_row_with_alpha - 1;
        }
    }
    else {
        has_alpha = false;
    }

    if(layer_ctx->area_act.y2 > layer_ctx->area_full.y2) layer_ctx->area_act.y2 = layer_ctx->area_full.y2;
    lv_draw_layer_adjust(draw_ctx, layer_ctx, has_alpha ? LV_DRAW_LAYER_FLAG_HAS_ALPHA : LV_DRAW_LAYER_FLAG_NONE);
}


void refr_obj(lv_draw_ctx_t * draw_ctx, lv_obj_t * obj)
{
    /*Do not refresh hidden objects*/
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return;
    lv_layer_type_t layer_type = _lv_obj_get_layer_type(obj);
    if(layer_type == LV_LAYER_TYPE_NONE) {
        lv_obj_redraw(draw_ctx, obj);
    }
    else {
        lv_opa_t opa = lv_obj_get_style_opa(obj, 0);
        if(opa < LV_OPA_MIN) return;

        lv_area_t layer_area_full;
        lv_res_t res = layer_get_area(draw_ctx, obj, layer_type, &layer_area_full);
        if(res != LV_RES_OK) return;

        lv_draw_layer_flags_t flags = LV_DRAW_LAYER_FLAG_HAS_ALPHA;

        if(_lv_area_is_in(&layer_area_full, &obj->coords, 0)) {
            lv_cover_check_info_t info;
            info.res = LV_COVER_RES_COVER;
            info.area = &layer_area_full;
            lv_event_send(obj, LV_EVENT_COVER_CHECK, &info);
            if(info.res == LV_COVER_RES_COVER) flags &= ~LV_DRAW_LAYER_FLAG_HAS_ALPHA;
        }

        if(layer_type == LV_LAYER_TYPE_SIMPLE) flags |= LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE;

        lv_draw_layer_ctx_t * layer_ctx = lv_draw_layer_create(draw_ctx, &layer_area_full, flags);
        if(layer_ctx == NULL) {
            LV_LOG_WARN("Couldn't create a new layer context");
            return;
        }
        lv_point_t pivot = {
            .x = lv_obj_get_style_transform_pivot_x(obj, 0),
            .y = lv_obj_get_style_transform_pivot_y(obj, 0)
        };

        lv_draw_img_dsc_t draw_dsc;
        lv_draw_img_dsc_init(&draw_dsc);
        draw_dsc.opa = opa;
        draw_dsc.angle = lv_obj_get_style_transform_angle(obj, 0);
        if(draw_dsc.angle > 3600) draw_dsc.angle -= 3600;
        else if(draw_dsc.angle < 0) draw_dsc.angle += 3600;

        draw_dsc.zoom = lv_obj_get_style_transform_zoom(obj, 0);
        draw_dsc.blend_mode = lv_obj_get_style_blend_mode(obj, 0);
        draw_dsc.antialias = disp_refr->driver->antialiasing;

        if(flags & LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE) {
            layer_ctx->area_act = layer_ctx->area_full;
            layer_ctx->area_act.y2 = layer_ctx->area_act.y1 + layer_ctx->max_row_with_no_alpha - 1;
            if(layer_ctx->area_act.y2 > layer_ctx->area_full.y2) layer_ctx->area_act.y2 = layer_ctx->area_full.y2;
        }

        while(layer_ctx->area_act.y1 <= layer_area_full.y2) {
            if(flags & LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE) {
                layer_alpha_test(obj, draw_ctx, layer_ctx, flags);
            }

            lv_obj_redraw(draw_ctx, obj);

            draw_dsc.pivot.x = obj->coords.x1 + pivot.x - draw_ctx->buf_area->x1;
            draw_dsc.pivot.y = obj->coords.y1 + pivot.y - draw_ctx->buf_area->y1;

            /*With LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE it should also go the next chunk*/
            lv_draw_layer_blend(draw_ctx, layer_ctx, &draw_dsc);

            if((flags & LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE) == 0) break;

            layer_ctx->area_act.y1 = layer_ctx->area_act.y2 + 1;
            layer_ctx->area_act.y2 = layer_ctx->area_act.y1 + layer_ctx->max_row_with_no_alpha - 1;
        }

        lv_draw_layer_destroy(draw_ctx, layer_ctx);
    }
}


static uint32_t get_max_row(lv_disp_t * disp, lv_coord_t area_w, lv_coord_t area_h)
{
    int32_t max_row = (uint32_t)disp->driver->draw_buf->size / area_w;

    if(max_row > area_h) max_row = area_h;

    /*Round down the lines of draw_buf if rounding is added*/
    if(disp_refr->driver->rounder_cb) {
        lv_area_t tmp;
        tmp.x1 = 0;
        tmp.x2 = 0;
        tmp.y1 = 0;

        lv_coord_t h_tmp = max_row;
        do {
            tmp.y2 = h_tmp - 1;
            disp_refr->driver->rounder_cb(disp_refr->driver, &tmp);

            /*If this height fits into `max_row` then fine*/
            if(lv_area_get_height(&tmp) <= max_row) break;

            /*Decrement the height of the area until it fits into `max_row` after rounding*/
            h_tmp--;
        } while(h_tmp > 0);

        if(h_tmp <= 0) {
            LV_LOG_WARN("Can't set draw_buf height using the round function. (Wrong round_cb or to "
                        "small draw_buf)");
            return 0;
        }
        else {
            max_row = tmp.y2 + 1;
        }
    }

    return max_row;
}

static void draw_buf_rotate_180(lv_disp_drv_t * drv, lv_area_t * area, lv_color_t * color_p)
{
    lv_coord_t area_w = lv_area_get_width(area);
    lv_coord_t area_h = lv_area_get_height(area);
    uint32_t total = area_w * area_h;
    /*Swap the beginning and end values*/
    lv_color_t tmp;
    uint32_t i = total - 1, j = 0;
    while(i > j) {
        tmp = color_p[i];
        color_p[i] = color_p[j];
        color_p[j] = tmp;
        i--;
        j++;
    }
    lv_coord_t tmp_coord;
    tmp_coord = area->y2;
    area->y2 = drv->ver_res - area->y1 - 1;
    area->y1 = drv->ver_res - tmp_coord - 1;
    tmp_coord = area->x2;
    area->x2 = drv->hor_res - area->x1 - 1;
    area->x1 = drv->hor_res - tmp_coord - 1;
}

static LV_ATTRIBUTE_FAST_MEM void draw_buf_rotate_90(bool invert_i, lv_coord_t area_w, lv_coord_t area_h,
                                                     lv_color_t * orig_color_p, lv_color_t * rot_buf)
{

    uint32_t invert = (area_w * area_h) - 1;
    uint32_t initial_i = ((area_w - 1) * area_h);
    for(lv_coord_t y = 0; y < area_h; y++) {
        uint32_t i = initial_i + y;
        if(invert_i)
            i = invert - i;
        for(lv_coord_t x = 0; x < area_w; x++) {
            rot_buf[i] = *(orig_color_p++);
            if(invert_i)
                i += area_h;
            else
                i -= area_h;
        }
    }
}

/**
 * Helper function for draw_buf_rotate_90_sqr. Given a list of four numbers, rotate the entire list to the left.
 */
static inline void draw_buf_rotate4(lv_color_t * a, lv_color_t * b, lv_color_t * c, lv_color_t * d)
{
    lv_color_t tmp;
    tmp = *a;
    *a = *b;
    *b = *c;
    *c = *d;
    *d = tmp;
}

/**
 * Rotate a square image 90/270 degrees in place.
 * @note inspired by https://stackoverflow.com/a/43694906
 */
static void draw_buf_rotate_90_sqr(bool is_270, lv_coord_t w, lv_color_t * color_p)
{
    for(lv_coord_t i = 0; i < w / 2; i++) {
        for(lv_coord_t j = 0; j < (w + 1) / 2; j++) {
            lv_coord_t inv_i = (w - 1) - i;
            lv_coord_t inv_j = (w - 1) - j;
            if(is_270) {
                draw_buf_rotate4(
                    &color_p[i * w + j],
                    &color_p[inv_j * w + i],
                    &color_p[inv_i * w + inv_j],
                    &color_p[j * w + inv_i]
                );
            }
            else {
                draw_buf_rotate4(
                    &color_p[i * w + j],
                    &color_p[j * w + inv_i],
                    &color_p[inv_i * w + inv_j],
                    &color_p[inv_j * w + i]
                );
            }

        }
    }
}

/**
 * Rotate the draw_buf to the display's native orientation.
 */
static void draw_buf_rotate(lv_area_t * area, lv_color_t * color_p)
{
    lv_disp_drv_t * drv = disp_refr->driver;
    if(disp_refr->driver->full_refresh && drv->sw_rotate) {
        LV_LOG_ERROR("cannot rotate a full refreshed display!");
        return;
    }
    if(drv->rotated == LV_DISP_ROT_180) {
        draw_buf_rotate_180(drv, area, color_p);
        call_flush_cb(drv, area, color_p);
    }
    else if(drv->rotated == LV_DISP_ROT_90 || drv->rotated == LV_DISP_ROT_270) {
        /*Allocate a temporary buffer to store rotated image*/
        lv_color_t * rot_buf = NULL;
        lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp_refr);
        lv_coord_t area_w = lv_area_get_width(area);
        lv_coord_t area_h = lv_area_get_height(area);
        /*Determine the maximum number of rows that can be rotated at a time*/
        lv_coord_t max_row = LV_MIN((lv_coord_t)((LV_DISP_ROT_MAX_BUF / sizeof(lv_color_t)) / area_w), area_h);
        lv_coord_t init_y_off;
        init_y_off = area->y1;
        if(drv->rotated == LV_DISP_ROT_90) {
            area->y2 = drv->ver_res - area->x1 - 1;
            area->y1 = area->y2 - area_w + 1;
        }
        else {
            area->y1 = area->x1;
            area->y2 = area->y1 + area_w - 1;
        }

        /*Rotate the screen in chunks, flushing after each one*/
        lv_coord_t row = 0;
        while(row < area_h) {
            lv_coord_t height = LV_MIN(max_row, area_h - row);
            draw_buf->flushing = 1;
            if((row == 0) && (area_h >= area_w)) {
                /*Rotate the initial area as a square*/
                height = area_w;
                draw_buf_rotate_90_sqr(drv->rotated == LV_DISP_ROT_270, area_w, color_p);
                if(drv->rotated == LV_DISP_ROT_90) {
                    area->x1 = init_y_off;
                    area->x2 = init_y_off + area_w - 1;
                }
                else {
                    area->x2 = drv->hor_res - 1 - init_y_off;
                    area->x1 = area->x2 - area_w + 1;
                }
            }
            else {
                /*Rotate other areas using a maximum buffer size*/
                if(rot_buf == NULL) rot_buf = lv_mem_buf_get(LV_DISP_ROT_MAX_BUF);
                draw_buf_rotate_90(drv->rotated == LV_DISP_ROT_270, area_w, height, color_p, rot_buf);

                if(drv->rotated == LV_DISP_ROT_90) {
                    area->x1 = init_y_off + row;
                    area->x2 = init_y_off + row + height - 1;
                }
                else {
                    area->x2 = drv->hor_res - 1 - init_y_off - row;
                    area->x1 = area->x2 - height + 1;
                }
            }

            /* The original part (chunk of the current area) were split into more parts here.
             * Set the original last_part flag on the last part of rotation. */
            if(row + height >= area_h && draw_buf->last_area && draw_buf->last_part) {
                draw_buf->flushing_last = 1;
            }
            else {
                draw_buf->flushing_last = 0;
            }

            /*Flush the completed area to the display*/
            call_flush_cb(drv, area, rot_buf == NULL ? color_p : rot_buf);
            /*FIXME: Rotation forces legacy behavior where rendering and flushing are done serially*/
            while(draw_buf->flushing) {
                if(drv->wait_cb) drv->wait_cb(drv);
            }
            color_p += area_w * height;
            row += height;
        }
        /*Free the allocated buffer at the end if necessary*/
        if(rot_buf != NULL) lv_mem_buf_release(rot_buf);
    }
}

/**
 * Flush the content of the draw buffer
 */
static void draw_buf_flush(lv_disp_t * disp)
{
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp_refr);

    /*Flush the rendered content to the display*/
    lv_draw_ctx_t * draw_ctx = disp->driver->draw_ctx;
    if(draw_ctx->wait_for_finish) draw_ctx->wait_for_finish(draw_ctx);

    /* In double buffered mode wait until the other buffer is freed
     * and driver is ready to receive the new buffer */
    if(draw_buf->buf1 && draw_buf->buf2) {
        while(draw_buf->flushing) {
            if(disp_refr->driver->wait_cb) disp_refr->driver->wait_cb(disp_refr->driver);
        }

        /*If the screen is transparent initialize it when the flushing is ready*/
#if LV_COLOR_SCREEN_TRANSP
        if(disp_refr->driver->screen_transp) {
            if(disp_refr->driver->clear_cb) {
                disp_refr->driver->clear_cb(disp_refr->driver, disp_refr->driver->draw_buf->buf_act, disp_refr->driver->draw_buf->size);
            }
            else {
                lv_memset_00(disp_refr->driver->draw_buf->buf_act, disp_refr->driver->draw_buf->size * LV_IMG_PX_SIZE_ALPHA_BYTE);
            }
        }
#endif
    }

    draw_buf->flushing = 1;

    if(disp_refr->driver->draw_buf->last_area && disp_refr->driver->draw_buf->last_part) draw_buf->flushing_last = 1;
    else draw_buf->flushing_last = 0;

    bool flushing_last = draw_buf->flushing_last;

    if(disp->driver->flush_cb) {
        /*Rotate the buffer to the display's native orientation if necessary*/
        if(disp->driver->rotated != LV_DISP_ROT_NONE && disp->driver->sw_rotate) {
            draw_buf_rotate(draw_ctx->buf_area, draw_ctx->buf);
        }
        else {
            call_flush_cb(disp->driver, draw_ctx->buf_area, draw_ctx->buf);
        }
    }
    /*If there are 2 buffers swap them. With direct mode swap only on the last area*/
    if(draw_buf->buf1 && draw_buf->buf2 && (!disp->driver->direct_mode || flushing_last)) {
        if(draw_buf->buf_act == draw_buf->buf1)
            draw_buf->buf_act = draw_buf->buf2;
        else
            draw_buf->buf_act = draw_buf->buf1;
    }
}

static void call_flush_cb(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    REFR_TRACE("Calling flush_cb on (%d;%d)(%d;%d) area with %p image pointer", area->x1, area->y1, area->x2, area->y2,
               (void *)color_p);

    lv_area_t offset_area = {
        .x1 = area->x1 + drv->offset_x,
        .y1 = area->y1 + drv->offset_y,
        .x2 = area->x2 + drv->offset_x,
        .y2 = area->y2 + drv->offset_y
    };

    drv->flush_cb(drv, &offset_area, color_p);
}

#if LV_USE_PERF_MONITOR
static void perf_monitor_init(perf_monitor_t * _perf_monitor)
{
    LV_ASSERT_NULL(_perf_monitor);
    _perf_monitor->elaps_sum = 0;
    _perf_monitor->fps_sum_all = 0;
    _perf_monitor->fps_sum_cnt = 0;
    _perf_monitor->frame_cnt = 0;
    _perf_monitor->perf_last_time = 0;
    _perf_monitor->perf_label = NULL;
}
#endif

#if LV_USE_MEM_MONITOR
static void mem_monitor_init(mem_monitor_t * _mem_monitor)
{
    LV_ASSERT_NULL(_mem_monitor);
    _mem_monitor->mem_last_time = 0;
    _mem_monitor->mem_label = NULL;
}
#endif

