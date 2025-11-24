/**
 * @file lv_xml_load_private.h
 *
 */

#ifndef LV_XML_LOAD_PRIVATE_H
#define LV_XML_LOAD_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_xml_load.h"
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

void lv_xml_load_init(void);

void lv_xml_load_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_XML*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_LOAD_PRIVATE_H*/
