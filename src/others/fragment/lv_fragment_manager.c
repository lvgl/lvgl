/**
 * @file lv_fragment_manager.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_fragment_private.h"

#if LV_USE_FRAGMENT

#include "../../misc/lv_ll.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct lv_fragment_stack_item_t {
    lv_fragment_managed_states_t * states;
} lv_fragment_stack_item_t;

struct lv_fragment_manager_t {
    lv_fragment_t * parent;
    /**
     * Linked list to store attached fragments
     */
    lv_ll_t attached;
    /**
     * Linked list to store fragments in stack
     */
    lv_ll_t stack;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void item_create_obj(lv_fragment_managed_states_t * item);

static void item_delete_obj(lv_fragment_managed_states_t * item);

static void item_delete_fragment(lv_fragment_managed_states_t * item);

static lv_fragment_managed_states_t * fragment_attach(lv_fragment_manager_t * manager, lv_fragment_t * fragment,
                                                      lv_obj_t * const * container);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_fragment_manager_t * lv_fragment_manager_create(lv_fragment_t * parent)
{
    lv_fragment_manager_t * instance = lv_malloc_zeroed(sizeof(lv_fragment_manager_t));
    instance->parent = parent;
    lv_ll_init(&instance->attached, sizeof(lv_fragment_managed_states_t));
    lv_ll_init(&instance->stack, sizeof(lv_fragment_stack_item_t));
    return instance;
}

void lv_fragment_manager_delete(lv_fragment_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    lv_fragment_managed_states_t * states;
    LV_LL_READ_BACK(&manager->attached, states) {
        item_delete_obj(states);
        item_delete_fragment(states);
    }
    lv_ll_clear(&manager->attached);
    lv_ll_clear(&manager->stack);
    lv_free(manager);
}

void lv_fragment_manager_create_obj(lv_fragment_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    lv_fragment_stack_item_t * top = lv_ll_get_tail(&manager->stack);
    lv_fragment_managed_states_t * states = NULL;
    LV_LL_READ(&manager->attached, states) {
        if(states->in_stack && top->states != states) {
            /*Only create obj for top item in stack*/
            continue;
        }
        item_create_obj(states);
    }
}

void lv_fragment_manager_delete_obj(lv_fragment_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    lv_fragment_managed_states_t * states = NULL;
    LV_LL_READ_BACK(&manager->attached, states) {
        item_delete_obj(states);
    }
}

void lv_fragment_manager_add(lv_fragment_manager_t * manager, lv_fragment_t * fragment, lv_obj_t * const * container)
{
    lv_fragment_managed_states_t * states = fragment_attach(manager, fragment, container);
    if(!manager->parent || manager->parent->managed->obj_created) {
        item_create_obj(states);
    }
}

void lv_fragment_manager_remove(lv_fragment_manager_t * manager, lv_fragment_t * fragment)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(fragment);
    LV_ASSERT_NULL(fragment->managed);
    LV_ASSERT(fragment->managed->manager == manager);
    lv_fragment_managed_states_t * states = fragment->managed;
    lv_fragment_managed_states_t * prev = NULL;
    bool was_top = false;
    if(states->in_stack) {
        void * stack_top = lv_ll_get_tail(&manager->stack);
        lv_fragment_stack_item_t * item = NULL;
        LV_LL_READ_BACK(&manager->stack, item) {
            if(item->states == states) {
                was_top = stack_top == item;
                void * stack_prev = lv_ll_get_prev(&manager->stack, item);
                if(!stack_prev) break;
                prev = ((lv_fragment_stack_item_t *) stack_prev)->states;
                break;
            }
        }
        if(item) {
            lv_ll_remove(&manager->stack, item);
            lv_free(item);
        }
    }
    item_delete_obj(states);
    item_delete_fragment(states);
    lv_ll_remove(&manager->attached, states);
    lv_free(states);
    if(prev && was_top) {
        item_create_obj(prev);
    }
}

