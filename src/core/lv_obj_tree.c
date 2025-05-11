/**
 * @file lv_obj_tree.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_private.h"
#include "lv_obj_class_private.h"
#include "../indev/lv_indev.h"
#include "../indev/lv_indev_private.h"
#include "../display/lv_display.h"
#include "../display/lv_display_private.h"
#include "../misc/lv_anim_private.h"
#include "../misc/lv_async.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_obj_class)
#define disp_ll_p &(LV_GLOBAL_DEFAULT()->disp_ll)

#define OBJ_DUMP_STRING_LEN 128
#define LV_OBJ_NAME_MAX_LEN 128

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_obj_delete_async_cb(void * obj);
static void obj_delete_core(lv_obj_t * obj);
static lv_obj_tree_walk_res_t walk_core(lv_obj_t * obj, lv_obj_tree_walk_cb_t cb, void * user_data);
static void dump_tree_core(lv_obj_t * obj, int32_t depth);
static lv_obj_t * lv_obj_get_first_not_deleting_child(lv_obj_t * obj);
#if LV_USE_OBJ_NAME
    static lv_obj_t * find_by_name_direct(const lv_obj_t * parent, const char * name, size_t len);
#endif /*LV_USE_OBJ_NAME*/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_obj_delete(lv_obj_t * obj)
{
    if(obj->is_deleting)
        return;

    LV_LOG_TRACE("begin (delete %p)", (void *)obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_invalidate(obj);

    lv_obj_t * par = lv_obj_get_parent(obj);

    lv_display_t * disp = NULL;
    bool act_screen_del = false;
    if(par == NULL) {
        disp = lv_obj_get_display(obj);
        if(!disp) return;   /*Shouldn't happen*/
        if(disp->act_scr == obj) act_screen_del = true;
    }

    obj_delete_core(obj);

    /*Call the ancestor's event handler to the parent to notify it about the child delete*/
    if(par && !par->is_deleting) {
        lv_obj_scrollbar_invalidate(par);
        lv_obj_send_event(par, LV_EVENT_CHILD_CHANGED, NULL);
        lv_obj_send_event(par, LV_EVENT_CHILD_DELETED, NULL);
    }

    /*Handle if the active screen was deleted*/
    if(act_screen_del) {
        LV_LOG_WARN("the active screen was deleted");
        disp->act_scr = NULL;
    }

    LV_ASSERT_MEM_INTEGRITY();
    LV_LOG_TRACE("finished (delete %p)", (void *)obj);
}

void lv_obj_clean(lv_obj_t * obj)
{
    LV_LOG_TRACE("begin (clean %p)", (void *)obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_invalidate(obj);

    uint32_t cnt = lv_obj_get_child_count(obj);
    lv_obj_t * child = lv_obj_get_first_not_deleting_child(obj);
    while(child) {
        obj_delete_core(child);
        child = lv_obj_get_first_not_deleting_child(obj);
    }
    /*Just to remove scroll animations if any*/
    lv_obj_scroll_to(obj, 0, 0, LV_ANIM_OFF);
    if(obj->spec_attr) {
        obj->spec_attr->scroll.x = 0;
        obj->spec_attr->scroll.y = 0;
    }

    if(lv_obj_get_child_count(obj) < cnt) {
        lv_obj_send_event(obj, LV_EVENT_CHILD_CHANGED, NULL);
        lv_obj_send_event(obj, LV_EVENT_CHILD_DELETED, NULL);
    }

    LV_ASSERT_MEM_INTEGRITY();

    LV_LOG_TRACE("finished (clean %p)", (void *)obj);
}

void lv_obj_delete_delayed(lv_obj_t * obj, uint32_t delay_ms)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, NULL);
    lv_anim_set_duration(&a, 1);
    lv_anim_set_delay(&a, delay_ms);
    lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);
    lv_anim_start(&a);
}

void lv_obj_delete_anim_completed_cb(lv_anim_t * a)
{
    lv_obj_delete(a->var);
}

void lv_obj_delete_async(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_async_call(lv_obj_delete_async_cb, obj);
}

