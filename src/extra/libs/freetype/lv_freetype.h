/**
 * @file lv_freetype.h
 *
 */
#ifndef LV_FREETYPE_H
#define LV_FREETYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_FREETYPE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    FT_FONT_STYLE_NORMAL = 0,
    FT_FONT_STYLE_ITALIC = 1 << 0,
    FT_FONT_STYLE_BOLD   = 1 << 1
} LV_FT_FONT_STYLE;

typedef struct {
    const char * name;  /* The name of the font file */
    const void * mem;   /* The pointer of the font file */
    size_t mem_size;    /* The size of the memory */
    lv_font_t * font;   /* point to lvgl font */
    uint16_t weight;    /* font size */
    uint16_t style;     /* font style */
} lv_ft_info_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * init freetype library
 * @param max_faces Maximum number of opened FT_Face objects managed by this cache instance. Use 0 for defaults.
 * @param max_sizes Maximum number of opened FT_Size objects managed by this cache instance. Use 0 for defaults.
 * @param max_bytes Maximum number of bytes to use for cached data nodes. Use 0 for defaults.
 *                  Note that this value does not account for managed FT_Face and FT_Size objects.
 * @return true on success, otherwise false.
 */
bool lv_freetype_init(uint16_t max_faces, uint16_t max_sizes, uint32_t max_bytes);

/**
 * Destroy freetype library
 */
void lv_freetype_destroy(void);

/**
 * Creates a font with info parameter specified.
 * @param info See lv_ft_info_t for details.
 *             when success, lv_ft_info_t->font point to the font you created.
 * @return true on success, otherwise false.
 */
bool lv_ft_font_init(lv_ft_info_t * info);

/**
 * Destroy a font that has been created.
 * @param font pointer to font.
 */
void lv_ft_font_destroy(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FREETYPE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_FREETYPE_H */
