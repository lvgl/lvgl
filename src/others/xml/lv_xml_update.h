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
#include "../../misc/lv_types.h"
#if LV_USE_XML && LV_USE_OBJ_NAME

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Load the styles, constants, another data of the component. It needs to be called only once for each component.
 * @param xml_def   the XML definition of the component as a NULL terminated string
 * @return          LV_RES_OK: loaded successfully, LV_RES_INVALID: otherwise
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


