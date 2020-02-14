/**
 * @file lv_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "../lv_core/lv_debug.h"
#include "../lv_misc/lv_mem.h"
#include "../lv_misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/
#define STYLE_MIX_MAX 256
#define STYLE_MIX_SHIFT 8 /*log2(STYLE_MIX_MAX)*/

#define VAL_PROP(v1, v2, r) v1 + (((v2 - v1) * r) >> STYLE_MIX_SHIFT)
#define STYLE_ATTR_MIX(attr, r)                                                                                        \
    if(start->attr != end->attr) {                                                                                     \
        res->attr = VAL_PROP(start->attr, end->attr, r);                                                               \
    } else {                                                                                                           \
        res->attr = start->attr;                                                                                       \
    }

#define LV_STYLE_PROP_TO_ID(prop) (prop & 0xFF);
#define LV_STYLE_PROP_GET_TYPE(prop) ((prop >> 8) & 0xFF);

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static inline int32_t get_property_index(const lv_style_t * style, lv_style_property_t prop);
static lv_style_t * get_local_style(lv_style_list_t * list);

/**********************
 *  GLOABAL VARIABLES
 **********************/
//lv_style_t lv_style_transp_tight;

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
 * Initialize a style
 * @param style pointer to a style to initialize
 */
void lv_style_init(lv_style_t * style)
{
    style->map = NULL;
}

/**
 * Copy a style with all its properties
 * @param style_dest pointer to the destination style. (Should be initialized with `lv_style_init()`)
 * @param style_src pointer to the source (to copy )style
 */
void lv_style_copy(lv_style_t * style_dest, const lv_style_t * style_src)
{
    lv_style_init(style_dest);

    if(style_src->map == NULL) return;

    uint16_t size = lv_style_get_mem_size(style_src);

    style_dest->map = lv_mem_alloc(size);
    memcpy(style_dest->map, style_src->map, size);
}

/**
 * Initialize a style list
 * @param list a style list to initialize
 */
void lv_style_list_init(lv_style_list_t * list)
{
    list->style_list = NULL;
    list->style_cnt = 0;
    list->has_local = 0;
}

/**
 * Copy a style list with all its styles and local style properties
 * @param list_dest pointer to the destination style list. (should be initialized with `lv_style_list_init()`)
 * @param list_src pointer to the source (to copy) style list.
 */
void lv_style_list_copy(lv_style_list_t * list_dest, const lv_style_list_t * list_src)
{
    lv_style_list_reset(list_dest);

    if(list_src->style_list == NULL) return;

    if(list_src->has_local == 0) {
        list_dest->style_list = lv_mem_alloc(list_src->style_cnt * sizeof(lv_style_t *));
        memcpy(list_dest->style_list, list_src->style_list, list_src->style_cnt * sizeof(lv_style_t *));

        list_dest->style_cnt = list_src->style_cnt;
    } else {
        list_dest->style_list = lv_mem_alloc((list_src->style_cnt - 1) * sizeof(lv_style_t *));
        memcpy(list_dest->style_list, list_src->style_list + 1, (list_src->style_cnt - 1) * sizeof(lv_style_t *));
        list_dest->style_cnt = list_src->style_cnt - 1;

        lv_style_t * local_style = get_local_style(list_dest);
        lv_style_copy(local_style, (lv_style_t *)get_local_style(list_src));
    }
}

/**
 * Add a style to a style list.
 * Only the the style pointer will be saved so the shouldn't be a local variable.
 * (It should be static, global or dynamically allocated)
 * @param list pointer to a style list
 * @param style pointer to a style to add
 */
void lv_style_list_add_style(lv_style_list_t * list, lv_style_t * style)
{
    if(list == NULL) return;

    /*Remove the style first if already exists*/
    lv_style_list_remove_style(list, style);

    lv_style_t ** new_classes;
    if(list->style_cnt == 0) new_classes = lv_mem_alloc(sizeof(lv_style_t *));
    else new_classes = lv_mem_realloc(list->style_list, sizeof(lv_style_t *) * (list->style_cnt + 1));
    LV_ASSERT_MEM(new_classes);
    if(new_classes == NULL) {
        LV_LOG_WARN("lv_style_list_add_style: couldn't add the class");
        return;
    }

    /*Make space for the new style at the beginning. Leave local style if exists*/
    uint8_t i;
    uint8_t first_style = list->has_local ? 1 : 0;
    for(i = list->style_cnt; i > first_style; i--) {
        new_classes[i] = new_classes[i - 1];
    }

    new_classes[first_style] = style;
    list->style_cnt++;
    list->style_list = new_classes;
}

