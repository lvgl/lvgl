/**
 * @file lv_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
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
LV_ATTRIBUTE_FAST_MEM static inline int32_t get_property_index(const lv_style_t * style, lv_style_property_t prop);
static lv_style_t * get_alloc_local_style(lv_style_list_t * list);
static inline void style_resize(lv_style_t * style, size_t sz);
static inline lv_style_property_t get_style_prop(const lv_style_t * style, size_t idx);
static inline uint8_t get_style_prop_id(const lv_style_t * style, size_t idx);
static inline uint8_t get_style_prop_attr(const lv_style_t * style, size_t idx);
static inline size_t get_prop_size(uint8_t prop_id);
static inline size_t get_next_prop_index(uint8_t prop_id, size_t id);

/**********************
 *  GLOBAL VARIABLES
 **********************/

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
    _lv_memset_00(style, sizeof(lv_style_t));
#if LV_USE_ASSERT_STYLE
    style->sentinel = LV_DEBUG_STYLE_SENTINEL_VALUE;
#endif
}

/**
 * Copy a style with all its properties
 * @param style_dest pointer to the destination style. (Should be initialized with `lv_style_init()`)
 * @param style_src pointer to the source (to copy )style
 */
void lv_style_copy(lv_style_t * style_dest, const lv_style_t * style_src)
{
    if(style_src == NULL) return;

    LV_ASSERT_STYLE(style_dest);
    LV_ASSERT_STYLE(style_src);

    if(style_src->map == NULL) return;

    uint16_t size = _lv_style_get_mem_size(style_src);
    style_dest->map = lv_mem_alloc(size);
    _lv_memcpy(style_dest->map, style_src->map, size);
}

/**
 * Remove a property from a style
 * @param style pointer to a style
 * @param prop  a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @return true: the property was found and removed; false: the property wasn't found
 */
bool lv_style_remove_prop(lv_style_t * style, lv_style_property_t prop)
{
    if(style == NULL) return false;
    LV_ASSERT_STYLE(style);

    int32_t id = get_property_index(style, prop);
    /*The property exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found = get_style_prop_attr(style, id);
        attr_goal = (prop >> 8) & 0xFFU;

        if(LV_STYLE_ATTR_GET_STATE(attr_found) == LV_STYLE_ATTR_GET_STATE(attr_goal)) {
            uint32_t map_size = _lv_style_get_mem_size(style);
            uint8_t prop_size = get_prop_size(prop);

            /*Move the props to fill the space of the property to delete*/
            uint32_t i;
            for(i = id; i < map_size - prop_size; i++) {
                style->map[i] = style->map[i + prop_size];
            }

            style_resize(style, map_size - prop_size);

            return true;
        }
    }

    return false;
}

/**
 * Initialize a style list
 * @param list a style list to initialize
 */
void lv_style_list_init(lv_style_list_t * list)
{
    _lv_memset_00(list, sizeof(lv_style_list_t));
#if LV_USE_ASSERT_STYLE
    list->sentinel = LV_DEBUG_STYLE_LIST_SENTINEL_VALUE;
#endif
}

/**
 * Copy a style list with all its styles and local style properties
 * @param list_dest pointer to the destination style list. (should be initialized with `lv_style_list_init()`)
 * @param list_src pointer to the source (to copy) style list.
 */
void lv_style_list_copy(lv_style_list_t * list_dest, const lv_style_list_t * list_src)
{
    LV_ASSERT_STYLE_LIST(list_dest);
    LV_ASSERT_STYLE_LIST(list_src);

    _lv_style_list_reset(list_dest);

    if(list_src->style_list == NULL) return;

    /*Copy the styles but skip the transitions*/
    if(list_src->has_local == 0) {
        if(list_src->has_trans) {
            list_dest->style_list = lv_mem_alloc((list_src->style_cnt - 1) * sizeof(lv_style_t *));
            _lv_memcpy(list_dest->style_list, list_src->style_list + 1, (list_src->style_cnt - 1) * sizeof(lv_style_t *));
            list_dest->style_cnt = list_src->style_cnt - 1;
        }
        else {
            list_dest->style_list = lv_mem_alloc(list_src->style_cnt * sizeof(lv_style_t *));
            _lv_memcpy(list_dest->style_list, list_src->style_list, list_src->style_cnt * sizeof(lv_style_t *));
            list_dest->style_cnt = list_src->style_cnt;
        }
    }
    else {
        if(list_src->has_trans) {
            list_dest->style_list = lv_mem_alloc((list_src->style_cnt - 2) * sizeof(lv_style_t *));
            _lv_memcpy(list_dest->style_list, list_src->style_list + 2, (list_src->style_cnt - 2) * sizeof(lv_style_t *));
            list_dest->style_cnt = list_src->style_cnt - 2;
        }
        else {
            list_dest->style_list = lv_mem_alloc((list_src->style_cnt - 1) * sizeof(lv_style_t *));
            _lv_memcpy(list_dest->style_list, list_src->style_list + 1, (list_src->style_cnt - 1) * sizeof(lv_style_t *));
            list_dest->style_cnt = list_src->style_cnt - 1;
        }

        lv_style_t * local_style = get_alloc_local_style(list_dest);
        lv_style_copy(local_style, get_alloc_local_style((lv_style_list_t *)list_src));
    }
}

