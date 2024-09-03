/**
 * @file lv_font_manager.h
 *
 */
#ifndef LV_FONT_MANAGER_H
#define LV_FONT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_types.h"

#if LV_USE_FONT_MANAGER

#if !LV_USE_FREETYPE
#error "LV_USE_FONT_MANAGER requires LV_USE_FREETYPE"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_font_manager_t lv_font_manager_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create main font manager.
 * @param recycle_cache_size number of fonts that were recently deleted from the cache.
 * @return pointer to main font manager.
 */
lv_font_manager_t * lv_font_manager_create(uint32_t recycle_cache_size);

/**
 * Delete main font manager.
 * @param manager pointer to main font manager.
 * @return return true if the deletion was successful.
 */
bool lv_font_manager_delete(lv_font_manager_t * manager);

/**
 * Add the font file path.
 * @param manager pointer to main font manager.
 * @param name font name.
 * @param path font file path.
 */
void lv_font_manager_add_path(lv_font_manager_t * manager, const char * name, const char * path);

/**
 * Add the font file path with static memory.
 * @param manager pointer to main font manager.
 * @param name font name.
 * @param path font file path.
 */
void lv_font_manager_add_path_static(lv_font_manager_t * manager, const char * name, const char * path);

/**
 * Remove the font file path.
 * @param manager pointer to main font manager.
 * @param name font name.
 * @return return true if the remove was successful.
 */
bool lv_font_manager_remove_path(lv_font_manager_t * manager, const char * name);

/**
 * Create font.
 * @param manager pointer to main font manager.
 * @param font_family font family name.
 * @param render_mode font render mode, see lv_freetype_font_render_mode_t.
 * @param size font size.
 * @param style font style, see lv_freetype_font_style_t.
 * @return point to the created font
 */
lv_font_t * lv_font_manager_create_font(lv_font_manager_t * manager, const char * font_family, uint16_t render_mode,
                                        uint32_t size, uint16_t style);

/**
 * Delete font.
 * @param manager pointer to main font manager.
 * @param font point to the font.
 * @return return true if the deletion was successful.
 */
void lv_font_manager_delete_font(lv_font_manager_t * manager, lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_FONT_MANAGER */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FONT_MANAGER_MANAGER_H */