/**
 * Remove a style from a style list
 * @param style_list pointer to a style list
 * @param style pointer to a style to remove
 */
void lv_style_list_remove_style(lv_style_list_t * list, lv_style_t * style)
{
    if(list->style_cnt == 0) return;

    /*Check if the style really exists here*/
    uint8_t i;
    bool found = false;
    for(i = 0; i < list->style_cnt; i++) {
        if(list->style_list[i] == style) {
            found = true;
            break;
        }
    }
    if(found == false) return;

    if(list->style_cnt == 1) {
        lv_mem_free(list->style_list);
        list->style_list = NULL;
        list->style_cnt = 0;
        list->has_local = 0;
        return;
    }

    lv_style_t ** new_classes = lv_mem_realloc(list->style_list, sizeof(lv_style_t *) * (list->style_cnt - 1));
    LV_ASSERT_MEM(new_classes);
    if(new_classes == NULL) {
        LV_LOG_WARN("lv_style_list_remove_style: couldn't reallocate class list");
        return;
    }
    uint8_t j;
    for(i = 0, j = 0; i < list->style_cnt; i++) {
        if(list->style_list[i] == style) continue;
        new_classes[j] = list->style_list[i];
        j++;

    }

    list->style_cnt--;
    list->style_list = new_classes;
}

/**
 * Remove all styles added from style list, clear the local style and free all allocated memories
 * @param list pointer to a style list.
 */
void lv_style_list_reset(lv_style_list_t * list)
{
    if(list == NULL) return;
    if(list->has_local) {
        lv_style_t * local = lv_style_list_get_style(list, 0);
        lv_style_reset(local);
        lv_mem_free(local);
    }
    if(list->style_cnt > 0) lv_mem_free(list->style_list);
    list->style_list = NULL;
    list->style_cnt = 0;
    list->has_local = 0;
}

/**
 * Clear all properties from a style and all allocated memories.
 * @param style pointer to a style
 */
void lv_style_reset(lv_style_t * style)
{
    lv_mem_free(style->map);
    style->map = NULL;
}

/**
 * Get the size of the properties in a style in bytes
 * @param style pointer to a style
 * @return size of the properties in bytes
 */
uint16_t lv_style_get_mem_size(const lv_style_t * style)
{
    if(style->map == NULL) return 0;

    size_t i = 0;
    while(style->map[i] != _LV_STYLE_CLOSEING_PROP) {
        /*Go to the next property*/
        if((style->map[i] & 0xF) < LV_STYLE_ID_COLOR) i+= sizeof(lv_style_int_t);
        else if((style->map[i] & 0xF) < LV_STYLE_ID_OPA) i+= sizeof(lv_color_t);
        else if((style->map[i] & 0xF) < LV_STYLE_ID_PTR) i+= sizeof(lv_opa_t);
        else i+= sizeof(void*);

        i += sizeof(lv_style_property_t);
    }

    return i + sizeof(lv_style_property_t);
}