/**
 * Add a style to a style list.
 * Only the the style pointer will be saved so the shouldn't be a local variable.
 * (It should be static, global or dynamically allocated)
 * @param list pointer to a style list
 * @param style pointer to a style to add
 */
void _lv_style_list_add_style(lv_style_list_t * list, lv_style_t * style)
{
    LV_ASSERT_STYLE_LIST(list);
    LV_ASSERT_STYLE(style);

    if(list == NULL) return;

    /*Remove the style first if already exists*/
    _lv_style_list_remove_style(list, style);

    lv_style_t ** new_classes;
    if(list->style_cnt == 0) new_classes = lv_mem_alloc(sizeof(lv_style_t *));
    else new_classes = lv_mem_realloc(list->style_list, sizeof(lv_style_t *) * (list->style_cnt + 1));
    LV_ASSERT_MEM(new_classes);
    if(new_classes == NULL) {
        LV_LOG_WARN("lv_style_list_add_style: couldn't add the class");
        return;
    }

    /*Make space for the new style at the beginning. Leave local and trans style if exists*/
    uint8_t i;
    uint8_t first_style = 0;
    if(list->has_trans) first_style++;
    if(list->has_local) first_style++;
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
void _lv_style_list_remove_style(lv_style_list_t * list, lv_style_t * style)
{
    LV_ASSERT_STYLE_LIST(list);
    LV_ASSERT_STYLE(style);

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

    lv_style_t ** new_classes = lv_mem_alloc(sizeof(lv_style_t *) * (list->style_cnt - 1));
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

    lv_mem_free(list->style_list);

    list->style_cnt--;
    list->style_list = new_classes;
}

/**
 * Remove all styles added from style list, clear the local style, transition style and free all allocated memories.
 * Leave `ignore_trans` flag as it is.
 * @param list pointer to a style list.
 */
void _lv_style_list_reset(lv_style_list_t * list)
{
    LV_ASSERT_STYLE_LIST(list);

    if(list == NULL) return;

    if(list->has_local) {
        lv_style_t * local = lv_style_list_get_local_style(list);
        if(local) {
            lv_style_reset(local);
            lv_mem_free(local);
        }
    }

    if(list->has_trans) {
        lv_style_t * trans = _lv_style_list_get_transition_style(list);
        if(trans) {
            lv_style_reset(trans);
            lv_mem_free(trans);
        }
    }

    if(list->style_cnt > 0) lv_mem_free(list->style_list);
    list->style_list = NULL;
    list->style_cnt = 0;
    list->has_local = 0;
    list->has_trans = 0;
    list->skip_trans = 0;

    /* Intentionally leave `ignore_trans` as it is,
     * because it's independent from the styles in the list*/
}

/**
 * Clear all properties from a style and all allocated memories.
 * @param style pointer to a style
 */
void lv_style_reset(lv_style_t * style)
{
    LV_ASSERT_STYLE(style);

    lv_mem_free(style->map);
    style->map = NULL;
}

/**
 * Get the size of the properties in a style in bytes
 * @param style pointer to a style
 * @return size of the properties in bytes
 */
uint16_t _lv_style_get_mem_size(const lv_style_t * style)
{
    LV_ASSERT_STYLE(style);

    if(style->map == NULL) return 0;

    size_t i = 0;
    uint8_t prop_id;
    while((prop_id = get_style_prop_id(style, i)) != _LV_STYLE_CLOSING_PROP) {
        i = get_next_prop_index(prop_id, i);
    }

    return i + sizeof(lv_style_property_t);
}

/**
 * Set an integer typed property in a style.
 * @param style pointer to a style where the property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note shouldn't be used directly. Use the specific property set functions instead.
 *       For example: `lv_style_set_border_width()`
 * @note for performance reasons it's not checked if the property really has integer type
 */
void _lv_style_set_int(lv_style_t * style, lv_style_property_t prop, lv_style_int_t value)
{
    LV_ASSERT_STYLE(style);

    int32_t id = get_property_index(style, prop);
    /*The property already exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found = get_style_prop_attr(style, id);
        attr_goal = (prop >> 8) & 0xFFU;

        if(LV_STYLE_ATTR_GET_STATE(attr_found) == LV_STYLE_ATTR_GET_STATE(attr_goal)) {
            _lv_memcpy_small(style->map + id + sizeof(lv_style_property_t), &value, sizeof(lv_style_int_t));
            return;
        }
    }

    /*Add new property if not exists yet*/
    uint8_t new_prop_size = (sizeof(lv_style_property_t) + sizeof(lv_style_int_t));
    lv_style_property_t end_mark = _LV_STYLE_CLOSING_PROP;
    uint8_t end_mark_size = sizeof(end_mark);

    uint16_t size = _lv_style_get_mem_size(style);
    if(size == 0) size += end_mark_size;
    size += sizeof(lv_style_property_t) + sizeof(lv_style_int_t);
    style_resize(style, size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    _lv_memcpy_small(style->map + size - new_prop_size - end_mark_size, &prop, sizeof(lv_style_property_t));
    _lv_memcpy_small(style->map + size - sizeof(lv_style_int_t) - end_mark_size, &value, sizeof(lv_style_int_t));
    _lv_memcpy_small(style->map + size - end_mark_size, &end_mark, sizeof(end_mark));
}

/**
 * Set a color typed property in a style.
 * @param style pointer to a style where the property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note shouldn't be used directly. Use the specific property set functions instead.
 *       For example: `lv_style_set_border_color()`
 * @note for performance reasons it's not checked if the property really has color type
 */
void _lv_style_set_color(lv_style_t * style, lv_style_property_t prop, lv_color_t color)
{
    LV_ASSERT_STYLE(style);

    int32_t id = get_property_index(style, prop);
    /*The property already exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found = get_style_prop_attr(style, id);
        attr_goal = (prop >> 8) & 0xFFU;

        if(LV_STYLE_ATTR_GET_STATE(attr_found) == LV_STYLE_ATTR_GET_STATE(attr_goal)) {
            _lv_memcpy_small(style->map + id + sizeof(lv_style_property_t), &color, sizeof(lv_color_t));
            return;
        }
    }

    /*Add new property if not exists yet*/
    uint8_t new_prop_size = (sizeof(lv_style_property_t) + sizeof(lv_color_t));
    lv_style_property_t end_mark = _LV_STYLE_CLOSING_PROP;
    uint8_t end_mark_size = sizeof(end_mark);

    uint16_t size = _lv_style_get_mem_size(style);
    if(size == 0) size += end_mark_size;

    size += sizeof(lv_style_property_t) + sizeof(lv_color_t);
    style_resize(style, size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    _lv_memcpy_small(style->map + size - new_prop_size - end_mark_size, &prop, sizeof(lv_style_property_t));
    _lv_memcpy_small(style->map + size - sizeof(lv_color_t) - end_mark_size, &color, sizeof(lv_color_t));
    _lv_memcpy_small(style->map + size - end_mark_size, &end_mark, sizeof(end_mark));
}

/**
 * Set an opacity typed property in a style.
 * @param style pointer to a style where the property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note shouldn't be used directly. Use the specific property set functions instead.
 *       For example: `lv_style_set_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
void _lv_style_set_opa(lv_style_t * style, lv_style_property_t prop, lv_opa_t opa)
{
    LV_ASSERT_STYLE(style);

    int32_t id = get_property_index(style, prop);
    /*The property already exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found = get_style_prop_attr(style, id);
        attr_goal = (prop >> 8) & 0xFFU;

        if(LV_STYLE_ATTR_GET_STATE(attr_found) == LV_STYLE_ATTR_GET_STATE(attr_goal)) {
            _lv_memcpy_small(style->map + id + sizeof(lv_style_property_t), &opa, sizeof(lv_opa_t));
            return;
        }
    }

    /*Add new property if not exists yet*/
    uint8_t new_prop_size = (sizeof(lv_style_property_t) + sizeof(lv_opa_t));
    lv_style_property_t end_mark = _LV_STYLE_CLOSING_PROP;
    uint8_t end_mark_size = sizeof(end_mark);

    uint16_t size = _lv_style_get_mem_size(style);
    if(size == 0) size += end_mark_size;

    size += sizeof(lv_style_property_t) + sizeof(lv_opa_t);
    style_resize(style, size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    _lv_memcpy_small(style->map + size - new_prop_size - end_mark_size, &prop, sizeof(lv_style_property_t));
    _lv_memcpy_small(style->map + size - sizeof(lv_opa_t) - end_mark_size, &opa, sizeof(lv_opa_t));
    _lv_memcpy_small(style->map + size - end_mark_size, &end_mark, sizeof(end_mark));
}

/**
 * Set a pointer typed property in a style.
 * @param style pointer to a style where the property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_POINTER | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note shouldn't be used directly. Use the specific property set functions instead.
 *       For example: `lv_style_set_border_width()`
 * @note for performance reasons it's not checked if the property is really has pointer type
 */
void _lv_style_set_ptr(lv_style_t * style, lv_style_property_t prop, const void * p)
{
    LV_ASSERT_STYLE(style);

    int32_t id = get_property_index(style, prop);
    /*The property already exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found = get_style_prop_attr(style, id);
        attr_goal = (prop >> 8) & 0xFFU;

        if(LV_STYLE_ATTR_GET_STATE(attr_found) == LV_STYLE_ATTR_GET_STATE(attr_goal)) {
            _lv_memcpy_small(style->map + id + sizeof(lv_style_property_t), &p, sizeof(const void *));
            return;
        }
    }

    /*Add new property if not exists yet*/
    uint8_t new_prop_size = (sizeof(lv_style_property_t) + sizeof(const void *));
    lv_style_property_t end_mark = _LV_STYLE_CLOSING_PROP;
    uint8_t end_mark_size = sizeof(end_mark);

    uint16_t size = _lv_style_get_mem_size(style);
    if(size == 0) size += end_mark_size;

    size += sizeof(lv_style_property_t) + sizeof(const void *);
    style_resize(style, size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    _lv_memcpy_small(style->map + size - new_prop_size - end_mark_size, &prop, sizeof(lv_style_property_t));
    _lv_memcpy_small(style->map + size - sizeof(const void *) - end_mark_size, &p, sizeof(const void *));
    _lv_memcpy_small(style->map + size - end_mark_size, &end_mark, sizeof(end_mark));
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
int16_t _lv_style_get_int(const lv_style_t * style, lv_style_property_t prop, void * v_res)
{
    lv_style_int_t * res = (lv_style_int_t *)v_res;
    LV_ASSERT_STYLE(style);

    if(style == NULL) return -1;
    if(style->map == NULL) return -1;

    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        return -1;
    }
    else {
        _lv_memcpy_small(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_style_int_t));
        lv_style_attr_t attr_act;
        attr_act = get_style_prop_attr(style, id);

        lv_style_attr_t attr_goal;
        attr_goal = (prop >> 8) & 0xFF;

        return LV_STYLE_ATTR_GET_STATE(attr_act) & LV_STYLE_ATTR_GET_STATE(attr_goal);
    }
}

/**
 * Get an opacity typed property from a style.
 * @param style pointer to a style from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result value
 * @return -1: the property wasn't found in the style.
 *         The matching state bits of the desired state (in `prop`) and the best matching property's state
 *         Higher value means match in higher precedence state.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
int16_t _lv_style_get_opa(const lv_style_t * style, lv_style_property_t prop, void * v_res)
{
    lv_opa_t * res = (lv_opa_t *)v_res;
    LV_ASSERT_STYLE(style);

    if(style == NULL) return -1;
    if(style->map == NULL) return -1;

    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        return -1;
    }
    else {
        _lv_memcpy_small(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_opa_t));
        lv_style_attr_t attr_act;
        attr_act = get_style_prop_attr(style, id);

        lv_style_attr_t attr_goal;
        attr_goal = (prop >> 8) & 0xFF;

        return LV_STYLE_ATTR_GET_STATE(attr_act) & LV_STYLE_ATTR_GET_STATE(attr_goal);
    }
}

/**
 * Get a color typed property from a style.
 * @param style pointer to a style from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result value
 * @return -1: the property wasn't found in the style.
 *         The matching state bits of the desired state (in `prop`) and the best matching property's state
 *         Higher value means match in higher precedence state.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_style_get_border_color()`
 * @note for performance reasons it's not checked if the property really has color type
 */
int16_t _lv_style_get_color(const lv_style_t * style, lv_style_property_t prop, void * v_res)
{
    lv_color_t * res = (lv_color_t *)v_res;
    if(style == NULL) return -1;
    if(style->map == NULL) return -1;
    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        return -1;
    }
    else {
        _lv_memcpy_small(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_color_t));
        lv_style_attr_t attr_act;
        attr_act = get_style_prop_attr(style, id);

        lv_style_attr_t attr_goal;
        attr_goal = (prop >> 8) & 0xFF;

        return LV_STYLE_ATTR_GET_STATE(attr_act) & LV_STYLE_ATTR_GET_STATE(attr_goal);
    }
}

/**
 * Get a pointer typed property from a style.
 * @param style pointer to a style from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result value
 * @return -1: the property wasn't found in the style.
 *         The matching state bits of the desired state (in `prop`) and the best matching property's state
 *         Higher value means match in higher precedence state.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_style_get_text_font()`
 * @note for performance reasons it's not checked if the property really has pointer type
 */
int16_t _lv_style_get_ptr(const lv_style_t * style, lv_style_property_t prop, void * v_res)
{
    const void ** res = (const void **)v_res;
    if(style == NULL) return -1;
    if(style->map == NULL) return -1;

    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        return -1;
    }
    else {
        _lv_memcpy_small(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(const void *));
        lv_style_attr_t attr_act;
        attr_act = get_style_prop_attr(style, id);

        lv_style_attr_t attr_goal;
        attr_goal = (prop >> 8) & 0xFF;

        return LV_STYLE_ATTR_GET_STATE(attr_act) & LV_STYLE_ATTR_GET_STATE(attr_goal);
    }
}

