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

#if LV_USE_PERF_MONITOR || LV_USE_MEM_MONITOR
    #include "../widgets/lv_label.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_refr_join_area(void);
static void lv_refr_areas(void);
static void lv_refr_area(const lv_area_t * area_p);
static void lv_refr_area_part(const lv_area_t * area_p);
static lv_obj_t * lv_refr_get_top_obj(const lv_area_t * area_p, lv_obj_t * obj);
static void lv_refr_obj_and_children(lv_obj_t * top_p, const lv_area_t * mask_p);
static void lv_refr_obj(lv_obj_t * obj, const lv_area_t * mask_ori_p);
static void draw_buf_flush(void);
static void call_flush_cb(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t px_num;
static lv_disp_t * disp_refr; /*Display being refreshed*/
#if LV_USE_PERF_MONITOR
    static uint32_t fps_sum_cnt;
    static uint32_t fps_sum_all;
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
    /*Nothing to do*/
}

/**
 * Redraw the invalidated areas now.
 * Normally the redrawing is periodically executed in `lv_timer_handler` but a long blocking process
 * can prevent the call of `lv_timer_handler`. In this case if the GUI is updated in the process
 * (e.g. progress bar) this function can be called when the screen should be updated.
 * @param disp pointer to display to refresh. NULL to refresh all displays.
 */
void lv_refr_now(lv_disp_t * disp)
{
    lv_anim_refr_now();

    if(disp) {
        _lv_disp_refr_timer(disp->refr_timer);
    }
    else {
        lv_disp_t * d;
        d = lv_disp_get_next(NULL);
        while(d) {
            _lv_disp_refr_timer(d->refr_timer);
            d = lv_disp_get_next(d);
        }
    }
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
        lv_timer_resume(disp->refr_timer);
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
    lv_timer_resume(disp->refr_timer);
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

    disp_refr = tmr->user_data;

#if LV_USE_PERF_MONITOR == 0 && LV_USE_MEM_MONITOR == 0
    /**
     * Ensure the timer does not run again automatically.
     * This is done before refreshing in case refreshing invalidates something else.
     */
    lv_timer_pause(tmr);
#endif

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

    lv_refr_areas();

    /*If refresh happened ...*/
    if(disp_refr->inv_p != 0) {
        if(disp_refr->driver->full_refresh) {
            draw_buf_flush();
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
    static lv_obj_t * perf_label = NULL;
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
    }

    static uint32_t perf_last_time = 0;
    static uint32_t elaps_sum = 0;
    static uint32_t frame_cnt = 0;
    if(lv_tick_elaps(perf_last_time) < 300) {
        if(px_num > 5000) {
            elaps_sum += elaps;
            frame_cnt ++;
        }
    }
    else {
        perf_last_time = lv_tick_get();
        uint32_t fps_limit = 1000 / disp_refr->refr_timer->period;
        unsigned int fps;

        if(elaps_sum == 0) elaps_sum = 1;
        if(frame_cnt == 0) fps = fps_limit;
        else fps = (1000 * frame_cnt) / elaps_sum;
        elaps_sum = 0;
        frame_cnt = 0;
        if(fps > fps_limit) fps = fps_limit;

        fps_sum_all += fps;
        fps_sum_cnt ++;
        unsigned int cpu = 100 - lv_timer_get_idle();
        lv_label_set_text_fmt(perf_label, "%u FPS\n%u%% CPU", fps, cpu);
    }
#endif

#if LV_USE_MEM_MONITOR && LV_MEM_CUSTOM == 0 && LV_USE_LABEL
    static lv_obj_t * mem_label = NULL;
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
    }

    static uint32_t mem_last_time = 0;
    if(lv_tick_elaps(mem_last_time) > 300) {
        mem_last_time = lv_tick_get();
        lv_mem_monitor_t mon;
        lv_mem_monitor(&mon);
        uint32_t used_size = mon.total_size - mon.free_size;;
        uint32_t used_kb = used_size / 1024;
        uint32_t used_kb_tenth = (used_size - (used_kb * 1024)) / 102;
        lv_label_set_text_fmt(mem_label, "%" LV_PRIu32 ".%" LV_PRIu32 " kB used (%d %%)\n" \
                              "%d%% frag.", used_kb, used_kb_tenth, mon.used_pct,
                              mon.frag_pct);
    }
#endif

    REFR_TRACE("finished");
}

