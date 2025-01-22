/**
 * @file lv_xml_event_parser.h
 *
 */

#ifndef LV_EVENT_XML_PARSER_H
#define LV_EVENT_XML_PARSER_H

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
void * lv_xml_event_call_function_create(lv_xml_parser_state_t * state, const char ** attrs);

void lv_xml_event_call_function_apply(lv_xml_parser_state_t * state, const char ** attrs);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_EVENT_XML_PARSE_H*/
