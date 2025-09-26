/**
 * @file lv_xml_translation.h
 *
 */

#ifndef LV_XML_TRANSLATION_H
#define LV_XML_TRANSLATION_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#if LV_USE_XML && LV_USE_TRANSLATION

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register translations from an XML file
 * @param path      path to an XML file (staring with a driver letter)
 * @return          LV_RESULT_OK: no error
 */
lv_result_t lv_xml_register_translation_from_file(const char * path);

/**
 * Register translations from an XML string
 * @param xml_def   the XML definition as a string
 * @return          LV_RESULT_OK: no error
 */
lv_result_t lv_xml_register_translation_from_data(const char * xml_def);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_TRANSLATION_H*/


