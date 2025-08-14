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
#include "../../misc/lv_event.h"
#include "../../others/observer/lv_observer.h"
#include "lv_xml_test.h"
#include "lv_xml_translation.h"
#include "lv_xml_component.h"
#include "lv_xml_widget.h"

/*********************
 *      DEFINES
 *********************/

#define LV_XML_MAX_PATH_LENGTH 256

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_xml_init(void);

void lv_xml_deinit(void);

void * lv_xml_create(lv_obj_t * parent, const char * name, const char ** attrs);

void * lv_xml_create_in_scope(lv_obj_t * parent, lv_xml_component_scope_t * parent_ctx,
                              lv_xml_component_scope_t * scope,
                              const char ** attrs);

/**
 * Set a path to prefix the image and font file source paths.
 *
 * In globals.xml usually the source path is like "images/logo.png".
 * But on the actual device it can be located at e.g. "A:ui/assets/images/logo.png".
 * By setting "A:ui/assets/" the path set in the XML files will be prefixed accordingly.
 *
 * @param path_prefix   the path to be used as prefix
 */
void lv_xml_set_default_asset_path(const char * path_prefix);

lv_result_t lv_xml_register_font(lv_xml_component_scope_t * scope, const char * name, const lv_font_t * font);

const lv_font_t * lv_xml_get_font(lv_xml_component_scope_t * scope, const char * name);

lv_result_t lv_xml_register_image(lv_xml_component_scope_t * scope, const char * name, const void * src);

const void * lv_xml_get_image(lv_xml_component_scope_t * scope, const char * name);

/**
 * Map globally available subject name to an actual subject variable
 * @param name      name of the subject
 * @param subject   pointer to a subject
 * @return          `LV_RESULT_OK`: success
 */
lv_result_t lv_xml_register_subject(lv_xml_component_scope_t * scope, const char * name, lv_subject_t * subject);

/**
 * Get a subject by name.
 * @param scope     If specified start searching in that component's subject list,
 *                  and if not found search in the global space.
 *                  If `NULL` search in global space immediately.
 * @param name      Name of the subject to find.
 * @return          Pointer to the subject or NULL if not found.
 */
lv_subject_t * lv_xml_get_subject(lv_xml_component_scope_t * scope, const char * name);

lv_result_t lv_xml_register_const(lv_xml_component_scope_t * scope, const char * name, const char * value);

const char * lv_xml_get_const(lv_xml_component_scope_t * scope, const char * name);

lv_result_t lv_xml_register_event_cb(lv_xml_component_scope_t * scope, const char * name, lv_event_cb_t cb);

lv_event_cb_t lv_xml_get_event_cb(lv_xml_component_scope_t * scope, const char * name);

lv_result_t lv_xml_register_timeline(lv_xml_component_scope_t * scope, const char * name);

void * lv_xml_get_timeline(lv_xml_component_scope_t * scope, const char * name);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_H*/
