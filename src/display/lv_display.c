/**
 * @file lv_display.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../display/lv_display_private.h"
#include "../misc/lv_event_private.h"
#include "../misc/lv_anim_private.h"
#include "../draw/lv_draw_private.h"
#include "../core/lv_obj_private.h"
#include "lv_display.h"
#include "../misc/lv_math.h"
#include "../core/lv_refr_private.h"
#include "../stdlib/lv_string.h"
#include "../themes/lv_theme.h"
#include "../core/lv_global.h"
#include "../others/sysmon/lv_sysmon.h"

#if LV_USE_DRAW_SW
    #include "../draw/sw/lv_draw_sw.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define disp_def LV_GLOBAL_DEFAULT()->disp_default
#define disp_ll_p &(LV_GLOBAL_DEFAULT()->disp_ll)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_tree_walk_res_t invalidate_layout_cb(lv_obj_t * obj, void * user_data);
static void update_resolution(lv_display_t * disp);
static void scr_load_internal(lv_obj_t * scr);
static void scr_load_anim_start(lv_anim_t * a);
static void opa_scale_anim(void * obj, int32_t v);
static void set_x_anim(void * obj, int32_t v);
static void set_y_anim(void * obj, int32_t v);
static void scr_anim_completed(lv_anim_t * a);
static bool is_out_anim(lv_screen_load_anim_t a);
static void disp_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_display_create(int32_t hor_res, int32_t ver_res)
{
    lv_display_t * disp = lv_ll_ins_head(disp_ll_p);
    LV_ASSERT_MALLOC(disp);
    if(!disp) return NULL;

    lv_memzero(disp, sizeof(lv_display_t));

    disp->hor_res          = hor_res;
    disp->ver_res          = ver_res;
    disp->physical_hor_res = -1;
    disp->physical_ver_res = -1;
    disp->offset_x         = 0;
    disp->offset_y         = 0;
    disp->antialiasing     = LV_COLOR_DEPTH > 8 ? 1 : 0;
    disp->dpi              = LV_DPI_DEF;
    disp->color_format = LV_COLOR_FORMAT_NATIVE;


#if defined(LV_DRAW_SW_DRAW_UNIT_CNT) && (LV_DRAW_SW_DRAW_UNIT_CNT != 0)
    disp->tile_cnt = LV_DRAW_SW_DRAW_UNIT_CNT;
#else
    disp->tile_cnt = 1;
#endif

    disp->layer_head = lv_malloc(sizeof(lv_layer_t));
    LV_ASSERT_MALLOC(disp->layer_head);
    if(disp->layer_head == NULL) return NULL;
    lv_layer_init(disp->layer_head);

    if(disp->layer_init) disp->layer_init(disp, disp->layer_head);
    disp->layer_head->buf_area.x1 = 0;
    disp->layer_head->buf_area.y1 = 0;
    disp->layer_head->buf_area.x2 = hor_res - 1;
    disp->layer_head->buf_area.y2 = ver_res - 1;
    disp->layer_head->color_format = disp->color_format;

    disp->inv_en_cnt = 1;
    disp->last_activity_time = lv_tick_get();

    lv_ll_init(&disp->sync_areas, sizeof(lv_area_t));

    lv_display_t * disp_def_tmp = disp_def;
    disp_def                 = disp; /*Temporarily change the default screen to create the default screens on the
                                        new display*/
    /*Create a refresh timer*/
    disp->refr_timer = lv_timer_create(lv_display_refr_timer, LV_DEF_REFR_PERIOD, disp);
    LV_ASSERT_MALLOC(disp->refr_timer);
    if(disp->refr_timer == NULL) {
        lv_free(disp);
        return NULL;
    }

#if LV_USE_THEME_DEFAULT
    if(lv_theme_default_is_inited() == false) {
        disp->theme = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                            LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
    }
    else {
        disp->theme = lv_theme_default_get();
    }
#elif LV_USE_THEME_SIMPLE
    if(lv_theme_simple_is_inited() == false) {
        disp->theme = lv_theme_simple_init(disp);
    }
    else {
        disp->theme = lv_theme_simple_get();
    }
#elif LV_USE_THEME_MONO
    if(lv_theme_mono_is_inited() == false) {
        disp->theme = lv_theme_mono_init(disp, false, LV_FONT_DEFAULT);
    }
    else {
        disp->theme = lv_theme_mono_get();
    }
