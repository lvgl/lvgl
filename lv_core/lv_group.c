/**
 * @file lv_group.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_group.h"
#if USE_LV_GROUP != 0
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
    lv_group_t * group = lv_mem_alloc(sizeof(lv_group_t));
    lv_ll_init(&group->obj_ll, sizeof(lv_obj_t *));

    group->style_mod = style_mod_def;
    group->obj_focus = NULL;
    group->frozen = 0;
    group->focus_cb = NULL;

    return group;
}

/**
 * Delete a group object
 * @param group pointer to a group
 */
void lv_group_del(lv_group_t * group)
{
	/*Defocus the the currently focussed object*/
    if(group->obj_focus != NULL) {
        (*group->obj_focus)->signal_func(*group->obj_focus, LV_SIGNAL_DEFOCUS, NULL);
        lv_obj_invalidate(*group->obj_focus);
    }

	/*Remove the objects from the group*/
	lv_obj_t ** obj;
	LL_READ(group->obj_ll, obj) {
		(*obj)->group_p = NULL;
	}

    lv_ll_clear(&(group->obj_ll));
    lv_mem_free(group);
}

/**
 * Add an object to a group
 * @param group pointer to a group
 * @param obj pointer to an object to add
 */
void lv_group_add_obj(lv_group_t * group, lv_obj_t * obj)
{
    obj->group_p = group;
    lv_obj_t ** next = lv_ll_ins_tail(&group->obj_ll);
    *next = obj;

    /* If the head and the tail is equal then there is only one object in the linked list.
     * In this case automatically activate it*/
    if(lv_ll_get_head(&group->obj_ll) == next) {
        lv_group_focus_next(group);
    }
}

/**
 * Remove an object from its group
 * @param obj pointer to an object to remove
 */
void lv_group_remove_obj(lv_obj_t * obj)
{
    lv_group_t * g = obj->group_p;
    if(g == NULL) return;

    if(*g->obj_focus == obj) {
         lv_group_focus_next(g);
     }

    /*Search the object and remove it from its group */
    lv_obj_t ** i;
    LL_READ(g->obj_ll, i) {
        if(*i == obj) {
            lv_ll_rem(&g->obj_ll, i);
            lv_mem_free(i);
            obj->group_p = NULL;
        }
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
                (*g->obj_focus)->signal_func(*g->obj_focus, LV_SIGNAL_DEFOCUS, NULL);
                lv_obj_invalidate(*g->obj_focus);
            }

            g->obj_focus = i;

            if(g->obj_focus != NULL){
                (*g->obj_focus)->signal_func(*g->obj_focus, LV_SIGNAL_FOCUS, NULL);
                lv_obj_invalidate(*g->obj_focus);
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
    if(group->frozen) return;

    if(group->obj_focus) {
        (*group->obj_focus)->signal_func(*group->obj_focus, LV_SIGNAL_DEFOCUS, NULL);
        lv_obj_invalidate(*group->obj_focus);
    }

    lv_obj_t ** obj_next;
    if(group->obj_focus == NULL) obj_next = lv_ll_get_head(&group->obj_ll);
    else obj_next = lv_ll_get_next(&group->obj_ll, group->obj_focus);

    if(obj_next == NULL) obj_next = lv_ll_get_head(&group->obj_ll);
    group->obj_focus = obj_next;

    if(group->obj_focus){
        (*group->obj_focus)->signal_func(*group->obj_focus, LV_SIGNAL_FOCUS, NULL);
        lv_obj_invalidate(*group->obj_focus);

        if(group->focus_cb) group->focus_cb(group);
    }
}

/**
 * Focus the previous object in a group (defocus the current)
 * @param group pointer to a group
 */
void lv_group_focus_prev(lv_group_t * group)
{
    if(group->frozen) return;

    if(group->obj_focus) {
        (*group->obj_focus)->signal_func(*group->obj_focus, LV_SIGNAL_DEFOCUS, NULL);
        lv_obj_invalidate(*group->obj_focus);
    }

    lv_obj_t ** obj_next;
    if(group->obj_focus == NULL) obj_next = lv_ll_get_tail(&group->obj_ll);
    else obj_next = lv_ll_get_prev(&group->obj_ll, group->obj_focus);

    if(obj_next == NULL) obj_next = lv_ll_get_tail(&group->obj_ll);
    group->obj_focus = obj_next;

    if(group->obj_focus != NULL){
        (*group->obj_focus)->signal_func(*group->obj_focus, LV_SIGNAL_FOCUS, NULL);
        lv_obj_invalidate(*group->obj_focus);

        if(group->focus_cb) group->focus_cb(group);
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
 * @param c a character (use LV_GROUP_KEY_.. to navigate)
 */
void lv_group_send_data(lv_group_t * group, uint32_t c)
{
    lv_obj_t * act = lv_group_get_focused(group);
    if(act == NULL) return;

    act->signal_func(act, LV_SIGNAL_CONTROLL, &c);
}

/**
 * Set a function for a group which will modify the object's style if it is in focus
 * @param group pointer to a group
 * @param style_mod_func the style modifier function pointer
 */
void lv_group_set_style_mod_cb(lv_group_t * group,lv_group_style_mod_func_t style_mod_func)
{
    group->style_mod = style_mod_func;
    if(group->obj_focus != NULL) lv_obj_invalidate(*group->obj_focus);
}


/**
 * Set a function for a group which will be called when a new object is focused
 * @param group pointer to a group
 * @param focus_cb the call back function or NULL if unused
 */
void lv_group_set_focus_cb(lv_group_t * group, lv_group_focus_cb_t focus_cb)
{
    group->focus_cb = focus_cb;
}

/**
 * Modify a style with the set 'style_mod' function. The input style remains unchanged.
 * @param group pointer to group
 * @param style pointer to a style to modify
 * @return a copy of the input style but modified with the 'style_mod' function
 */
lv_style_t * lv_group_mod_style(lv_group_t * group, const lv_style_t * style)
{
    lv_style_copy(&group->style_tmp, style);

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

/**
 * Get a the style modifier function of a group
 * @param group pointer to a group
 * @return pointer to the style modifier function
 */
lv_group_style_mod_func_t lv_group_get_style_mod_cb(lv_group_t * group)
{
    return group->style_mod ;
}

/**
 * Get the focus callback function of a group
 * @param group pointer to a group
 * @return the call back function or NULL if not set
 */
lv_group_focus_cb_t lv_group_get_focus_cb(lv_group_t * group)
{
    return group->focus_cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Default style modifier function
 * @param style pointer to a style to modify. (Typically group.style_tmp) It will be OVERWRITTEN.
 */
static void style_mod_def(lv_style_t * style)
{
#if LV_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_ORANGE;

    /*If not empty or has border then emphasis the border*/
    if(style->body.empty == 0 || style->body.border.width != 0) style->body.border.width = LV_DPI / 20;

    style->body.main_color = lv_color_mix(style->body.main_color, LV_COLOR_ORANGE, LV_OPA_70);
    style->body.grad_color = lv_color_mix(style->body.grad_color, LV_COLOR_ORANGE, LV_OPA_70);
    style->body.shadow.color = lv_color_mix(style->body.shadow.color, LV_COLOR_ORANGE, LV_OPA_60);

    style->text.color = lv_color_mix(style->text.color, LV_COLOR_ORANGE, LV_OPA_70);
#else
    style->body.border.opa = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_BLACK;
    style->body.border.width = 3;

#endif

}

#endif /*USE_LV_GROUP != 0*/
