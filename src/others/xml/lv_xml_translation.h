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
 * Initialize the translation module. Used internally.
 */
void lv_xml_translation_init(void);


/**
 * Register translations from arrays
 * @param languages     The languages for translations, e.g. {"en", "de", NULL}
 * @param tags          Tags to identify the translations, e.g. {"dog", "cat", "bird", NULL}
 * @param translations  `languages x tags` string, e.g. {"Dog", "Hund", "Cat", "Katze", "Bird", "Vogel"}
 * @return          LV_RES_OK: no error
 */
lv_result_t lv_xml_translation_register_from_array(const char * languages[], const char * tags[],
                                                   const char * translations[]);

/**
 * Register translations from an XML file
 * @param path      path to an XML file (staring with a driver letter)
 * @return          LV_RES_OK: no error
 */
lv_result_t lv_xml_translation_register_from_file(const char * path);

/**
 * Register translations from an XML string
 * @param xml_def   the XML definition as a string
 * @return          LV_RES_OK: no error
 */
lv_result_t lv_xml_translation_register_from_data(const char * xml_def);

/**
 * Select the language to translate to
 * @param lang      the target language as a string, e.g. "en"
 */
void lv_xml_set_language(const char * lang);

/**
 * Get the translation on the selected language
 * @param tag   identifier of a translation, e.g. "dog"
 * @return      the translated text, e.g. "My Dog"
 */
const char * lv_xml_get_translation(const char * tag);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_TRANSLATION_H*/


