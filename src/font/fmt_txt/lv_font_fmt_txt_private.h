/**
 * @file lv_font_fmt_txt_private.h
 *
 */

#ifndef LV_FONT_FMT_TXT_PRIVATE_H
#define LV_FONT_FMT_TXT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"
#include "../../osal/lv_os_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * On demand glyph bitmap loader.
 */
struct _lv_font_fmt_txt_glyph_loader_t {
    /**
     * Load the raw (i.e. still encoded the same way as `glyph_bitmap` would store it) bitmap of a
     * glyph and return a pointer to it.
     *
     * Called with `lock` held.
     *
     * @param loader    pointer to the loader itself
     * @param gid       glyph index to load, i.e. the index in `lv_font_fmt_txt_dsc_t.glyph_dsc`
     * @return          pointer to the raw bitmap or `NULL` on error
     */
    const uint8_t * (*get_glyph_bitmap_cb)(lv_font_fmt_txt_glyph_loader_t * loader, uint32_t gid);

    /** Serializes the access to the shared scratch buffer and to the underlying source */
    lv_mutex_t lock;
};

#if LV_USE_FONT_COMPRESSED

typedef enum {
    RLE_STATE_SINGLE = 0,
    RLE_STATE_REPEATED,
    RLE_STATE_COUNTER,
} lv_font_fmt_rle_state_t;

typedef struct {
    uint32_t rdp;
    const uint8_t * in;
    uint8_t bpp;
    uint8_t prev_v;
    uint8_t count;
    lv_font_fmt_rle_state_t state;
} lv_font_fmt_rle_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_FMT_TXT_PRIVATE_H*/
