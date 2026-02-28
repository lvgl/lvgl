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

#include "lv_font_fmt_txt.h"
#include "../../misc/lv_fs.h"
#include "../../osal/lv_os_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#if LV_BINFONT_DYNAMIC_LOAD
/**
 * Glyph loader for dynamic bitmap loading from binary font files.
 * Enables on-demand loading of glyph bitmaps to reduce memory usage.
 * Thread-safe with mutex protection for concurrent access.
 * @note Only one glyph bitmap can be held at a time for raw bitmap requests.
 * @note The file handle must remain open for the lifetime of the font.
 */
struct _lv_font_fmt_txt_glyph_loader_t {
    uint8_t * (*get_glyph_bitmap_cb)(void * fmt_dsc, void * glyph_dsc); /**< Callback to load glyph bitmap on demand */
    lv_fs_file_t * fp;              /**< File handle for the font binary file (must stay open) */
    uint32_t loca_count;            /**< Number of glyphs in the font */
    uint32_t * glyph_offset;        /**< Array of file offsets for each glyph */
    uint32_t glyph_start;           /**< File position where glyph data section starts */
    uint32_t glyph_length;          /**< Total length of the glyph data section in bytes */
    uint32_t glyph_per_bits;        /**< Number of bits per glyph descriptor (advance_width + xy + wh bits) */
    lv_mutex_t lock;                /**< Mutex for thread-safe access to file and shared buffer */
    bool raw_lock_held;             /**< Flag indicating if raw bitmap lock is currently held */
    uint32_t raw_lock_gid;          /**< Glyph ID of the currently locked raw bitmap */
};
#endif

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
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_FMT_TXT_PRIVATE_H*/
