/**
 * @file lv_font_manager_utils.h
 *
 */
#ifndef LV_FONT_MANAGER_UTILS_H
#define LV_FONT_MANAGER_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_font_manager_types.h"

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
 * Compare font information.
 * @param ft_info_1 font information 1.
 * @param ft_info_2 font information 2.
 * @return return true if the fonts are equal.
 */
bool lv_font_info_is_equal(const lv_font_info_t * ft_info_1, const lv_font_info_t * ft_info_2);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FONT_MANAGER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_FONT_MANAGER_UTILS_H */
