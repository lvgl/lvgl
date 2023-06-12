/**
* @file lv_rb.h
*
*/


#ifndef LV_RB_H
#define LV_RB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_types.h"

#include "../misc/lv_gc.h"
#include "../misc/lv_mem.h"
#include "lv_assert.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_RB_COLOR_RED,
    LV_RB_COLOR_BLACK
} lv_rb_color_t;

typedef struct lv_rb_node_t {
    struct lv_rb_node_t * parent;
    struct lv_rb_node_t * left;
    struct lv_rb_node_t * right;
    lv_rb_color_t color;
    void * key;
    void * data;
} lv_rb_node_t;

typedef int (*lv_rb_compare_t)(const void * a, const void * b);

typedef struct {
    lv_rb_node_t * root;
    lv_rb_compare_t compare;
} lv_rb_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_rb_t * lv_rb_create(lv_rb_compare_t compare);
bool lv_rb_insert(lv_rb_t * tree, void * key, void * data);
void * lv_rb_search(lv_rb_t * tree, const void * key);
void lv_rb_delete(lv_rb_t * tree, const void * key);
void lv_rb_destroy(lv_rb_t * tree);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_RB_H*/
