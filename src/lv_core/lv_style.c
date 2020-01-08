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
#if LV_USE_ANIMATION
static void style_animator(lv_style_anim_dsc_t * dsc, lv_anim_value_t val);
static void style_animation_common_end_cb(lv_anim_t * a);
#endif

/**********************
 *  GLOABAL VARIABLES
 **********************/
lv_style_t lv_style_transp_tight;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_style_built_in_init(void)
{
    lv_style_init(&lv_style_transp_tight);
    lv_style_set_opa(&lv_style_transp_tight, LV_STYLE_BG_OPA, LV_OPA_TRANSP);
    lv_style_set_opa(&lv_style_transp_tight, LV_STYLE_BORDER_OPA, LV_OPA_TRANSP);
    lv_style_set_opa(&lv_style_transp_tight, LV_STYLE_SHADOW_OPA, LV_OPA_TRANSP);
    lv_style_set_opa(&lv_style_transp_tight, LV_STYLE_PATTERN_OPA, LV_OPA_TRANSP);
    lv_style_set_int(&lv_style_transp_tight, LV_STYLE_PAD_LEFT, 0);
    lv_style_set_int(&lv_style_transp_tight, LV_STYLE_PAD_RIGHT, 0);
    lv_style_set_int(&lv_style_transp_tight, LV_STYLE_PAD_TOP, 0);
    lv_style_set_int(&lv_style_transp_tight, LV_STYLE_PAD_BOTTOM, 0);
    lv_style_set_int(&lv_style_transp_tight, LV_STYLE_PAD_INNER, 0);
}

void lv_style_init(lv_style_t * style)
{
    style->map = NULL;
    style->size = 0;
}

void lv_style_copy(lv_style_t * style_dest, const lv_style_t * style_src)
{
    lv_style_init(style_dest);
    style_dest->map = lv_mem_alloc(style_src->size);
    memcpy(style_dest->map, style_src->map, style_src->size);
    style_dest->size = style_src->size;
}

void lv_style_dsc_init(lv_style_dsc_t * style_dsc)
{
    lv_style_init(&style_dsc->local);
    style_dsc->classes = NULL;
    style_dsc->class_cnt = 0;
    memset(&style_dsc->cache, 0xff, sizeof(lv_style_cache_t));
    style_dsc->cache.enabled = 1;
}


void lv_style_dsc_add_class(lv_style_dsc_t * dsc, lv_style_t * class)
{
    /* Do not allocate memory for the first class.
     * It can be simply stored as a pointer.*/
    if(dsc->class_cnt == 0) {
        dsc->classes = (lv_style_t**)class;
        dsc->class_cnt = 1;
    } else {

        lv_style_t ** new_classes;
        if(dsc->class_cnt == 1) new_classes = lv_mem_alloc(sizeof(lv_style_t *) * (dsc->class_cnt + 1));
        else new_classes = lv_mem_realloc(dsc->classes, sizeof(lv_style_t *) * (dsc->class_cnt + 1));
        LV_ASSERT_MEM(new_classes);
        if(new_classes == NULL) {
            LV_LOG_WARN("lv_style_dsc_add_class: couldn't add the class");
            return;
        }

        if(dsc->class_cnt == 1) new_classes[0] = (lv_style_t*)dsc->classes;
        new_classes[dsc->class_cnt] = class;

        dsc->class_cnt++;
        dsc->classes = new_classes;
    }

    lv_style_cache_update(dsc);
}

void lv_style_dsc_remove_class(lv_style_dsc_t * dsc, lv_style_t * class)
{
    if(dsc->class_cnt == 0) return;
    if(dsc->class_cnt == 1) {
        if((lv_style_t*)dsc->classes == class) {
            dsc->classes = NULL;
            dsc->class_cnt = 0;
        }
    } else {
        lv_style_t ** new_classes = lv_mem_realloc(dsc->classes, sizeof(lv_style_t *) * (dsc->class_cnt - 1));
        LV_ASSERT_MEM(new_classes);
        if(new_classes == NULL) {
            LV_LOG_WARN("lv_style_dsc_remove_class: couldn't remove the class");
            return;
        }
        uint8_t i,j;
        for(i = 0, j = 0; i < dsc->class_cnt; i++) {
            if(dsc->classes[i] == class) continue;
            new_classes[j] = dsc->classes[i];
            j++;

        }

        dsc->class_cnt--;
        dsc->classes = new_classes;
    }

    lv_style_cache_update(dsc);
}

