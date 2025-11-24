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
#include "../misc/lv_types.h"
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
 * Register a Widget for the XML parser. When a Widget with a given name
 * is created in XML `create_cb` will be called to create an instance, and
 * then `apply_cb` will be called to apply the properties (e.g. `width=100""`).
 * @param name          Name of the Widget (e.g. "my_slider", referenced as <my_slider> in XML)
 * @param create_cb     Called to create an instance of the Widget
 * @param apply_cb      Called to apply its properties
 * @return              Pointer to the created Widget
 * @note                E.g. Chart series, are also considered Widgets although
 *                      they don't have `lv_obj_t *` type.
 */
lv_result_t lv_xml_register_widget(const char * name, lv_xml_widget_create_cb_t create_cb,
                                   lv_xml_widget_apply_cb_t apply_cb);

/**
 * Get a descriptor that was created when the Widget was registered.
 * @param name      The name that was used when the Widget was registered
 * @return          The descriptor of the Widget
 */
lv_widget_processor_t * lv_xml_widget_get_processor(const char * name);

/**
 * Get the descriptor of the Widget that is extended by a given Widget, Component, or Screen.
 * E.g. in a Component `<view extends="lv_slider">` return the descriptor of `lv_slider`.
 * @param extends   The name of a Component, Screen, or Widget whose ancestor Widget shall be returned
 * @return          The descriptor of the extended Widget
 * @note            If a component extends an other component which based on `lv_slider`
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