#if LV_USE_PERF_MONITOR
uint32_t lv_refr_get_fps_avg(void)
{
    return fps_sum_all / fps_sum_cnt;
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
static void lv_refr_areas(void)
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

    disp_refr->driver->draw_buf->last_area = 0;
    disp_refr->driver->draw_buf->last_part = 0;

    for(i = 0; i < disp_refr->inv_p; i++) {
        /*Refresh the unjoined areas*/
        if(disp_refr->inv_area_joined[i] == 0) {

            if(i == last_i) disp_refr->driver->draw_buf->last_area = 1;
            disp_refr->driver->draw_buf->last_part = 0;
            lv_refr_area(&disp_refr->inv_areas[i]);

            px_num += lv_area_get_size(&disp_refr->inv_areas[i]);
        }
    }
}

/**
 * Refresh an area if there is Virtual Display Buffer
 * @param area_p  pointer to an area to refresh
 */
static void lv_refr_area(const lv_area_t * area_p)
{
    /*With full refresh just redraw directly into the buffer*/
    if(disp_refr->driver->full_refresh) {
        lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp_refr);
        draw_buf->area.x1        = 0;
        draw_buf->area.x2        = lv_disp_get_hor_res(disp_refr) - 1;
        draw_buf->area.y1        = 0;
        draw_buf->area.y2        = lv_disp_get_ver_res(disp_refr) - 1;
        disp_refr->driver->draw_buf->last_part = 1;
        lv_refr_area_part(area_p);
        return;
    }

    /*Normal refresh: draw the area in parts*/
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp_refr);
    /*Calculate the max row num*/
    lv_coord_t w = lv_area_get_width(area_p);
    lv_coord_t h = lv_area_get_height(area_p);
    lv_coord_t y2 = area_p->y2 >= lv_disp_get_ver_res(disp_refr) ?
                    lv_disp_get_ver_res(disp_refr) - 1 : area_p->y2;

    int32_t max_row = (uint32_t)draw_buf->size / w;

    if(max_row > h) max_row = h;

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
            return;
        }
        else {
            max_row = tmp.y2 + 1;
        }
    }

    /*In direct mode draw directly on the absolute coordinates of the buffer*/
    if(disp_refr->driver->direct_mode) {
        draw_buf->area.x1 = 0;
        draw_buf->area.x2 = lv_disp_get_hor_res(disp_refr) - 1;
        draw_buf->area.y1 = 0;
        draw_buf->area.y2 = lv_disp_get_ver_res(disp_refr) - 1;
        disp_refr->driver->draw_buf->last_part = disp_refr->driver->draw_buf->last_area;
        lv_refr_area_part(area_p);
    }
    /*Else assume the buffer starts at the given area*/
    else {
        /*Always use the full row*/
        lv_coord_t row;
        lv_coord_t row_last = 0;
        for(row = area_p->y1; row + max_row - 1 <= y2; row += max_row) {
            /*Calc. the next y coordinates of draw_buf*/
            draw_buf->area.x1 = area_p->x1;
            draw_buf->area.x2 = area_p->x2;
            draw_buf->area.y1 = row;
            draw_buf->area.y2 = row + max_row - 1;
            if(draw_buf->area.y2 > y2) draw_buf->area.y2 = y2;
            row_last = draw_buf->area.y2;
            if(y2 == row_last) disp_refr->driver->draw_buf->last_part = 1;
            lv_refr_area_part(area_p);
        }

        /*If the last y coordinates are not handled yet ...*/
        if(y2 != row_last) {
            /*Calc. the next y coordinates of draw_buf*/
            draw_buf->area.x1 = area_p->x1;
            draw_buf->area.x2 = area_p->x2;
            draw_buf->area.y1 = row;
            draw_buf->area.y2 = y2;

            disp_refr->driver->draw_buf->last_part = 1;
            lv_refr_area_part(area_p);
        }
    }
}

/**
 * Refresh a part of an area which is on the actual Virtual Display Buffer
 * @param area_p pointer to an area to refresh
 */
