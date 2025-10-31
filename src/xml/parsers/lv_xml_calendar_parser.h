/**
 * @file lv_xml_calendar_parser.h
 *
 */

#ifndef LV_XML_CALENDAR_PARSER_H
#define LV_XML_CALENDAR_PARSER_H

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

void * lv_xml_calendar_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_calendar_apply(lv_xml_parser_state_t * state, const char ** attrs);
void * lv_xml_calendar_header_arrow_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_calendar_header_arrow_apply(lv_xml_parser_state_t * state, const char ** attrs);
void * lv_xml_calendar_header_dropdown_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_calendar_header_dropdown_apply(lv_xml_parser_state_t * state, const char ** attrs);

/**********************
 *      MACROS
 **********************/
#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_CALENDAR_PARSER_H*/
