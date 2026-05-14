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

/*----------------
 * Images
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

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
#include "screens/lv_example_tabview_tab_button_content_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*XML_PROJECT_GEN_H*/