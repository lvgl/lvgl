/**
 * @file lv_xml_base_types.h
 *
 */

#ifndef LV_XML_BASE_TYPES_H
#define LV_XML_BASE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#include "../../misc/lv_style.h"
#if LV_USE_XML

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Process inputs "content", "32", "32px", or "25%"
 * and convert them to integer
 * @param txt       the input string
 * @return          the integer size
 */
int32_t lv_xml_to_size(const char * txt);


/**
 * Convert an align string to enum
 * @param txt       e.g. "center"
 * @return          the related enum, e.g. `LV_ALIGN_CENTER`
 */
lv_align_t lv_xml_align_to_enum(const char * txt);

/**
 * Convert a direction string to enum
 * @param txt       e.g. "top"
 * @return          the related enum, e.g. `LV_DIR_TOP`
 */
lv_dir_t lv_xml_dir_to_enum(const char * txt);

/**
 * Convert a direction string to enum
 * @param txt       e.g. "top"
 * @return          the related enum, e.g. `LV_BORDER_SIDE_TOP`
 */
lv_border_side_t lv_xml_border_side_to_enum(const char * txt);

/**
 * Convert a base dir string to enum
 * @param txt       e.g. "rtl"
 * @return          the related enum, e.g. `LV_BASE_DIR_RTL`
 */
lv_base_dir_t lv_xml_base_dir_to_enum(const char * txt);

/**
 * Convert a grad dir string to enum
 * @param txt       e.g. "hor"
 * @return          the related enum, e.g. `LV_GRAD_DIR_HOR`
 */
lv_grad_dir_t lv_xml_grad_dir_to_enum(const char * txt);


/**
 * Convert a text align string to enum
 * @param txt       e.g. "left"
 * @return          the related enum, e.g. `LV_TEXT_ALIGN_LEFT`
 */
lv_text_align_t lv_xml_text_align_to_enum(const char * txt);

/**
 * Convert a text decor string to enum
 * @param txt       e.g. "underline"
 * @return          the related enum, e.g. `LV_TEXT_DECOR_UNDERLINE`
 */
lv_text_decor_t lv_xml_text_decor_to_enum(const char * txt);

/**
 * Convert a flex flow string to enum
 * @param txt       e.g. "row_wrap"
 * @return          the related enum, e.g. `LV_FLEX_FLOW_ROW_WRAP`
 */
lv_flex_flow_t lv_xml_flex_flow_to_enum(const char * txt);

/**
 * Convert a flex align string to enum
 * @param txt       e.g. "space_between"
 * @return          the related enum, e.g. `LV_FLEX_ALIGN_SPACE_BETWEEN`
 */
lv_flex_align_t lv_xml_flex_align_to_enum(const char * txt);

/**
 * Convert a grid align string to enum
 * @param txt       e.g. "space_between"
 * @return          the related enum, e.g. `LV_GRID_ALIGN_SPACE_BETWEEN`
 */
lv_grid_align_t lv_xml_grid_align_to_enum(const char * txt);

/**
 * Convert a layout string to enum
 * @param txt       e.g. "flex"
 * @return          the related enum, e.g. `LV_LAYOUT_FLEX`
 */
lv_layout_t lv_xml_layout_to_enum(const char * txt);

/**
 * Convert a blend mode string to enum
 * @param txt       e.g. "additive"
 * @return          the related enum, e.g. `LV_BLEND_MODE_ADDITIVE`
 */
lv_blend_mode_t lv_xml_blend_mode_to_enum(const char * txt);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_BASE_TYPES_H*/
