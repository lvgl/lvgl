/**
 * @file lv_xml_update.h
 *
 */

#ifndef LV_XML_UPDATE_H
#define LV_XML_UPDATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#if LV_USE_XML && LV_USE_OBJ_NAME

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Change the properties of a given widget by processing XML snippets.
 * For example `<lv_slider name="my_slider_1" min_value="30" "style_bg_color="0xff0000"/>`
 * Note that the tag should be the underlying widget's name and the component's name.
 * @param xml_def   the XML to process as a string
 * @return          LV_RESULT_OK: loaded successfully, LV_RES_INVALID: otherwise
 */
lv_result_t lv_xml_update_from_data(const char * xml_def);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_UPDATE_H*/


