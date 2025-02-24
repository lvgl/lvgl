/**
 * @file lv_xml_component_private.h
 *
 */

#ifndef LV_XML_COMPONENT_PRIVATE_H
#define LV_XML_COMPONENT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml.h"
#if LV_USE_XML

#include "lv_xml_utils.h"
#include "../../misc/lv_ll.h"
#include "../../misc/lv_style.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef  void * (*lv_xml_component_process_cb_t)(lv_obj_t * parent, const char * data, const char ** attrs);

struct _lv_xml_component_ctx_t {
    const char * name;
    lv_ll_t style_ll;
    lv_ll_t const_ll;
    lv_ll_t param_ll;
    lv_ll_t gradient_ll;
    const char * view_def;
    struct _lv_widget_processor_t * root_widget;
    uint32_t is_widget : 1;                         /*1: not component but widget registered as a component for preview*/
    struct _lv_xml_component_ctx_t * next;
};

typedef struct {
    const char * name;
    const char * value;
} lv_xml_const_t;

typedef struct {
    const char * name;
    const char * def;
    const char * type;
} lv_xml_param_t;

typedef struct {
    const char * name;
    lv_grad_dsc_t grad_dsc;
} lv_xml_grad_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the components system.
 */
void lv_xml_component_init(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_COMPONENT_PRIVATE_H*/
