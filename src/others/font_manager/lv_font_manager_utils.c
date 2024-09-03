/**
 * @file lv_font_manager_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_font_manager_utils.h"

#if LV_USE_FONT_MANAGER
#include "../../misc/lv_assert.h"

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

bool lv_freetype_info_is_equal(const lv_freetype_info_t * ft_info_1, const lv_freetype_info_t * ft_info_2)
{
    LV_ASSERT_NULL(ft_info_1);
    LV_ASSERT_NULL(ft_info_2);

    bool is_equal = (ft_info_1->size == ft_info_2->size
                     && ft_info_1->style == ft_info_2->style
                     && ft_info_1->render_mode == ft_info_2->render_mode
                     && lv_strcmp(ft_info_1->name, ft_info_2->name) == 0);

    return is_equal;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_FONT_MANAGER */