void lv_obj_set_parent(lv_obj_t * obj, lv_obj_t * parent)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_OBJ(parent, MY_CLASS);

    if(obj->parent == NULL) {
        LV_LOG_WARN("Can't set the parent of a screen");
        return;
    }

    if(parent == NULL) {
        LV_LOG_WARN("Can't set parent == NULL to an object");
        return;
    }

    if(parent == obj->parent) {
        return;
    }

    lv_obj_invalidate(obj);

    lv_obj_allocate_spec_attr(parent);

    lv_obj_t * old_parent = obj->parent;
    /*Remove the object from the old parent's child list*/
    int32_t i;
    for(i = lv_obj_get_index(obj); i <= (int32_t)lv_obj_get_child_count(old_parent) - 2; i++) {
        old_parent->spec_attr->children[i] = old_parent->spec_attr->children[i + 1];
    }
    old_parent->spec_attr->child_cnt--;
    if(old_parent->spec_attr->child_cnt) {
        old_parent->spec_attr->children = lv_realloc(old_parent->spec_attr->children,
                                                     old_parent->spec_attr->child_cnt * (sizeof(lv_obj_t *)));
    }
    else {
        lv_free(old_parent->spec_attr->children);
        old_parent->spec_attr->children = NULL;
    }

    /*Add the child to the new parent as the last (newest child)*/
    parent->spec_attr->child_cnt++;
    parent->spec_attr->children = lv_realloc(parent->spec_attr->children,
                                             parent->spec_attr->child_cnt * (sizeof(lv_obj_t *)));
    parent->spec_attr->children[lv_obj_get_child_count(parent) - 1] = obj;

    obj->parent = parent;

    /*Notify the original parent because one of its children is lost*/
    lv_obj_scrollbar_invalidate(old_parent);
    lv_obj_send_event(old_parent, LV_EVENT_CHILD_CHANGED, obj);
    lv_obj_send_event(old_parent, LV_EVENT_CHILD_DELETED, NULL);

    /*Notify the new parent about the child*/
    lv_obj_send_event(parent, LV_EVENT_CHILD_CHANGED, obj);
    lv_obj_send_event(parent, LV_EVENT_CHILD_CREATED, NULL);

    lv_obj_mark_layout_as_dirty(obj);

    lv_obj_invalidate(obj);
}

void lv_obj_move_to_index(lv_obj_t * obj, int32_t index)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /* Check parent validity */
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(!parent) {
        LV_LOG_WARN("parent is NULL");
        return;
    }

    const uint32_t parent_child_count = lv_obj_get_child_count(parent);
    /* old_index only can be 0 or greater, this point cannot be reached if the parent is not null */
    const int32_t old_index = lv_obj_get_index(obj);
    LV_ASSERT(0 <= old_index);

    if(index < 0) {
        index += parent_child_count;
    }

    /* Index was negative and the absolute value is greater than parent child count */
    if((index < 0)
       /* Index is same or bigger than parent child count */
       || (index >= (int32_t) parent_child_count)
       /* If both previous and new index are the same */
       || (index == old_index)) {

        return;
    }

    int32_t i = old_index;
    if(index < old_index) {
        while(i > index)  {
            parent->spec_attr->children[i] = parent->spec_attr->children[i - 1];
            i--;
        }
    }
    else {
        while(i < index) {
            parent->spec_attr->children[i] = parent->spec_attr->children[i + 1];
            i++;
        }
    }

    parent->spec_attr->children[index] = obj;
    lv_obj_send_event(parent, LV_EVENT_CHILD_CHANGED, NULL);
    lv_obj_invalidate(parent);
}

void lv_obj_swap(lv_obj_t * obj1, lv_obj_t * obj2)
{
    LV_ASSERT_OBJ(obj1, MY_CLASS);
    LV_ASSERT_OBJ(obj2, MY_CLASS);

    lv_obj_t * parent = lv_obj_get_parent(obj1);
    lv_obj_t * parent2 = lv_obj_get_parent(obj2);

    uint_fast32_t index1 = lv_obj_get_index(obj1);
    uint_fast32_t index2 = lv_obj_get_index(obj2);

    lv_obj_send_event(parent2, LV_EVENT_CHILD_DELETED, obj2);
    lv_obj_send_event(parent, LV_EVENT_CHILD_DELETED, obj1);

    parent->spec_attr->children[index1] = obj2;
    obj2->parent = parent;

    parent2->spec_attr->children[index2] = obj1;
    obj1->parent = parent2;

    lv_obj_send_event(parent, LV_EVENT_CHILD_CHANGED, obj2);
    lv_obj_send_event(parent, LV_EVENT_CHILD_CREATED, obj2);
    lv_obj_send_event(parent2, LV_EVENT_CHILD_CHANGED, obj1);
    lv_obj_send_event(parent2, LV_EVENT_CHILD_CREATED, obj1);

    lv_obj_invalidate(parent);

    if(parent != parent2) {
        lv_obj_invalidate(parent2);
    }
    lv_group_swap_obj(obj1, obj2);
}