/**
 * Get the local style of a style list
 * @param list pointer to a style list where the local property should be set
 * @return pointer to the local style if exists else `NULL`.
 */
lv_style_t * lv_style_list_get_local_style(lv_style_list_t * list)
{
    LV_ASSERT_STYLE_LIST(list);

    if(!list->has_local) return NULL;
    if(list->has_trans) return list->style_list[1];
    else return list->style_list[0];
}

/**
 * Get the transition style of a style list
 * @param list pointer to a style list where the local property should be set
 * @return pointer to the transition style if exists else `NULL`.
 */
lv_style_t * _lv_style_list_get_transition_style(lv_style_list_t * list)
{
    LV_ASSERT_STYLE_LIST(list);

    if(!list->has_trans) return NULL;
    return list->style_list[0];
}

/**
 * Allocate the transition style in a style list. If already exists simply return it.
 * @param list pointer to a style list
 * @return the transition style of a style list
 */
lv_style_t * _lv_style_list_add_trans_style(lv_style_list_t * list)
{
    LV_ASSERT_STYLE_LIST(list);
    if(list->has_trans) return _lv_style_list_get_transition_style(list);

    lv_style_t * trans_style = lv_mem_alloc(sizeof(lv_style_t));
    LV_ASSERT_MEM(trans_style);
    if(trans_style == NULL) {
        LV_LOG_WARN("lv_style_list_add_trans_style: couldn't create transition style");
        return NULL;
    }

    lv_style_init(trans_style);

    _lv_style_list_add_style(list, trans_style);
    list->has_trans = 1;

    /*If the list has local style trans was added after it. But trans should be the first so swap them*/
    if(list->has_local) {
        lv_style_t * tmp = list->style_list[0];
        list->style_list[0] = list->style_list[1];
        list->style_list[1] = tmp;
    }
    return trans_style;
}