void lv_style_set_int(lv_style_t * style, lv_style_property_t prop, lv_style_int_t value)
{
    int32_t id = get_property_index(style, prop);
    /*The property already exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found.full = *(style->map + id + 1);
        attr_goal.full = (prop >> 8) & 0xFFU;

        if(attr_found.bits.state == attr_goal.bits.state) {
            memcpy(style->map + id + sizeof(lv_style_property_t), &value, sizeof(lv_style_int_t));
            return;
        }
    }

    /*Add new property if not exists yet*/
    uint8_t new_prop_size = (sizeof(lv_style_property_t) + sizeof(lv_style_int_t));
    lv_style_property_t end_mark = _LV_STYLE_CLOSEING_PROP;
    uint8_t end_mark_size = sizeof(end_mark);

    uint16_t size = lv_style_get_mem_size(style);
    if(size == 0) size += end_mark_size;
    size += sizeof(lv_style_property_t) + sizeof(lv_style_int_t);
    style->map = lv_mem_realloc(style->map, size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    memcpy(style->map + size - new_prop_size - end_mark_size , &prop, sizeof(lv_style_property_t));
    memcpy(style->map + size - sizeof(lv_style_int_t) - end_mark_size, &value, sizeof(lv_style_int_t));
    memcpy(style->map + size - end_mark_size, &end_mark, sizeof(end_mark));
}

void lv_style_set_color(lv_style_t * style, lv_style_property_t prop, lv_color_t color)
{
    int32_t id = get_property_index(style, prop);
    /*The property already exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found.full = *(style->map + id + 1);
        attr_goal.full = (prop >> 8) & 0xFFU;

        if(attr_found.bits.state == attr_goal.bits.state) {
            memcpy(style->map + id + sizeof(lv_style_property_t), &color, sizeof(lv_color_t));
            return;
        }
    }

    /*Add new property if not exists yet*/
    uint8_t new_prop_size = (sizeof(lv_style_property_t) + sizeof(lv_color_t));
    lv_style_property_t end_mark = _LV_STYLE_CLOSEING_PROP;
    uint8_t end_mark_size = sizeof(end_mark);

    uint16_t size = lv_style_get_mem_size(style);
    if(size == 0) size += end_mark_size;

    size += sizeof(lv_style_property_t) + sizeof(lv_color_t);
    style->map = lv_mem_realloc(style->map, size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    memcpy(style->map + size - new_prop_size - end_mark_size, &prop, sizeof(lv_style_property_t));
    memcpy(style->map + size - sizeof(lv_color_t) - end_mark_size, &color, sizeof(lv_color_t));
    memcpy(style->map + size - end_mark_size, &end_mark, sizeof(end_mark));
}

void lv_style_set_opa(lv_style_t * style, lv_style_property_t prop, lv_opa_t opa)
{
    int32_t id = get_property_index(style, prop);
    /*The property already exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found.full = *(style->map + id + 1);
        attr_goal.full = (prop >> 8) & 0xFFU;

        if(attr_found.bits.state == attr_goal.bits.state) {
            memcpy(style->map + id + sizeof(lv_style_property_t), &opa, sizeof(lv_opa_t));
            return;
        }
    }

    /*Add new property if not exists yet*/
    uint8_t new_prop_size = (sizeof(lv_style_property_t) + sizeof(lv_opa_t));
    lv_style_property_t end_mark = _LV_STYLE_CLOSEING_PROP;
    uint8_t end_mark_size = sizeof(end_mark);

    uint16_t size = lv_style_get_mem_size(style);
    if(size == 0) size += end_mark_size;

    size += sizeof(lv_style_property_t) + sizeof(lv_opa_t);
    style->map = lv_mem_realloc(style->map, size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    memcpy(style->map + size - new_prop_size - end_mark_size, &prop, sizeof(lv_style_property_t));
    memcpy(style->map + size - sizeof(lv_opa_t) - end_mark_size, &opa, sizeof(lv_opa_t));
    memcpy(style->map + size - end_mark_size, &end_mark, sizeof(end_mark));
}

void lv_style_set_ptr(lv_style_t * style, lv_style_property_t prop, const void * p)
{
    int32_t id = get_property_index(style, prop);
    /*The property already exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found.full = *(style->map + id + 1);
        attr_goal.full = (prop >> 8) & 0xFFU;

        if(attr_found.bits.state == attr_goal.bits.state) {
            memcpy(style->map + id + sizeof(lv_style_property_t), &p, sizeof(void *));
            return;
        }
    }

    /*Add new property if not exists yet*/
    uint8_t new_prop_size = (sizeof(lv_style_property_t) + sizeof(void *));
    lv_style_property_t end_mark = _LV_STYLE_CLOSEING_PROP;
    uint8_t end_mark_size = sizeof(end_mark);

    uint16_t size = lv_style_get_mem_size(style);
    if(size == 0) size += end_mark_size;

    size += sizeof(lv_style_property_t) + sizeof(void *);
    style->map = lv_mem_realloc(style->map, size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    memcpy(style->map + size - new_prop_size - end_mark_size , &prop, sizeof(lv_style_property_t));
    memcpy(style->map + size - sizeof(void *) - end_mark_size , &p, sizeof(void *));
    memcpy(style->map + size - end_mark_size, &end_mark, sizeof(end_mark));
}


/**
 * Get the a property from a style.
 * Take into account the style state and return the property which matches the best.
 * @param style pointer to a style where to search
 * @param prop the property, might contain ORed style states too
 * @param res buffer to store the result
 * @return the weight of the found property (how well it fits to the style state).
 *         Higher number is means better fit
 *         -1 if the not found (`res` will be undefined)
 */
int16_t lv_style_get_int(const lv_style_t * style, lv_style_property_t prop, lv_style_int_t * res)
{
    if(style == NULL) return -1;
    if(style->map == NULL) return -1;
    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        return -1;
    } else {
        memcpy(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_style_int_t));
        lv_style_attr_t attr_act;
        attr_act.full = style->map[id + 1];

        lv_style_attr_t attr_goal;
        attr_goal.full = (prop >> 8) & 0xFF;

        return attr_act.bits.state & attr_goal.bits.state;
    }
}


int16_t lv_style_get_opa(const lv_style_t * style, lv_style_property_t prop, lv_opa_t * res)
{
    if(style == NULL) return -1;
    if(style->map == NULL) return -1;
    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        return -1;
    } else {
        memcpy(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_opa_t));
        lv_style_attr_t attr_act;
        attr_act.full = style->map[id + 1];

        lv_style_attr_t attr_goal;
        attr_goal.full = (prop >> 8) & 0xFF;

        return attr_act.bits.state & attr_goal.bits.state;
    }
}