lv_obj_t * lv_obj_get_screen(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    const lv_obj_t * par = obj;
    const lv_obj_t * act_par;

    do {
        act_par = par;
        par = lv_obj_get_parent(act_par);
    } while(par != NULL);

    return (lv_obj_t *)act_par;
}

lv_display_t * lv_obj_get_display(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    const lv_obj_t * scr;

    if(obj->parent == NULL) scr = obj;  /*`obj` is a screen*/
    else scr = lv_obj_get_screen(obj);  /*get the screen of `obj`*/

    lv_display_t * d;
    lv_ll_t * disp_head = disp_ll_p;
    LV_LL_READ(disp_head, d) {
        uint32_t i;
        for(i = 0; i < d->screen_cnt; i++) {
            if(d->screens[i] == scr) return d;
        }
    }

    LV_LOG_WARN("No screen found");
    return NULL;
}

lv_obj_t * lv_obj_get_parent(const lv_obj_t * obj)
{
    if(obj == NULL) return NULL;
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return obj->parent;
}

lv_obj_t * lv_obj_get_child(const lv_obj_t * obj, int32_t idx)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(obj->spec_attr == NULL) return NULL;

    uint32_t idu;
    if(idx < 0) {
        idx = obj->spec_attr->child_cnt + idx;
        if(idx < 0) return NULL;
        idu = (uint32_t) idx;
    }
    else {
        idu = idx;
    }

    if(idu >= obj->spec_attr->child_cnt) return NULL;
    else return obj->spec_attr->children[idx];
}

lv_obj_t * lv_obj_get_child_by_type(const lv_obj_t * obj, int32_t idx, const lv_obj_class_t * class_p)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(obj->spec_attr == NULL) return NULL;

    int32_t i;
    int32_t cnt = (int32_t)obj->spec_attr->child_cnt;
    if(idx >= 0) {
        for(i = 0; i < cnt; i++) {
            if(obj->spec_attr->children[i]->class_p == class_p) {
                if(idx == 0) return obj->spec_attr->children[i];
                else idx--;
            }
        }
    }
    else {
        idx++;   /*-1 means the first child*/
        for(i = cnt - 1; i >= 0; i--) {
            if(obj->spec_attr->children[i]->class_p == class_p) {
                if(idx == 0) return obj->spec_attr->children[i];
                else idx++;
            }
        }
    }
    return NULL;
}

lv_obj_t * lv_obj_get_sibling(const lv_obj_t * obj, int32_t idx)
{
    lv_obj_t * parent = lv_obj_get_parent(obj);
    int32_t sibling_idx = (int32_t)lv_obj_get_index(obj) + idx;
    if(sibling_idx < 0) return NULL;

    return lv_obj_get_child(parent, sibling_idx);
}

lv_obj_t * lv_obj_get_sibling_by_type(const lv_obj_t * obj, int32_t idx, const lv_obj_class_t * class_p)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t * parent = lv_obj_get_parent(obj);
    int32_t sibling_idx = (int32_t)lv_obj_get_index_by_type(obj, class_p) + idx;
    if(sibling_idx < 0) return NULL;

    return lv_obj_get_child_by_type(parent, sibling_idx, class_p);
}

uint32_t lv_obj_get_child_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(obj->spec_attr == NULL) return 0;
    return obj->spec_attr->child_cnt;
}

uint32_t lv_obj_get_child_count_by_type(const lv_obj_t * obj, const lv_obj_class_t * class_p)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(obj->spec_attr == NULL) return 0;

    uint32_t i;
    uint32_t cnt = 0;
    for(i = 0; i < obj->spec_attr->child_cnt; i++) {
        if(obj->spec_attr->children[i]->class_p == class_p) cnt++;
    }
    return cnt;
}

#if LV_USE_OBJ_NAME