/**
 * Set a local integer typed property in a style list.
 * @param list pointer to a style list where the local property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note for performance reasons it's not checked if the property really has integer type
 */
void _lv_style_list_set_local_int(lv_style_list_t * list, lv_style_property_t prop, lv_style_int_t value)
{
    LV_ASSERT_STYLE_LIST(list);

    lv_style_t * local = get_alloc_local_style(list);
    _lv_style_set_int(local, prop, value);
}

/**
 * Set a local opacity typed property in a style list.
 * @param list pointer to a style list where the local property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note for performance reasons it's not checked if the property really has opacity type
 */
void _lv_style_list_set_local_opa(lv_style_list_t * list, lv_style_property_t prop, lv_opa_t value)
{
    LV_ASSERT_STYLE_LIST(list);

    lv_style_t * local = get_alloc_local_style(list);
    _lv_style_set_opa(local, prop, value);
}

/**
 * Set a local color typed property in a style list.
 * @param list pointer to a style list where the local property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note for performance reasons it's not checked if the property really has color type
 */
void _lv_style_list_set_local_color(lv_style_list_t * list, lv_style_property_t prop, lv_color_t value)
{
    LV_ASSERT_STYLE_LIST(list);

    lv_style_t * local = get_alloc_local_style(list);
    _lv_style_set_color(local, prop, value);
}