int16_t lv_style_get_color(const lv_style_t * style, lv_style_property_t prop, lv_color_t * res)
{
    if(style == NULL) return -1;
    if(style->map == NULL) return -1;
    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        return -1;
    } else {
        memcpy(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_color_t));
        lv_style_attr_t attr_act;
        attr_act.full = style->map[id + 1];

        lv_style_attr_t attr_goal;
        attr_goal.full = (prop >> 8) & 0xFF;

        return attr_act.bits.state & attr_goal.bits.state;
    }
}


int16_t lv_style_get_ptr(const lv_style_t * style, lv_style_property_t prop, void ** res)
{
    if(style == NULL) return -1;
    if(style->map == NULL) return -1;
    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        return -1;
    } else {
        memcpy(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(void*));
        lv_style_attr_t attr_act;
        attr_act.full = style->map[id + 1];

        lv_style_attr_t attr_goal;
        attr_goal.full = (prop >> 8) & 0xFF;

        return attr_act.bits.state & attr_goal.bits.state;
    }
}


void lv_style_list_set_local_int(lv_style_list_t * list, lv_style_property_t prop, lv_style_int_t value)
{
    lv_style_t * local = get_local_style(list);
    lv_style_set_int(local, prop, value);
}

void lv_style_list_set_local_opa(lv_style_list_t * list, lv_style_property_t prop, lv_opa_t value)
{
    lv_style_t * local = get_local_style(list);
    lv_style_set_opa(local, prop, value);
}

void lv_style_list_set_local_color(lv_style_list_t * list, lv_style_property_t prop, lv_color_t value)
{
    lv_style_t * local = get_local_style(list);
    lv_style_set_color(local, prop, value);
}

void lv_style_list_set_local_ptr(lv_style_list_t * list, lv_style_property_t prop, const void * value)
{
    lv_style_t * local = get_local_style(list);
    lv_style_set_ptr(local, prop, value);
}


lv_res_t lv_style_list_get_int(lv_style_list_t * list, lv_style_property_t prop, lv_style_int_t * value)
{
    if(list == NULL) return LV_RES_INV;
    if(list->style_list == NULL) return LV_RES_INV;

    lv_style_attr_t attr;
    attr.full = prop >> 8;
    int16_t weight_goal = attr.full;

    int16_t weight_act;
    int16_t weight = -1;

    lv_style_int_t value_act;

    int16_t ci;
    for(ci = 0; ci < list->style_cnt; ci++) {
        lv_style_t * class = lv_style_list_get_style(list, ci);
        weight_act = lv_style_get_int(class, prop, &value_act);
        /*On perfect match return the value immediately*/
        if(weight_act == weight_goal) {
            *value = value_act;
            return LV_RES_OK;
        }
        /*If the found ID is better the current candidate then use it*/
        else if(weight_act > weight) {
            weight =  weight_act;
            *value = value_act;
        }
    }

    if(weight >= 0) return LV_RES_OK;
    else return LV_RES_INV;

}


lv_res_t lv_style_list_get_color(lv_style_list_t * list, lv_style_property_t prop, lv_color_t * value)
{
    if(list == NULL) return LV_RES_INV;
    if(list->style_list == NULL) return LV_RES_INV;

    lv_style_attr_t attr;
    attr.full = prop >> 8;
    int16_t weight_goal = attr.full;

    int16_t weight_act;
    int16_t weight = -1;

    lv_color_t value_act;

    int16_t ci;
    for(ci = 0; ci < list->style_cnt; ci++) {
        lv_style_t * class = lv_style_list_get_style(list, ci);
        weight_act = lv_style_get_color(class, prop, &value_act);
        /*On perfect match return the value immediately*/
        if(weight_act == weight_goal) {
            *value = value_act;
            return LV_RES_OK;
        }
        /*If the found ID is better the current candidate then use it*/
        else if(weight_act > weight) {
            weight =  weight_act;
            *value = value_act;
        }
    }

    if(weight >= 0)  return LV_RES_OK;
    else return LV_RES_INV;
}



