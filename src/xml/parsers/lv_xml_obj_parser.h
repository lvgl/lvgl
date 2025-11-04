/**
 * @file lv_xml_obj_parser.h
 *
 */

#ifndef LV_XML_OBJ_PARSER_H
#define LV_XML_OBJ_PARSER_H

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

void * lv_xml_obj_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_xml_obj_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_style_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_style_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_remove_style_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_remove_style_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_remove_style_all_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_remove_style_all_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_event_cb_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_event_cb_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_subject_toggle_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_subject_toggle_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_subject_set_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_subject_set_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_subject_increment_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_subject_increment_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_bind_style_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_bind_style_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_bind_flag_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_bind_flag_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_bind_state_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_bind_state_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_screen_load_event_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_screen_load_event_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_screen_create_event_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_screen_create_event_apply(lv_xml_parser_state_t * state, const char ** attrs);

void * lv_obj_xml_play_timeline_event_create(lv_xml_parser_state_t * state, const char ** attrs);
void lv_obj_xml_play_timeline_event_apply(lv_xml_parser_state_t * state, const char ** attrs);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_OBJ_PARSER_H*/