void lv_obj_set_name(lv_obj_t * obj, const char * name)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_allocate_spec_attr(obj);

    if(!obj->spec_attr->name_static && obj->spec_attr->name) lv_free((void *)obj->spec_attr->name);

    if(name == NULL) {
        obj->spec_attr->name = NULL;
        obj->spec_attr->name_static = 1;
    }
    else {
        obj->spec_attr->name = lv_strdup(name);
        obj->spec_attr->name_static = 0;
    }
}

void lv_obj_set_name_static(lv_obj_t * obj, const char * name)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_allocate_spec_attr(obj);

    if(!obj->spec_attr->name_static && obj->spec_attr->name) lv_free((void *)obj->spec_attr->name);

    obj->spec_attr->name = name;
    obj->spec_attr->name_static = 1;
}

const char * lv_obj_get_name(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(obj->spec_attr == NULL) return NULL;
    else return obj->spec_attr->name;
}

void lv_obj_get_name_resolved(const lv_obj_t * obj, char buf[], size_t buf_size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    const char * name = lv_obj_get_name(obj);
    /*Use a default name which auto-indexing*/
    char name_buf[LV_OBJ_NAME_MAX_LEN];
    if(name == NULL) {
        lv_snprintf(name_buf, sizeof(name_buf), "%s_#", obj->class_p->name);
        name = name_buf;
    }

    size_t name_len = lv_strlen(name);
    lv_obj_t * parent = lv_obj_get_parent(obj);

    /*If the last character is # automatically index the children with the same name start*/
    if(parent && name_len > 0 && name[name_len - 1] == '#') {
        uint32_t child_cnt = lv_obj_get_child_count(parent);
        uint32_t cnt = 0;
        uint32_t i;
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            /*All siblings older siblings are checked, craft the name of this widget*/
            if(child == obj) {
                char num_buf[8];
                size_t num_len;
                num_len = lv_snprintf(num_buf, sizeof(num_buf), "%d", cnt);
                /*Is there enough space for the name and the index?*/
                if(buf_size > name_len + num_len) {
                    /*E.g. buf = "some_name_", so trim the # from the end*/
                    lv_strncpy(buf, name, name_len - 1);
                    lv_strcpy(&buf[name_len - 1], num_buf);
                }
                else {
                    /*Use the name as it is as a fallback*/
                    lv_strlcpy(buf, obj->spec_attr->name, buf_size);
                }
                break;
            }
            /*Check the older siblings. IF they start with the same name count them*/
            else {
                const char * child_name = lv_obj_get_name(child);
                if(child_name == NULL) {
                    /*If the name we are looking for start with the child's class name
                     *increment the index. E.g. <class_name>_#*/
                    size_t class_name_len = lv_strlen(child->class_p->name);
                    if(name_len > 3 && class_name_len == name_len - 2 &&
                       lv_strncmp(child->class_p->name, name, class_name_len) == 0) {
                        cnt++;
                    }
                }
                /*The name is set, check if it's e.g. <some_name>#*/
                else {
                    if(lv_strcmp(child->spec_attr->name, name) == 0) {
                        cnt++;
                    }
                }
            }
        }
    }
    else {
        /*Just use the set name*/
        lv_strlcpy(buf, obj->spec_attr->name, buf_size);
    }
}

lv_obj_t * lv_obj_get_child_by_name(const lv_obj_t * parent, const char * path)
{
    LV_ASSERT_OBJ(parent, MY_CLASS);

    if(parent == NULL || parent->spec_attr == NULL || path == NULL) return NULL;

    while(*path) {
        const char * segment = path;
        uint32_t len = 0;

        /* Calculate the length of the current segment */
        while(path[len] && path[len] != '/')
            len++;

        /* Look for a child whose resolved name exactly matches the segment */
        lv_obj_t * child = find_by_name_direct(parent, segment, len);
        if(!child) return NULL; /*Segment not found*/

        /* Advance to the next segment */
        path += len;
        if(*path == '/') path++; /* Skip the '/' */

        /* If there is no further segment, we've found the target child */
        if(*path == '\0') return child;

        parent = child;
    }

    return NULL;

}