lv_res_t lv_style_list_get_opa(lv_style_list_t * list, lv_style_property_t prop, lv_opa_t * value)
{
    if(list == NULL) return LV_RES_INV;
    if(list->style_list == NULL) return LV_RES_INV;

    lv_style_attr_t attr;
    attr.full = prop >> 8;
    int16_t weight_goal = attr.full;

    int16_t weight_act;
    int16_t weight = -1;

    lv_opa_t value_act = LV_OPA_TRANSP;

    int16_t ci;
    for(ci = 0; ci < list->style_cnt; ci++) {
        lv_style_t * class = lv_style_list_get_style(list, ci);
        weight_act = lv_style_get_opa(class, prop, &value_act);
        /*On perfect match return the value immediately*/
        if(weight_act == weight_goal) {
            *value = value_act;
            return LV_RES_OK;
        }
        /*If the found ID is better the current candidate then use it*/
        else if(weight_act > weight) {
            weight =  weight_act;
            *value = value_act;
        }
    }

    if(weight >= 0)  return LV_RES_OK;
    else return LV_RES_INV;
}


lv_res_t lv_style_list_get_ptr(lv_style_list_t * list, lv_style_property_t prop, void ** value)
{
    if(list == NULL) return LV_RES_INV;
    if(list->style_list == NULL) return LV_RES_INV;

    lv_style_attr_t attr;
    attr.full = prop >> 8;
    int16_t weight_goal = attr.full;

    int16_t weight_act;
    int16_t weight = -1;

    void * value_act = NULL;

    int16_t ci;
    for(ci = 0; ci < list->style_cnt; ci++) {
        lv_style_t * class = lv_style_list_get_style(list, ci);
        weight_act = lv_style_get_ptr(class, prop, &value_act);
        /*On perfect match return the value immediately*/
        if(weight_act == weight_goal) {
            *value = value_act;
            return LV_RES_OK;
        }
        /*If the found ID is better the current candidate then use it*/
        else if(weight_act > weight) {
            weight =  weight_act;
            *value = value_act;
        }
    }

    if(weight >= 0)  return LV_RES_OK;
    else return LV_RES_INV;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline int32_t get_property_index(const lv_style_t * style, lv_style_property_t prop)
{
    if(style->map == NULL) return -1;

    uint8_t id_to_find = prop & 0xFF;
    lv_style_attr_t attr;
    attr.full = (prop >> 8) & 0xFF;

    int16_t weight = -1;
    int16_t id_guess = -1;

    size_t i = 0;
    while(style->map[i] != _LV_STYLE_CLOSEING_PROP) {
        if(style->map[i] == id_to_find) {
			lv_style_attr_t attr_act;
			attr_act.full = style->map[i + 1];

            /*If the state perfectly matches return this property*/
            if(attr_act.bits.state == attr.bits.state) {
                return i;
            }
            /* Be sure the property not specifies other state than the requested.
             * E.g. For HOVER+PRESS, HOVER only is OK, but HOVER+FOCUS not*/
            else if((attr_act.bits.state & (~attr.bits.state)) == 0) {
                /* Use this property if it describes better the requested state than the current candidate.
                 * E.g. for HOVER+FOCUS+PRESS prefer HOVER+FOCUS over FOCUS*/
                if(attr_act.bits.state > weight) {
                    weight = attr_act.bits.state;
                    id_guess = i;
                }
            }
        }

        /*Go to the next property*/
        if((style->map[i] & 0xF) < LV_STYLE_ID_COLOR) i+= sizeof(lv_style_int_t);
        else if((style->map[i] & 0xF) < LV_STYLE_ID_OPA) i+= sizeof(lv_color_t);
        else if((style->map[i] & 0xF) < LV_STYLE_ID_PTR) i+= sizeof(lv_opa_t);
        else i+= sizeof(void*);

        i += sizeof(lv_style_property_t);
    }

    return id_guess;
}


static lv_style_t * get_local_style(lv_style_list_t * list)
{

    if(list->has_local) return lv_style_list_get_style(list, 0);


    lv_style_t * local_style = lv_mem_alloc(sizeof(lv_style_t));
    LV_ASSERT_MEM(local_style);
    if(local_style == NULL) {
        LV_LOG_WARN("get_local_style: couldn't create local style");
        return NULL;
    }
    lv_style_init(local_style);

    lv_style_list_add_style(list, local_style);
    list->has_local = 1;

    return local_style;
}

