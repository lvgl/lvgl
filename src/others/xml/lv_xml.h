/**
 * @file lv_xml.h
 *
 */

#ifndef LV_XML_H
#define LV_XML_H

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

void lv_xml_init(void);

lv_obj_t * lv_xml_create(lv_obj_t * parent, const char * name, const char ** attrs);

lv_obj_t * lv_xml_create_from_ctx(lv_obj_t * parent, lv_xml_component_ctx_t * parent_ctx, lv_xml_component_ctx_t * ctx,
                                  const char ** attrs);

lv_result_t lv_xml_register_font(const char * name, const lv_font_t * font);

const lv_font_t * lv_xml_get_font(const char * name);

lv_result_t lv_xml_register_image(const char * name, const void * src);

const void * lv_xml_get_image(const char * name);


/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_H*/
