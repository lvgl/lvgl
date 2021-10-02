/**
 * @file lv_gpu_sdl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#include "../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_gpu_sdl.h"
#include "sdl/lv_gpu_sdl_utils.h"
#include "sdl/lv_gpu_sdl_texture_cache.h"

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

void lv_gpu_sdl_init()
{
    _lv_gpu_sdl_utils_init();
    _lv_gpu_sdl_texture_cache_init();
}

void lv_gpu_sdl_deinit()
{
    _lv_gpu_sdl_texture_cache_deinit();
    _lv_gpu_sdl_utils_deinit();
}
/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif /*LV_USE_GPU_SDL*/
