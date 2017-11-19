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
#include "lv_conf.h"
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/
/*Predefined keys to control the focused object via lv_group_send(group, c)*/
/*For compatibility in signal function define the keys regardless to LV_OBJ_GROUP*/
#define LV_GROUP_KEY_UP             17      /*0x11*/
#define LV_GROUP_KEY_DOWN           18      /*0x12*/
#define LV_GROUP_KEY_RIGHT          19      /*0x13*/
#define LV_GROUP_KEY_LEFT           20      /*0x14*/
#define LV_GROUP_KEY_ESC            33      /*0x1B*/
#define LV_GROUP_KEY_ENTER          10      /*0x0A, '\n'*/
#define LV_GROUP_KEY_NEXT           9       /*0x09, '\t'*/
#define LV_GROUP_KEY_PREV           11      /*0x0B, '*/

#if LV_OBJ_GROUP  != 0
/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_group_t
{
    ll_dsc_t obj_ll;
    lv_obj_t ** obj_focus;
    void (*style_mod)(lv_style_t * style);
    lv_style_t style_tmp;
    uint8_t frozen:1;
}lv_group_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_group_t * lv_group_create(void);
void lv_group_add_obj(lv_group_t * group, lv_obj_t * obj);
void lv_group_rem_obj(lv_obj_t * obj);
void lv_group_focus_obj(lv_obj_t * obj);
void lv_group_focus_next(lv_group_t * group);
void lv_group_focus_prev(lv_group_t * group);
void lv_group_focus_freeze(lv_group_t * group, bool en);
void lv_group_send(lv_group_t * group, uint32_t c);
lv_style_t * lv_group_mod_style(lv_group_t * group, const lv_style_t * style);
lv_obj_t * lv_group_get_focused(lv_group_t * group);

/**********************
 *      MACROS
 **********************/

#endif /*LV_OBJ_GROUP != 0*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GROUP_H*/