#endif

    disp->bottom_layer = lv_obj_create(NULL); /*Create bottom layer on the display*/
    disp->act_scr   = lv_obj_create(NULL); /*Create a default screen on the display*/
    disp->top_layer = lv_obj_create(NULL); /*Create top layer on the display*/
    disp->sys_layer = lv_obj_create(NULL); /*Create sys layer on the display*/
    lv_obj_remove_style_all(disp->bottom_layer);
    lv_obj_remove_style_all(disp->top_layer);
    lv_obj_remove_style_all(disp->sys_layer);
    lv_obj_remove_flag(disp->top_layer, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(disp->sys_layer, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_scrollbar_mode(disp->bottom_layer, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(disp->top_layer, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(disp->sys_layer, LV_SCROLLBAR_MODE_OFF);

    lv_obj_invalidate(disp->act_scr);

    disp_def = disp_def_tmp; /*Revert the default display*/
    if(disp_def == NULL) disp_def = disp; /*Initialize the default display*/

    lv_display_add_event_cb(disp, disp_event_cb, LV_EVENT_REFR_REQUEST, NULL);

    lv_timer_ready(disp->refr_timer); /*Be sure the screen will be refreshed immediately on start up*/

#if LV_USE_PERF_MONITOR
    lv_sysmon_show_performance(disp);
#endif

#if LV_USE_MEM_MONITOR
    lv_sysmon_show_memory(disp);
#endif

    return disp;
}

void lv_display_delete(lv_display_t * disp)
{
    bool was_default = false;
    bool was_refr = false;
    if(disp == lv_display_get_default()) was_default = true;
    if(disp == lv_refr_get_disp_refreshing()) was_refr = true;

    lv_display_send_event(disp, LV_EVENT_DELETE, NULL);
    lv_event_remove_all(&(disp->event_list));

    /*Detach the input devices*/
    lv_indev_t * indev;
    indev = lv_indev_get_next(NULL);
    while(indev) {
        if(lv_indev_get_display(indev) == disp) {
            lv_indev_set_display(indev, NULL);
        }
        indev = lv_indev_get_next(indev);
    }

    /* Delete screen and other obj */
    if(disp->sys_layer) {
        lv_obj_delete(disp->sys_layer);
        disp->sys_layer = NULL;
    }
    if(disp->top_layer) {
        lv_obj_delete(disp->top_layer);
        disp->top_layer = NULL;
    }

    if(disp->bottom_layer) {
        lv_obj_delete(disp->bottom_layer);
        disp->bottom_layer = NULL;
    }

    disp->act_scr = NULL;

    while(disp->screen_cnt != 0) {
        /*Delete the screens*/
        lv_obj_delete(disp->screens[0]);
    }

    lv_ll_clear(&disp->sync_areas);
    lv_ll_remove(disp_ll_p, disp);
    if(disp->refr_timer) lv_timer_delete(disp->refr_timer);

    if(disp->layer_deinit) disp->layer_deinit(disp, disp->layer_head);
    lv_free(disp->layer_head);

    lv_free(disp);

    if(was_default) lv_display_set_default(lv_ll_get_head(disp_ll_p));

    if(was_refr) lv_refr_set_disp_refreshing(NULL);
}

void lv_display_set_default(lv_display_t * disp)
{
    disp_def = disp;
}

lv_display_t * lv_display_get_default(void)
{
    return disp_def;
}

lv_display_t * lv_display_get_next(lv_display_t * disp)
{
    if(disp == NULL)
        return lv_ll_get_head(disp_ll_p);
    else
        return lv_ll_get_next(disp_ll_p, disp);
}

/*---------------------
 * RESOLUTION
 *--------------------*/

void lv_display_set_resolution(lv_display_t * disp, int32_t hor_res, int32_t ver_res)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    if(disp->hor_res == hor_res && disp->ver_res == ver_res) return;

    disp->hor_res = hor_res;
    disp->ver_res = ver_res;

    update_resolution(disp);
}

void lv_display_set_physical_resolution(lv_display_t * disp, int32_t hor_res, int32_t ver_res)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->physical_hor_res = hor_res;
    disp->physical_ver_res = ver_res;

    lv_obj_invalidate(disp->sys_layer);

}

void lv_display_set_offset(lv_display_t * disp, int32_t x, int32_t y)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->offset_x = x;
    disp->offset_y = y;

    lv_obj_invalidate(disp->sys_layer);

}

void lv_display_set_dpi(lv_display_t * disp, int32_t dpi)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->dpi = dpi;
}

int32_t lv_display_get_horizontal_resolution(const lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotation) {
            case LV_DISPLAY_ROTATION_90:
            case LV_DISPLAY_ROTATION_270:
                return disp->ver_res;
            default:
                return disp->hor_res;
        }
    }
}

int32_t lv_display_get_vertical_resolution(const lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotation) {
            case LV_DISPLAY_ROTATION_90:
            case LV_DISPLAY_ROTATION_270:
                return disp->hor_res;
            default:
                return disp->ver_res;
        }
    }
}

int32_t lv_display_get_original_horizontal_resolution(const lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        return 0;
    }

    return disp->hor_res;
}

int32_t lv_display_get_original_vertical_resolution(const lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        return 0;
    }

    return disp->ver_res;
}

int32_t lv_display_get_physical_horizontal_resolution(const lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotation) {
            case LV_DISPLAY_ROTATION_90:
            case LV_DISPLAY_ROTATION_270:
                return disp->physical_ver_res > 0 ? disp->physical_ver_res : disp->ver_res;
            default:
                return disp->physical_hor_res > 0 ? disp->physical_hor_res : disp->hor_res;
        }
    }
}

int32_t lv_display_get_physical_vertical_resolution(const lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotation) {
            case LV_DISPLAY_ROTATION_90:
            case LV_DISPLAY_ROTATION_270:
                return disp->physical_hor_res > 0 ? disp->physical_hor_res : disp->hor_res;
            default:
                return disp->physical_ver_res > 0 ? disp->physical_ver_res : disp->ver_res;
        }
    }
}

