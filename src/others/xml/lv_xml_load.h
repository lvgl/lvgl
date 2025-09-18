/**
 * @file lv_xml_load.h
 *
 */

#ifndef LV_XML_LOAD_H
#define LV_XML_LOAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_types.h"
#if LV_USE_XML

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
 * Recurse into a directory, loading all XML components,
 * screens, globals, and translations.
 * @param path   the path to a directory to load files from
 * @return       `LV_RESULT_OK` if there were no issues or
 *               `LV_RESULT_INVALID` otherwise.
 */
lv_result_t lv_xml_load_all_from_path(const char * path);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_XML*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_LOAD_H*/