/**
 * Set a local pointer typed property in a style list.
 * @param list pointer to a style list where the local property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note for performance reasons it's not checked if the property really has pointer type
 */
void _lv_style_list_set_local_ptr(lv_style_list_t * list, lv_style_property_t prop, const void * value)
{
    LV_ASSERT_STYLE_LIST(list);

    lv_style_t * local = get_alloc_local_style(list);
    _lv_style_set_ptr(local, prop, value);
}



/**
 * Get an integer typed property from a style list.
 * It will return the property which match best with given state.
 * @param list pointer to a style list from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result
 * @return LV_RES_OK: there was a matching property in the list
 *         LV_RES_INV: there was NO matching property in the list
 * @note for performance reasons it's not checked if the property really has integer type
 */
lv_res_t _lv_style_list_get_int(lv_style_list_t * list, lv_style_property_t prop, lv_style_int_t * res)
{
    LV_ASSERT_STYLE_LIST(list);

    if(list == NULL) return LV_RES_INV;
    if(list->style_list == NULL) return LV_RES_INV;

    lv_style_attr_t attr;
    attr = prop >> 8;
    int16_t weight_goal = attr;

    int16_t weight = -1;

    lv_style_int_t value_act = 0;

    int16_t ci;
    for(ci = 0; ci < list->style_cnt; ci++) {
        /* changed class to _class to allow compilation as c++ */
        lv_style_t * _class = lv_style_list_get_style(list, ci);
        int16_t weight_act = _lv_style_get_int(_class, prop, &value_act);

        /*On perfect match return the value immediately*/
        if(weight_act == weight_goal) {
            *res = value_act;
            return LV_RES_OK;
        }
        else if(list->has_trans && weight_act >= 0 && ci == 0 && !list->skip_trans) {
            *res = value_act;
            return LV_RES_OK;
        }
        /*If the found ID is better the current candidate then use it*/
        else if(weight_act > weight) {
            weight =  weight_act;
            *res = value_act;
        }
    }

    if(weight >= 0) return LV_RES_OK;
    else return LV_RES_INV;

}

