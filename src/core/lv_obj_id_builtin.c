/**
 * @file lv_obj_id.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "lv_global.h"
#include "../osal/lv_os.h"
#include "../stdlib/lv_sprintf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

typedef struct _class_info_t {
    const lv_obj_class_t * class_p;
    uint32_t obj_count;
} class_info_t;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_USE_OBJ_ID_BUILTIN

void lv_obj_assign_id(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_ASSERT(obj && class_p);

    uint32_t i;
    uint32_t id = 0;
    lv_global_t * global = LV_GLOBAL_DEFAULT();
    class_info_t * info = NULL;

    if(obj == NULL || class_p == NULL) return;
    if(global == NULL) return;

    obj->id = NULL;

    for(i = 0; i < global->objid_count; i ++) {
        info = ((class_info_t *)global->objid_array) + i;
        if(class_p == info->class_p) break;
    }

    /*Resize array*/
    if(i == global->objid_count) {
        void * array = lv_realloc(global->objid_array, sizeof(class_info_t) * (global->objid_count + 1));
        LV_ASSERT_MALLOC(array);
        if(array == NULL) return;
        global->objid_array = array;
        global->objid_count ++;
        info = ((class_info_t *)global->objid_array) + i;
        info->obj_count = 0;
        info->class_p = class_p;
    }

    id = ++info->obj_count;

    obj->id = (void *)(lv_uintptr_t)id;
}

void lv_obj_free_id(lv_obj_t * obj)
{
    LV_UNUSED(obj);
}

const char * lv_obj_stringify_id(lv_obj_t * obj, char * buf, uint32_t len)
{
    const char * name;
    if(obj == NULL || obj->class_p == NULL) return NULL;
    if(buf == NULL) return NULL;

    name = obj->class_p->name;
    if(name == NULL) name = "nameless";

    lv_snprintf(buf, len, "%s%" LV_PRId32 "", name, (uint32_t)(lv_uintptr_t)obj->id);
    return buf;
}

void lv_objid_builtin_destroy(void)
{
    lv_global_t * global = LV_GLOBAL_DEFAULT();
    if(global == NULL) return;

    lv_free(global->objid_array);
    global->objid_count = 0;
}

#endif /*LV_USE_OBJ_ID_BUILTIN*/
