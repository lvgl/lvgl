/**
 * @file lv_obj_class.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_class_private.h"
#include "lv_obj_private.h"
#include "../themes/lv_theme.h"
#include "../display/lv_display.h"
#include "../display/lv_display_private.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_obj_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_obj_construct(const lv_obj_class_t * class_p, lv_obj_t * obj);
static uint32_t get_instance_size(const lv_obj_class_t * class_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_obj_class_create_obj(const lv_obj_class_t * class_p, lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("Creating object with %p class on %p parent", (void *)class_p, (void *)parent);
    uint32_t s = get_instance_size(class_p);
    lv_obj_t * obj = lv_malloc_zeroed(s);
    if(obj == NULL) return NULL;
    obj->class_p = class_p;
    obj->parent = parent;

    /*Create a screen*/
    if(parent == NULL) {
        LV_TRACE_OBJ_CREATE("creating a screen");
        lv_display_t * disp = lv_display_get_default();
        if(!disp) {
            LV_LOG_WARN("No display created yet. No place to assign the new screen");
            lv_free(obj);
            return NULL;
        }

        if(disp->screens == NULL) {
            disp->screen_cnt = 0;
        }

        lv_obj_t ** screens = lv_realloc(disp->screens, sizeof(lv_obj_t *) * (disp->screen_cnt + 1));
        LV_ASSERT_MALLOC(screens);
        if(screens == NULL) {
            lv_free(obj);
            return NULL;
        }

        disp->screen_cnt++;
        disp->screens = screens;
        disp->screens[disp->screen_cnt - 1] = obj;

        /*Set coordinates to full screen size*/
        obj->coords.x1 = 0;
        obj->coords.y1 = 0;
        obj->coords.x2 = lv_display_get_horizontal_resolution(NULL) - 1;
        obj->coords.y2 = lv_display_get_vertical_resolution(NULL) - 1;
    }
    /*Create a normal object*/
    else {
        LV_TRACE_OBJ_CREATE("creating normal object");
        LV_ASSERT_OBJ(parent, MY_CLASS);
        if(parent->spec_attr == NULL) {
            lv_obj_allocate_spec_attr(parent);
        }

        parent->spec_attr->child_cnt++;
        parent->spec_attr->children = lv_realloc(parent->spec_attr->children,
                                                 sizeof(lv_obj_t *) * parent->spec_attr->child_cnt);
        parent->spec_attr->children[parent->spec_attr->child_cnt - 1] = obj;
    }

    return obj;
}

void lv_obj_class_init_obj(lv_obj_t * obj)
{
    if(obj == NULL) return;

    lv_obj_mark_layout_as_dirty(obj);
    lv_obj_enable_style_refresh(false);

    lv_theme_apply(obj);
    lv_obj_construct(obj->class_p, obj);

    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY);

    lv_obj_refresh_self_size(obj);

    lv_group_t * def_group = lv_group_get_default();
    if(def_group && lv_obj_is_group_def(obj)) {
        lv_group_add_obj(def_group, obj);
    }

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        /*Call the ancestor's event handler to the parent to notify it about the new child.
         *Also triggers layout update*/
        lv_obj_send_event(parent, LV_EVENT_CHILD_CHANGED, obj);
        lv_obj_send_event(parent, LV_EVENT_CHILD_CREATED, obj);

        /*Invalidate the area if not screen created*/
        lv_obj_invalidate(obj);
    }
}

void lv_obj_destruct(lv_obj_t * obj)
{
    if(obj->class_p->destructor_cb) obj->class_p->destructor_cb(obj->class_p, obj);

    if(obj->class_p->base_class) {
        /*Don't let the descendant methods run during destructing the ancestor type*/
        obj->class_p = obj->class_p->base_class;

        /*Call the base class's destructor too*/
        lv_obj_destruct(obj);
    }
}

bool lv_obj_is_editable(lv_obj_t * obj)
{
    const lv_obj_class_t * class_p = obj->class_p;

    /*Find a base in which editable is set*/
    while(class_p && class_p->editable == LV_OBJ_CLASS_EDITABLE_INHERIT) class_p = class_p->base_class;

    if(class_p == NULL) return false;

    return class_p->editable == LV_OBJ_CLASS_EDITABLE_TRUE;
}

bool lv_obj_is_group_def(lv_obj_t * obj)
{
    const lv_obj_class_t * class_p = obj->class_p;

    /*Find a base in which group_def is set*/
    while(class_p && class_p->group_def == LV_OBJ_CLASS_GROUP_DEF_INHERIT) class_p = class_p->base_class;

    if(class_p == NULL) return false;

    return class_p->group_def == LV_OBJ_CLASS_GROUP_DEF_TRUE;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_obj_construct(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    if(obj->class_p->base_class) {
        const lv_obj_class_t * original_class_p = obj->class_p;

        /*Don't let the descendant methods run during constructing the ancestor type*/
        obj->class_p = obj->class_p->base_class;

        /*Construct the base first*/
        lv_obj_construct(class_p, obj);

        /*Restore the original class*/
        obj->class_p = original_class_p;
    }

    if(obj->class_p->constructor_cb) obj->class_p->constructor_cb(class_p, obj);
}

static uint32_t get_instance_size(const lv_obj_class_t * class_p)
{
    /*Find a base in which instance size is set*/
    const lv_obj_class_t * base = class_p;
    while(base && base->instance_size == 0) base = base->base_class;

    if(base == NULL) return 0;  /*Never happens: set at least in `lv_obj` class*/

    return base->instance_size;
}
