/**
 * @file lv_xml_imagebutton_parser.h
 *
 */

#ifndef LV_XML_IMAGEBUTTON_PARSER_H
#define LV_XML_IMAGEBUTTON_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_xml.h"
#if LV_USE_XML && LV_USE_IMAGEBUTTON

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void * lv_xml_imagebutton_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_imagebutton_apply(lv_xml_parser_state_t * state, const char ** attrs);
void * lv_xml_imagebutton_src_left_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_imagebutton_src_left_apply(lv_xml_parser_state_t * state, const char ** attrs);
void * lv_xml_imagebutton_src_right_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_imagebutton_src_right_apply(lv_xml_parser_state_t * state, const char ** attrs);
void * lv_xml_imagebutton_src_mid_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_imagebutton_src_mid_apply(lv_xml_parser_state_t * state, const char ** attrs);

/**********************
 *      MACROS
 **********************/
#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_IMAGEBUTTON_PARSER_H*/
