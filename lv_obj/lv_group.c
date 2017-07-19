/**
 * @file lv_group.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_group.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void style_activate_def(lv_style_t * style);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_group_t * lv_group_create(void)
{
    lv_group_t * group = dm_alloc(sizeof(lv_group_t));
    ll_init(&group->obj_ll, sizeof(lv_obj_t *));

    group->style_activate = style_activate_def;
    group->actve_obj = NULL;

    return group;
}

void lv_group_add(lv_group_t * group, lv_obj_t * obj)
{
    obj->group_p = group;
    lv_obj_t ** next = ll_ins_tail(&group->obj_ll);
    *next = obj;
}

void lv_group_activate_obj(lv_group_t * group, lv_obj_t * obj)
{

}

void lv_group_set_style_cb(lv_group_t * group, void (*style_cb)(lv_style_t * style))
{
    group->style_activate = style_cb;
}

void lv_group_activate_next(lv_group_t * group)
{
    if(group->actve_obj != NULL) lv_obj_inv(*group->actve_obj);

    lv_obj_t ** obj_next;
    if(group->actve_obj == NULL) obj_next = ll_get_head(&group->obj_ll);
    else obj_next = ll_get_next(&group->obj_ll, group->actve_obj);

    if(obj_next == NULL) obj_next = ll_get_head(&group->obj_ll);
    group->actve_obj = obj_next;

    if(group->actve_obj != NULL) lv_obj_inv(*group->actve_obj);
}

void lv_group_activate_prev(lv_group_t * group)
{
    if(group->actve_obj != NULL) lv_obj_inv(*group->actve_obj);

    lv_obj_t ** obj_next;
    if(group->actve_obj == NULL) obj_next = ll_get_tail(&group->obj_ll);
    else obj_next = ll_get_prev(&group->obj_ll, group->actve_obj);

    if(obj_next == NULL) obj_next = ll_get_tail(&group->obj_ll);
    group->actve_obj = obj_next;

    if(group->actve_obj != NULL) lv_obj_inv(*group->actve_obj);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_activate_def(lv_style_t * style)
{
    style->bcolor = COLOR_ORANGE;
    style->bopa = OPA_COVER;
    style->bwidth = style->bwidth * 2;
    style->mcolor = color_mix(style->mcolor, COLOR_ORANGE, OPA_80);
    style->gcolor = color_mix(style->gcolor, COLOR_ORANGE, OPA_80);
}
