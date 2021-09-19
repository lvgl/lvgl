/**
 * @file lv_extra.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

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

void lv_extra_init(void)
{
#if LV_USE_FLEX
    lv_flex_init();
#endif

#if LV_USE_GRID
    lv_grid_init();
#endif

#if LV_USE_FS_FATFS != '\0'
    lv_fs_fatfs_init();
#endif

#if LV_USE_FS_STDIO != '\0'
    lv_fs_stdio_init();
#endif

#if LV_USE_FS_POSIX != '\0'
    lv_fs_posix_init();
#endif

#if LV_USE_PNG
    lv_png_init();
#endif

#if LV_USE_SJPG
    lv_split_jpeg_init();
#endif

#if LV_USE_BMP
    lv_bmp_init();
#endif

}

/**********************
 *   STATIC FUNCTIONS
 **********************/
