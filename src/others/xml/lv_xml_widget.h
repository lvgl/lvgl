/**
 * @file lv_xml_widget.h
 *
 */

#ifndef LV_XML_WIDGET_H
#define LV_XML_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#if LV_USE_XML

#include "lv_xml.h"
#include "lv_xml_utils.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef  void * (*lv_xml_widget_create_cb_t)(lv_xml_parser_state_t * state, const char ** parent_attrs);
typedef  void (*lv_xml_widget_apply_cb_t)(lv_xml_parser_state_t * state, const char ** parent_attrs);

typedef struct _lv_widget_processor_t {
    const char * name;
    lv_xml_widget_create_cb_t create_cb;
    lv_xml_widget_apply_cb_t apply_cb;
    struct _lv_widget_processor_t * next;
} lv_widget_processor_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_result_t lv_xml_widget_register(const char * name, lv_xml_widget_create_cb_t create_cb,
                                   lv_xml_widget_apply_cb_t apply_cb);

lv_widget_processor_t * lv_xml_widget_get_processor(const char * name);

lv_widget_processor_t * lv_xml_widget_get_extended_widget_processor(const char * extends);
/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_WIDGET_H*/