int32_t lv_display_get_offset_x(const lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotation) {
            case LV_DISPLAY_ROTATION_90:
                return disp->offset_y;
            case LV_DISPLAY_ROTATION_180:
                return lv_display_get_physical_horizontal_resolution(disp) - disp->offset_x;
            case LV_DISPLAY_ROTATION_270:
                return lv_display_get_physical_horizontal_resolution(disp) - disp->offset_y;
            default:
                return disp->offset_x;
        }
    }
}

int32_t lv_display_get_offset_y(const lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotation) {
            case LV_DISPLAY_ROTATION_90:
                return disp->offset_x;
            case LV_DISPLAY_ROTATION_180:
                return lv_display_get_physical_vertical_resolution(disp) - disp->offset_y;
            case LV_DISPLAY_ROTATION_270:
                return lv_display_get_physical_vertical_resolution(disp) - disp->offset_x;
            default:
                return disp->offset_y;
        }
    }
}

int32_t lv_display_get_dpi(const lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return LV_DPI_DEF;  /*Do not return 0 because it might be a divider*/
    return disp->dpi;
}

/*---------------------
 * BUFFERING
 *--------------------*/

void lv_display_set_draw_buffers(lv_display_t * disp, lv_draw_buf_t * buf1, lv_draw_buf_t * buf2)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->buf_1 = buf1;
    disp->buf_2 = buf2;
    disp->buf_act = disp->buf_1;

    disp->stride_is_auto = 0;
}

void lv_display_set_3rd_draw_buffer(lv_display_t * disp, lv_draw_buf_t * buf3)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    LV_ASSERT_MSG(disp->buf_1 != NULL, "buf1 is null");
    LV_ASSERT_MSG(disp->buf_2 != NULL, "buf2 is null");

    disp->buf_3 = buf3;
}

void lv_display_set_buffers(lv_display_t * disp, void * buf1, void * buf2, uint32_t buf_size,
                            lv_display_render_mode_t render_mode)
{
    LV_ASSERT_MSG(buf1 != NULL, "Null buffer");
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t w = lv_display_get_original_horizontal_resolution(disp);
    uint32_t h = lv_display_get_original_vertical_resolution(disp);
    LV_ASSERT_MSG(w != 0 && h != 0, "display resolution is 0");

    /* buf1 or buf2 is not aligned according to LV_DRAW_BUF_ALIGN */
    LV_ASSERT_FORMAT_MSG(buf1 == lv_draw_buf_align(buf1, cf), "buf1 is not aligned: %p", buf1);
    LV_ASSERT_FORMAT_MSG(buf2 == NULL || buf2 == lv_draw_buf_align(buf2, cf), "buf2 is not aligned: %p", buf2);

    uint32_t stride = lv_draw_buf_width_to_stride(w, cf);
    if(render_mode == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        LV_ASSERT_FORMAT_MSG(stride != 0, "stride is 0, check your color format %d and width: %" LV_PRIu32, cf, w);
        /* for partial mode, we calculate the height based on the buf_size and stride */
        h = buf_size / stride;
        LV_ASSERT_MSG(h != 0, "the buffer is too small");
    }
    else {
        LV_ASSERT_FORMAT_MSG(stride * h <= buf_size, "%s mode requires screen sized buffer(s)",
                             render_mode == LV_DISPLAY_RENDER_MODE_FULL ? "FULL" : "DIRECT");
    }

    lv_draw_buf_init(&disp->_static_buf1, w, h, cf, stride, buf1, buf_size);
    lv_draw_buf_init(&disp->_static_buf2, w, h, cf, stride, buf2, buf_size);
    lv_display_set_draw_buffers(disp, &disp->_static_buf1, buf2 ? &disp->_static_buf2 : NULL);
    lv_display_set_render_mode(disp, render_mode);

    /* the stride was not set explicitly */
    disp->stride_is_auto = 1;
}

void lv_display_set_buffers_with_stride(lv_display_t * disp, void * buf1, void * buf2, uint32_t buf_size,
                                        uint32_t stride, lv_display_render_mode_t render_mode)
{
    if(stride == LV_STRIDE_AUTO) {
        lv_display_set_buffers(disp, buf1, buf2, buf_size, render_mode);
        return;
    }

    LV_ASSERT_MSG(buf1 != NULL, "Null buffer");
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t w = lv_display_get_original_horizontal_resolution(disp);
    uint32_t h = lv_display_get_original_vertical_resolution(disp);
    LV_ASSERT_MSG(w != 0 && h != 0, "display resolution is 0");

    if(render_mode == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        /* for partial mode, we calculate the height based on the buf_size and stride */
        h = buf_size / stride;
        LV_ASSERT_MSG(h != 0, "the buffer is too small");
    }
    else {
        LV_ASSERT_FORMAT_MSG(stride * h <= buf_size, "%s mode requires screen sized buffer(s)",
                             render_mode == LV_DISPLAY_RENDER_MODE_FULL ? "FULL" : "DIRECT");
    }

    lv_draw_buf_init(&disp->_static_buf1, w, h, cf, stride, buf1, buf_size);
    lv_draw_buf_init(&disp->_static_buf2, w, h, cf, stride, buf2, buf_size);
    lv_display_set_draw_buffers(disp, &disp->_static_buf1, buf2 ? &disp->_static_buf2 : NULL);
    lv_display_set_render_mode(disp, render_mode);

    disp->stride_is_auto = 0;
}

