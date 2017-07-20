/**
 * @file lv_group.h
 *
 */

#ifndef LV_GROUP_H
#define LV_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    ll_dsc_t obj_ll;
    lv_obj_t ** actve_obj;
    void (*style_activate)(lv_style_t * style);
    lv_style_t style_tmp;
}lv_group_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_group_t * lv_group_create(void);
void lv_group_add(lv_group_t * group, lv_obj_t * obj);
void lv_group_activate_obj(lv_group_t * group, lv_obj_t * obj);
void lv_group_activate_next(lv_group_t * group);
void lv_group_activate_prev(lv_group_t * group);
lv_style_t * lv_group_activate_style(lv_group_t * group, lv_style_t * style);
lv_obj_t * lv_group_get_active(lv_group_t * group);
void lv_group_inc_active(lv_group_t * group);
void lv_group_dec_active(lv_group_t * group);
void lv_group_sel_active(lv_group_t * group);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GROUP_H*/
