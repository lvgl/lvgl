/**
 * @file lv_hal_disp.c
 *
 * @description HAL layer for display driver
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>
#include "lv_hal.h"
#include "../misc/lv_mem.h"
#include "../misc/lv_gc.h"
#include "../misc/lv_assert.h"
#include "../core/lv_obj.h"
#include "../core/lv_refr.h"
#include "../core/lv_theme.h"
#include "../draw/sdl/lv_draw_sdl.h"
#include "../draw/sw/lv_draw_sw.h"
#include "../draw/sdl/lv_draw_sdl.h"
#if LV_USE_GPU_STM32_DMA2D
    #include "../draw/stm32_dma2d/lv_gpu_stm32_dma2d.h"
#endif

#if LV_USE_GPU_SWM341_DMA2D
    #include "../draw/swm341_dma2d/lv_gpu_swm341_dma2d.h"
#endif

#if LV_USE_GPU_ARM2D
    #include "../draw/arm2d/lv_gpu_arm2d.h"
#endif

#if LV_USE_GPU_NXP_PXP || LV_USE_GPU_NXP_VG_LITE
    #include "../draw/nxp/lv_gpu_nxp.h"
#endif

#if LV_USE_THEME_DEFAULT
    #include "../themes/default/lv_theme_default.h"
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
static lv_obj_tree_walk_res_t invalidate_layout_cb(lv_obj_t * obj, void * user_data);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_t * disp_def;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize a display driver with default values.
 * It is used to ensure all fields have known values and not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
lv_disp_t * lv_disp_create(void)
{
    lv_disp_t * disp = _lv_ll_ins_head(&LV_GC_ROOT(_lv_disp_ll));
    LV_ASSERT_MALLOC(disp);
    if(!disp) return NULL;

    lv_disp_init(disp);

    return disp;
}

void lv_disp_init(lv_disp_t * disp)
{
    lv_memzero(disp, sizeof(lv_disp_t));

    disp->hor_res          = 320;
    disp->ver_res          = 240;
    disp->physical_hor_res = -1;
    disp->physical_ver_res = -1;
    disp->offset_x         = 0;
    disp->offset_y         = 0;
    disp->antialiasing     = LV_COLOR_DEPTH > 8 ? 1 : 0;
    disp->screen_transp    = 0;
    disp->dpi              = LV_DPI_DEF;
    disp->color_chroma_key = LV_COLOR_CHROMA_KEY;

#if LV_COLOR_DEPTH == 1
    disp->color_format = LV_COLOR_FORMAT_L1;
#elif LV_COLOR_DEPTH == 8
    disp->color_format = LV_COLOR_FORMAT_L8;
#else
    disp->color_format = LV_COLOR_FORMAT_NATIVE;
#endif

#if LV_USE_GPU_STM32_DMA2D
    disp->draw_ctx_init = lv_draw_stm32_dma2d_ctx_init;
    disp->draw_ctx_deinit = lv_draw_stm32_dma2d_ctx_deinit;
    disp->draw_ctx_size = sizeof(lv_draw_stm32_dma2d_ctx_t);
#elif LV_USE_GPU_SWM341_DMA2D
    disp->draw_ctx_init = lv_draw_swm341_dma2d_ctx_init;
    disp->draw_ctx_deinit = lv_draw_swm341_dma2d_ctx_deinit;
    disp->draw_ctx_size = sizeof(lv_draw_swm341_dma2d_ctx_t);
#elif LV_USE_GPU_NXP_PXP || LV_USE_GPU_NXP_VG_LITE
    disp->draw_ctx_init = lv_draw_nxp_ctx_init;
    disp->draw_ctx_deinit = lv_draw_nxp_ctx_deinit;
    disp->draw_ctx_size = sizeof(lv_draw_nxp_ctx_t);
#elif LV_USE_DRAW_SDL
    disp->draw_ctx_init = lv_draw_sdl_init_ctx;
    disp->draw_ctx_deinit = lv_draw_sdl_deinit_ctx;
    disp->draw_ctx_size = sizeof(lv_draw_sdl_ctx_t);
#elif LV_USE_GPU_ARM2D
    disp->draw_ctx_init = lv_draw_arm2d_ctx_init;
    disp->draw_ctx_deinit = lv_draw_arm2d_ctx_deinit;
    disp->draw_ctx_size = sizeof(lv_draw_arm2d_ctx_t);
#else
    disp->draw_ctx_init = lv_draw_sw_init_ctx;
    disp->draw_ctx_deinit = lv_draw_sw_deinit_ctx;
    disp->draw_ctx_size = sizeof(lv_draw_sw_ctx_t);
#endif

    /*Create a draw context if not created yet*/
    if(disp->draw_ctx == NULL) {
        lv_draw_ctx_t * draw_ctx = lv_malloc(disp->draw_ctx_size);
        LV_ASSERT_MALLOC(draw_ctx);
        if(draw_ctx == NULL) return;
        disp->draw_ctx_init(disp, draw_ctx);
        disp->draw_ctx = draw_ctx;
    }

    disp->draw_ctx->color_format = disp->color_format;
    disp->draw_ctx->render_with_alpha = disp->screen_transp;

    disp->inv_en_cnt = 1;

    lv_disp_t * disp_def_tmp = disp_def;
    disp_def                 = disp; /*Temporarily change the default screen to create the default screens on the
                                        new display*/
    /*Create a refresh timer*/
    disp->refr_timer = lv_timer_create(_lv_disp_refr_timer, LV_DEF_REFR_PERIOD, disp);
    LV_ASSERT_MALLOC(disp->refr_timer);
    if(disp->refr_timer == NULL) {
        lv_free(disp);
        return;
    }

    if(disp->full_refresh && disp->draw_buf_size < (uint32_t)disp->hor_res * disp->ver_res) {
        disp->full_refresh = 0;
        LV_LOG_WARN("full_refresh requires at least screen sized draw buffer(s)");
    }

    disp->bg_color = lv_color_white();
    disp->bg_opa = LV_OPA_COVER;

