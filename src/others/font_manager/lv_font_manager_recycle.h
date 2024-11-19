/**
 * @file lv_font_manager_recycle.h
 *
 */

#ifndef LV_FONT_MANAGER_RECYCLE_H
#define LV_FONT_MANAGER_RECYCLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_font_manager_utils.h"

#if LV_USE_FONT_MANAGER

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_font_manager_recycle_t lv_font_manager_recycle_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create font recycle manager.
 * @param max_size recycle size.
 * @return pointer to font recycle manager.
 */
lv_font_manager_recycle_t * lv_font_manager_recycle_create(uint32_t max_size);

/**
 * Delete font recycle manager.
 * @param manager pointer to font recycle manager.
 */
void lv_font_manager_recycle_delete(lv_font_manager_recycle_t * manager);

/**
 * Get a reusable font.
 * @param manager pointer to font recycle manager.
 * @param ft_info font info.
 * @return returns true on success.
 */
lv_font_t * lv_font_manager_recycle_get_reuse(lv_font_manager_recycle_t * manager, const lv_freetype_info_t * ft_info);

/**
 * Set fonts to be reused.
 * @param manager pointer to font recycle manager.
 * @param ft_info font info.
 */
void lv_font_manager_recycle_set_reuse(lv_font_manager_recycle_t * manager, lv_font_t * font,
                                       const lv_freetype_info_t * ft_info);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_FONT_MANAGER */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_FONT_MANAGER_RECYCLE_H */
