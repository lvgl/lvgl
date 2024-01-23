/**
 * @file lv_obj_id.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"
#include "../stdlib/lv_string.h"
#include "lv_obj_property.h"

#if LV_USE_OBJ_PROPERTY

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_property_set_int_t)(lv_obj_t *, int32_t);
typedef void (*lv_property_set_pointer_t)(lv_obj_t *, const void *);
typedef void (*lv_property_set_color_t)(lv_obj_t *, lv_color_t);
typedef lv_result_t (*lv_property_setter_t)(lv_obj_t *, lv_prop_id_t, const lv_property_t *);

typedef int32_t (*lv_property_get_int_t)(const lv_obj_t *);
typedef void * (*lv_property_get_pointer_t)(const lv_obj_t *);
typedef lv_color_t (*lv_property_get_color_t)(const lv_obj_t *);
typedef lv_result_t (*lv_property_getter_t)(const lv_obj_t *, lv_prop_id_t, lv_property_t *);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t obj_property(lv_obj_t * obj, lv_prop_id_t id, lv_property_t * value, bool set);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_obj_set_property(lv_obj_t * obj, const lv_property_t * value)
{
    LV_ASSERT(obj && value);

    if(value->id == LV_PROPERTY_ID_INVALID) {
        LV_LOG_WARN("invalid property id set to %p\n", obj);
        return LV_RESULT_INVALID;
    }

    if(value->id < LV_PROPERTY_ID_START) {
        lv_obj_set_local_style_prop(obj, value->id, value->_style, 0);
        return LV_RESULT_OK;
    }

    return obj_property(obj, value->id, (lv_property_t *)value, true);
}

lv_result_t lv_obj_set_properties(lv_obj_t * obj, const lv_property_t * value, uint32_t count)
{
    for(uint32_t i = 0; i < count; i++) {
        lv_result_t result = lv_obj_set_property(obj, &value[i]);
        if(result != LV_RESULT_OK) {
            return result;
        }
    }

    return LV_RESULT_OK;
}

lv_property_t lv_obj_get_property(lv_obj_t * obj, lv_prop_id_t id)
{
    lv_result_t result;
    lv_property_t value;

    if(id == LV_PROPERTY_ID_INVALID) {
        LV_LOG_WARN("invalid property id to get from %p\n", obj);
        value.id = 0;
        value.num = 0;
        return value;
    }

    if(id < LV_PROPERTY_ID_START) {
        lv_obj_get_local_style_prop(obj, id, &value._style, 0);
        value.id = id;
        return value;
    }

    result = obj_property(obj, id, &value, false);
    if(result != LV_RESULT_OK)
        value.id = 0;

    return value;
}

static lv_result_t obj_property(lv_obj_t * obj, lv_prop_id_t id, lv_property_t * value, bool set)
{
    const lv_property_ops_t * properties;
    const lv_property_ops_t * prop;

    const lv_obj_class_t * clz;
    uint32_t index = LV_PROPERTY_ID_INDEX(id);

    for(clz = obj->class_p ; clz; clz = clz->base_class) {
        properties = clz->properties;
        if(properties == NULL) {
            /* try base class*/
            continue;
        }

        if(id != LV_PROPERTY_ID_ANY && (index < clz->prop_index_start || index > clz->prop_index_end)) {
            /* try base class*/
            continue;
        }

        /*Check if there's setter available for this class*/
        for(uint32_t i = 0; i < clz->properties_count; i++) {
            prop = &properties[i];

            /*pass id and value directly to widget's property method*/
            if(prop->id == LV_PROPERTY_ID_ANY) {
                value->id = prop->id;
                if(set) return ((lv_property_setter_t)prop->setter)(obj, id, value);
                else return ((lv_property_getter_t)prop->getter)(obj, id, value);
            }

            /*Not this id, check next*/
            if(prop->id != id)
                continue;

            /*id matched but we got null pointer to functions*/
            if(set ? prop->setter == NULL : prop->getter == NULL) {
                LV_LOG_WARN("null %s provided, id: %d\n", set ? "setter" : "getter", id);
                return LV_RESULT_INVALID;
            }

            /*Update value id if it's a read*/
            if(!set) value->id = prop->id;

            switch(LV_PROPERTY_ID_TYPE(prop->id)) {
                case LV_PROPERTY_TYPE_INT:
                    if(set)((lv_property_set_int_t)(prop->setter))(obj, value->num);
                    else value->num = ((lv_property_get_int_t)(prop->getter))(obj);
                    break;
                case LV_PROPERTY_TYPE_POINTER:
                case LV_PROPERTY_TYPE_IMGSRC:
                    if(set)((lv_property_set_pointer_t)(prop->setter))(obj, value->ptr);
                    else value->ptr = ((lv_property_get_pointer_t)(prop->getter))(obj);
                    break;
                case LV_PROPERTY_TYPE_COLOR:
                    if(set)((lv_property_set_color_t)prop->setter)(obj, value->color);
                    else value->color = ((lv_property_get_color_t)(prop->getter))(obj);
                    break;
                default:
                    LV_LOG_WARN("unknown property id: 0x%08x\n", prop->id);
                    return LV_RESULT_INVALID;
                    break;
            }

            return LV_RESULT_OK;
        }

        /*If no setter found, try base class then*/
    }

    LV_LOG_WARN("unknown property id: 0x%08x\n", id);
    return LV_RESULT_INVALID;
}

#endif /*LV_USE_OBJ_PROPERTY*/
