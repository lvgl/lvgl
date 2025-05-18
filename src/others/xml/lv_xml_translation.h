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
#if LV_USE_XML && LV_USE_OBJ_NAME

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_xml_translation_init(void);


lv_result_t lv_xml_translation_register_from_array(const char * languages[], const char * tags[],
                                                   const char * translations[]);

lv_result_t lv_xml_translation_register_from_file(const char * path);

/**
 * Load the styles, constants, another data of the component. It needs to be called only once for each component.
 * @param xml_def   the XML definition of the component as a NULL terminated string
 * @return          LV_RES_OK: loaded successfully, LV_RES_INVALID: otherwise
 */
lv_result_t lv_xml_translation_register_from_data(const char * xml_def);

void lv_xml_set_language(const char * lang);

const char * lv_xml_get_translation(const char * tag);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_TRANSLATION_H*/


