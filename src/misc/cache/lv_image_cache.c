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
void lv_image_cache_drop(const void * src, const uint8_t src_type)
{
    if(src == NULL) {
        lv_cache_drop_all(img_cache_p, NULL);
        return;
    }

    lv_image_decoder_cache_data_t search_key = {
        .src = src,
        .src_type = src_type,
    };

    lv_cache_drop(img_cache_p, &search_key, NULL);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