void lv_display_set_render_mode(lv_display_t * disp, lv_display_render_mode_t render_mode)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;
    disp->render_mode = render_mode;
}

void lv_display_set_flush_cb(lv_display_t * disp, lv_display_flush_cb_t flush_cb)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->flush_cb = flush_cb;
}

void lv_display_set_flush_wait_cb(lv_display_t * disp, lv_display_flush_wait_cb_t wait_cb)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->flush_wait_cb = wait_cb;
}

void lv_display_set_color_format(lv_display_t * disp, lv_color_format_t color_format)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->color_format = color_format;
    disp->layer_head->color_format = color_format;
    if(disp->buf_1) disp->buf_1->header.cf = color_format;
    if(disp->buf_2) disp->buf_2->header.cf = color_format;
    if(disp->buf_3) disp->buf_3->header.cf = color_format;

    lv_display_send_event(disp, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);
}

lv_color_format_t lv_display_get_color_format(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return LV_COLOR_FORMAT_UNKNOWN;

    return disp->color_format;
}

void lv_display_set_tile_cnt(lv_display_t * disp, uint32_t tile_cnt)
{
    LV_ASSERT_FORMAT_MSG(tile_cnt < 256, "tile_cnt must be smaller than 256 (%" LV_PRId32 " was used)", tile_cnt);

    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->tile_cnt = tile_cnt;
}

uint32_t lv_display_get_tile_cnt(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return 0;

    return disp->tile_cnt;
}

void lv_display_set_antialiasing(lv_display_t * disp, bool en)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->antialiasing = en;
}

bool lv_display_get_antialiasing(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return false;

    return disp->antialiasing;
}

LV_ATTRIBUTE_FLUSH_READY void lv_display_flush_ready(lv_display_t * disp)
{
    disp->flushing = 0;
}

LV_ATTRIBUTE_FLUSH_READY bool lv_display_flush_is_last(lv_display_t * disp)
{
    return disp->flushing_last;
}

bool lv_display_is_double_buffered(lv_display_t * disp)
{
    return disp->buf_2 != NULL;
}

/*---------------------
  * SCREENS
  *--------------------*/

lv_obj_t * lv_display_get_screen_active(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered to get its active screen");
        return NULL;
    }

    return disp->act_scr;
}

lv_obj_t * lv_display_get_screen_prev(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered to get its previous screen");
        return NULL;
    }

    return disp->prev_scr;
}

lv_obj_t * lv_display_get_layer_top(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_layer_top: no display registered to get its top layer");
        return NULL;
    }

    return disp->top_layer;
}

lv_obj_t * lv_display_get_layer_sys(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_layer_sys: no display registered to get its sys. layer");
        return NULL;
    }

    return disp->sys_layer;
}

lv_obj_t * lv_display_get_layer_bottom(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_layer_bottom: no display registered to get its bottom layer");
        return NULL;
    }

    return disp->bottom_layer;
}

#if LV_USE_OBJ_NAME

lv_obj_t * lv_display_get_screen_by_name(const lv_display_t * disp, const char * screen_name)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered to get a screen by name");
        return NULL;
    }

    uint32_t i;
    for(i = 0; i < disp->screen_cnt; i++) {
        const char * n = lv_obj_get_name(disp->screens[i]);
        if(n && lv_streq(screen_name, n)) return disp->screens[i];
    }

    return NULL;

}

#endif /*LV_USE_OBJ_NAME*/

void lv_screen_load(struct _lv_obj_t * scr)
{
    lv_screen_load_anim(scr, LV_SCREEN_LOAD_ANIM_NONE, 0, 0, false);
}