#if LV_USE_THEME_DEFAULT
    if(lv_theme_default_is_inited() == false) {
        disp->theme = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                            LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
    }
    else {
        disp->theme = lv_theme_default_get();
    }
#endif

    disp->act_scr   = lv_obj_create(NULL); /*Create a default screen on the display*/
    disp->top_layer = lv_obj_create(NULL); /*Create top layer on the display*/
    disp->sys_layer = lv_obj_create(NULL); /*Create sys layer on the display*/
    lv_obj_remove_style_all(disp->top_layer);
    lv_obj_remove_style_all(disp->sys_layer);
    lv_obj_clear_flag(disp->top_layer, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(disp->sys_layer, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_scrollbar_mode(disp->top_layer, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(disp->sys_layer, LV_SCROLLBAR_MODE_OFF);

    lv_obj_invalidate(disp->act_scr);

    disp_def = disp_def_tmp; /*Revert the default display*/
    if(disp_def == NULL) disp_def = disp; /*Initialize the default display*/

    lv_timer_ready(disp->refr_timer); /*Be sure the screen will be refreshed immediately on start up*/

    return;
}

/**
 * Update the driver in run time.
 * @param disp pointer to a display. (return value of `lv_disp_drv_register`)
 * @param new_drv pointer to the new driver
 */
void lv_disp_drv_update(lv_disp_t * disp)
{
    //    disp->driver = new_drv;
    //
    //    if(disp->driver->full_refresh &&
    //       disp->driver->draw_buf->draw_buf_size < (uint32_t)disp->driver->hor_res * disp->driver->ver_res) {
    //        disp->driver->full_refresh = 0;
    //        LV_LOG_WARN("full_refresh requires at least screen sized draw buffer(s)");
    //    }
    //
    //    lv_coord_t w = lv_disp_get_hor_res(disp);
    //    lv_coord_t h = lv_disp_get_ver_res(disp);
    //    uint32_t i;
    //    for(i = 0; i < disp->screen_cnt; i++) {
    //        lv_area_t prev_coords;
    //        lv_obj_get_coords(disp->screens[i], &prev_coords);
    //        lv_area_set_width(&disp->screens[i]->coords, w);
    //        lv_area_set_height(&disp->screens[i]->coords, h);
    //        lv_event_send(disp->screens[i], LV_EVENT_SIZE_CHANGED, &prev_coords);
    //    }
    //
    //    /*
    //     * This method is usually called upon orientation change, thus the screen is now a
    //     * different size.
    //     * The object invalidated its previous area. That area is now out of the screen area
    //     * so we reset all invalidated areas and invalidate the active screen's new area only.
    //     */
    //    lv_memzero(disp->inv_areas, sizeof(disp->inv_areas));
    //    lv_memzero(disp->inv_area_joined, sizeof(disp->inv_area_joined));
    //    disp->inv_p = 0;
    //    if(disp->act_scr != NULL) lv_obj_invalidate(disp->act_scr);
    //
    //    lv_obj_tree_walk(NULL, invalidate_layout_cb, NULL);
    //
    //    if(disp->driver->drv_update_cb) disp->driver->drv_update_cb(disp->driver);
}

/**
 * Remove a display
 * @param disp pointer to display
 */
void lv_disp_remove(lv_disp_t * disp)
{
    //    bool was_default = false;
    //    if(disp == lv_disp_get_default()) was_default = true;
    //
    //    /*Detach the input devices*/
    //    lv_indev_t * indev;
    //    indev = lv_indev_get_next(NULL);
    //    while(indev) {
    //        if(indev->driver->disp == disp) {
    //            indev->driver->disp = NULL;
    //        }
    //        indev = lv_indev_get_next(indev);
    //    }
    //
    //    /** delete screen and other obj */
    //    if(disp->sys_layer) {
    //        lv_obj_del(disp->sys_layer);
    //        disp->sys_layer = NULL;
    //    }
    //    if(disp->top_layer) {
    //        lv_obj_del(disp->top_layer);
    //        disp->top_layer = NULL;
    //    }
    //    while(disp->screen_cnt != 0) {
    //        /*Delete the screenst*/
    //        lv_obj_del(disp->screens[0]);
    //    }
    //
    //    _lv_ll_remove(&LV_GC_ROOT(_lv_disp_ll), disp);
    //    if(disp->refr_timer) lv_timer_del(disp->refr_timer);
    //    lv_free(disp);
    //
    //    if(was_default) lv_disp_set_default(_lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll)));
}

/**
 * Set a default display. The new screens will be created on it by default.
 * @param disp pointer to a display
 */
void lv_disp_set_default(lv_disp_t * disp)
{
    disp_def = disp;
}

/**
 * Get the default display
 * @return pointer to the default display
 */
lv_disp_t * lv_disp_get_default(void)
{
    return disp_def;
}

/**
 * Get the horizontal resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the horizontal resolution of the display
 */
lv_coord_t lv_disp_get_hor_res(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotated) {
            case LV_DISP_ROTATION_90:
            case LV_DISP_ROTATION_270:
                return disp->ver_res;
            default:
                return disp->hor_res;
        }
    }
}