void lv_style_dsc_reset(lv_style_dsc_t * style_dsc)
{
    if(style_dsc->class_cnt > 1) lv_mem_free(style_dsc->classes);
    style_dsc->classes = NULL;
    style_dsc->class_cnt = 0;
    lv_style_reset(&style_dsc->local);
    memset(&style_dsc->cache, 0xff, sizeof(lv_style_cache_t));
}


void lv_style_reset(lv_style_t * style)
{
    lv_mem_free(style->map);
    style->map = NULL;
    style->size = 0;
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
    style->size += sizeof(lv_style_property_t) + sizeof(lv_style_int_t);
    style->map = lv_mem_realloc(style->map, style->size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    memcpy(style->map + style->size - (sizeof(lv_style_property_t) + sizeof(lv_style_int_t)), &prop, sizeof(lv_style_property_t));
    memcpy(style->map + style->size - sizeof(lv_style_int_t), &value, sizeof(lv_style_int_t));
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
    style->size += sizeof(lv_style_property_t) + sizeof(lv_color_t);
    style->map = lv_mem_realloc(style->map, style->size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    memcpy(style->map + style->size - (sizeof(lv_style_property_t) + sizeof(lv_color_t)), &prop, sizeof(lv_style_property_t));
    memcpy(style->map + style->size - sizeof(lv_color_t), &color, sizeof(lv_color_t));
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
    style->size += sizeof(lv_style_property_t) + sizeof(lv_opa_t);
    style->map = lv_mem_realloc(style->map, style->size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    memcpy(style->map + style->size - (sizeof(lv_style_property_t) + sizeof(lv_opa_t)), &prop, sizeof(lv_style_property_t));
    memcpy(style->map + style->size - sizeof(lv_opa_t), &opa, sizeof(lv_opa_t));
}

void lv_style_set_ptr(lv_style_t * style, lv_style_property_t prop, void * p)
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
    style->size += sizeof(lv_style_property_t) + sizeof(void *);
    style->map = lv_mem_realloc(style->map, style->size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    memcpy(style->map + style->size - (sizeof(lv_style_property_t) + sizeof(void *)), &prop, sizeof(lv_style_property_t));
    memcpy(style->map + style->size - sizeof(void *), &p, sizeof(void *));
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
uint32_t prop_fooled[256];

lv_res_t lv_style_dsc_get_int(lv_style_dsc_t * dsc, lv_style_property_t prop, lv_style_int_t * value)
{
    if(dsc == NULL) return LV_RES_INV;

    lv_res_t res = LV_RES_OK;

    if(dsc->cache.enabled) {
        switch(prop & (~LV_STYLE_STATE_MASK)) {
        case LV_STYLE_BG_BLEND_MODE:
            res = dsc->cache.bg_blend_mode;
            break;
        case LV_STYLE_BORDER_BLEND_MODE:
            res = dsc->cache.border_blend_mode;
            break;
        case LV_STYLE_IMAGE_BLEND_MODE:
            res = dsc->cache.image_blend_mode;
            break;
        case LV_STYLE_TEXT_BLEND_MODE:
            res = dsc->cache.text_blend_mode;
            break;
        case LV_STYLE_LINE_BLEND_MODE:
            res = dsc->cache.line_blend_mode;
            break;
        case LV_STYLE_SHADOW_BLEND_MODE:
            res = dsc->cache.shadow_blend_mode;
            break;
        case LV_STYLE_PATTERN_BLEND_MODE:
            res = dsc->cache.pattern_blend_mode;
            break;
        case LV_STYLE_CLIP_CORNER:
            res = dsc->cache.clip_corner;
            break;
        case LV_STYLE_LETTER_SPACE:
            res = dsc->cache.letter_space;
            break;
        case LV_STYLE_LINE_SPACE:
            res = dsc->cache.line_space;
            break;
        case LV_STYLE_BORDER_PART:
            res = dsc->cache.border_part;
            break;
        case LV_STYLE_BORDER_WIDTH:
            res = dsc->cache.border_width;
            break;
        case LV_STYLE_SHADOW_WIDTH:
            res = dsc->cache.shadow_width;
            break;
        }
    }

    if(res == LV_RES_INV) return LV_RES_INV;

    lv_style_attr_t attr;
    attr.full = prop >> 8;
    int16_t weight_goal = attr.full;

    int16_t weight_act;
    int16_t weight = -1;

    lv_style_int_t value_act;
    weight_act = lv_style_get_int(&dsc->local, prop, &value_act);

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

    int16_t ci;
    for(ci = dsc->class_cnt - 1; ci >= 0; ci--) {
        lv_style_t * class = lv_style_dsc_get_class(dsc, ci);
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

    if(weight >= 0) {
        prop_fooled[prop&0xFF]++;
        return LV_RES_OK;
    }
    else return LV_RES_INV;

}


lv_res_t lv_style_dsc_get_color(lv_style_dsc_t * dsc, lv_style_property_t prop, lv_color_t * value)
{
    if(dsc == NULL) return LV_RES_INV;

    lv_res_t res = LV_RES_OK;

    if(dsc->cache.enabled) {
        switch(prop & (~LV_STYLE_STATE_MASK)) {
        case LV_STYLE_TEXT_COLOR:
            res = dsc->cache.text_color;
            break;
        }
    }

    if(res == LV_RES_INV) return LV_RES_INV;

    lv_style_attr_t attr;
    attr.full = prop >> 8;
    int16_t weight_goal = attr.full;

    int16_t weight_act;
    int16_t weight = -1;

    lv_color_t value_act;
    weight_act = lv_style_get_color(&dsc->local, prop, &value_act);

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

    int16_t ci;
    for(ci = dsc->class_cnt - 1; ci >= 0; ci--) {
        lv_style_t * class = lv_style_dsc_get_class(dsc, ci);
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

    if(weight >= 0) {
        prop_fooled[prop&0xFF]++;
        return LV_RES_OK;
    }
    else return LV_RES_INV;
}



lv_res_t lv_style_dsc_get_opa(lv_style_dsc_t * dsc, lv_style_property_t prop, lv_opa_t * value)
{
    if(dsc == NULL) return LV_RES_INV;

    lv_res_t res = LV_RES_OK;

    if(dsc->cache.enabled) {
        switch(prop & (~LV_STYLE_STATE_MASK)) {
        case LV_STYLE_OPA_SCALE:
            res = dsc->cache.opa_scale;
            break;
        case LV_STYLE_BG_OPA:
            res = dsc->cache.bg_opa;
            break;
        case LV_STYLE_BORDER_OPA:
            res = dsc->cache.border_opa;
            break;
        case LV_STYLE_IMAGE_OPA:
            res = dsc->cache.image_opa;
            break;
        case LV_STYLE_IMAGE_RECOLOR:
            res = dsc->cache.image_recolor_opa;
            break;
        case LV_STYLE_TEXT_OPA:
            res = dsc->cache.text_opa;
            break;
        case LV_STYLE_LINE_OPA:
            res = dsc->cache.line_opa;
            break;
        case LV_STYLE_SHADOW_OPA:
            res = dsc->cache.shadow_opa;
            break;
        case LV_STYLE_OVERLAY_OPA:
            res = dsc->cache.overlay_opa;
            break;
        case LV_STYLE_PATTERN_OPA:
            res = dsc->cache.pattern_opa;
            break;
        }
    }

    if(res == LV_RES_INV) return LV_RES_INV;

    lv_style_attr_t attr;
    attr.full = prop >> 8;
    int16_t weight_goal = attr.full;

    int16_t weight_act;
    int16_t weight = -1;

    lv_opa_t value_act;
    weight_act = lv_style_get_opa(&dsc->local, prop, &value_act);

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

    int16_t ci;
    for(ci = dsc->class_cnt - 1; ci >= 0; ci--) {
        lv_style_t * class = lv_style_dsc_get_class(dsc, ci);
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

    if(weight >= 0) {
        prop_fooled[prop&0xFF]++;
        return LV_RES_OK;
    }
    else return LV_RES_INV;
}


lv_res_t lv_style_dsc_get_ptr(lv_style_dsc_t * dsc, lv_style_property_t prop, void ** value)
{
    if(dsc == NULL) return LV_RES_INV;

    lv_res_t res = LV_RES_OK;

    if(dsc->cache.enabled) {
        switch(prop & (~LV_STYLE_STATE_MASK)) {
        case LV_STYLE_PATTERN_IMAGE:
            res = dsc->cache.pattern_image;
            break;
        case LV_STYLE_FONT:
            res = dsc->cache.font;
            break;
        }
    }

    if(res == LV_RES_INV) return LV_RES_INV;

    lv_style_attr_t attr;
    attr.full = prop >> 8;
    int16_t weight_goal = attr.full;

    int16_t weight_act;
    int16_t weight = -1;

    void * value_act;
    weight_act = lv_style_get_ptr(&dsc->local, prop, &value_act);

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

    int16_t ci;
    for(ci = dsc->class_cnt - 1; ci >= 0; ci--) {
        lv_style_t * class = lv_style_dsc_get_class(dsc, ci);
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

    if(weight >= 0) {
        prop_fooled[prop&0xFF]++;
        return LV_RES_OK;
    }
    else return LV_RES_INV;
}


lv_res_t lv_style_cache_update(lv_style_dsc_t * dsc)
{
    if(dsc == NULL) return LV_RES_INV;

    if(!dsc->cache.enabled) return LV_RES_OK;
    dsc->cache.enabled = 0;


    lv_style_int_t value;
    lv_opa_t opa;
    void * ptr;
    lv_color_t color;

    dsc->cache.bg_blend_mode = lv_style_dsc_get_int(dsc, LV_STYLE_BG_BLEND_MODE | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.border_blend_mode = lv_style_dsc_get_int(dsc, LV_STYLE_BORDER_BLEND_MODE | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.image_blend_mode = lv_style_dsc_get_int(dsc, LV_STYLE_IMAGE_BLEND_MODE | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.text_blend_mode = lv_style_dsc_get_int(dsc, LV_STYLE_TEXT_BLEND_MODE | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.line_blend_mode = lv_style_dsc_get_int(dsc, LV_STYLE_LINE_BLEND_MODE | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.shadow_blend_mode = lv_style_dsc_get_int(dsc, LV_STYLE_SHADOW_BLEND_MODE | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.pattern_blend_mode = lv_style_dsc_get_int(dsc, LV_STYLE_PATTERN_BLEND_MODE | LV_STYLE_STATE_ALL, &value) & 0x1U;

    dsc->cache.clip_corner = lv_style_dsc_get_int(dsc, LV_STYLE_PATTERN_BLEND_MODE | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.letter_space = lv_style_dsc_get_int(dsc, LV_STYLE_LETTER_SPACE | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.line_space = lv_style_dsc_get_int(dsc, LV_STYLE_LINE_SPACE | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.border_part = lv_style_dsc_get_int(dsc, LV_STYLE_BORDER_PART  | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.border_width = lv_style_dsc_get_int(dsc, LV_STYLE_BORDER_WIDTH | LV_STYLE_STATE_ALL, &value) & 0x1U;
    dsc->cache.shadow_width = lv_style_dsc_get_int(dsc, LV_STYLE_SHADOW_WIDTH | LV_STYLE_STATE_ALL, &value) & 0x1U;


    dsc->cache.opa_scale = lv_style_dsc_get_opa(dsc, LV_STYLE_OPA_SCALE | LV_STYLE_STATE_ALL, &opa) & 0x1U;
    dsc->cache.bg_opa = lv_style_dsc_get_opa(dsc, LV_STYLE_BG_OPA | LV_STYLE_STATE_ALL, &opa) & 0x1U;
    dsc->cache.border_opa = lv_style_dsc_get_opa(dsc, LV_STYLE_BORDER_OPA | LV_STYLE_STATE_ALL, &opa) & 0x1U;
    dsc->cache.image_opa = lv_style_dsc_get_opa(dsc, LV_STYLE_IMAGE_OPA | LV_STYLE_STATE_ALL, &opa) & 0x1U;
    dsc->cache.image_recolor_opa = lv_style_dsc_get_opa(dsc, LV_STYLE_IMAGE_RECOLOR_OPA | LV_STYLE_STATE_ALL, &opa) & 0x1U;
    dsc->cache.text_opa = lv_style_dsc_get_opa(dsc, LV_STYLE_TEXT_OPA | LV_STYLE_STATE_ALL, &opa) & 0x1U;
    dsc->cache.line_opa = lv_style_dsc_get_opa(dsc, LV_STYLE_LINE_OPA | LV_STYLE_STATE_ALL, &opa) & 0x1U;
    dsc->cache.shadow_opa = lv_style_dsc_get_opa(dsc, LV_STYLE_SHADOW_OPA | LV_STYLE_STATE_ALL, &opa) & 0x1U;
    dsc->cache.overlay_opa = lv_style_dsc_get_opa(dsc, LV_STYLE_OVERLAY_OPA | LV_STYLE_STATE_ALL, &opa) & 0x1U;
    dsc->cache.pattern_opa = lv_style_dsc_get_opa(dsc, LV_STYLE_PATTERN_OPA | LV_STYLE_STATE_ALL, &opa) & 0x1U;

    dsc->cache.text_color = lv_style_dsc_get_color(dsc, LV_STYLE_TEXT_COLOR | LV_STYLE_STATE_ALL, &color) & 0x1U;

    dsc->cache.font = lv_style_dsc_get_ptr(dsc, LV_STYLE_FONT | LV_STYLE_STATE_ALL, &ptr) & 0x1U;
    dsc->cache.pattern_image = lv_style_dsc_get_ptr(dsc, LV_STYLE_PATTERN_IMAGE | LV_STYLE_STATE_ALL, &ptr) & 0x1U;

    dsc->cache.enabled = 1;

    return LV_RES_OK;
}

#if LV_USE_ANIMATION

void lv_style_anim_init(lv_anim_t * a)
{
    lv_anim_init(a);
    a->start    = 0;
    a->end      = STYLE_MIX_MAX;
    a->exec_cb  = (lv_anim_exec_xcb_t)style_animator;
    a->path_cb  = lv_anim_path_linear;
    a->ready_cb = style_animation_common_end_cb;

    lv_style_anim_dsc_t * dsc;

    dsc = lv_mem_alloc(sizeof(lv_style_anim_dsc_t));
    LV_ASSERT_MEM(dsc);
    if(dsc == NULL) return;
    dsc->ready_cb   = NULL;
    dsc->style_anim = NULL;
    lv_style_init(&dsc->style_start);
    lv_style_init(&dsc->style_end);

    a->var = (void *)dsc;
}

void lv_style_anim_set_styles(lv_anim_t * a, lv_style_t * to_anim, const lv_style_t * start, const lv_style_t * end)
{
    lv_style_anim_dsc_t * dsc = a->var;
    dsc->style_anim           = to_anim;

    lv_style_copy(&dsc->style_start, start);
    lv_style_copy(&dsc->style_end, end);
}
#endif
/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t cnt = 0;
static uint32_t stat[256];
static inline int32_t get_property_index(const lv_style_t * style, lv_style_property_t prop)
{
    uint8_t id_to_find = prop & 0xFF;
    lv_style_attr_t attr;
    attr.full = (prop >> 8) & 0xFF;

    int16_t weight = -1;
    int16_t id_guess = -1;

    if(id_to_find == (LV_STYLE_RADIUS & 0xFF)) {
        volatile uint8_t i = 0;
    }


    cnt++;
    if(cnt > 100000) {
        cnt = 0;
        uint32_t i;

////        printf("\nQuerry:\n");
//        for(i = 0; i < 256; i++) {
//            if(stat[i]) printf("%02x: %d\n", i, stat[i]);
//        }
//        memset(stat, 0x00, sizeof(stat));
//
////        printf("\nFooled:\n");
//        for(i = 0; i < 256; i++) {
//            if(prop_fooled[i]) printf("%02x: %d\n", i, prop_fooled[i]);
//        }
//        memset(prop_fooled, 0x00, sizeof(stat));
//        printf("\n");
    }
    size_t i = 0;
    while(i < style->size) {

        stat[id_to_find]++;
        lv_style_attr_t attr_act;
        attr_act.full = style->map[i + 1];
        if(style->map[i] == id_to_find) {
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

#if LV_USE_ANIMATION

/**
 * Used by the style animations to set the values of a style according to start and end style.
 * @param dsc the 'animated variable' set by lv_style_anim_create()
 * @param val the current state of the animation between 0 and LV_ANIM_RESOLUTION
 */
static void style_animator(lv_style_anim_dsc_t * dsc, lv_anim_value_t val)
{
    const lv_style_t * start = &dsc->style_start;
    const lv_style_t * end   = &dsc->style_end;
    lv_style_t * act         = dsc->style_anim;

    size_t i = 0;
    lv_style_property_t prop_act;
    while(i < start->size) {
        prop_act = start->map[i] + (start->map[i + 1] << 8);

        /*Value*/
        if((start->map[i] & 0xF) < LV_STYLE_ID_COLOR) {
            lv_style_int_t v1;
            memcpy(&v1, &start->map[i + sizeof(lv_style_property_t)], sizeof(lv_style_int_t));

            int16_t res2;
            lv_style_int_t v2;
            res2 = lv_style_get_int(end, prop_act, &v2);

            if(res2 >= 0) {
                lv_style_int_t vres = v1 + ((int32_t)((int32_t)(v2-v1) * val) >> 8);
                lv_style_set_int(act, prop_act, vres);
            }

            i+= sizeof(lv_style_int_t);
        }
        /*Color*/
        else if((start->map[i] & 0xF) < LV_STYLE_ID_OPA) {
            lv_color_t color1;
            memcpy(&color1, &start->map[i + sizeof(lv_style_property_t)], sizeof(lv_color_t));

            int16_t res2;
            lv_color_t color2;
            res2 = lv_style_get_color(end, prop_act, &color2);

            if(res2 >= 0) {
                lv_color_t color_res = val == 256 ? color2 : lv_color_mix(color2, color1, (lv_opa_t)val);
                lv_style_set_color(act, prop_act, color_res);
            }

            i+= sizeof(lv_color_t);
        }
        /*Opa*/
        else if((start->map[i] & 0xF) < LV_STYLE_ID_PTR) {
            lv_opa_t opa1;
            memcpy(&opa1, &start->map[i + sizeof(lv_style_property_t)], sizeof(lv_opa_t));

            int16_t res2;
            lv_opa_t opa2;
            res2 = lv_style_get_opa(end, prop_act, &opa2);

            if(res2 >= 0) {
                lv_opa_t opa_res = opa1 + ((uint16_t)((uint16_t)(opa2 - opa1) * val) >> 8);
                lv_style_set_opa(act, prop_act, opa_res);
            }

            i+= sizeof(lv_opa_t);
        }
        else {
            void * p1;
            memcpy(p1, &start->map[i + sizeof(lv_style_property_t)], sizeof(void *));

            int16_t res2;
            void * p2;
            res2 = lv_style_get_ptr(end, prop_act, &p2);

            if(res2 >= 0) {
                if(val > 128) lv_style_set_ptr(act, prop_act, p2);
                else if(val > 128) lv_style_set_ptr(act, prop_act, p1);
            }

            i+= sizeof(void*);
        }

        i += sizeof(lv_style_property_t);
    }

    lv_obj_report_style_mod(dsc->style_anim);
}

/**
 * Called when a style animation is ready
 * It called the user defined call back and free the allocated memories
 * @param a pointer to the animation
 */
static void style_animation_common_end_cb(lv_anim_t * a)
{

    (void)a;                            /*Unused*/
    lv_style_anim_dsc_t * dsc = a->var; /*To avoid casting*/

    if(dsc->ready_cb) dsc->ready_cb(a);

    lv_mem_free(dsc);
}

#endif