static void lv_refr_area_part(const lv_area_t * area_p)
{
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp_refr);

    /* Below the `area_p` area will be redrawn into the draw buffer.
     * In single buffered mode wait here until the buffer is freed.*/
    if(draw_buf->buf1 && !draw_buf->buf2) {
        while(draw_buf->flushing) {
            if(disp_refr->driver->wait_cb) disp_refr->driver->wait_cb(disp_refr->driver);
        }
    }

    lv_obj_t * top_act_scr = NULL;
    lv_obj_t * top_prev_scr = NULL;

    /*Get the new mask from the original area and the act. draw_buf
     It will be a part of 'area_p'*/
    lv_area_t start_mask;
    _lv_area_intersect(&start_mask, area_p, &draw_buf->area);

    /*Get the most top object which is not covered by others*/
    top_act_scr = lv_refr_get_top_obj(&start_mask, lv_disp_get_scr_act(disp_refr));
    if(disp_refr->prev_scr) {
        top_prev_scr = lv_refr_get_top_obj(&start_mask, disp_refr->prev_scr);
    }

    /*Draw a display background if there is no top object*/
    if(top_act_scr == NULL && top_prev_scr == NULL) {
        if(disp_refr->bg_fn) {
            disp_refr->bg_fn(&start_mask);
        } else if(disp_refr->bg_img) {
            lv_draw_img_dsc_t dsc;
            lv_draw_img_dsc_init(&dsc);
            dsc.opa = disp_refr->bg_opa;
            lv_img_header_t header;
            lv_res_t res;
            res = lv_img_decoder_get_info(disp_refr->bg_img, &header);
            if(res == LV_RES_OK) {
                lv_area_t a;
                lv_area_set(&a, 0, 0, header.w - 1, header.h - 1);
                lv_draw_img(&a, &start_mask, disp_refr->bg_img, &dsc);
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
            lv_draw_rect(&start_mask, &start_mask, &dsc);

        }
    }
    /*Refresh the previous screen if any*/
    if(disp_refr->prev_scr) {
        /*Get the most top object which is not covered by others*/
        if(top_prev_scr == NULL) {
            top_prev_scr = disp_refr->prev_scr;
        }
        /*Do the refreshing from the top object*/
        lv_refr_obj_and_children(top_prev_scr, &start_mask);

    }

    if(top_act_scr == NULL) {
        top_act_scr = disp_refr->act_scr;
    }
    /*Do the refreshing from the top object*/
    lv_refr_obj_and_children(top_act_scr, &start_mask);

    /*Also refresh top and sys layer unconditionally*/
    lv_refr_obj_and_children(lv_disp_get_layer_top(disp_refr), &start_mask);
    lv_refr_obj_and_children(lv_disp_get_layer_sys(disp_refr), &start_mask);

    /*In true double buffered mode flush only once when all areas were rendered.
     *In normal mode flush after every area*/
    if(disp_refr->driver->full_refresh == false) {
        draw_buf_flush();
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

    /*If this object is fully cover the draw area check the children too*/
    if(_lv_area_is_in(area_p, &obj->coords, 0) && lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN) == false) {
        lv_cover_check_info_t info;
        info.res = LV_COVER_RES_COVER;
        info.area = area_p;
        lv_event_send(obj, LV_EVENT_COVER_CHECK, &info);
        if(info.res == LV_COVER_RES_MASKED) return NULL;

        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            found_p = lv_refr_get_top_obj(area_p, child);

            /*If a children is ok then break*/
            if(found_p != NULL) {
                break;
            }
        }

        /*If no better children use this object*/
        if(found_p == NULL) {
            if(info.res == LV_COVER_RES_COVER) {
                found_p = obj;
            }
        }
    }

    return found_p;
}

/**
 * Make the refreshing from an object. Draw all its children and the youngers too.
 * @param top_p pointer to an objects. Start the drawing from it.
 * @param mask_p pointer to an area, the objects will be drawn only here
 */
static void lv_refr_obj_and_children(lv_obj_t * top_p, const lv_area_t * mask_p)
{
    /*Normally always will be a top_obj (at least the screen)
     *but in special cases (e.g. if the screen has alpha) it won't.
     *In this case use the screen directly*/
    if(top_p == NULL) top_p = lv_disp_get_scr_act(disp_refr);
    if(top_p == NULL) return;  /*Shouldn't happen*/

    /*Refresh the top object and its children*/
    lv_refr_obj(top_p, mask_p);

    /*Draw the 'younger' sibling objects because they can be on top_obj*/
    lv_obj_t * par;
    lv_obj_t * border_p = top_p;

    par = lv_obj_get_parent(top_p);

    /*Do until not reach the screen*/
    while(par != NULL) {
        bool go = false;
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(par);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = par->spec_attr->children[i];
            if(!go) {
                if(child == border_p) go = true;
            }
            else {
                /*Refresh the objects*/
                lv_refr_obj(child, mask_p);
            }
        }

        /*Call the post draw draw function of the parents of the to object*/
        lv_event_send(par, LV_EVENT_DRAW_POST_BEGIN, (void *)mask_p);
        lv_event_send(par, LV_EVENT_DRAW_POST, (void *)mask_p);
        lv_event_send(par, LV_EVENT_DRAW_POST_END, (void *)mask_p);

        /*The new border will be the last parents,
         *so the 'younger' brothers of parent will be refreshed*/
        border_p = par;
        /*Go a level deeper*/
        par = lv_obj_get_parent(par);
    }
}

/**
 * Refresh an object an all of its children. (Called recursively)
 * @param obj pointer to an object to refresh
 * @param mask_ori_p pointer to an area, the objects will be drawn only here
 */