void lv_screen_load_anim(lv_obj_t * new_scr, lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay,
                         bool auto_del)
{
    lv_display_t * d = lv_obj_get_display(new_scr);
    lv_obj_t * act_scr = d->act_scr;

    if(act_scr == new_scr || d->scr_to_load == new_scr) {
        return;
    }

    /*If another screen load animation is in progress
     *make target screen loaded immediately. */
    if(d->scr_to_load && act_scr != d->scr_to_load) {
        lv_anim_delete(d->scr_to_load, NULL);
        lv_obj_set_pos(d->scr_to_load, 0, 0);
        lv_obj_remove_local_style_prop(d->scr_to_load, LV_STYLE_OPA, 0);

        d->prev_scr = d->act_scr;
        act_scr = d->scr_to_load; /*Active screen changed.*/

        scr_load_internal(d->scr_to_load);
    }

    d->scr_to_load = new_scr;

    if(d->prev_scr && d->del_prev) lv_obj_delete(d->prev_scr);
    d->prev_scr = NULL;

    d->draw_prev_over_act = is_out_anim(anim_type);
    d->del_prev = auto_del;

    /*Be sure there is no other animation on the screens*/
    lv_anim_delete(new_scr, NULL);
    if(act_scr) lv_anim_delete(act_scr, NULL);

    /*Be sure both screens are in a normal position*/
    lv_obj_set_pos(new_scr, 0, 0);
    if(act_scr) lv_obj_set_pos(act_scr, 0, 0);
    lv_obj_remove_local_style_prop(new_scr, LV_STYLE_OPA, 0);
    if(act_scr) lv_obj_remove_local_style_prop(act_scr, LV_STYLE_OPA, 0);

    /*Shortcut for immediate load*/
    if(time == 0 && delay == 0) {
        scr_load_internal(new_scr);
        if(auto_del && act_scr) lv_obj_delete(act_scr);
        return;
    }

    lv_anim_t a_new;
    lv_anim_init(&a_new);
    lv_anim_set_var(&a_new, new_scr);
    lv_anim_set_start_cb(&a_new, scr_load_anim_start);
    lv_anim_set_completed_cb(&a_new, scr_anim_completed);
    lv_anim_set_duration(&a_new, time);
    lv_anim_set_delay(&a_new, delay);

    lv_anim_t a_old;
    lv_anim_init(&a_old);
    lv_anim_set_var(&a_old, act_scr);
    lv_anim_set_duration(&a_old, time);
    lv_anim_set_delay(&a_old, delay);

    switch(anim_type) {
        case LV_SCREEN_LOAD_ANIM_NONE:
            /*Create a dummy animation to apply the delay*/
            lv_anim_set_exec_cb(&a_new, set_x_anim);
            lv_anim_set_values(&a_new, 0, 0);
            break;
        case LV_SCREEN_LOAD_ANIM_OVER_LEFT:
            lv_anim_set_exec_cb(&a_new, set_x_anim);
            lv_anim_set_values(&a_new, lv_display_get_horizontal_resolution(d), 0);
            break;
        case LV_SCREEN_LOAD_ANIM_OVER_RIGHT:
            lv_anim_set_exec_cb(&a_new, set_x_anim);
            lv_anim_set_values(&a_new, -lv_display_get_horizontal_resolution(d), 0);
            break;
        case LV_SCREEN_LOAD_ANIM_OVER_TOP:
            lv_anim_set_exec_cb(&a_new, set_y_anim);
            lv_anim_set_values(&a_new, lv_display_get_vertical_resolution(d), 0);
            break;
        case LV_SCREEN_LOAD_ANIM_OVER_BOTTOM:
            lv_anim_set_exec_cb(&a_new, set_y_anim);
            lv_anim_set_values(&a_new, -lv_display_get_vertical_resolution(d), 0);
            break;
        case LV_SCREEN_LOAD_ANIM_MOVE_LEFT:
            lv_anim_set_exec_cb(&a_new, set_x_anim);
            lv_anim_set_values(&a_new, lv_display_get_horizontal_resolution(d), 0);

            lv_anim_set_exec_cb(&a_old, set_x_anim);
            lv_anim_set_values(&a_old, 0, -lv_display_get_horizontal_resolution(d));
            break;
        case LV_SCREEN_LOAD_ANIM_MOVE_RIGHT:
            lv_anim_set_exec_cb(&a_new, set_x_anim);
            lv_anim_set_values(&a_new, -lv_display_get_horizontal_resolution(d), 0);

            lv_anim_set_exec_cb(&a_old, set_x_anim);
            lv_anim_set_values(&a_old, 0, lv_display_get_horizontal_resolution(d));
            break;
        case LV_SCREEN_LOAD_ANIM_MOVE_TOP:
            lv_anim_set_exec_cb(&a_new, set_y_anim);
            lv_anim_set_values(&a_new, lv_display_get_vertical_resolution(d), 0);

            lv_anim_set_exec_cb(&a_old, set_y_anim);
            lv_anim_set_values(&a_old, 0, -lv_display_get_vertical_resolution(d));
            break;
        case LV_SCREEN_LOAD_ANIM_MOVE_BOTTOM:
            lv_anim_set_exec_cb(&a_new, set_y_anim);
            lv_anim_set_values(&a_new, -lv_display_get_vertical_resolution(d), 0);

            lv_anim_set_exec_cb(&a_old, set_y_anim);
            lv_anim_set_values(&a_old, 0, lv_display_get_vertical_resolution(d));
            break;
        case LV_SCREEN_LOAD_ANIM_FADE_IN:
            lv_anim_set_exec_cb(&a_new, opa_scale_anim);
            lv_anim_set_values(&a_new, LV_OPA_TRANSP, LV_OPA_COVER);
            break;
        case LV_SCREEN_LOAD_ANIM_FADE_OUT:
            lv_anim_set_exec_cb(&a_old, opa_scale_anim);
            lv_anim_set_values(&a_old, LV_OPA_COVER, LV_OPA_TRANSP);
            break;
        case LV_SCREEN_LOAD_ANIM_OUT_LEFT:
            lv_anim_set_exec_cb(&a_old, set_x_anim);
            lv_anim_set_values(&a_old, 0, -lv_display_get_horizontal_resolution(d));
            break;
        case LV_SCREEN_LOAD_ANIM_OUT_RIGHT:
            lv_anim_set_exec_cb(&a_old, set_x_anim);
            lv_anim_set_values(&a_old, 0, lv_display_get_horizontal_resolution(d));
            break;
        case LV_SCREEN_LOAD_ANIM_OUT_TOP:
            lv_anim_set_exec_cb(&a_old, set_y_anim);
            lv_anim_set_values(&a_old, 0, -lv_display_get_vertical_resolution(d));
            break;
        case LV_SCREEN_LOAD_ANIM_OUT_BOTTOM:
            lv_anim_set_exec_cb(&a_old, set_y_anim);
            lv_anim_set_values(&a_old, 0, lv_display_get_vertical_resolution(d));
            break;
    }

    if(act_scr) lv_obj_send_event(act_scr, LV_EVENT_SCREEN_UNLOAD_START, NULL);

    lv_anim_start(&a_new);
    if(act_scr) lv_anim_start(&a_old);
}

