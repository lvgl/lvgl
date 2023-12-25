/**
* @file lv_image_cache.c
*
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_assert.h"
#include "lv_image_cache.h"
#include "../../core/lv_global.h"
/*********************
 *      DEFINES
 *********************/
#define img_cache_p (LV_GLOBAL_DEFAULT()->img_cache)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL VARIABLES
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
void lv_image_cache_drop(const void * src)
{
#if LV_CACHE_DEF_SIZE > 0
    if(src == NULL) {
        lv_cache_drop_all(img_cache_p, NULL);
        return;
    }

    lv_image_cache_data_t search_key = {
        .src = src,
        .src_type = lv_image_src_get_type(src),
    };

    lv_cache_drop(img_cache_p, &search_key, NULL);
#else
    LV_UNUSED(src);
#endif
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
