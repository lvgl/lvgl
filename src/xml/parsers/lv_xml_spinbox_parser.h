/**
 * @file lv_xml_spinbox_parser.h
 *
 */

#ifndef LV_XML_SPINBOX_PARSER_H
#define LV_XML_SPINBOX_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_xml.h"
#if LV_USE_XML && LV_USE_SPINBOX

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void * lv_xml_spinbox_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_spinbox_apply(lv_xml_parser_state_t * state, const char ** attrs);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_SPINBOX_PARSER_H*/
