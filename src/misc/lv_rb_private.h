/**
 * @file lv_rb_private.h
 *
 */

#ifndef LV_RB_PRIVATE_H
#define LV_RB_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_rb.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_rb_node_t {
    struct lv_rb_node_t * parent;
    struct lv_rb_node_t * left;
    struct lv_rb_node_t * right;
    lv_rb_color_t color;
    void * data;
};

struct lv_rb_t {
    lv_rb_node_t * root;
    lv_rb_compare_t compare;
    size_t size;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_RB_PRIVATE_H*/
