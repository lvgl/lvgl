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
 *  STATIC VARIABLES
 **********************/
//lv_style_t lv_style_scr;
//lv_style_t lv_style_transp;
//lv_style_t lv_style_transp_fit;
//lv_style_t lv_style_transp_tight;
//lv_style_t lv_style_plain;
//lv_style_t lv_style_plain_color;
//lv_style_t lv_style_pretty;
//lv_style_t lv_style_pretty_color;
//lv_style_t lv_style_btn_rel;
//lv_style_t lv_style_btn_pr;
//lv_style_t lv_style_btn_tgl_rel;
//lv_style_t lv_style_btn_tgl_pr;
//lv_style_t lv_style_btn_ina;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 *  Init. the built-in styles
 */
void lv_style_built_in_init(void)
{

}

void lv_style_init(lv_style_t * style)
{
    style->map = NULL;
    style->size = 0;
    style->used_groups = 0;
}

void lv_style_dsc_init(lv_style_dsc_t * style_dsc)
{
    lv_style_init(&style_dsc->local);
    style_dsc->classes = NULL;
}

/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void lv_style_copy(lv_style_t * dest, const lv_style_t * src)
{
    memcpy(dest, src, sizeof(lv_style_t));
}

void lv_style_set_value(lv_style_t * style, lv_style_property_t prop, lv_style_value_t value)
{
    int32_t id = get_property_index(style, prop);
    /*If exists update the property*/
    if(id >= 0) {
        memcpy(style->map + id + sizeof(lv_style_property_t), &value, sizeof(lv_style_value_t));
    }
    /*Add new property if not exists yet*/
    else {
        style->size += sizeof(lv_style_property_t) + sizeof(lv_style_value_t);
        style->map = lv_mem_realloc(style->map, style->size);
        LV_ASSERT_MEM(style->map);
        if(style->map == NULL) return;

        memcpy(style->map + style->size - (sizeof(lv_style_property_t) + sizeof(lv_style_value_t)), &prop, sizeof(lv_style_property_t));
        memcpy(style->map + style->size - sizeof(lv_style_value_t), &value, sizeof(lv_style_value_t));
    }
}

void lv_style_set_color(lv_style_t * style, lv_style_property_t prop, lv_color_t color)
{
    int32_t id = get_property_index(style, prop);
    /*If exists update the property*/
    if(id >= 0) {
        memcpy(style->map + id + sizeof(lv_style_property_t), &color, sizeof(lv_color_t));
    }
    /*Add new property if not exists yet*/
    else {
        style->size += sizeof(lv_style_property_t) + sizeof(lv_color_t);
        style->map = lv_mem_realloc(style->map, style->size);
        LV_ASSERT_MEM(style->map);
        if(style == NULL) return;

        memcpy(style->map + style->size - (sizeof(lv_style_property_t) + sizeof(lv_color_t)), &prop, sizeof(lv_style_property_t));
        memcpy(style->map + style->size - sizeof(lv_color_t), &color, sizeof(lv_color_t));
    }
}

void lv_style_set_opa(lv_style_t * style, lv_style_property_t prop, lv_opa_t opa)
{
    int32_t id = get_property_index(style, prop);
    /*If exists update the property*/
    if(id >= 0) {
        memcpy(style->map + id + sizeof(lv_style_property_t), &opa, sizeof(lv_opa_t));
    }
    /*Add new property if not exists yet*/
    else {
        style->size += sizeof(lv_style_property_t) + sizeof(lv_opa_t);
        style->map = lv_mem_realloc(style->map, style->size);
        LV_ASSERT_MEM(style->map);
        if(style == NULL) return;

        memcpy(style->map + style->size - (sizeof(lv_style_property_t) + sizeof(lv_opa_t)), &prop, sizeof(lv_style_property_t));
        memcpy(style->map + style->size - sizeof(lv_opa_t), &opa, sizeof(lv_opa_t));
    }
}


lv_res_t lv_style_get_value(const lv_style_t * style, lv_style_property_t prop, lv_style_value_t * res)
{
    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        res = 0;
        return LV_RES_INV;
    } else {
        memcpy(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_style_value_t));
        return LV_RES_OK;
    }
}


lv_res_t lv_style_get_opa(const lv_style_t * style, lv_style_property_t prop, lv_opa_t * res)
{

    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        res = 0;
        return LV_RES_INV;
    } else {
        memcpy(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_opa_t));
        return LV_RES_OK;
    }
}

lv_res_t lv_style_get_color(const lv_style_t * style, lv_style_property_t prop, lv_color_t * res)
{
    int32_t id = get_property_index(style, prop);
    if(id < 0) {
        res = 0;
        return LV_RES_INV;
    } else {
        memcpy(res, &style->map[id + sizeof(lv_style_property_t)], sizeof(lv_color_t));
        return LV_RES_OK;
    }
}

