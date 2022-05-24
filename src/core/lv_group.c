/**
 * @file lv_group.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>

#include "lv_group.h"
#include "../misc/lv_gc.h"
#include "../core/lv_obj.h"
#include "../core/lv_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool focus_next_core(lv_group_t * group, void * (*begin)(const lv_ll_t *),
                            void * (*move)(const lv_ll_t *, const void *));
static void lv_group_refocus(lv_group_t * g);
static lv_indev_t * get_indev(const lv_group_t * g);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_group_t * default_group;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_group_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_group_ll), sizeof(lv_group_t));
}

lv_group_t * lv_group_create(void)
{
    lv_group_t * group = _lv_ll_ins_head(&LV_GC_ROOT(_lv_group_ll));
    LV_ASSERT_MALLOC(group);
    if(group == NULL) return NULL;
    _lv_ll_init(&group->obj_ll, sizeof(lv_obj_t *));

    group->obj_focus      = NULL;
    group->frozen         = 0;
    group->focus_cb       = NULL;
    group->edge_cb        = NULL;
    group->editing        = 0;
    group->refocus_policy = LV_GROUP_REFOCUS_POLICY_PREV;
    group->wrap           = 1;

#if LV_USE_USER_DATA
    group->user_data = NULL;
#endif

    return group;
}

void lv_group_del(lv_group_t * group)
{
    /*Defocus the currently focused object*/
    if(group->obj_focus != NULL) {
        lv_event_send(*group->obj_focus, LV_EVENT_DEFOCUSED, get_indev(group));
        lv_obj_invalidate(*group->obj_focus);
    }

    /*Remove the objects from the group*/
    lv_obj_t ** obj;
    _LV_LL_READ(&group->obj_ll, obj) {
        if((*obj)->spec_attr)(*obj)->spec_attr->group_p = NULL;
    }

    /*Remove the group from any indev devices */
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(indev->group == group) {
            lv_indev_set_group(indev, NULL);
        }
        indev = lv_indev_get_next(indev);
    }

    _lv_ll_clear(&(group->obj_ll));
    _lv_ll_remove(&LV_GC_ROOT(_lv_group_ll), group);
    lv_mem_free(group);
}

void lv_group_set_default(lv_group_t * group)
{
    default_group = group;
}

lv_group_t * lv_group_get_default(void)
{
    return default_group;
}

void lv_group_add_obj(lv_group_t * group, lv_obj_t * obj)
{
    if(group == NULL) return;

    LV_LOG_TRACE("begin");

    /*Do not add the object twice*/
    lv_obj_t ** obj_i;
    _LV_LL_READ(&group->obj_ll, obj_i) {
        if((*obj_i) == obj) {
            LV_LOG_INFO("the object is already added to this group");
            return;
        }
    }

    /*If the object is already in a group and focused then refocus it*/
    lv_group_t * group_cur = lv_obj_get_group(obj);
    if(group_cur) {
        if(obj->spec_attr->group_p && *(obj->spec_attr->group_p->obj_focus) == obj) {
            lv_group_refocus(group_cur);

            LV_LOG_INFO("changing object's group");
        }
    }

    if(obj->spec_attr == NULL) lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->group_p = group;

    lv_obj_t ** next = _lv_ll_ins_tail(&group->obj_ll);
    LV_ASSERT_MALLOC(next);
    if(next == NULL) return;
    *next = obj;

    /*If the head and the tail is equal then there is only one object in the linked list.
     *In this case automatically activate it*/
    if(_lv_ll_get_head(&group->obj_ll) == next) {
        lv_group_refocus(group);
    }

    LV_LOG_TRACE("finished");
}

void lv_group_swap_obj(lv_obj_t * obj1, lv_obj_t * obj2)
{
    lv_group_t * g1 = lv_obj_get_group(obj1);
    lv_group_t * g2 = lv_obj_get_group(obj2);
    if(g1 != g2) return;
    if(g1 == NULL) return;

    /*Do not add the object twice*/
    lv_obj_t ** obj_i;
    _LV_LL_READ(&g1->obj_ll, obj_i) {
        if((*obj_i) == obj1)(*obj_i) = obj2;
        else if((*obj_i) == obj2)(*obj_i) = obj1;
    }

    if(*g1->obj_focus == obj1) lv_group_focus_obj(obj2);
    else if(*g1->obj_focus == obj2) lv_group_focus_obj(obj1);

}