/**
 * Get a color typed property from a style list.
 * It will return the property which match best with given state.
 * @param list pointer to a style list from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result
 * @return LV_RES_OK: there was a matching property in the list
 *         LV_RES_INV: there was NO matching property in the list
 * @note for performance reasons it's not checked if the property really has color type
 */
lv_res_t _lv_style_list_get_color(lv_style_list_t * list, lv_style_property_t prop, lv_color_t * res)
{
    LV_ASSERT_STYLE_LIST(list);

    if(list == NULL) return LV_RES_INV;
    if(list->style_list == NULL) return LV_RES_INV;

    lv_style_attr_t attr;
    attr = prop >> 8;
    int16_t weight_goal = attr;

    int16_t weight = -1;

    lv_color_t value_act;
    value_act.full = 0;

    int16_t ci;
    for(ci = 0; ci < list->style_cnt; ci++) {
        lv_style_t * _class = lv_style_list_get_style(list, ci);
        int16_t weight_act = _lv_style_get_color(_class, prop, &value_act);
        /*On perfect match return the value immediately*/
        if(weight_act == weight_goal) {
            *res = value_act;
            return LV_RES_OK;
        }
        else if(list->has_trans && weight_act >= 0 && ci == 0 && !list->skip_trans) {
            *res = value_act;
            return LV_RES_OK;
        }
        /*If the found ID is better the current candidate then use it*/
        else if(weight_act > weight) {
            weight =  weight_act;
            *res = value_act;
        }
    }

    if(weight >= 0)  return LV_RES_OK;
    else return LV_RES_INV;
}

/**
 * Get an opacity typed property from a style list.
 * It will return the property which match best with given state.
 * @param list pointer to a style list from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA| (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result
 * @return LV_RES_OK: there was a matching property in the list
 *         LV_RES_INV: there was NO matching property in the list
 * @note for performance reasons it's not checked if the property really has opacity type
 */
