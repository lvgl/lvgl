/**
 * @file lv_xml_polarchart_parser.h
 *
 */

#ifndef LV_XML_POLARCHART_PARSER_H
#define LV_XML_POLARCHART_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_xml.h"
#if LV_USE_XML && LV_USE_POLARCHART

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void * lv_xml_polarchart_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_polarchart_apply(lv_xml_parser_state_t * state, const char ** attrs);
void * lv_xml_polarchart_series_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_polarchart_series_apply(lv_xml_parser_state_t * state, const char ** attrs);
void * lv_xml_polarchart_cursor_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_polarchart_cursor_apply(lv_xml_parser_state_t * state, const char ** attrs);
void * lv_xml_polarchart_axis_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_polarchart_axis_apply(lv_xml_parser_state_t * state, const char ** attrs);

/**********************
 *      MACROS
 **********************/
#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_POLARCHART_PARSER_H*/