/**
 * Get the vertical resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the vertical resolution of the display
 */
lv_coord_t lv_disp_get_ver_res(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotated) {
            case LV_DISP_ROTATION_90:
            case LV_DISP_ROTATION_270:
                return disp->hor_res;
            default:
                return disp->ver_res;
        }
    }
}

/**
 * Get the full / physical horizontal resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the full / physical horizontal resolution of the display
 */
lv_coord_t lv_disp_get_physical_hor_res(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotated) {
            case LV_DISP_ROTATION_90:
            case LV_DISP_ROTATION_270:
                return disp->physical_ver_res > 0 ? disp->physical_ver_res : disp->ver_res;
            default:
                return disp->physical_hor_res > 0 ? disp->physical_hor_res : disp->hor_res;
        }
    }
}

/**
 * Get the full / physical vertical resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the full / physical vertical resolution of the display
 */
lv_coord_t lv_disp_get_physical_ver_res(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotated) {
            case LV_DISP_ROTATION_90:
            case LV_DISP_ROTATION_270:
                return disp->physical_hor_res > 0 ? disp->physical_hor_res : disp->hor_res;
            default:
                return disp->physical_ver_res > 0 ? disp->physical_ver_res : disp->ver_res;
        }
    }
}

/**
 * Get the horizontal offset from the full / physical display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the horizontal offset from the full / physical display
 */