lv_obj_t * lv_obj_find_by_name(const lv_obj_t * parent, const char * name)
{
    if(parent == NULL) parent = lv_display_get_screen_active(NULL);
    if(parent == NULL) return NULL;

    lv_obj_t * child = find_by_name_direct(parent, name, UINT16_MAX);
    if(child) return child;

    /*Search children recursively*/
    uint32_t child_cnt = lv_obj_get_child_count(parent);
    uint32_t i;
    for(i = 0; i < child_cnt; i++) {
        child = parent->spec_attr->children[i];
        lv_obj_t * found = lv_obj_find_by_name(child, name);
        if(found != NULL) return found;
    }

    return NULL;
}

#endif /*LV_USE_OBJ_NAME*/

int32_t lv_obj_get_index(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent == NULL) return -1;

    int32_t i = 0;
    for(i = 0; i < parent->spec_attr->child_cnt; i++) {
        if(parent->spec_attr->children[i] == obj) return i;
    }

    /*Shouldn't reach this point*/
    LV_ASSERT(0);
    return -1;
}

int32_t lv_obj_get_index_by_type(const lv_obj_t * obj, const lv_obj_class_t * class_p)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent == NULL) return 0xFFFFFFFF;

    uint32_t i = 0;
    uint32_t idx = 0;
    for(i = 0; i < parent->spec_attr->child_cnt; i++) {
        lv_obj_t * child = parent->spec_attr->children[i];
        if(child->class_p == class_p) {
            if(child == obj) return idx;
            idx++;
        }
    }

    /*Can happen if there was no children with the given type*/
    return -1;
}

void lv_obj_tree_walk(lv_obj_t * start_obj, lv_obj_tree_walk_cb_t cb, void * user_data)
{
    walk_core(start_obj, cb, user_data);
}