void lv_fragment_manager_push(lv_fragment_manager_t * manager, lv_fragment_t * fragment, lv_obj_t * const * container)
{
    lv_fragment_stack_item_t * top = lv_ll_get_tail(&manager->stack);
    if(top != NULL) {
        item_delete_obj(top->states);
    }
    lv_fragment_managed_states_t * states = fragment_attach(manager, fragment, container);
    states->in_stack = true;
    /*Add fragment to the top of the stack*/
    lv_fragment_stack_item_t * item = lv_ll_ins_tail(&manager->stack);
    lv_memzero(item, sizeof(lv_fragment_stack_item_t));
    item->states = states;
    item_create_obj(states);
}

bool lv_fragment_manager_pop(lv_fragment_manager_t * manager)
{
    lv_fragment_t * top = lv_fragment_manager_get_top(manager);
    if(top == NULL) return false;
    lv_fragment_manager_remove(manager, top);
    return true;
}

void lv_fragment_manager_replace(lv_fragment_manager_t * manager, lv_fragment_t * fragment,
                                 lv_obj_t * const * container)
{
    lv_fragment_t * top = lv_fragment_manager_find_by_container(manager, *container);
    if(top != NULL) {
        lv_fragment_manager_remove(manager, top);
    }
    lv_fragment_manager_add(manager, fragment, container);
}

bool lv_fragment_manager_send_event(lv_fragment_manager_t * manager, int code, void * userdata)
{
    LV_ASSERT_NULL(manager);
    lv_fragment_managed_states_t * p = NULL;
    LV_LL_READ_BACK(&manager->attached, p) {
        if(!p->obj_created || p->destroying_obj) continue;
        lv_fragment_t * instance = p->instance;
        if(!instance) continue;
        if(lv_fragment_manager_send_event(instance->child_manager, code, userdata)) return true;
        if(p->cls->event_cb && p->cls->event_cb(instance, code, userdata)) return true;
    }
    return false;
}

size_t lv_fragment_manager_get_stack_size(lv_fragment_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    return lv_ll_get_len(&manager->stack);
}

lv_fragment_t * lv_fragment_manager_get_top(lv_fragment_manager_t * manager)
{
    LV_ASSERT(manager);
    lv_fragment_stack_item_t * top = lv_ll_get_tail(&manager->stack);
    if(!top)return NULL;
    return top->states->instance;
}

lv_fragment_t * lv_fragment_manager_find_by_container(lv_fragment_manager_t * manager, const lv_obj_t * container)
{
    LV_ASSERT(manager);
    lv_fragment_managed_states_t * states;
    LV_LL_READ(&manager->attached, states) {
        if(*states->container == container) return states->instance;
    }
    return NULL;
}

lv_fragment_t * lv_fragment_manager_get_parent_fragment(lv_fragment_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    return manager->parent;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void item_create_obj(lv_fragment_managed_states_t * item)
{
    LV_ASSERT(item->instance);
    lv_fragment_create_obj(item->instance, item->container ? *item->container : NULL);
}

static void item_delete_obj(lv_fragment_managed_states_t * item)
{
    lv_fragment_delete_obj(item->instance);
}

/**
 * Detach, then destroy fragment
 * @param item fragment states
 */
static void item_delete_fragment(lv_fragment_managed_states_t * item)
{
    lv_fragment_t * instance = item->instance;
    if(instance->cls->detached_cb) {
        instance->cls->detached_cb(instance);
    }
    instance->managed = NULL;
    lv_fragment_delete(instance);
    item->instance = NULL;
}

static lv_fragment_managed_states_t * fragment_attach(lv_fragment_manager_t * manager, lv_fragment_t * fragment,
                                                      lv_obj_t * const * container)
{
    LV_ASSERT(manager);
    LV_ASSERT(fragment);
    LV_ASSERT(fragment->managed == NULL);
    lv_fragment_managed_states_t * states = lv_ll_ins_tail(&manager->attached);
    lv_memzero(states, sizeof(lv_fragment_managed_states_t));
    states->cls = fragment->cls;
    states->manager = manager;
    states->container = container;
    states->instance = fragment;
    fragment->managed = states;
    if(fragment->cls->attached_cb) {
        fragment->cls->attached_cb(fragment);
    }
    return states;
}

#endif /*LV_USE_FRAGMENT*/
