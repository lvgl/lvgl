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

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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
    memset(&style_dsc->cache, 0x00, sizeof(lv_style_cache_t));
    style_dsc->cache.enabled = 1;
}


void lv_style_dsc_add_class(lv_style_dsc_t * style_dsc, lv_style_t * class)
{
    /* Do not allocate memory for the first class.
     * It can be simply stored as a pointer.*/
    if(style_dsc->class_cnt == 0) {
        style_dsc->classes = (lv_style_t**)class;
        style_dsc->class_cnt = 1;
    } else {

        lv_style_t ** new_classes;
        if(style_dsc->class_cnt == 1) new_classes = lv_mem_alloc(sizeof(lv_style_t *) * (style_dsc->class_cnt + 1));
        else new_classes = lv_mem_realloc(style_dsc->classes, sizeof(lv_style_t *) * (style_dsc->class_cnt + 1));
        LV_ASSERT_MEM(new_classes);
        if(new_classes == NULL) {
            LV_LOG_WARN("lv_style_dsc_add_class: couldn't add the class");
            return;
        }

        if(style_dsc->class_cnt == 1) new_classes[0] = (lv_style_t*)style_dsc->classes;
        new_classes[style_dsc->class_cnt] = class;

        style_dsc->class_cnt++;
        style_dsc->classes = new_classes;
    }
}

void lv_style_dsc_remove_class(lv_style_dsc_t * style_dsc, lv_style_t * class)
{
    if(style_dsc->class_cnt == 0) return;
    if(style_dsc->class_cnt == 1) {
        if((lv_style_t*)style_dsc->classes == class) {
            style_dsc->classes = NULL;
            style_dsc->class_cnt = 0;
        }
    } else {
        lv_style_t ** new_classes = lv_mem_realloc(style_dsc->classes, sizeof(lv_style_t *) * (style_dsc->class_cnt - 1));
        LV_ASSERT_MEM(new_classes);
        if(new_classes == NULL) {
            LV_LOG_WARN("lv_style_dsc_remove_class: couldn't remove the class");
            return;
        }
        uint8_t i,j;
        for(i = 0, j = 0; i < style_dsc->class_cnt; i++) {
            if(style_dsc->classes[i] == class) continue;
            new_classes[j] = style_dsc->classes[i];
            j++;

        }

        style_dsc->class_cnt--;
        style_dsc->classes = new_classes;
    }
}

void lv_style_dsc_reset(lv_style_dsc_t * style_dsc)
{
    if(style_dsc->class_cnt > 1) lv_mem_free(style_dsc->classes);
    style_dsc->classes = NULL;
    style_dsc->class_cnt = 0;
    lv_style_reset(&style_dsc->local);
}


void lv_style_reset(lv_style_t * style)
{
    lv_mem_free(style->map);
    style->map = NULL;
    style->size = 0;
}

void lv_style_set_value(lv_style_t * style, lv_style_property_t prop, lv_style_value_t value)
{
    int32_t id = get_property_index(style, prop);
    /*The property already exists but not sure it's state is the same*/
    if(id >= 0) {
        lv_style_attr_t attr_found;
        lv_style_attr_t attr_goal;

        attr_found.full = *(style->map + id + 1);
        attr_goal.full = (prop >> 8) & 0xFFU;

        if(attr_found.bits.state == attr_goal.bits.state) {
            memcpy(style->map + id + sizeof(lv_style_property_t), &value, sizeof(lv_style_value_t));
            return;
        }
    }

    /*Add new property if not exists yet*/
    style->size += sizeof(lv_style_property_t) + sizeof(lv_style_value_t);
    style->map = lv_mem_realloc(style->map, style->size);
    LV_ASSERT_MEM(style->map);
    if(style == NULL) return;

    memcpy(style->map + style->size - (sizeof(lv_style_property_t) + sizeof(lv_style_value_t)), &prop, sizeof(lv_style_property_t));
    memcpy(style->map + style->size - sizeof(lv_style_value_t), &value, sizeof(lv_style_value_t));
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
int16_t lv_style_get_value(const lv_style_t * style, lv_style_property_t prop, lv_style_value_t * res)
{
    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        return -1;
    } else {
        memcpy(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_style_value_t));
        lv_style_attr_t attr_act;
        attr_act.full = style->map[id + 1];

        lv_style_attr_t attr_goal;
        attr_goal.full = (prop >> 8) & 0xFF;

        return attr_act.bits.state & attr_goal.bits.state;
    }
}


int16_t lv_style_get_opa(const lv_style_t * style, lv_style_property_t prop, lv_opa_t * res)
{
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

static inline int32_t get_property_index(const lv_style_t * style, lv_style_property_t prop)
{
    uint8_t id_to_find = prop & 0xFF;
    lv_style_attr_t attr;
    attr.full = (prop >> 8) & 0xFF;

    int16_t weight = -1;
    int16_t id_guess = -1;

    size_t i = 0;
    while(i < style->size) {
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
        if((style->map[i] & 0xF) < LV_STYLE_ID_COLOR) i+= sizeof(lv_style_value_t);
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
            lv_style_value_t v1;
            memcpy(&v1, &start->map[i + sizeof(lv_style_property_t)], sizeof(lv_style_value_t));

            int16_t res2;
            lv_style_value_t v2;
            res2 = lv_style_get_value(end, prop_act, &v2);

            if(res2 >= 0) {
                lv_style_value_t vres = v1 + ((int32_t)((int32_t)(v2-v1) * val) >> 8);
                lv_style_set_value(act, prop_act, vres);
            }

            i+= sizeof(lv_style_value_t);
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
