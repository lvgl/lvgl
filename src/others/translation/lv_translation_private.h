/**
 * @file lv_translation_private.h
 *
 */

#ifndef LV_TRANSLATION_PRIVATE_H
#define LV_TRANSLATION_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_TRANSLATION

#include LV_STDINT_INCLUDE
#include "../../misc/lv_array.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_translation_tag_dsc_t {
    const char * tag;
    const char ** translations; /**< Translations for each language*/
};

struct _lv_translation_pack_t {
    const char ** languages;
    uint32_t language_cnt;
    uint32_t is_static; /*In the union translations_p is used*/
    const char ** tag_p;
    const char ** translation_p; /*E.g. {{"a", "b"}, {"c", "d"}}*/
    lv_array_t translation_array;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TRANSLATION*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_TRANSLATION_PRIVATE_H */
