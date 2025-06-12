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

/**
 * Convert sections of a string to int.
 * The end of the string is indicated by the `delimiter`.
 * @param str       pointer to a string, it will point to the character after the delimiter
 * @param delimiter a character to indicate the end of the int
 * @return          the int before the next delimiter
 */
int32_t lv_xml_atoi_split(const char ** str, char delimiter);

#if LV_USE_FLOAT
float lv_xml_atof(const char * str);

/**
 * Convert sections of a string to float.
 * The end of the string is indicated by the `delimiter`.
 * @param str       pointer to a string, it will point to the character after the delimiter
 * @param delimiter a character to indicate the end of the float
 * @return          the float before the next delimiter
 */
float lv_xml_atof_split(const char ** str, char delimiter);
#endif

lv_color_t lv_xml_to_color(const char * str);

/**
 * Concert percentage or integer opacity value from string to integer.
 * @param str   e.g. "70%" or 180
 * @return      0..255
 */
lv_opa_t lv_xml_to_opa(const char * str);

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
