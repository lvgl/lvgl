/**
 * @file lv_obj_id.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_private.h"
#include "../core/lv_obj.h"
#include "../stdlib/lv_string.h"
#include "../misc/lv_utils.h"
#include "lv_obj_property.h"
#include "lv_obj_class_private.h"

#if LV_USE_OBJ_PROPERTY

/*********************
 *      DEFINES
 *********************/

#define HANDLE_PROPERTY_TYPE(type, field) \
    if(!set) { \
        value->field = ((lv_property_get_##type##_t)(prop->getter))(obj); \
    } else { \
        switch(LV_PROPERTY_ID_TYPE2(prop->id)) { \
            case LV_PROPERTY_ID_INVALID: \
                ((lv_property_set_##type##_t)(prop->setter))(obj, value->field); \
                break; \
            case LV_PROPERTY_TYPE_INT: \
                ((lv_property_set_##type##_integer_t)(prop->setter))(obj, value->arg1.field, value->arg2.num); \
                break; \
            case LV_PROPERTY_TYPE_BOOL: \
                ((lv_property_set_##type##_boolean_t)(prop->setter))(obj, value->arg1.field, value->arg2.enable); \
                break; \
            case LV_PROPERTY_TYPE_PRECISE: \
                ((lv_property_set_##type##_precise_t)(prop->setter))(obj, value->arg1.field, value->arg2.precise); \
                break; \
            case LV_PROPERTY_TYPE_COLOR: \
                ((lv_property_set_##type##_color_t)(prop->setter))(obj, value->arg1.field, value->arg2.color); \
                break; \
            case LV_PROPERTY_TYPE_POINTER: \
            case LV_PROPERTY_TYPE_IMGSRC: \
            case LV_PROPERTY_TYPE_TEXT: \
            case LV_PROPERTY_TYPE_OBJ: \
            case LV_PROPERTY_TYPE_DISPLAY: \
            case LV_PROPERTY_TYPE_FONT: \
                ((lv_property_set_##type##_pointer_t)(prop->setter))(obj, value->arg1.field, value->arg2.ptr); \
                break; \
        } \
    }


/**********************
 *      TYPEDEFS
 **********************/

typedef int32_t integer;
typedef bool boolean;
typedef lv_value_precise_t precise;
typedef lv_color_t color;
typedef const void * pointer;

#define DEFINE_PROPERTY_SETTER_TYPES(type) \
    typedef void (*lv_property_set_##type##_t)(lv_obj_t *, type); \
    typedef void (*lv_property_set_##type##_integer_t)(lv_obj_t *, type, int32_t); \
    typedef void (*lv_property_set_##type##_boolean_t)(lv_obj_t *, type, bool); \
    typedef void (*lv_property_set_##type##_precise_t)(lv_obj_t *, type, lv_value_precise_t); \
    typedef void (*lv_property_set_##type##_color_t)(lv_obj_t *, type, lv_color_t); \
    typedef void (*lv_property_set_##type##_pointer_t)(lv_obj_t *, type, const void *)

DEFINE_PROPERTY_SETTER_TYPES(integer);
DEFINE_PROPERTY_SETTER_TYPES(boolean);
DEFINE_PROPERTY_SETTER_TYPES(precise);
DEFINE_PROPERTY_SETTER_TYPES(color);
DEFINE_PROPERTY_SETTER_TYPES(pointer);

typedef void (*lv_property_set_point_t)(lv_obj_t *, lv_point_t *);
typedef lv_result_t (*lv_property_setter_t)(lv_obj_t *, lv_prop_id_t, const lv_property_t *);

typedef integer(*lv_property_get_integer_t)(const lv_obj_t *);
typedef bool (*lv_property_get_boolean_t)(const lv_obj_t *);
typedef lv_value_precise_t (*lv_property_get_precise_t)(const lv_obj_t *);
typedef lv_color_t (*lv_property_get_color_t)(const lv_obj_t *);
typedef void * (*lv_property_get_pointer_t)(const lv_obj_t *);
typedef lv_point_t (*lv_property_get_point_t)(lv_obj_t *);

typedef lv_result_t (*lv_property_getter_t)(const lv_obj_t *, lv_prop_id_t, lv_property_t *);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t obj_property(lv_obj_t * obj, lv_prop_id_t id, lv_property_t * value, bool set);
static int property_name_compare(const void * ref, const void * element);

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

    uint32_t index = LV_PROPERTY_ID_INDEX(value->id);
    if(value->id == LV_PROPERTY_ID_INVALID || index > LV_PROPERTY_ID_ANY) {
        LV_LOG_WARN("Invalid property id set to %p", obj);
        return LV_RESULT_INVALID;
    }

    if(index < LV_PROPERTY_ID_START) {
        lv_obj_set_local_style_prop(obj, index, value->style, value->selector);
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
    lv_property_t value = { 0 };

    uint32_t index = LV_PROPERTY_ID_INDEX(id);
    if(id == LV_PROPERTY_ID_INVALID || index > LV_PROPERTY_ID_ANY) {
        LV_LOG_WARN("Invalid property id to get from %p", obj);
        value.id = LV_PROPERTY_ID_INVALID;
        value.num = 0;
        return value;
    }

    if(index < LV_PROPERTY_ID_START) {
        lv_obj_get_local_style_prop(obj, index, &value.style, 0);
        value.id = id;
        value.selector = 0;
        return value;
    }

    result = obj_property(obj, id, &value, false);
    if(result != LV_RESULT_OK)
        value.id = LV_PROPERTY_ID_INVALID;

    return value;
}

lv_property_t lv_obj_get_style_property(lv_obj_t * obj, lv_prop_id_t id, uint32_t selector)
{
    lv_property_t value;
    uint32_t index = LV_PROPERTY_ID_INDEX(id);

    if(index == LV_PROPERTY_ID_INVALID || index >= LV_PROPERTY_ID_START) {
        LV_LOG_WARN("invalid style property id 0x%" LV_PRIx32, id);
        value.id = LV_PROPERTY_ID_INVALID;
        value.num = 0;
        return value;
    }

    lv_obj_get_local_style_prop(obj, id, &value.style, selector);
    value.id = id;
    value.selector = selector;
    return value;
}

lv_prop_id_t lv_style_property_get_id(const char * name)
{
#if LV_USE_OBJ_PROPERTY_NAME
    lv_property_name_t * found;
    /*Check style property*/
    found = lv_utils_bsearch(name, lv_style_property_names, sizeof(lv_style_property_names) / sizeof(lv_property_name_t),
                             sizeof(lv_property_name_t), property_name_compare);
    if(found) return found->id;
#else
    LV_UNUSED(name);
#endif
    return LV_PROPERTY_ID_INVALID;
}

lv_prop_id_t lv_obj_class_property_get_id(const lv_obj_class_t * clz, const char * name)
{
#if LV_USE_OBJ_PROPERTY_NAME
    const lv_property_name_t * names;
    lv_property_name_t * found;

    names = clz->property_names;
    if(names == NULL) {
        /* try base class*/
        return LV_PROPERTY_ID_INVALID;
    }

    found = lv_utils_bsearch(name, names, clz->names_count, sizeof(lv_property_name_t), property_name_compare);
    if(found) return found->id;
#else
    LV_UNUSED(obj);
    LV_UNUSED(name);
    LV_UNUSED(property_name_compare);
#endif
    return LV_PROPERTY_ID_INVALID;
}

lv_prop_id_t lv_obj_property_get_id(const lv_obj_t * obj, const char * name)
{
#if LV_USE_OBJ_PROPERTY_NAME
    const lv_obj_class_t * clz;
    lv_prop_id_t id;

    for(clz = obj->class_p; clz; clz = clz->base_class) {
        id = lv_obj_class_property_get_id(clz, name);
        if(id != LV_PROPERTY_ID_INVALID) return id;
    }

    /*Check style property*/
    id = lv_style_property_get_id(name);
    if(id != LV_PROPERTY_ID_INVALID) return id;
#else
    LV_UNUSED(obj);
    LV_UNUSED(name);
    LV_UNUSED(property_name_compare);
#endif
    return LV_PROPERTY_ID_INVALID;
}

/**********************
 *  STATIC FUNCTIONS
 **********************/

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
                LV_LOG_WARN("NULL %s provided, id: 0x%" LV_PRIx32, set ? "setter" : "getter", id);
                return LV_RESULT_INVALID;
            }

            /*Update value id if it's a read*/
            if(!set) value->id = prop->id;

            switch(LV_PROPERTY_ID_TYPE(prop->id)) {
                case LV_PROPERTY_TYPE_INT:
                    HANDLE_PROPERTY_TYPE(integer, num);
                    break;
                case LV_PROPERTY_TYPE_BOOL:
                    HANDLE_PROPERTY_TYPE(boolean, enable);
                    break;
                case LV_PROPERTY_TYPE_PRECISE:
                    HANDLE_PROPERTY_TYPE(precise, precise);
                    break;
                case LV_PROPERTY_TYPE_COLOR:
                    HANDLE_PROPERTY_TYPE(color, color);
                    break;
                case LV_PROPERTY_TYPE_POINTER:
                case LV_PROPERTY_TYPE_IMGSRC:
                case LV_PROPERTY_TYPE_TEXT:
                case LV_PROPERTY_TYPE_OBJ:
                case LV_PROPERTY_TYPE_DISPLAY:
                case LV_PROPERTY_TYPE_FONT:
                    HANDLE_PROPERTY_TYPE(pointer, ptr);
                    break;
                case LV_PROPERTY_TYPE_POINT: {
                        lv_point_t * point = &value->point;
                        if(set)((lv_property_set_point_t)(prop->setter))(obj, point);
                        else *point = ((lv_property_get_point_t)(prop->getter))(obj);
                        break;
                    }
                default: {
                        LV_LOG_WARN("Unknown property id: 0x%08" LV_PRIx32, prop->id);
                        return LV_RESULT_INVALID;
                    }
            }

            return LV_RESULT_OK;
        }

        /*If no setter found, try base class then*/
    }

    LV_LOG_WARN("Unknown property id: 0x%08" LV_PRIx32, id);
    return LV_RESULT_INVALID;
}

static int property_name_compare(const void * ref, const void * element)
{
    const lv_property_name_t * prop = element;
    return lv_strcmp(ref, prop->name);
}

#endif /*LV_USE_OBJ_PROPERTY*/
