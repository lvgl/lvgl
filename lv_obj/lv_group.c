/**
 * @file lv_group.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_group.h"
#if LV_OBJ_GROUP != 0
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void style_mod_def(lv_style_t * style);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a new object group
 * @return pointer to the new object group
 */
lv_group_t * lv_group_create(void)
{
    lv_group_t * group = dm_alloc(sizeof(lv_group_t));
    ll_init(&group->obj_ll, sizeof(lv_obj_t *));

    group->style_mod = style_mod_def;
    group->obj_focus = NULL;
    group->frozen = 0;

    return group;
}

/**
 * Add an object to a group
 * @param group pointer to a group
 * @param obj pointer to an object to add
 */
void lv_group_add_obj(lv_group_t * group, lv_obj_t * obj)
{
    obj->group_p = group;
    lv_obj_t ** next = ll_ins_tail(&group->obj_ll);
    *next = obj;

    /* If the head and the tail is equal then there is only one object in the linked list.
     * In this case automatically activate it*/
    if(ll_get_head(&group->obj_ll) == next) {
        lv_group_focus_next(group);
    }
}

/**
 * Remove an object from its group
 * @param obj pointer to an objectto remove
 */
void lv_group_rem_obj(lv_obj_t * obj)
{
    lv_group_t * g = obj->group_p;
    if(g == NULL) return;
    lv_obj_t ** i;

    LL_READ(g->obj_ll, i) {
        if(*i == obj) {
            ll_rem(&g->obj_ll, i);
            break;
        }
    }

    if(*g->obj_focus == obj) {
        g->obj_focus = NULL;
        lv_group_focus_next(g);
    }
}


/**
 * Focus on an object (defocus the current)
 * @param obj pointer to an object to focus on
 */
void lv_group_focus_obj(lv_obj_t * obj)
{
    lv_group_t * g = obj->group_p;
    if(g == NULL) return;

    if(g->frozen != 0) return;

    lv_obj_t ** i;

    LL_READ(g->obj_ll, i) {
        if(*i == obj) {
            if(g->obj_focus != NULL) {
                (*g->obj_focus)->signal_f(*g->obj_focus, LV_SIGNAL_DEFOCUS, NULL);
                lv_obj_inv(*g->obj_focus);
            }

            g->obj_focus = i;

            if(g->obj_focus != NULL){
                (*g->obj_focus)->signal_f(*g->obj_focus, LV_SIGNAL_FOCUS, NULL);
                lv_obj_inv(*g->obj_focus);
            }
            break;
        }
    }
}

/**
 * Focus the next object in a group (defocus the current)
 * @param group pointer to a group
 */
void lv_group_focus_next(lv_group_t * group)
{
    if(group->frozen != 0) return;

    if(group->obj_focus != NULL) {
        (*group->obj_focus)->signal_f(*group->obj_focus, LV_SIGNAL_DEFOCUS, NULL);
        lv_obj_inv(*group->obj_focus);
    }

    lv_obj_t ** obj_next;
    if(group->obj_focus == NULL) obj_next = ll_get_head(&group->obj_ll);
    else obj_next = ll_get_next(&group->obj_ll, group->obj_focus);

    if(obj_next == NULL) obj_next = ll_get_head(&group->obj_ll);
    group->obj_focus = obj_next;

    if(group->obj_focus != NULL){
        (*group->obj_focus)->signal_f(*group->obj_focus, LV_SIGNAL_FOCUS, NULL);
        lv_obj_inv(*group->obj_focus);
    }
}

/**
 * Focus the previous object in a group (defocus the current)
 * @param group pointer to a group
 */
void lv_group_focus_prev(lv_group_t * group)
{
    if(group->frozen != 0) return;

    if(group->obj_focus != NULL) {
        (*group->obj_focus)->signal_f(*group->obj_focus, LV_SIGNAL_DEFOCUS, NULL);
        lv_obj_inv(*group->obj_focus);
    }

    lv_obj_t ** obj_next;
    if(group->obj_focus == NULL) obj_next = ll_get_tail(&group->obj_ll);
    else obj_next = ll_get_prev(&group->obj_ll, group->obj_focus);

    if(obj_next == NULL) obj_next = ll_get_tail(&group->obj_ll);
    group->obj_focus = obj_next;

    if(group->obj_focus != NULL){
        (*group->obj_focus)->signal_f(*group->obj_focus, LV_SIGNAL_FOCUS, NULL);
        lv_obj_inv(*group->obj_focus);
    }

}

/**
 * Do not let to change the focus from the current object
 * @param group pointer to a group
 * @param en true: freeze, false: release freezing (normal mode)
 */
void lv_group_focus_freeze(lv_group_t * group, bool en)
{
    if(en == false) group->frozen = 0;
    else group->frozen = 1;
}

/**
 * Send a control character to the focuses object of a group
 * @param group pointer to a group
 * @param c a control character (use LV_GROUP_KEY_.. to navigate)
 */
void lv_group_send(lv_group_t * group, char c)
{
    lv_obj_t * act = lv_group_get_focused(group);
    if(act == NULL) return;

    act->signal_f(act, LV_SIGNAL_CONTROLL, &c);
}


/**
 * Set a function for a group which will modify the object's style if it is in focus
 * @param group pointer to a group
 * @param style_cb the style modifier function pointer
 */
void lv_group_set_style_mod_cb(lv_group_t * group, void (*style_cb)(lv_style_t * style))
{
    group->style_mod = style_cb;
    if(group->obj_focus != NULL) lv_obj_inv(*group->obj_focus);
}

/**
 * Modify a style with the set 'style_mod' function. The input style remains unchanged.
 * @param group pointer to group
 * @param style pointer to a style to modify
 * @return a copy of the input style but modified with the 'style_mod' function
 */
lv_style_t * lv_group_mod_style(lv_group_t * group, const lv_style_t * style)
{
    lv_style_cpy(&group->style_tmp, style);

    if(group->style_mod != NULL) group->style_mod(&group->style_tmp);
    else style_mod_def(&group->style_tmp);

    return &group->style_tmp;
}

/**
 * Get the focused object or NULL if there isn't one
 * @param group pointer to a group
 * @return pointer to the focused object
 */
lv_obj_t * lv_group_get_focused(lv_group_t * group)
{
    if(group == NULL) return NULL;
    if(group->obj_focus == NULL) return NULL;

    return *group->obj_focus;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Default style modifier function
 * @param style pointer to a style to modify. (Typically &group->style_tmp) It will be OVERWRITTEN.
 */
static void style_mod_def(lv_style_t * style)
{
    /*Make the style a little bit orange*/
    style->border.color = COLOR_ORANGE;
    style->border.opa = OPA_COVER;
    if(style->border.width == 0 && style->body.empty == 0) style->border.width = 2 * LV_DOWNSCALE;   /*Add border to not transparent styles*/
    else style->border.width = style->border.width * 2;                                         /*Make the border thicker*/
    style->body.color_main = color_mix(style->body.color_main, COLOR_ORANGE, OPA_80);
    style->body.color_grad = color_mix(style->body.color_grad, COLOR_ORANGE, OPA_80);
}

#endif /*LV_OBJ_GROUP != 0*/
