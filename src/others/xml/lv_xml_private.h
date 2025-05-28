/**
 * @file lv_xml_private.h
 *
 */

#ifndef LV_XML_PRIVATE_H
#define LV_XML_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml.h"
#if LV_USE_XML

#include "parsers/lv_xml_obj_parser.h"
#include "lv_xml_parser.h"
#include "lv_xml_base_types.h"
#include "lv_xml_utils.h"
#include "lv_xml_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    const lv_font_t * font;
    void (*font_destroy_cb)(lv_font_t *);
} lv_xml_font_t;

typedef struct {
    const char * name;
    const void * src;
} lv_xml_image_t;

typedef struct {
    const char * name;
    lv_event_cb_t cb;
} lv_xml_event_cb_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_PRIVATE_H*/
