/**
 * @file sdl.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_platform_sdl.h"
#if LV_USE_DRV_SDL

#include LV_PLATFORM_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_platform_sdl_init(lv_platform_sdl_dsc_t * dsc)
{
    //    lv_memset_00(dsc, sizeof(lv_platform_sdl_dsc_t));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_StartTextInput();
}

void lv_platform_sdl_deinit(void)
{
    SDL_Quit();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_PLATFORM_SDL*/