static void lv_refr_obj(lv_obj_t * obj, const lv_area_t * mask_ori_p)
{
    /*Do not refresh hidden objects*/
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return;

    bool union_ok; /*Store the return value of area_union*/
    /*Truncate the original mask to the coordinates of the parent
     *because the parent and its children are visible only here*/
    lv_area_t obj_mask;
    lv_area_t obj_ext_mask;
    lv_area_t obj_area;
    lv_coord_t ext_size = _lv_obj_get_ext_draw_size(obj);
    lv_obj_get_coords(obj, &obj_area);
    obj_area.x1 -= ext_size;
    obj_area.y1 -= ext_size;
    obj_area.x2 += ext_size;
    obj_area.y2 += ext_size;
    union_ok = _lv_area_intersect(&obj_ext_mask, mask_ori_p, &obj_area);

    /*Draw the parent and its children only if they ore on 'mask_parent'*/
    if(union_ok != false) {
        /*Redraw the object*/
        lv_event_send(obj, LV_EVENT_DRAW_MAIN_BEGIN, &obj_ext_mask);
        lv_event_send(obj, LV_EVENT_DRAW_MAIN, &obj_ext_mask);
        lv_event_send(obj, LV_EVENT_DRAW_MAIN_END, &obj_ext_mask);

#if LV_USE_REFR_DEBUG
        lv_color_t debug_color = lv_color_make(lv_rand(0, 0xFF), lv_rand(0, 0xFF), lv_rand(0, 0xFF));
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_color.full = debug_color.full;
        draw_dsc.bg_opa = LV_OPA_20;
        draw_dsc.border_width = 1;
        draw_dsc.border_opa = LV_OPA_30;
        draw_dsc.border_color = debug_color;
        lv_draw_rect(&obj_ext_mask, &obj_ext_mask, &draw_dsc);
#endif
        /*Create a new 'obj_mask' without 'ext_size' because the children can't be visible there*/
        lv_obj_get_coords(obj, &obj_area);
        union_ok = _lv_area_intersect(&obj_mask, mask_ori_p, &obj_area);
        if(union_ok != false) {
            lv_area_t mask_child; /*Mask from obj and its child*/
            lv_area_t child_area;
            uint32_t i;
            uint32_t child_cnt = lv_obj_get_child_cnt(obj);
            for(i = 0; i < child_cnt; i++) {
                lv_obj_t * child = obj->spec_attr->children[i];
                lv_obj_get_coords(child, &child_area);
                ext_size = _lv_obj_get_ext_draw_size(child);
                child_area.x1 -= ext_size;
                child_area.y1 -= ext_size;
                child_area.x2 += ext_size;
                child_area.y2 += ext_size;
                /*Get the union (common parts) of original mask (from obj)
                 *and its child*/
                union_ok = _lv_area_intersect(&mask_child, &obj_mask, &child_area);

                /*If the parent and the child has common area then refresh the child*/
                if(union_ok) {
                    /*Refresh the next children*/
                    lv_refr_obj(child, &mask_child);
                }
            }
        }

        /*If all the children are redrawn make 'post draw' draw*/
        lv_event_send(obj, LV_EVENT_DRAW_POST_BEGIN, &obj_ext_mask);
        lv_event_send(obj, LV_EVENT_DRAW_POST, &obj_ext_mask);
        lv_event_send(obj, LV_EVENT_DRAW_POST_END, &obj_ext_mask);
    }
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
static void draw_buf_flush(void)
{
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp_refr);
    lv_color_t * color_p = draw_buf->buf_act;

    /*Flush the rendered content to the display*/
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver->gpu_wait_cb) disp->driver->gpu_wait_cb(disp->driver);

    /* In double buffered mode wait until the other buffer is freed
     * and driver is ready to receive the new buffer */
    if(draw_buf->buf1 && draw_buf->buf2) {
        while(draw_buf->flushing) {
            if(disp_refr->driver->wait_cb) disp_refr->driver->wait_cb(disp_refr->driver);
        }
    }

    draw_buf->flushing = 1;

    if(disp_refr->driver->draw_buf->last_area && disp_refr->driver->draw_buf->last_part) draw_buf->flushing_last = 1;
    else draw_buf->flushing_last = 0;

    if(disp->driver->flush_cb) {
        /*Rotate the buffer to the display's native orientation if necessary*/
        if(disp->driver->rotated != LV_DISP_ROT_NONE && disp->driver->sw_rotate) {
            draw_buf_rotate(&draw_buf->area, draw_buf->buf_act);
        }
        else {
            call_flush_cb(disp->driver, &draw_buf->area, color_p);
        }
    }
    if(draw_buf->buf1 && draw_buf->buf2) {
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