lv_res_t _lv_style_list_get_opa(lv_style_list_t * list, lv_style_property_t prop, lv_opa_t * res)
{
    LV_ASSERT_STYLE_LIST(list);

    if(list == NULL) return LV_RES_INV;
    if(list->style_list == NULL) return LV_RES_INV;

    lv_style_attr_t attr;
    attr = prop >> 8;
    int16_t weight_goal = attr;

    int16_t weight = -1;

    lv_opa_t value_act = LV_OPA_TRANSP;

    int16_t ci;
    for(ci = 0; ci < list->style_cnt; ci++) {
        lv_style_t * _class = lv_style_list_get_style(list, ci);
        int16_t weight_act = _lv_style_get_opa(_class, prop, &value_act);
        /*On perfect match return the value immediately*/
        if(weight_act == weight_goal) {
            *res = value_act;
            return LV_RES_OK;
        }
        else if(list->has_trans && weight_act >= 0 && ci == 0 && !list->skip_trans) {
            *res = value_act;
            return LV_RES_OK;
        }
        /*If the found ID is better the current candidate then use it*/
        else if(weight_act > weight) {
            weight =  weight_act;
            *res = value_act;
        }
    }

    if(weight >= 0)  return LV_RES_OK;
    else return LV_RES_INV;
}

/**
 * Get a pointer typed property from a style list.
 * It will return the property which match best with given state.
 * @param list pointer to a style list from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result
 * @return LV_RES_OK: there was a matching property in the list
 *         LV_RES_INV: there was NO matching property in the list
 * @note for performance reasons it's not checked if the property really has pointer type
 */
lv_res_t _lv_style_list_get_ptr(lv_style_list_t * list, lv_style_property_t prop, const void ** res)
{
    LV_ASSERT_STYLE_LIST(list);

    if(list == NULL) return LV_RES_INV;
    if(list->style_list == NULL) return LV_RES_INV;

    lv_style_attr_t attr;
    attr = prop >> 8;
    int16_t weight_goal = attr;

    int16_t weight = -1;

    const void * value_act;

    int16_t ci;
    for(ci = 0; ci < list->style_cnt; ci++) {
        lv_style_t * _class = lv_style_list_get_style(list, ci);
        int16_t weight_act = _lv_style_get_ptr(_class, prop, &value_act);
        /*On perfect match return the value immediately*/
        if(weight_act == weight_goal) {
            *res = value_act;
            return LV_RES_OK;
        }
        else if(list->has_trans && weight_act >= 0 && ci == 0 && !list->skip_trans) {
            *res = value_act;
            return LV_RES_OK;
        }
        /*If the found ID is better the current candidate then use it*/
        else if(weight_act > weight) {
            weight =  weight_act;
            *res = value_act;
        }
    }

    if(weight >= 0)  return LV_RES_OK;
    else return LV_RES_INV;
}

/**
 * Check whether a style is valid (initialized correctly)
 * @param style pointer to a style
 * @return true: valid
 */
bool lv_debug_check_style(const lv_style_t * style)
{
    if(style == NULL) return true;  /*NULL style is still valid*/

#if LV_USE_ASSERT_STYLE
    if(style->sentinel != LV_DEBUG_STYLE_SENTINEL_VALUE) {
        LV_LOG_WARN("Invalid style (local variable or not initialized?)");
        return false;
    }
#endif

    return true;
}

/**
 * Check whether a style list is valid (initialized correctly)
 * @param style pointer to a style
 * @return true: valid
 */
