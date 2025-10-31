/**
 * @file lv_xml_image_parser.h
 *
 */

#ifndef LV_XML_IMAGE_PARSER_H
#define LV_XML_IMAGE_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_xml.h"
#if LV_USE_XML && LV_USE_IMAGE

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void * lv_xml_image_create(lv_xml_parser_state_t * state, const char ** attrs);

void lv_xml_image_apply(lv_xml_parser_state_t * state, const char ** attrs);

void lv_xml_check_file(const char * filepath);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_IMAGE_PARSER_H*/