/*---------------------
 * OTHERS
 *--------------------*/

void lv_display_add_event_cb(lv_display_t * disp, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data)
{
    LV_ASSERT_NULL(disp);

    lv_event_add(&disp->event_list, event_cb, filter, user_data);
}

uint32_t lv_display_get_event_count(lv_display_t * disp)
{
    LV_ASSERT_NULL(disp);
    return lv_event_get_count(&disp->event_list);
}

lv_event_dsc_t * lv_display_get_event_dsc(lv_display_t * disp, uint32_t index)
{
    LV_ASSERT_NULL(disp);
    return lv_event_get_dsc(&disp->event_list, index);

}

bool lv_display_delete_event(lv_display_t * disp, uint32_t index)
{
    LV_ASSERT_NULL(disp);

    return lv_event_remove(&disp->event_list, index);
}

uint32_t lv_display_remove_event_cb_with_user_data(lv_display_t * disp, lv_event_cb_t event_cb, void * user_data)
{
    LV_ASSERT_NULL(disp);

    uint32_t event_cnt = lv_display_get_event_count(disp);
    uint32_t removed_count = 0;
    int32_t i;

    for(i = event_cnt - 1; i >= 0; i--) {
        lv_event_dsc_t * dsc = lv_display_get_event_dsc(disp, i);
        if(dsc && dsc->cb == event_cb && dsc->user_data == user_data) {
            lv_display_delete_event(disp, i);
            removed_count ++;
        }
    }

    return removed_count;
}

lv_result_t lv_display_send_event(lv_display_t * disp, lv_event_code_t code, void * param)
{

    lv_event_t e;
    lv_memzero(&e, sizeof(e));
    e.code = code;
    e.current_target = disp;
    e.original_target = disp;
    e.param = param;
    lv_result_t res;
    res = lv_event_send(&disp->event_list, &e, true);
    if(res != LV_RESULT_OK) return res;

    res = lv_event_send(&disp->event_list, &e, false);
    if(res != LV_RESULT_OK) return res;

    return res;
}

lv_area_t * lv_event_get_invalidated_area(lv_event_t * e)
{
    if(e->code == LV_EVENT_INVALIDATE_AREA) {
        return lv_event_get_param(e);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

void lv_display_set_rotation(lv_display_t * disp, lv_display_rotation_t rotation)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    disp->rotation = rotation;
    update_resolution(disp);
}

lv_display_rotation_t lv_display_get_rotation(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return LV_DISPLAY_ROTATION_0;
    return disp->rotation;
}

void lv_display_set_matrix_rotation(lv_display_t * disp, bool enable)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return;

    if(!(disp->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT || disp->render_mode == LV_DISPLAY_RENDER_MODE_FULL)) {
        LV_LOG_WARN("Unsupported rendering mode: %d", disp->render_mode);
        return;
    }

    disp->matrix_rotation = enable;
#else
    (void)disp;
    (void)enable;
    LV_LOG_WARN("LV_DRAW_TRANSFORM_USE_MATRIX was not enabled");
#endif
}

bool lv_display_get_matrix_rotation(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) return false;
    return disp->matrix_rotation;
}

void lv_display_set_theme(lv_display_t * disp, lv_theme_t * th)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered");
        return;
    }

    disp->theme = th;

    if(disp->screen_cnt == 4 &&
       lv_obj_get_child_count(disp->screens[0]) == 0 &&
       lv_obj_get_child_count(disp->screens[1]) == 0 &&
       lv_obj_get_child_count(disp->screens[2]) == 0) {
        lv_theme_apply(disp->screens[0]);
    }
}

lv_theme_t * lv_display_get_theme(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    return disp->theme;
}

uint32_t lv_display_get_inactive_time(const lv_display_t * disp)
{
    if(disp) return lv_tick_elaps(disp->last_activity_time);

    lv_display_t * d;
    uint32_t t = UINT32_MAX;
    d          = lv_display_get_next(NULL);
    while(d) {
        uint32_t elaps = lv_tick_elaps(d->last_activity_time);
        t = LV_MIN(t, elaps);
        d = lv_display_get_next(d);
    }

    return t;
}

