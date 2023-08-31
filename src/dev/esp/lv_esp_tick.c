/**
 * @file lv_esp_lcd.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_esp_lcd.h"
#if LV_USE_ESP_LCD

#include <lvgl/lvgl.h>
#include "../../../lvgl_private.h"

#include "esp_timer.h"


/*********************
 *      DEFINES
 *********************/
#ifndef LV_ESP_LCD_COLOR_IN_PSRAM
    #define LV_ESP_LCD_COLOR_IN_PSRAM 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    //    lv_disp_t                 * disp;
    esp_timer_handle_t      tick_timer;
    esp_lcd_panel_handle_t  panel_handle;
} lv_esp_lcd_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/

static void flush_cb(lv_disp_t * disp, const lv_area_t * area_p, uint8_t * color_p);
static void res_chg_event_cb(lv_event_t * e);
static void tick_timer_cb(void * arg);


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_disp_t * lv_esp_lcd_create(uint32_t hor_res, uint32_t ver_res, esp_lcd_panel_handle_t panel_handle)
{
    lv_esp_lcd_t * dsc = (lv_esp_lcd_t *)lv_malloc(sizeof(lv_esp_lcd_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;
    lv_memzero(dsc, sizeof(lv_esp_lcd_t));

    lv_disp_t * disp = lv_disp_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }

    dsc->panel_handle = panel_handle;
    lv_disp_set_driver_data(disp, (void *)dsc);
    lv_disp_set_flush_cb(disp, flush_cb);
    lv_disp_add_event(disp, res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    /*Allocate draw buffers*/

    lv_color_t * draw_buf = NULL;
    lv_color_t * draw_buf_2 = NULL;

    /*
    #if CONFIG_LCD_I80_COLOR_IN_PSRAM
        lv_color_t *buf1 = heap_caps_aligned_alloc(PSRAM_DATA_ALIGNMENT, LCD_H_RES * BUF_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    #else
        lv_color_t *buf1 = heap_caps_malloc(LCD_H_RES * BUF_V_RES * 2, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    #endif
        assert(buf1);
    #if CONFIG_LCD_I80_COLOR_IN_PSRAM
        lv_color_t *buf2 = heap_caps_aligned_alloc(PSRAM_DATA_ALIGNMENT, LCD_H_RES * BUF_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    #else
        lv_color_t *buf2 = heap_caps_malloc(LCD_H_RES * BUF_V_RES * 2, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    #endif
        assert(buf2);
     */

    uint32_t px_size = lv_color_format_get_size(lv_disp_get_color_format(disp));
#if LV_DISP_BUFFER_COUNT > 0
    uint32_t buf_size = hor_res * ver_res * px_size;
    lv_disp_render_mode_t render_mode = LV_DISP_RENDER_MODE_FULL;
#else
    uint32_t buf_size = hor_res * LV_ESP_LCD_BUFFER_SIZE * px_size;
    lv_disp_render_mode_t render_mode = LV_DISP_RENDER_MODE_PARTIAL;
#endif

    draw_buf = heap_caps_aligned_alloc(64, buf_size,
                                       LV_ESP_LCD_COLOR_IN_PSRAM ? MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT : MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if(draw_buf == NULL) {
        LV_LOG_ERROR("display draw_buf malloc failed");
        lv_free(lcd);
        return NULL;
    }

#if LV_ESP_LCD_BUFFER_COUNT == 2
    draw_buf_2 = heap_caps_aligned_alloc(64, buf_size,
                                         LV_ESP_LCD_COLOR_IN_PSRAM ? MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT : MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if(draw_buf_2 == NULL) {
        LV_LOG_ERROR("display draw_buf_2 malloc failed");
        lv_free(lcd);
        heap_caps_aligned_free(draw_buf);
        return NULL;
    }
#endif

    lv_disp_set_draw_buffers(disp, draw_buf, draw_buf_2, buf_size, render_mode);

    return disp;
}

bool lv_esp_lcd_notify_color_trans_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t * edata,
                                        void * user_ctx)
{
    lv_disp_t * drv = *((lv_disp_t **)user_ctx);
    lv_disp_flush_ready(drv);
    return false;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void swap16_buffer(uint8_t * buf, int length)
{
    uint8_t temp = 0;
    while(length > 0) {
        temp = *buf;
        *buf = *(buf + 1);
        buf++;
        *buf++ = temp;
        length--;
    }
}

static void flush_cb(lv_disp_t * disp, const lv_area_t * area_p, uint8_t * color_p)
{
    esp_lcd_panel_handle_t panel_handle = ((lv_esp_lcd_t *) lv_disp_get_driver_data(disp))->panel_handle;
    /*NOTE: this is just a temporary solution to swap the bytes of the buffer*/
    swap16_buffer(px_map, lv_area_get_size(area));
    /*Copy a buffer's content to a specific area of the display*/
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, px_map);
}


static void tick_timer_cb(void * arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}


#if 0

/* Rotate display and touch, when rotated screen in LVGL. Called when driver parameters are updated. */
static void lvgl_port_update_callback(lv_disp_drv_t * drv)
{
#if 0
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    switch(drv->rotated) {
        case LV_DISP_ROT_NONE:
            // Rotate LCD display
            esp_lcd_panel_swap_xy(panel_handle, false);
            esp_lcd_panel_mirror(panel_handle, true, false);
#if CONFIG_LCD_TOUCH_ENABLED
            // Rotate LCD touch
            esp_lcd_touch_set_mirror_y(tp, false);
            esp_lcd_touch_set_mirror_x(tp, false);
#endif
            break;
        case LV_DISP_ROT_90:
            // Rotate LCD display
            esp_lcd_panel_swap_xy(panel_handle, true);
            esp_lcd_panel_mirror(panel_handle, true, true);
#if CONFIG_LCD_TOUCH_ENABLED
            // Rotate LCD touch
            esp_lcd_touch_set_mirror_y(tp, false);
            esp_lcd_touch_set_mirror_x(tp, false);
#endif
            break;
        case LV_DISP_ROT_180:
            // Rotate LCD display
            esp_lcd_panel_swap_xy(panel_handle, false);
            esp_lcd_panel_mirror(panel_handle, false, true);
#if CONFIG_LCD_TOUCH_ENABLED
            // Rotate LCD touch
            esp_lcd_touch_set_mirror_y(tp, false);
            esp_lcd_touch_set_mirror_x(tp, false);
#endif
            break;
        case LV_DISP_ROT_270:
            // Rotate LCD display
            esp_lcd_panel_swap_xy(panel_handle, true);
            esp_lcd_panel_mirror(panel_handle, false, false);
#if CONFIG_LCD_TOUCH_ENABLED
            // Rotate LCD touch
            esp_lcd_touch_set_mirror_y(tp, false);
            esp_lcd_touch_set_mirror_x(tp, false);
#endif
            break;
    }
#endif
}

#endif

static void res_chg_event_cb(lv_event_t * e)
{
    lv_disp_t * disp = lv_event_get_target(e);

    int32_t hor_res = lv_disp_get_hor_res(disp);
    int32_t ver_res = lv_disp_get_ver_res(disp);

    esp_lcd_panel_handle_t panel_handle = ((lv_esp_lcd_t *) lv_disp_get_driver_data(disp))->panel_handle;
    /*
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    if(dsc->ignore_size_chg == false) {
        SDL_SetWindowSize(dsc->window, hor_res * dsc->zoom, ver_res * dsc->zoom);
    }

    texture_resize(disp);
    */
}

#endif /*LV_USE_ESP_LCD*/