bool lv_debug_check_style_list(const lv_style_list_t * list)
{
    if(list == NULL) return true;  /*NULL list is still valid*/

#if LV_USE_ASSERT_STYLE
    if(list->sentinel != LV_DEBUG_STYLE_LIST_SENTINEL_VALUE) {
        LV_LOG_WARN("Invalid style (local variable or not initialized?)");
        return false;
    }
#endif

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get a property's index (byte index in `style->map`) from a style.
 * Return best matching property's index considering the state of `prop`
 * @param style pointer to a style
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @return
 */
LV_ATTRIBUTE_FAST_MEM static inline int32_t get_property_index(const lv_style_t * style, lv_style_property_t prop)
{
    LV_ASSERT_STYLE(style);

    if(style->map == NULL) return -1;

    uint8_t id_to_find = prop & 0xFF;
    lv_style_attr_t attr;
    attr = (prop >> 8) & 0xFF;

    int16_t weight = -1;
    int16_t id_guess = -1;

    size_t i = 0;

    uint8_t prop_id;
    while((prop_id = get_style_prop_id(style, i)) != _LV_STYLE_CLOSING_PROP) {
        if(prop_id == id_to_find) {
            lv_style_attr_t attr_i;
            attr_i = get_style_prop_attr(style, i);

            /*If the state perfectly matches return this property*/
            if(LV_STYLE_ATTR_GET_STATE(attr_i) == LV_STYLE_ATTR_GET_STATE(attr)) {
                return i;
            }
            /* Be sure the property not specifies other state than the requested.
             * E.g. For HOVER+PRESS, HOVER only is OK, but HOVER+FOCUS not*/
            else if((LV_STYLE_ATTR_GET_STATE(attr_i) & (~LV_STYLE_ATTR_GET_STATE(attr))) == 0) {
                /* Use this property if it describes better the requested state than the current candidate.
                 * E.g. for HOVER+FOCUS+PRESS prefer HOVER+FOCUS over FOCUS*/
                if(LV_STYLE_ATTR_GET_STATE(attr_i) > weight) {
                    weight = LV_STYLE_ATTR_GET_STATE(attr_i);
                    id_guess = i;
                }
            }
        }

        i = get_next_prop_index(prop_id, i);
    }

    return id_guess;
}

/**
 * Get he local style from a style list. Allocate it if not exists yet.
 * @param list pointer to a style list
 * @return pointer to the local style
 */
static lv_style_t * get_alloc_local_style(lv_style_list_t * list)
{
    LV_ASSERT_STYLE_LIST(list);

    if(list->has_local) return lv_style_list_get_style(list, list->has_trans ? 1 : 0);

    lv_style_t * local_style = lv_mem_alloc(sizeof(lv_style_t));
    LV_ASSERT_MEM(local_style);
    if(local_style == NULL) {
        LV_LOG_WARN("get_local_style: couldn't create local style");
        return NULL;
    }
    lv_style_init(local_style);

    /*Add the local style to the first place*/
    _lv_style_list_add_style(list, local_style);
    list->has_local = 1;

    return local_style;
}

/**
 * Resizes a style map. Useful entry point for debugging.
 * @param style pointer to the style to be resized.
 * @param size new size
 */
static inline void style_resize(lv_style_t * style, size_t sz)
{
    style->map = lv_mem_realloc(style->map, sz);
}

/**
 * Get style property in index.
 * @param style pointer to style.
 * @param idx index of the style in style->map
 * @return property in style->map + idx
 */
static inline lv_style_property_t get_style_prop(const lv_style_t * style, size_t idx)
{
    lv_style_property_t prop;
    uint8_t * prop_p = (uint8_t *)&prop;
    prop_p[0] = style->map[idx];
    prop_p[1] = style->map[idx + 1];
    return prop;
}

/**
 * Get style property id in index.
 * @param style pointer to style.
 * @param idx index of the style in style->map
 * @return id of property in style->map + idx
 */
static inline uint8_t get_style_prop_id(const lv_style_t * style, size_t idx)
{
    return get_style_prop(style, idx) & 0xFF;
}

/**
 * Get style property attributes for index.
 * @param style pointer to style.
 * @param idx index of the style in style->map
 * @return attribute of property in style->map + idx
 */
static inline uint8_t get_style_prop_attr(const lv_style_t * style, size_t idx)
{
    return ((get_style_prop(style, idx) >> 8) & 0xFFU);
}


/**
 * Get property size.
 * @param prop_id property id.
 * @param idx index of the style in style->map
 * @return attribute of property in style->map + idx
 */
static inline size_t get_prop_size(uint8_t prop_id)
{
    prop_id &= 0xF;
    size_t size = sizeof(lv_style_property_t);
    if(prop_id < LV_STYLE_ID_COLOR) size += sizeof(lv_style_int_t);
    else if(prop_id < LV_STYLE_ID_OPA) size += sizeof(lv_color_t);
    else if(prop_id < LV_STYLE_ID_PTR) size += sizeof(lv_opa_t);
    else size += sizeof(const void *);
    return size;
}

/**
 * Get next property index, given current property and index.
 * @param prop_id property id.
 * @param idx index of the style in style->map
 * @return index of next property in style->map
 */
static inline size_t get_next_prop_index(uint8_t prop_id, size_t idx)
{
    return idx + get_prop_size(prop_id);
}
