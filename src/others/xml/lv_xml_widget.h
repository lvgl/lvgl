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

/**
 * Register a widget for the XML parser. When a widget with a given name
 * is created in XML `create_cb` will be called to create an instance, and
 * then `apply_cb` will be called to apply the properties (e.g. `width=100""`).
 * @param name          name of the widget (e.g. "my_slider", referenced as <my_slider> in XML)
 * @param create_cb     called to create an instance of the widget
 * @param apply_cb      called to apply the properties
 * @return              pointer to the created widget
 * @note                E.g. chart series, are also considered widgets although
 *                      they don't have `lv_obj_t *` type.
 */
lv_result_t lv_xml_register_widget(const char * name, lv_xml_widget_create_cb_t create_cb,
                                   lv_xml_widget_apply_cb_t apply_cb);

/**
 * Get a descriptor that was created when the widget was registered.
 * @param name      the name that was used when the widget was registered
 * @return          the descriptor of the widget
 */
lv_widget_processor_t * lv_xml_widget_get_processor(const char * name);

/**
 * Get the descriptor of the widget that is extended by a given widget, component or screen.
 * E.g. in a component `<view extends="lv_slider">` return the descriptor of `lv_slider`
 * @param extends   the name of a component screen or widget whose ancestor widget shall be returned
 * @return          the descriptor of the extended widget
 * @note            if a component extends an other component which based on lv_slider
 *                  lv_slider's descriptor will be returned.
 */
lv_widget_processor_t * lv_xml_widget_get_extended_widget_processor(const char * extends);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_WIDGET_H*/


