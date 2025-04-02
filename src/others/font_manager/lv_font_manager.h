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

#include "lv_font_manager_backends.h"

#if LV_USE_FONT_MANAGER

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

void lv_font_manager_add_src(lv_font_manager_t * manager,
                             const char * name,
                             const void * src,
                             const lv_font_class_t * class_p);

void lv_font_manager_add_src_static(lv_font_manager_t * manager,
                                    const char * name,
                                    const void * src,
                                    const lv_font_class_t * class_p);

bool lv_font_manager_remove_src(lv_font_manager_t * manager, const char * name);

#if LV_USE_FREETYPE

/**
 * Add the freetype font file path.
 * @param manager pointer to main font manager.
 * @param name font name.
 * @param path font file path.
 */
static inline void lv_font_manager_add_path(lv_font_manager_t * manager, const char * name, const char * path)
{
    lv_font_manager_add_src(manager, name, path, &lv_freetype_font_class);
}

/**
 * Add the freetype font file path with static memory.
 * @param manager pointer to main font manager.
 * @param name font name.
 * @param path font file path.
 */
static inline void lv_font_manager_add_path_static(lv_font_manager_t * manager, const char * name, const char * path)
{
    lv_font_manager_add_src_static(manager, name, path, &lv_freetype_font_class);
}

#endif /* LV_USE_FREETYPE */

/**
 * Remove the font file path.
 * @param manager pointer to main font manager.
 * @param name font name.
 * @return return true if the remove was successful.
 */
static inline bool lv_font_manager_remove_path(lv_font_manager_t * manager, const char * name)
{
    return lv_font_manager_remove_src(manager, name);
}

/**
 * Create font.
 * @param manager pointer to main font manager.
 * @param font_family font family name.
 * @param render_mode font render mode.
 * @param size font size.
 * @param style font style.
 * @return point to the created font
 */
lv_font_t * lv_font_manager_create_font(lv_font_manager_t * manager,
                                        const char * font_family,
                                        uint32_t render_mode,
                                        uint32_t size,
                                        uint32_t style);

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