void lv_group_remove_obj(lv_obj_t * obj)
{
    lv_group_t * g = lv_obj_get_group(obj);
    if(g == NULL) return;

    LV_LOG_TRACE("begin");

    /*Focus on the next object*/
    if(g->obj_focus && *g->obj_focus == obj) {
        if(g->frozen) g->frozen = 0;

        /*If this is the only object in the group then focus to nothing.*/
        if(_lv_ll_get_head(&g->obj_ll) == g->obj_focus && _lv_ll_get_tail(&g->obj_ll) == g->obj_focus) {
            lv_event_send(*g->obj_focus, LV_EVENT_DEFOCUSED, get_indev(g));
        }
        /*If there more objects in the group then focus to the next/prev object*/
        else {
            lv_group_refocus(g);
        }
    }

    /*If the focuses object is still the same then it was the only object in the group but it will
     *be deleted. Set the `obj_focus` to NULL to get back to the initial state of the group with
     *zero objects*/
    if(g->obj_focus && *g->obj_focus == obj) {
        g->obj_focus = NULL;
    }

    /*Search the object and remove it from its group*/
    lv_obj_t ** i;
    _LV_LL_READ(&g->obj_ll, i) {
        if(*i == obj) {
            _lv_ll_remove(&g->obj_ll, i);
            lv_mem_free(i);
            if(obj->spec_attr) obj->spec_attr->group_p = NULL;
            break;
        }
    }
    LV_LOG_TRACE("finished");
}

void lv_group_remove_all_objs(lv_group_t * group)
{
    /*Defocus the currently focused object*/
    if(group->obj_focus != NULL) {
        lv_event_send(*group->obj_focus, LV_EVENT_DEFOCUSED, get_indev(group));
        lv_obj_invalidate(*group->obj_focus);
        group->obj_focus = NULL;
    }

    /*Remove the objects from the group*/
    lv_obj_t ** obj;
    _LV_LL_READ(&group->obj_ll, obj) {
        if((*obj)->spec_attr)(*obj)->spec_attr->group_p = NULL;
    }

    _lv_ll_clear(&(group->obj_ll));
}

void lv_group_focus_obj(lv_obj_t * obj)
{
    if(obj == NULL) return;
    lv_group_t * g = lv_obj_get_group(obj);
    if(g == NULL) return;

    if(g->frozen != 0) return;

    /*On defocus edit mode must be leaved*/
    lv_group_set_editing(g, false);

    lv_obj_t ** i;
    _LV_LL_READ(&g->obj_ll, i) {
        if(*i == obj) {
            if(g->obj_focus != NULL && obj != *g->obj_focus) {  /*Do not defocus if the same object needs to be focused again*/
                lv_res_t res = lv_event_send(*g->obj_focus, LV_EVENT_DEFOCUSED, get_indev(g));
                if(res != LV_RES_OK) return;
                lv_obj_invalidate(*g->obj_focus);
            }

            g->obj_focus = i;

            if(g->obj_focus != NULL) {
                if(g->focus_cb) g->focus_cb(g);
                lv_res_t res = lv_event_send(*g->obj_focus, LV_EVENT_FOCUSED, get_indev(g));
                if(res != LV_RES_OK) return;
                lv_obj_invalidate(*g->obj_focus);
            }
            break;
        }
    }
}

void lv_group_focus_next(lv_group_t * group)
{
    bool focus_changed = focus_next_core(group, _lv_ll_get_head, _lv_ll_get_next);
    if(group->edge_cb) {
        if(!focus_changed)
            group->edge_cb(group, true);
    }
}

void lv_group_focus_prev(lv_group_t * group)
{
    bool focus_changed = focus_next_core(group, _lv_ll_get_tail, _lv_ll_get_prev);
    if(group->edge_cb) {
        if(!focus_changed)
            group->edge_cb(group, false);
    }
}

void lv_group_focus_freeze(lv_group_t * group, bool en)
{
    if(en == false) group->frozen = 0;
    else group->frozen = 1;
}

lv_res_t lv_group_send_data(lv_group_t * group, uint32_t c)
{
    lv_obj_t * act = lv_group_get_focused(group);
    if(act == NULL) return LV_RES_OK;

    if(lv_obj_has_state(act, LV_STATE_DISABLED)) return LV_RES_OK;

    return lv_event_send(act, LV_EVENT_KEY, &c);
}

void lv_group_set_focus_cb(lv_group_t * group, lv_group_focus_cb_t focus_cb)
{
    group->focus_cb = focus_cb;
}

void lv_group_set_edge_cb(lv_group_t * group, lv_group_edge_cb_t edge_cb)
{
    group->edge_cb = edge_cb;
}

void lv_group_set_editing(lv_group_t * group, bool edit)
{
    if(group == NULL) return;
    uint8_t en_val = edit ? 1 : 0;

    if(en_val == group->editing) return; /*Do not set the same mode again*/

    group->editing     = en_val;
    lv_obj_t * focused = lv_group_get_focused(group);

    if(focused) {
        lv_res_t res = lv_event_send(*group->obj_focus, LV_EVENT_FOCUSED, get_indev(group));
        if(res != LV_RES_OK) return;

        lv_obj_invalidate(focused);
    }
}

void lv_group_set_refocus_policy(lv_group_t * group, lv_group_refocus_policy_t policy)
{
    group->refocus_policy = policy & 0x01;
}

void lv_group_set_wrap(lv_group_t * group, bool en)
{
    group->wrap = en ? 1 : 0;
}