lv_coord_t lv_disp_get_offset_x(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotated) {
            case LV_DISP_ROTATION_90:
                return disp->offset_y;
            case LV_DISP_ROTATION_180:
                return lv_disp_get_physical_hor_res(disp) - disp->offset_x;
            case LV_DISP_ROTATION_270:
                return lv_disp_get_physical_hor_res(disp) - disp->offset_y;
            default:
                return disp->offset_x;
        }
    }
}

/**
 * Get the vertical offset from the full / physical display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the horizontal offset from the full / physical display
 */
lv_coord_t lv_disp_get_offset_y(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();

    if(disp == NULL) {
        return 0;
    }
    else {
        switch(disp->rotated) {
            case LV_DISP_ROTATION_90:
                return disp->offset_x;
            case LV_DISP_ROTATION_180:
                return lv_disp_get_physical_ver_res(disp) - disp->offset_y;
            case LV_DISP_ROTATION_270:
                return lv_disp_get_physical_ver_res(disp) - disp->offset_x;
            default:
                return disp->offset_y;
        }
    }
}

/**
 * Get if anti-aliasing is enabled for a display or not
 * @param disp pointer to a display (NULL to use the default display)
 * @return true: anti-aliasing is enabled; false: disabled
 */
bool lv_disp_get_antialiasing(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();
    if(disp == NULL) return false;

    return disp->antialiasing ? true : false;
}

/**
 * Get the DPI of the display
 * @param disp pointer to a display (NULL to use the default display)
 * @return dpi of the display
 */
lv_coord_t lv_disp_get_dpi(const lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();
    if(disp == NULL) return LV_DPI_DEF;  /*Do not return 0 because it might be a divider*/
    return disp->dpi;
}

/**
 * Call in the display driver's `flush_cb` function when the flushing is finished
 * @param disp_drv pointer to display driver in `flush_cb` where this function is called
 */
LV_ATTRIBUTE_FLUSH_READY void lv_disp_flush_ready(lv_disp_t * disp)
{
    disp->flushing = 0;
    disp->flushing_last = 0;
}

/**
 * Tell if it's the last area of the refreshing process.
 * Can be called from `flush_cb` to execute some special display refreshing if needed when all areas area flushed.
 * @param disp_drv pointer to display driver
 * @return true: it's the last area to flush; false: there are other areas too which will be refreshed soon
 */
LV_ATTRIBUTE_FLUSH_READY bool lv_disp_flush_is_last(lv_disp_t * disp)
{
    return disp->flushing_last;
}

/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
lv_disp_t * lv_disp_get_next(lv_disp_t * disp)
{
    if(disp == NULL)
        return _lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll));
    else
        return _lv_ll_get_next(&LV_GC_ROOT(_lv_disp_ll), disp);
}

/**
 * Set the rotation of this display.
 * @param disp pointer to a display (NULL to use the default display)
 * @param rotation rotation angle
 */
void lv_disp_set_rotation(lv_disp_t * disp, lv_disp_rotation_t rotation)
{
    if(disp == NULL) disp = lv_disp_get_default();
    if(disp == NULL) return;

    disp->rotated = rotation;
    //    lv_disp_drv_update(disp, disp->driver);
}

/**
 * Get the current rotation of this display.
 * @param disp pointer to a display (NULL to use the default display)
 * @return rotation angle
 */
lv_disp_rotation_t lv_disp_get_rotation(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();
    if(disp == NULL) return LV_DISP_ROTATION_NONE;
    return disp->rotated;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_tree_walk_res_t invalidate_layout_cb(lv_obj_t * obj, void * user_data)
{
    LV_UNUSED(user_data);
    lv_obj_mark_layout_as_dirty(obj);
    return LV_OBJ_TREE_WALK_NEXT;
}
