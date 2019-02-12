
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
    node->sys_layer = lv_obj_create(NULL, NULL);  /*Create top layer on the display*/
    lv_obj_set_style(node->top_layer, &lv_style_transp);
    lv_obj_set_style(node->sys_layer, &lv_style_transp);

    return node;
}

lv_disp_t * lv_disp_get_next(lv_disp_t * disp)
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
    if(disp == NULL) disp = lv_disp_get_last();

    if(disp == NULL) return LV_HOR_RES_MAX;
    else return disp->driver.hor_res;
}


lv_coord_t lv_disp_get_ver_res(lv_disp_t * disp)
{
    if(disp == NULL) disp = lv_disp_get_last();

    if(disp == NULL) return LV_VER_RES_MAX;
    else return disp->driver.ver_res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

