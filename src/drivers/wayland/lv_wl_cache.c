/**
 * @file lv_wl_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_wayland.h"

#if LV_USE_WAYLAND

#include "lv_wayland_private.h"
#include <string.h>

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

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

void lv_wayland_cache_apply_areas(struct window * window, void * dest, void * src, smm_buffer_t * src_buf)
{
    unsigned long offset;
    unsigned char start;
    int32_t y;
    lv_area_t * dmg;
    lv_area_t * next_dmg;
    smm_buffer_t * next_buf                    = smm_next(src_buf);
    const struct smm_buffer_properties * props = SMM_BUFFER_PROPERTIES(src_buf);
    struct graphic_object * obj                = SMM_GROUP_PROPERTIES(props->group)->tag[TAG_LOCAL];
    uint8_t bpp;

    if(next_buf == NULL) {
        next_dmg = (window->dmg_cache.cache + window->dmg_cache.end);
    }
    else {
        next_dmg = SMM_BUFFER_PROPERTIES(next_buf)->tag[TAG_BUFFER_DAMAGE];
    }

    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    /* Apply all buffer damage areas */
    start = ((lv_area_t *)SMM_BUFFER_PROPERTIES(src_buf)->tag[TAG_BUFFER_DAMAGE] - window->dmg_cache.cache);
    while((window->dmg_cache.cache + start) != next_dmg) {
        /* Copy an area from source to destination (line-by-line) */
        dmg = (window->dmg_cache.cache + start);
        for(y = dmg->y1; y <= dmg->y2; y++) {
            offset = (dmg->x1 + (y * obj->width)) * bpp;

            memcpy(((char *)dest) + offset, ((char *)src) + offset, ((dmg->x2 - dmg->x1 + 1) * bpp));
        }

        start++;
        start %= DMG_CACHE_CAPACITY;
    }
}

void lv_wayland_cache_add_area(struct window * window, smm_buffer_t * buf, const lv_area_t * area)
{
    if(SMM_BUFFER_PROPERTIES(buf)->tag[TAG_BUFFER_DAMAGE] == NULL) {
        /* Buffer damage beyond cache capacity */
        return;
    }

    if((window->dmg_cache.start == window->dmg_cache.end) && (window->dmg_cache.size)) {
        /* This buffer has more damage then the cache's capacity, so
         * clear cache and leave buffer damage unrecorded
         */
        lv_wayland_cache_clear(window);
        SMM_TAG(buf, TAG_BUFFER_DAMAGE, NULL);
        return;
    }

    /* Add damage area to cache */
    memcpy(window->dmg_cache.cache + window->dmg_cache.end, area, sizeof(lv_area_t));
    window->dmg_cache.end++;
    window->dmg_cache.end %= DMG_CACHE_CAPACITY;
    window->dmg_cache.size++;
}

void lv_wayland_cache_clear(struct window * window)
{
    window->dmg_cache.start = window->dmg_cache.end;
    window->dmg_cache.size  = 0;
}

void lv_wayland_cache_purge(struct window * window, smm_buffer_t * buf)
{
    lv_area_t * next_dmg;
    smm_buffer_t * next_buf = smm_next(buf);

    /* Remove all damage areas up until start of next buffers damage */
    if(next_buf == NULL) {
        lv_wayland_cache_clear(window);
    }
    else {
        next_dmg = SMM_BUFFER_PROPERTIES(next_buf)->tag[TAG_BUFFER_DAMAGE];
        while((window->dmg_cache.cache + window->dmg_cache.start) != next_dmg) {
            window->dmg_cache.start++;
            window->dmg_cache.start %= DMG_CACHE_CAPACITY;
            window->dmg_cache.size--;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_WAYLAND */
