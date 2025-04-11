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

#include "../../font/lv_font.h"

#if LV_USE_FONT_MANAGER

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

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
 * Add font resource.
 * @param manager pointer to main font manager.
 * @param name font name.
 * @param src font source. Need to strictly correspond to the font class.
 * @param class_p font class. eg. lv_freetype_font_class, lv_builtin_font_class.
 * @return return true if the add was successful.
 */
bool lv_font_manager_add_src(lv_font_manager_t * manager,
                             const char * name,
                             const void * src,
                             const lv_font_class_t * class_p);

/**
 * Add font resource with static memory.
 * @param manager pointer to main font manager.
 * @param name font name. It cannot be a local variable.
 * @param src font source. Need to strictly correspond to the font class. And it cannot be a local variable.
 * @param class_p font class. E.g. lv_freetype_font_class, lv_builtin_font_class.
 * @return return true if the add was successful.
 */
bool lv_font_manager_add_src_static(lv_font_manager_t * manager,
                                    const char * name,
                                    const void * src,
                                    const lv_font_class_t * class_p);

/**
 * Remove font resource.
 * @param manager pointer to main font manager.
 * @param name font name.
 * @return return true if the remove was successful.
 */
bool lv_font_manager_remove_src(lv_font_manager_t * manager, const char * name);

/**
 * Create font.
 * @param manager pointer to main font manager.
 * @param font_family font family name. Matches the font resource name, using commas to separate different names. E.g. "my_font_1,my_font_2".
 * @param render_mode font render mode. see `lv_freetype_font_render_mode_t`.
 * @param size font size in pixel.
 * @param style font style. see `lv_freetype_font_style_t`.
 * @param kerning kerning mode. see `lv_font_kerning_t`.
 * @return point to the created font.
 */
lv_font_t * lv_font_manager_create_font(lv_font_manager_t * manager,
                                        const char * font_family,
                                        uint32_t render_mode,
                                        uint32_t size,
                                        uint32_t style,
                                        lv_font_kerning_t kerning);

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