lv_obj_t * lv_group_get_focused(const lv_group_t * group)
{
    if(!group) return NULL;
    if(group->obj_focus == NULL) return NULL;

    return *group->obj_focus;
}

lv_group_focus_cb_t lv_group_get_focus_cb(const lv_group_t * group)
{
    if(!group) return NULL;
    return group->focus_cb;
}

lv_group_edge_cb_t lv_group_get_edge_cb(const lv_group_t * group)
{
    if(!group) return NULL;
    return group->edge_cb;
}

bool lv_group_get_editing(const lv_group_t * group)
{
    if(!group) return false;
    return group->editing ? true : false;
}

bool lv_group_get_wrap(lv_group_t * group)
{
    if(!group) return false;
    return group->wrap ? true : false;
}

uint32_t lv_group_get_obj_count(lv_group_t * group)
{
    return _lv_ll_get_len(&group->obj_ll);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_group_refocus(lv_group_t * g)
{
    /*Refocus must temporarily allow wrapping to work correctly*/
    uint8_t temp_wrap = g->wrap;
    g->wrap           = 1;

    if(g->refocus_policy == LV_GROUP_REFOCUS_POLICY_NEXT)
        lv_group_focus_next(g);
    else if(g->refocus_policy == LV_GROUP_REFOCUS_POLICY_PREV)
        lv_group_focus_prev(g);
    /*Restore wrap property*/
    g->wrap = temp_wrap;
}

static bool focus_next_core(lv_group_t * group, void * (*begin)(const lv_ll_t *),
                            void * (*move)(const lv_ll_t *, const void *))
{
    bool focus_changed = false;
    if(group->frozen) return focus_changed;

    lv_obj_t ** obj_next     = group->obj_focus;
    lv_obj_t ** obj_sentinel = NULL;
    bool can_move            = true;
    bool can_begin           = true;

    for(;;) {
        if(obj_next == NULL) {
            if(group->wrap || obj_sentinel == NULL) {
                if(!can_begin) return focus_changed;
                obj_next  = begin(&group->obj_ll);
                can_move  = false;
                can_begin = false;
            }
            else {
                /*Currently focused object is the last/first in the group, keep it that way*/
                return focus_changed;
            }
        }

        if(obj_sentinel == NULL) {
            obj_sentinel = obj_next;
            if(obj_sentinel == NULL) return focus_changed; /*Group is empty*/
        }

        if(can_move) {
            obj_next = move(&group->obj_ll, obj_next);

            /*Give up if we walked the entire list and haven't found another visible object*/
            if(obj_next == obj_sentinel) return focus_changed;
        }

        can_move = true;

        if(obj_next == NULL) continue;
        if(lv_obj_get_state(*obj_next) & LV_STATE_DISABLED) continue;

        /*Hidden objects don't receive focus.
         *If any parent is hidden, the object is also hidden)*/
        lv_obj_t * parent = *obj_next;
        while(parent) {
            if(lv_obj_has_flag(parent, LV_OBJ_FLAG_HIDDEN)) break;
            parent = lv_obj_get_parent(parent);
        }

        if(parent && lv_obj_has_flag(parent, LV_OBJ_FLAG_HIDDEN)) continue;

        /*If we got her a good candidate is found*/
        break;
    }

    if(obj_next == group->obj_focus) return focus_changed; /*There's only one visible object and it's already focused*/

    if(group->obj_focus) {
        lv_res_t res = lv_event_send(*group->obj_focus, LV_EVENT_DEFOCUSED, get_indev(group));
        if(res != LV_RES_OK) return focus_changed;
        lv_obj_invalidate(*group->obj_focus);
    }

    group->obj_focus = obj_next;

    lv_res_t res = lv_event_send(*group->obj_focus, LV_EVENT_FOCUSED, get_indev(group));
    if(res != LV_RES_OK) return focus_changed;

    lv_obj_invalidate(*group->obj_focus);

    if(group->focus_cb) group->focus_cb(group);
    focus_changed = true;
    return focus_changed;
}

/**
 * Find an indev preferably with KEYPAD or ENCOEDR type that uses the given group.
 * In other words, find an indev, that is related to the given group.
 * In the worst case simply return the latest indev
 * @param g     a group the find in the indevs
 * @return      the suggested indev
 */
static lv_indev_t * get_indev(const lv_group_t * g)
{
    lv_indev_t * indev_encoder = NULL;
    lv_indev_t * indev_group = NULL;
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev->group == g) {
            /*Prefer KEYPAD*/
            if(indev_type == LV_INDEV_TYPE_KEYPAD) return indev;
            if(indev_type == LV_INDEV_TYPE_ENCODER) indev_encoder = indev;
            indev_group = indev;
        }
        indev = lv_indev_get_next(indev);
    }

    if(indev_encoder) return indev_encoder;
    if(indev_group) return indev_group;

    /*In lack of a better option use the first input device. (It can be NULL if there is no input device)*/
    return lv_indev_get_next(NULL);
}

