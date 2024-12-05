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
lv_align_t lv_xml_align_string_to_enum_value(const char * txt);

/**
 * Convert a direction string to enum
 * @param txt       e.g. "top"
 * @return          the related enum, e.g. `LV_DIR_TOP`
 */
lv_dir_t lv_xml_dir_string_to_enum_value(const char * txt);


/**
 * Convert a flex flow string to enum
 * @param txt       e.g. "row_wrap"
 * @return          the related enum, e.g. `LV_FLEX_FLOW_ROW_WRAP`
 */
lv_flex_flow_t lv_xml_flex_flow_string_to_enum_value(const char * txt);

/**
 * Convert a flex align string to enum
 * @param txt       e.g. "space_between"
 * @return          the related enum, e.g. `LV_FLEX_ALIGN_SPACE_BETWEEN`
 */
lv_flex_align_t lv_xml_flex_align_string_to_enum_value(const char * txt);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_BASE_TYPES_H*/
