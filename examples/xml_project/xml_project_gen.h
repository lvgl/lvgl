/**
 * @file xml_project_gen.h
 */

#ifndef XML_PROJECT_GEN_H
#define XML_PROJECT_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
    #define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lvgl_private.h"
#else
#include "lvgl/lvgl.h"
#include "lvgl/lvgl_private.h"
#endif

#ifdef LV_USE_XML
#include "lv_xml/lv_xml.h"
#endif



/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Fonts
 *----------------*/

extern lv_font_t * font_large;

/*----------------
 * Images
 *----------------*/

extern const void * img_example_lvgl_logo;
extern const void * img_arc_bg;
extern const void * img_arc_indicator;
extern const void * img_bar_bg;
extern const void * img_bar_indicator;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t subject_value;
extern lv_subject_t subject_value2;
extern lv_subject_t subject_opa;
extern lv_subject_t subject_index;
extern lv_subject_t subject_flag;
extern lv_subject_t subject_text;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void xml_project_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "screens/lv_example_slider_img_indicator_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*XML_PROJECT_GEN_H*/