/**
 * Mix two styles according to a given ratio
 * @param start start style
 * @param end end style
 * @param res store the result style here
 * @param ratio the ratio of mix [0..256]; 0: `start` style; 256: `end` style
 */
void lv_style_mix(const lv_style_t * start, const lv_style_t * end, lv_style_t * res, uint16_t ratio)
{
//    STYLE_ATTR_MIX(body.opa, ratio);
//    STYLE_ATTR_MIX(body.radius, ratio);
//    STYLE_ATTR_MIX(body.border.width, ratio);
//    STYLE_ATTR_MIX(body.border.opa, ratio);
//    STYLE_ATTR_MIX(body.shadow.width, ratio);
//    STYLE_ATTR_MIX(body.shadow.offset.x, ratio);
//    STYLE_ATTR_MIX(body.shadow.offset.y, ratio);
//    STYLE_ATTR_MIX(body.shadow.spread, ratio);
//    STYLE_ATTR_MIX(body.padding.left, ratio);
//    STYLE_ATTR_MIX(body.padding.right, ratio);
//    STYLE_ATTR_MIX(body.padding.top, ratio);
//    STYLE_ATTR_MIX(body.padding.bottom, ratio);
//    STYLE_ATTR_MIX(body.padding.inner, ratio);
//    STYLE_ATTR_MIX(text.line_space, ratio);
//    STYLE_ATTR_MIX(text.letter_space, ratio);
//    STYLE_ATTR_MIX(text.opa, ratio);
//    STYLE_ATTR_MIX(line.width, ratio);
//    STYLE_ATTR_MIX(line.opa, ratio);
//    STYLE_ATTR_MIX(image.intense, ratio);
//    STYLE_ATTR_MIX(image.opa, ratio);
//
//    lv_opa_t opa = ratio == STYLE_MIX_MAX ? LV_OPA_COVER : ratio;
//
//    res->body.main_color   = lv_color_mix(end->body.main_color, start->body.main_color, opa);
//    res->body.grad_color   = lv_color_mix(end->body.grad_color, start->body.grad_color, opa);
//    res->body.border.color = lv_color_mix(end->body.border.color, start->body.border.color, opa);
//    res->body.shadow.color = lv_color_mix(end->body.shadow.color, start->body.shadow.color, opa);
//    res->text.color        = lv_color_mix(end->text.color, start->text.color, opa);
//    res->image.color       = lv_color_mix(end->image.color, start->image.color, opa);
//    res->line.color        = lv_color_mix(end->line.color, start->line.color, opa);
//
//    if(ratio < (STYLE_MIX_MAX >> 1)) {
//        res->body.border.part = start->body.border.part;
//        res->glass            = start->glass;
//        res->text.font        = start->text.font;
//        res->line.rounded     = start->line.rounded;
//    } else {
//        res->body.border.part = end->body.border.part;
//        res->glass            = end->glass;
//        res->text.font        = end->text.font;
//        res->line.rounded     = end->line.rounded;
//    }
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
    lv_style_copy(&dsc->style_start, &lv_style_plain);
    lv_style_copy(&dsc->style_end, &lv_style_plain);

    a->var = (void *)dsc;
}

void lv_style_anim_set_styles(lv_anim_t * a, lv_style_t * to_anim, const lv_style_t * start, const lv_style_t * end)
{

    lv_style_anim_dsc_t * dsc = a->var;
    dsc->style_anim           = to_anim;
    memcpy(&dsc->style_start, start, sizeof(lv_style_t));
    memcpy(&dsc->style_end, end, sizeof(lv_style_t));
    memcpy(dsc->style_anim, start, sizeof(lv_style_t));
}
#endif
/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline int32_t get_property_index(const lv_style_t * style, lv_style_property_t prop)
{
    uint8_t id_to_find = prop & 0xFF;

    size_t i = 0;
    while(i < style->size) {

        if(style->map[i] == id_to_find) {
            return i;
        } else {
            lv_style_attr_t attr;
            attr.full = style->map[i + 1];
            switch(attr.bits.type) {
            case LV_STYLE_ATTR_TYPE_VALUE:
                i+= sizeof(lv_style_value_t);
                break;
            case LV_STYLE_ATTR_TYPE_OPA:
                i+= sizeof(lv_opa_t);
                break;
            case LV_STYLE_ATTR_TYPE_COLOR:
                i+= sizeof(lv_color_t);
                break;
            case LV_STYLE_ATTR_TYPE_PTR:
                i+= sizeof(void*);
                break;
            }

            i += sizeof(lv_style_property_t);
        }
    }

    return -1;
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

    lv_style_mix(start, end, act, val);

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