void lv_obj_dump_tree(lv_obj_t * start_obj)
{
    if(start_obj == NULL) {
        lv_display_t * disp = lv_display_get_next(NULL);
        while(disp) {
            uint32_t i;
            for(i = 0; i < disp->screen_cnt; i++) {
                dump_tree_core(disp->screens[i], 0);
            }
            disp = lv_display_get_next(disp);
        }
    }
    else {
        dump_tree_core(start_obj, 0);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_obj_delete_async_cb(void * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_delete(obj);
}

static void obj_indev_reset(lv_indev_t * indev, lv_obj_t * obj)
{
    /* If the input device is already in the release state,
     * there is no need to wait for the input device to be released
     */
    if(lv_indev_get_state(indev) != LV_INDEV_STATE_RELEASED) {
        /*Wait for release to avoid accidentally triggering other obj to be clicked*/
        lv_indev_wait_release(indev);
    }

    /*Reset the input device*/
    lv_indev_reset(indev, obj);
}

static void obj_delete_core(lv_obj_t * obj)
{
    if(obj->is_deleting)
        return;

    obj->is_deleting = true;

    /*Let the user free the resources used in `LV_EVENT_DELETE`*/
    lv_result_t res = lv_obj_send_event(obj, LV_EVENT_DELETE, NULL);
    if(res == LV_RESULT_INVALID) {
        obj->is_deleting = false;
        return;
    }

    /*Clean registered event_cb*/
    if(obj->spec_attr) lv_event_remove_all(&(obj->spec_attr->event_list));

    /*Recursively delete the children*/
    lv_obj_t * child = lv_obj_get_child(obj, 0);
    while(child) {
        obj_delete_core(child);
        child = lv_obj_get_child(obj, 0);
    }

    lv_group_t * group = lv_obj_get_group(obj);

    /*Reset all input devices if the object to delete is used*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type == LV_INDEV_TYPE_POINTER || indev_type == LV_INDEV_TYPE_BUTTON) {
            if(indev->pointer.act_obj == obj || indev->pointer.last_obj == obj || indev->pointer.scroll_obj == obj) {
                obj_indev_reset(indev, obj);
            }
            if(indev->pointer.last_pressed == obj) {
                indev->pointer.last_pressed = NULL;
            }
            if(indev->pointer.last_hovered == obj) {
                indev->pointer.last_hovered = NULL;
            }
        }

        if(indev->group == group && obj == lv_indev_get_active_obj()) {
            obj_indev_reset(indev, obj);
        }
        indev = lv_indev_get_next(indev);
    }

    /*Delete all pending async del-s*/
    lv_result_t async_cancel_res = LV_RESULT_OK;
    while(async_cancel_res == LV_RESULT_OK) {
        async_cancel_res = lv_async_call_cancel(lv_obj_delete_async_cb, obj);
    }

    /*All children deleted. Now clean up the object specific data*/
    lv_obj_destruct(obj);

    /*Remove the screen for the screen list*/
    if(obj->parent == NULL) {
        lv_display_t * disp = lv_obj_get_display(obj);
        uint32_t i;
        /*Find the screen in the list*/
        for(i = 0; i < disp->screen_cnt; i++) {
            if(disp->screens[i] == obj) break;
        }

        uint32_t id = i;
        for(i = id; i < disp->screen_cnt - 1; i++) {
            disp->screens[i] = disp->screens[i + 1];
        }
        disp->screen_cnt--;
        disp->screens = lv_realloc(disp->screens, disp->screen_cnt * sizeof(lv_obj_t *));
    }
    /*Remove the object from the child list of its parent*/
    else {
        int32_t id = lv_obj_get_index(obj);
        uint16_t i;
        for(i = id; i < obj->parent->spec_attr->child_cnt - 1; i++) {
            obj->parent->spec_attr->children[i] = obj->parent->spec_attr->children[i + 1];
        }
        obj->parent->spec_attr->child_cnt--;
        obj->parent->spec_attr->children = lv_realloc(obj->parent->spec_attr->children,
                                                      obj->parent->spec_attr->child_cnt * sizeof(lv_obj_t *));
    }

    /*Free the object itself*/
    lv_free(obj);
}

static lv_obj_tree_walk_res_t walk_core(lv_obj_t * obj, lv_obj_tree_walk_cb_t cb, void * user_data)
{
    lv_obj_tree_walk_res_t res = LV_OBJ_TREE_WALK_NEXT;

    if(obj == NULL) {
        lv_display_t * disp = lv_display_get_next(NULL);
        while(disp) {
            uint32_t i;
            for(i = 0; i < disp->screen_cnt; i++) {
                walk_core(disp->screens[i], cb, user_data);
            }
            disp = lv_display_get_next(disp);
        }
        return LV_OBJ_TREE_WALK_END;    /*The value doesn't matter as it wasn't called recursively*/
    }

    res = cb(obj, user_data);

    if(res == LV_OBJ_TREE_WALK_END) return LV_OBJ_TREE_WALK_END;

    if(res != LV_OBJ_TREE_WALK_SKIP_CHILDREN) {
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_count(obj); i++) {
            res = walk_core(lv_obj_get_child(obj, i), cb, user_data);
            if(res == LV_OBJ_TREE_WALK_END) return LV_OBJ_TREE_WALK_END;
        }
    }
    return LV_OBJ_TREE_WALK_NEXT;
}

static void dump_tree_core(lv_obj_t * obj, int32_t depth)
{
#if LV_USE_LOG
    const char * id;

#if LV_USE_OBJ_ID
    char buf[OBJ_DUMP_STRING_LEN];
    id = lv_obj_stringify_id(obj, buf, sizeof(buf));
    if(id == NULL) id = "obj0";
#else
    id = "obj0";
#endif

    /*id of `obj0` is an invalid id for builtin id*/
    LV_LOG_USER("%*sobj:%p, id:%s;", (int)(2 * depth), "", (void *)obj, id);
#endif /*LV_USE_LOG*/

    if(obj && obj->spec_attr && obj->spec_attr->child_cnt) {
        for(uint32_t i = 0; i < obj->spec_attr->child_cnt; i++) {
            dump_tree_core(lv_obj_get_child(obj, i), depth + 1);
        }
    }
}

static lv_obj_t * lv_obj_get_first_not_deleting_child(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(obj->spec_attr == NULL) return NULL;

    int32_t i;
    int32_t cnt = (int32_t)obj->spec_attr->child_cnt;
    for(i = 0; i < cnt; i++) {
        if(!obj->spec_attr->children[i]->is_deleting) {
            return obj->spec_attr->children[i];
        }
    }

    return NULL;
}

#if LV_USE_OBJ_NAME

static lv_obj_t * find_by_name_direct(const lv_obj_t * parent, const char * name, size_t len)
{
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_count(parent);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = parent->spec_attr->children[i];

        char child_name_resolved[LV_OBJ_NAME_MAX_LEN];
        lv_obj_get_name_resolved(child, child_name_resolved, sizeof(child_name_resolved));
        if(lv_strncmp(child_name_resolved, name, len) == 0) return child;
    }

    return NULL;
}

#endif /*LV_USE_OBJ_NAME*/
