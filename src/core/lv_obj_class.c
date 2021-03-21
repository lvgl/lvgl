/**
 * @file lv_obj_class.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_obj_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_obj_construct(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
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

lv_obj_t * lv_obj_create_from_class(const lv_obj_class_t * class_p, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_TRACE_OBJ_CREATE("Creating object with %p class on %p parent", class_p, parent);
    uint32_t s = get_instance_size(class_p);
    lv_obj_t * obj = lv_mem_alloc(s);
    lv_memset_00(obj, s);
    obj->class_p = class_p;
    obj->parent = parent;
    const lv_obj_class_t * class_start = class_p;

    while(class_start && class_start->constructor_cb == NULL) class_start = class_start->base_class;

    lv_obj_construct(obj, parent, copy);

   if(parent) {
       /*Send a Call the ancestor's event handler to the parent to notify it about the new child.
        *Also triggers layout update*/
       lv_event_send(parent, LV_EVENT_CHILD_CHANGED, obj);

       /*Invalidate the area if not screen created*/
       lv_obj_invalidate(obj);
   }

    if(!copy) lv_theme_apply(obj);
//    else lv_style_list_copy(&checkbox->style_indic, &checkbox_copy->style_indic);

    LV_TRACE_OBJ_CREATE("Object created at %p address with %p class on %p parent", obj, class_p, parent);
    return obj;
}

void _lv_obj_destruct(lv_obj_t * obj)
{
    if(obj->class_p->destructor_cb) obj->class_p->destructor_cb(obj);

    if(obj->class_p->base_class) {
        /*Don't let the descendant methods run during constructing the ancestor type*/
        obj->class_p = obj->class_p->base_class;

        /*Call the base class's destructor too*/
        _lv_obj_destruct(obj);
    }
}

bool lv_obj_is_editable(struct _lv_obj_t * obj)
{
    const lv_obj_class_t * class_p = obj->class_p;

    /*Find a base in which editable is set*/
    while(class_p && class_p->editable == LV_OBJ_CLASS_EDITABLE_INHERIT) class_p = class_p->base_class;

    if(class_p == NULL) return false;

    return class_p->editable == LV_OBJ_CLASS_EDITABLE_TRUE ? true : false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_obj_construct(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    const lv_obj_class_t * original_class_p = obj->class_p;

    if(obj->class_p->base_class) {
        /*Don't let the descendant methods run during constructing the ancestor type*/
        obj->class_p = obj->class_p->base_class;

        /*Construct the base first*/
        lv_obj_construct(obj, parent, copy);
    }

    /*Restore the original class*/
    obj->class_p = original_class_p;

    if(obj->class_p->constructor_cb) obj->class_p->constructor_cb(obj, copy);


}
static uint32_t get_instance_size(const lv_obj_class_t * class_p)
{
    /*Find a base in which instance size is set*/
    const lv_obj_class_t * base = class_p;
    while(base && base->instance_size == 0) base = base->base_class;

    if(base == NULL) return 0;  /*Never happens: set at least in `lv_obj` class*/

    return base->instance_size;
}