void lv_display_trigger_activity(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_display_trigger_activity: no display registered");
        return;
    }

    disp->last_activity_time = lv_tick_get();
}

void lv_display_enable_invalidation(lv_display_t * disp, bool en)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered");
        return;
    }

    disp->inv_en_cnt += en ? 1 : -1;
}

bool lv_display_is_invalidation_enabled(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered");
        return false;
    }

    return (disp->inv_en_cnt > 0);
}

lv_timer_t * lv_display_get_refr_timer(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return NULL;

    return disp->refr_timer;
}

void lv_display_delete_refr_timer(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp || !disp->refr_timer) return;

    lv_timer_delete(disp->refr_timer);
    disp->refr_timer = NULL;
}

lv_result_t lv_display_send_vsync_event(lv_display_t * disp, void * param)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return LV_RESULT_INVALID;

    if(disp->vsync_count > 0)
        return lv_display_send_event(disp, LV_EVENT_VSYNC, param);

    return LV_RESULT_INVALID;
}

bool lv_display_register_vsync_event(lv_display_t * disp, lv_event_cb_t event_cb, void * user_data)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return false;

    lv_display_add_event_cb(disp, event_cb, LV_EVENT_VSYNC, user_data);

    /*only send once*/
    if(disp->vsync_count == 0)
        lv_display_send_event(disp, LV_EVENT_VSYNC_REQUEST, disp);

    disp->vsync_count++;
    return true;
}

bool lv_display_unregister_vsync_event(lv_display_t * disp, lv_event_cb_t event_cb, void * user_data)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return false;

    uint32_t removed_count = lv_display_remove_event_cb_with_user_data(disp, event_cb, user_data);
    if(removed_count == 0)
        return false;

    disp->vsync_count -= removed_count;
    /*only send once*/
    if(disp->vsync_count == 0)
        lv_display_send_event(disp, LV_EVENT_VSYNC_REQUEST, NULL);

    return true;
}

void lv_display_set_user_data(lv_display_t * disp, void * user_data)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return;
    disp->user_data = user_data;
}

void lv_display_set_driver_data(lv_display_t * disp, void * driver_data)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return;

    disp->driver_data = driver_data;
}

void * lv_display_get_user_data(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return NULL;
    return disp->user_data;
}

void * lv_display_get_driver_data(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return NULL;

    return disp->driver_data;
}

lv_draw_buf_t * lv_display_get_buf_active(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return NULL;
    return disp->buf_act;
}

void lv_display_rotate_area(lv_display_t * disp, lv_area_t * area)
{
    lv_display_rotation_t rotation = lv_display_get_rotation(disp);

    if(rotation == LV_DISPLAY_ROTATION_0) return;

    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    switch(rotation) {
        case LV_DISPLAY_ROTATION_90:
            area->y2 = disp->ver_res - area->x1 - 1;
            area->x1 = area->y1;
            area->x2 = area->x1 + h - 1;
            area->y1 = area->y2 - w + 1;
            break;
        case LV_DISPLAY_ROTATION_180:
            area->y2 = disp->ver_res - area->y1 - 1;
            area->y1 = area->y2 - h + 1;
            area->x2 = disp->hor_res - area->x1 - 1;
            area->x1 = area->x2 - w + 1;
            break;
        case LV_DISPLAY_ROTATION_270:
            area->x1 = disp->hor_res - area->y2 - 1;
            area->y2 = area->x2;
            area->x2 = area->x1 + h - 1;
            area->y1 = area->y2 - w + 1;
            break;
        default:
            break;
    }
}

uint32_t lv_display_get_draw_buf_size(lv_display_t * disp)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return 0;

    if(disp->buf_1) {
        return disp->buf_1->data_size;
    }
    return 0;
}

uint32_t lv_display_get_invalidated_draw_buf_size(lv_display_t * disp, uint32_t width, uint32_t height)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return 0;

    if(disp->render_mode == LV_DISPLAY_RENDER_MODE_FULL) {
        width = lv_display_get_horizontal_resolution(disp);
        height = lv_display_get_vertical_resolution(disp);
    }

    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
    uint32_t buf_size = stride * height;

    LV_ASSERT(disp->buf_1 && disp->buf_1->data_size >= buf_size);
    if(disp->buf_2) LV_ASSERT(disp->buf_2->data_size >= buf_size);
    if(disp->buf_3) LV_ASSERT(disp->buf_3->data_size >= buf_size);

    return buf_size;
}

lv_obj_t * lv_screen_active(void)
{
    return lv_display_get_screen_active(lv_display_get_default());
}

lv_obj_t * lv_layer_top(void)
{
    return lv_display_get_layer_top(lv_display_get_default());
}

lv_obj_t * lv_layer_sys(void)
{
    return lv_display_get_layer_sys(lv_display_get_default());
}

lv_obj_t * lv_layer_bottom(void)
{
    return lv_display_get_layer_bottom(lv_display_get_default());
}

int32_t lv_dpx(int32_t n)
{
    return LV_DPX(n);
}

