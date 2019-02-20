
/**
 * @file hal_disp.c
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
#include "../lv_misc/lv_mem.h"
#include "../lv_core/lv_obj.h"
#include "../lv_misc/lv_gc.h"

#if defined(LV_GC_INCLUDE)
#   include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */


/*********************
 *      DEFINES
 *********************/
#ifndef LV_ATTRIBUTE_FLUSH_READY
#  define LV_ATTRIBUTE_FLUSH_READY
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

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
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void lv_disp_drv_init(lv_disp_drv_t * driver)
{
    memset(driver, 0, sizeof(lv_disp_drv_t));

    driver->disp_flush = NULL;
    driver->hor_res = LV_HOR_RES_MAX;
    driver->ver_res = LV_VER_RES_MAX;
    driver->buffer = NULL;

#if USE_LV_GPU
    driver->mem_blend = NULL;
    driver->mem_fill = NULL;
#endif

    driver->vdb_wr = NULL;
}


/**
 * Initialize a display buffer
 * @param disp_buf pointer `lv_disp_buf_t` variable to initialize
 * @param buf1 A buffer to be used by LittlevGL to draw the image.
 *             Always has to specified and can't be NULL.
 *             Can be an array allocated by the user. E.g. `static lv_color_t disp_buf1[1024 * 10]`
 *             Or a memory address e.g. in external SRAM
 * @param buf2 Optionally specify a second buffer to make image rendering and image flushing
 *             (sending to the display) parallel.
 *             In the `disp_drv->flush` you should use DMA or similar hardware to send
 *             the image to the display in the background.
 *             It lets LittlevGL to render next frame into the other buffer while previous is being sent.
 *             Set to `NULL` if unused.
 * @param size size of the `buf1` and `buf2` in pixel count.
 */
void lv_disp_buf_init(lv_disp_buf_t * disp_buf, void * buf1, void * buf2, uint32_t size)
{
    memset(disp_buf, 0, sizeof(lv_disp_buf_t));

    disp_buf->buf1 = buf1;
    disp_buf->buf2 = buf2;
    disp_buf->buf_act = disp_buf->buf1;
    disp_buf->size = size;
}


/**
 * Register an initialized display driver.
 * Automatically set the first display as active.
 * @param driver pointer to an initialized 'lv_disp_drv_t' variable (can be local variable)
 * @return pointer to the new display or NULL on error
 */
lv_disp_t * lv_disp_drv_register(lv_disp_drv_t * driver)
{
    lv_disp_t * disp = lv_ll_ins_head(&LV_GC_ROOT(_lv_disp_ll));
    if(!disp) {
        lv_mem_assert(disp);
        return NULL;
    }

    memcpy(&disp->driver, driver, sizeof(lv_disp_drv_t));

    lv_ll_init(&disp->scr_ll, sizeof(lv_obj_t));

    if(disp_def == NULL) disp_def = disp;

    disp->act_scr = lv_obj_create(NULL, NULL);  /*Create a default screen on the display*/
    disp->top_layer = lv_obj_create(NULL, NULL);  /*Create top layer on the display*/
    disp->sys_layer = lv_obj_create(NULL, NULL);  /*Create top layer on the display*/
    lv_obj_set_style(disp->top_layer, &lv_style_transp);
    lv_obj_set_style(disp->sys_layer, &lv_style_transp);

    lv_disp_assign_screen(disp, disp->act_scr);
    lv_disp_assign_screen(disp, disp->top_layer);
    lv_disp_assign_screen(disp, disp->sys_layer);

    disp->inv_p = 0;
    disp->vdb_act = 0;
    disp->vdb_flushing = 0;

    lv_obj_invalidate(disp->act_scr);


    return disp;
}

/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
lv_disp_t * lv_disp_get_next(lv_disp_t * disp)
{
    if(disp == NULL) return lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll));
    else return lv_ll_get_next(&LV_GC_ROOT(_lv_disp_ll), disp);
}


void lv_disp_set_default(lv_disp_t * disp)
{
    disp_def = disp;
}


lv_disp_t * lv_disp_get_default(void)
{
    return disp_def;
}

lv_disp_buf_t * lv_disp_get_vdb(lv_disp_t * disp)
{
    return disp->driver.buffer;
}

lv_coord_t lv_disp_get_hor_res(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();

    if(disp == NULL) return LV_HOR_RES_MAX;
    else return disp->driver.hor_res;
}


lv_coord_t lv_disp_get_ver_res(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_default();

    if(disp == NULL) return LV_VER_RES_MAX;
    else return disp->driver.ver_res;
}

bool lv_disp_is_double_vdb(lv_disp_t * disp)
{
    if(disp->driver.buffer->buf1 && disp->driver.buffer->buf2) return true;
    else return false;
}

bool lv_disp_is_true_double_buffered(lv_disp_t * disp)
{
    if(lv_disp_is_double_vdb(disp) && disp->driver.buffer->size == disp->driver.hor_res * disp->driver.ver_res) return true;
    else return false;
}


/**
 * Call in the display driver's `flush` function when the flushing is finished
 */
LV_ATTRIBUTE_FLUSH_READY void lv_disp_flush_ready(lv_disp_t * disp)
{
    disp->driver.buffer->flushing = 0;

    /*If the screen is transparent initialize it when the flushing is ready*/
#if LV_COLOR_SCREEN_TRANSP
    memset(vdb_buf, 0x00, LV_VDB_SIZE_IN_BYTES);
#endif
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

