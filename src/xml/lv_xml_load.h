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

#if LV_USE_FS_FROGFS
/**
 * Mount a data blob and recurse through it, loading all XML components,
 * screens, globals, and translations.
 * @param buf       the data blob
 * @param buf_size  the size of the data blob
 * @return          a handle that can be used to unload it later
 */
lv_xml_load_t * lv_xml_load_all_from_data(const void * buf, uint32_t buf_size);

/**
 * Mount a data blob located at `file_path` in a filesystem and recurse through it,
 * loading all XML components, screens, globals, and translations.
 * @param file_path   the path of the data blob
 * @return            a handle that can be used to unload it later
 */
lv_xml_load_t * lv_xml_load_all_from_file(const char * file_path);

/**
 * Unload XML data that was loaded by a function that returned `lv_xml_load_t *`.
 * Any assets in the loaded data will not be accessible anymore.
 * @param load       a loaded XML data handle, or `NULL` to unload all.
 */
void lv_xml_unload(lv_xml_load_t * load);
#endif /*LV_USE_FS_FROGFS*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_XML*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_LOAD_H*/
