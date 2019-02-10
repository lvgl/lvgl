
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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_t * active;

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
    driver->disp_fill = NULL;
    driver->disp_map = NULL;
    driver->disp_flush = NULL;
    driver->hor_res = LV_HOR_RES_MAX;
    driver->ver_res = LV_VER_RES_MAX;

#if USE_LV_GPU
    driver->mem_blend = NULL;
    driver->mem_fill = NULL;
#endif

#if LV_VDB_SIZE
    driver->vdb_wr = NULL;
#endif
}

/**
 * Register an initialized display driver.
 * Automatically set the first display as active.
 * @param driver pointer to an initialized 'lv_disp_drv_t' variable (can be local variable)
 * @return pointer to the new display or NULL on error
 */
lv_disp_t * lv_disp_drv_register(lv_disp_drv_t * driver)
{

    lv_disp_t * node = lv_ll_ins_head(&LV_GC_ROOT(_lv_disp_ll));
    if(!node) {
        lv_mem_assert(node);
        return NULL;
    }

    memcpy(&node->driver, driver, sizeof(lv_disp_drv_t));

    lv_ll_init(&node->scr_ll, sizeof(lv_obj_t));

    node->act_scr = lv_obj_create(NULL, NULL);  /*Create a default screen on the display*/
    node->top_layer = lv_obj_create(NULL, NULL);  /*Create top layer on the display*/

    return node;
}

void * lv_disp_get_next(lv_disp_t * disp)
{
    if(disp == NULL) return lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll));
    else return lv_ll_get_next(&LV_GC_ROOT(_lv_disp_ll), disp);
}


lv_disp_t * lv_disp_get_last(void)
{
    return lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll));
}


/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
lv_disp_t * lv_disp_next(lv_disp_t * disp)
{
    if(disp == NULL) return lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll));
    else return lv_ll_get_next(&LV_GC_ROOT(_lv_disp_ll), disp);
}

lv_coord_t lv_disp_get_hor_res(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_active();

    if(disp == NULL) return LV_HOR_RES_MAX;
    else return disp->driver.hor_res;
}


lv_coord_t lv_disp_get_ver_res(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_active();

    if(disp == NULL) return LV_VER_RES_MAX;
    else return disp->driver.ver_res;
}

/**
 * Write the content of the internal buffer (VDB) to the display
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p fill color
 */
void lv_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    if(active == NULL) return;
    if(active->driver.disp_fill != NULL) active->driver.disp_fill(x1, y1, x2, y2, color);
}

/**
 * Fill a rectangular area with a color on the active display
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p pointer to an array of colors
 */
void lv_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t * color_p)
{
    if(active == NULL) return;
    if(active->driver.disp_flush != NULL) {

        LV_LOG_TRACE("disp flush  started");
        active->driver.disp_flush(x1, y1, x2, y2, color_p);
        LV_LOG_TRACE("disp flush ready");

    } else {
        LV_LOG_WARN("disp flush function registered");
    }
}

/**
 * Put a color map to a rectangular area on the active display
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_map pointer to an array of colors
 */
void lv_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_map)
{
    if(active == NULL) return;
    if(active->driver.disp_map != NULL)  active->driver.disp_map(x1, y1, x2, y2, color_map);
}

#if USE_LV_GPU

/**
 * Blend pixels to a destination memory from a source memory
 * In 'lv_disp_drv_t' 'mem_blend' is optional. (NULL if not available)
 * @param dest a memory address. Blend 'src' here.
 * @param src pointer to pixel map. Blend it to 'dest'.
 * @param length number of pixels in 'src'
 * @param opa opacity (0, LV_OPA_TRANSP: transparent ... 255, LV_OPA_COVER, fully cover)
 */
void lv_disp_mem_blend(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    if(active == NULL) return;
    if(active->driver.mem_blend != NULL) active->driver.mem_blend(dest, src, length, opa);
}

/**
 * Fill a memory with a color (GPUs may support it)
 * In 'lv_disp_drv_t' 'mem_fill' is optional. (NULL if not available)
 * @param dest a memory address. Copy 'src' here.
 * @param src pointer to pixel map. Copy it to 'dest'.
 * @param length number of pixels in 'src'
 * @param opa opacity (0, LV_OPA_TRANSP: transparent ... 255, LV_OPA_COVER, fully cover)
 */
void lv_disp_mem_fill(lv_color_t * dest, uint32_t length, lv_color_t color)
{
    if(active == NULL) return;
    if(active->driver.mem_fill != NULL) active->driver.mem_fill(dest, length, color);
}

/**
 * Shows if memory blending (by GPU) is supported or not
 * @return false: 'mem_blend' is not supported in the driver; true: 'mem_blend' is supported in the driver
 */
bool lv_disp_is_mem_blend_supported(void)
{
    if(active == NULL) return false;
    if(active->driver.mem_blend) return true;
    else return false;
}

/**
 * Shows if memory fill (by GPU) is supported or not
 * @return false: 'mem_fill' is not supported in the drover; true: 'mem_fill' is supported in the driver
 */
bool lv_disp_is_mem_fill_supported(void)
{
    if(active == NULL) return false;
    if(active->driver.mem_fill) return true;
    else return false;
}

#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

