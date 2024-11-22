/**
 * @file lv_xml_utils.h
 *
 */

#ifndef LV_XML_UTILS_H
#define LV_XML_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_XML

#include LV_STDINT_INCLUDE
#include "../../misc/lv_color.h"

/**********************
 * GLOBAL PROTOTYPES
 **********************/
const char * lv_xml_get_value_of(const char ** attrs, const char * name);

int32_t lv_xml_atoi(const char * str);

lv_color_t lv_xml_to_color(const char * str);

bool lv_xml_to_bool(const char * str);

int32_t lv_xml_strtol(const char * str, char ** endptr, int32_t base);

/**
 * Find a delimiter in a string, terminate the string on the delimiter and
 * update the source string to the next part
 * @param src           point to a variable containing the input string
 * @param delimiter     a delimiter character, e.g. ':'
 * @return              the beginning of next section in the string closed at the delimiter
 */
char * lv_xml_split_str(char ** src, char delimiter);

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_UTILS_H*/
