/**
 * @file lv_xml_dropdown_parser.h
 *
 */

#ifndef LV_XML_DROPDOWN_PARSER_H
#define LV_XML_DROPDOWN_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_xml.h"
#if LV_USE_XML

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void * lv_xml_dropdown_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_dropdown_apply(lv_xml_parser_state_t * state, const char ** attrs);
void * lv_xml_dropdown_list_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_dropdown_list_apply(lv_xml_parser_state_t * state, const char ** attrs);

/**********************
 *      MACROS
 **********************/
#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_DROPDOWN_PARSER_H*/