int32_t lv_display_dpx(const lv_display_t * disp, int32_t n)
{
    return LV_DPX_CALC(lv_display_get_dpi(disp), n);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void update_resolution(lv_display_t * disp)
{
    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);

    lv_area_t prev_coords;
    lv_obj_get_coords(disp->sys_layer, &prev_coords);
    uint32_t i;
    for(i = 0; i < disp->screen_cnt; i++) {
        lv_area_set_width(&disp->screens[i]->coords, hor_res);
        lv_area_set_height(&disp->screens[i]->coords, ver_res);
        lv_obj_send_event(disp->screens[i], LV_EVENT_SIZE_CHANGED, &prev_coords);
    }

    lv_area_set_width(&disp->top_layer->coords, hor_res);
    lv_area_set_height(&disp->top_layer->coords, ver_res);
    lv_obj_send_event(disp->top_layer, LV_EVENT_SIZE_CHANGED, &prev_coords);

    lv_area_set_width(&disp->sys_layer->coords, hor_res);
    lv_area_set_height(&disp->sys_layer->coords, ver_res);
    lv_obj_send_event(disp->sys_layer, LV_EVENT_SIZE_CHANGED, &prev_coords);

    lv_area_set_width(&disp->bottom_layer->coords, hor_res);
    lv_area_set_height(&disp->bottom_layer->coords, ver_res);
    lv_obj_send_event(disp->bottom_layer, LV_EVENT_SIZE_CHANGED, &prev_coords);

    lv_memzero(disp->inv_areas, sizeof(disp->inv_areas));
    lv_memzero(disp->inv_area_joined, sizeof(disp->inv_area_joined));
    disp->inv_p = 0;
    lv_obj_invalidate(disp->sys_layer);

    lv_obj_tree_walk(NULL, invalidate_layout_cb, NULL);

    lv_display_send_event(disp, LV_EVENT_RESOLUTION_CHANGED, NULL);
}

static lv_obj_tree_walk_res_t invalidate_layout_cb(lv_obj_t * obj, void * user_data)
{
    LV_UNUSED(user_data);
    lv_obj_mark_layout_as_dirty(obj);
    return LV_OBJ_TREE_WALK_NEXT;
}

static void scr_load_internal(lv_obj_t * scr)
{
    /*scr must not be NULL, but d->act_scr might be*/
    LV_ASSERT_NULL(scr);
    if(scr == NULL) return;

    lv_display_t * d = lv_obj_get_display(scr);
    if(!d) return;  /*Shouldn't happen, just to be sure*/

    lv_obj_t * old_scr = d->act_scr;

    if(old_scr) lv_obj_send_event(old_scr, LV_EVENT_SCREEN_UNLOAD_START, NULL);
    lv_obj_send_event(scr, LV_EVENT_SCREEN_LOAD_START, NULL);

    d->act_scr = scr;
    d->scr_to_load = NULL;

    lv_obj_send_event(scr, LV_EVENT_SCREEN_LOADED, NULL);
    if(old_scr) lv_obj_send_event(old_scr, LV_EVENT_SCREEN_UNLOADED, NULL);

    lv_obj_invalidate(scr);
}

static void scr_load_anim_start(lv_anim_t * a)
{
    lv_display_t * d = lv_obj_get_display(a->var);

    d->prev_scr = d->act_scr;
    d->act_scr = a->var;

    lv_obj_send_event(d->act_scr, LV_EVENT_SCREEN_LOAD_START, NULL);
}

static void opa_scale_anim(void * obj, int32_t v)
{
    lv_obj_set_style_opa(obj, v, 0);
}

static void set_x_anim(void * obj, int32_t v)
{
    lv_obj_set_x(obj, v);
}

static void set_y_anim(void * obj, int32_t v)
{
    lv_obj_set_y(obj, v);
}

static void scr_anim_completed(lv_anim_t * a)
{
    lv_display_t * d = lv_obj_get_display(a->var);

    lv_obj_send_event(d->act_scr, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_send_event(d->prev_scr, LV_EVENT_SCREEN_UNLOADED, NULL);

    if(d->prev_scr && d->del_prev) lv_obj_delete(d->prev_scr);
    d->prev_scr = NULL;
    d->draw_prev_over_act = false;
    d->scr_to_load = NULL;
    lv_obj_remove_local_style_prop(a->var, LV_STYLE_OPA, 0);
    lv_obj_invalidate(d->act_scr);
}

static bool is_out_anim(lv_screen_load_anim_t anim_type)
{
    return anim_type == LV_SCREEN_LOAD_ANIM_FADE_OUT  ||
           anim_type == LV_SCREEN_LOAD_ANIM_OUT_LEFT  ||
           anim_type == LV_SCREEN_LOAD_ANIM_OUT_RIGHT ||
           anim_type == LV_SCREEN_LOAD_ANIM_OUT_TOP   ||
           anim_type == LV_SCREEN_LOAD_ANIM_OUT_BOTTOM;
}

static void disp_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_display_t * disp = lv_event_get_target(e);
    switch(code) {
        case LV_EVENT_REFR_REQUEST:
            if(disp->refr_timer) lv_timer_resume(disp->refr_timer);
            break;

        default:
            break;
    }
}
