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

/*Basic styles*/
lv_style_t lv_style_plain;
lv_style_t lv_style_panel;
lv_style_t lv_style_panel;
lv_style_t lv_style_btn;

/*Color styles*/
lv_style_t lv_style_dark;
lv_style_t lv_style_light;
lv_style_t lv_style_red;
lv_style_t lv_style_green;
lv_style_t lv_style_blue;

/*Transparent styles*/
lv_style_t lv_style_transp;
lv_style_t lv_style_frame;

/*Padding styles*/
lv_style_t lv_style_tight;
lv_style_t lv_style_fit;

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
 *  Init. the built-in styles
 */
void lv_style_built_in_init(void)
{

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n", (int)mon.total_size - mon.free_size,
            mon.used_pct,
            mon.frag_pct,
            (int)mon.free_biggest_size);


    /*Basic styles*/
    lv_style_init(&lv_style_plain);
    lv_style_set_value(&lv_style_plain, LV_STYLE_PAD_LEFT, LV_DPI / 12);
    lv_style_set_value(&lv_style_plain, LV_STYLE_PAD_RIGHT, LV_DPI / 12);
    lv_style_set_value(&lv_style_plain, LV_STYLE_PAD_TOP, LV_DPI / 12);
    lv_style_set_value(&lv_style_plain, LV_STYLE_PAD_BOTTOM, LV_DPI / 12);
    lv_style_set_value(&lv_style_plain, LV_STYLE_PAD_INNER, LV_DPI / 16);

    lv_style_init(&lv_style_panel);
    lv_style_set_value(&lv_style_panel, LV_STYLE_PAD_LEFT, LV_DPI / 12);
    lv_style_set_value(&lv_style_panel, LV_STYLE_PAD_RIGHT, LV_DPI / 12);
    lv_style_set_value(&lv_style_panel, LV_STYLE_PAD_TOP, LV_DPI / 12);
    lv_style_set_value(&lv_style_panel, LV_STYLE_PAD_BOTTOM, LV_DPI / 12);
    lv_style_set_value(&lv_style_panel, LV_STYLE_PAD_INNER, LV_DPI / 20);
    lv_style_set_value(&lv_style_panel, LV_STYLE_RADIUS, LV_DPI / 16);
    lv_style_set_value(&lv_style_panel, LV_STYLE_BORDER_WIDTH, LV_DPI / 50 > 0 ? LV_DPI / 50 : 1);
    lv_style_set_color(&lv_style_panel, LV_STYLE_BG_COLOR, LV_COLOR_SILVER);
    lv_style_set_color(&lv_style_panel, LV_STYLE_BG_GRAD_COLOR, LV_COLOR_GRAY);
    lv_style_set_color(&lv_style_panel, LV_STYLE_BORDER_COLOR, LV_COLOR_GRAY);

    lv_style_init(&lv_style_btn);
    lv_style_set_value(&lv_style_btn, LV_STYLE_PAD_LEFT, LV_DPI / 6);
    lv_style_set_value(&lv_style_btn, LV_STYLE_PAD_RIGHT, LV_DPI / 6);
    lv_style_set_value(&lv_style_btn, LV_STYLE_PAD_TOP, LV_DPI / 10);
    lv_style_set_value(&lv_style_btn, LV_STYLE_PAD_BOTTOM, LV_DPI / 10);
    lv_style_set_value(&lv_style_btn, LV_STYLE_PAD_INNER, LV_DPI / 16);
    lv_style_set_value(&lv_style_btn, LV_STYLE_RADIUS, LV_DPI / 20);
    lv_style_set_value(&lv_style_btn, LV_STYLE_BORDER_WIDTH, LV_DPI / 50 > 0 ? LV_DPI / 50 : 1);
    lv_style_set_value(&lv_style_btn, LV_STYLE_BG_GRAD_DIR, LV_GRAD_DIR_VER);
    lv_style_set_color(&lv_style_btn, LV_STYLE_BG_COLOR, LV_COLOR_BLUE);
    lv_style_set_color(&lv_style_btn, LV_STYLE_BG_GRAD_COLOR, LV_COLOR_NAVY);
    lv_style_set_color(&lv_style_btn, LV_STYLE_BORDER_COLOR, LV_COLOR_NAVY);
    lv_style_set_color(&lv_style_btn, LV_STYLE_BG_GRAD_COLOR | LV_STYLE_STATE_PRESSED, LV_COLOR_BLACK);
    lv_style_set_color(&lv_style_btn, LV_STYLE_TEXT_COLOR, LV_COLOR_WHITE);

    /*Color styles*/
    lv_style_init(&lv_style_light);
    lv_style_set_color(&lv_style_light, LV_STYLE_BG_COLOR, LV_COLOR_SILVER);
    lv_style_set_color(&lv_style_light, LV_STYLE_BG_GRAD_COLOR, LV_COLOR_GRAY);
    lv_style_set_color(&lv_style_light, LV_STYLE_BORDER_COLOR, LV_COLOR_GRAY);

    lv_style_init(&lv_style_dark);
    lv_style_set_color(&lv_style_dark, LV_STYLE_BG_COLOR, lv_color_hex3(0x444));
    lv_style_set_color(&lv_style_dark, LV_STYLE_BG_GRAD_COLOR, lv_color_hex3(0x222));
    lv_style_set_color(&lv_style_dark, LV_STYLE_BORDER_COLOR, LV_COLOR_GRAY);
    lv_style_set_color(&lv_style_blue, LV_STYLE_TEXT_COLOR, LV_COLOR_WHITE);

    lv_style_init(&lv_style_red);
    lv_style_set_color(&lv_style_red, LV_STYLE_BG_COLOR, LV_COLOR_RED);
    lv_style_set_color(&lv_style_red, LV_STYLE_BG_GRAD_COLOR, LV_COLOR_MAROON);
    lv_style_set_color(&lv_style_red, LV_STYLE_BORDER_COLOR, LV_COLOR_MAROON);
    lv_style_set_color(&lv_style_blue, LV_STYLE_TEXT_COLOR, LV_COLOR_WHITE);

    lv_style_init(&lv_style_green);
    lv_style_set_color(&lv_style_green, LV_STYLE_BG_COLOR, LV_COLOR_LIME);
    lv_style_set_color(&lv_style_green, LV_STYLE_BG_GRAD_COLOR, LV_COLOR_GREEN);
    lv_style_set_color(&lv_style_green, LV_STYLE_BORDER_COLOR, LV_COLOR_GREEN);
    lv_style_set_color(&lv_style_blue, LV_STYLE_TEXT_COLOR, LV_COLOR_WHITE);

    lv_style_init(&lv_style_blue);
    lv_style_set_color(&lv_style_blue, LV_STYLE_BG_COLOR, LV_COLOR_BLUE);
    lv_style_set_color(&lv_style_blue, LV_STYLE_BG_GRAD_COLOR, LV_COLOR_NAVY);
    lv_style_set_color(&lv_style_blue, LV_STYLE_BORDER_COLOR, LV_COLOR_NAVY);
    lv_style_set_color(&lv_style_blue, LV_STYLE_BG_GRAD_COLOR | LV_STYLE_STATE_PRESSED, LV_COLOR_BLACK);
    lv_style_set_color(&lv_style_blue, LV_STYLE_TEXT_COLOR, LV_COLOR_WHITE);

    /*Transparent styles*/
    lv_style_init(&lv_style_transp);
    lv_style_set_opa(&lv_style_transp, LV_STYLE_BG_OPA, LV_OPA_TRANSP);
    lv_style_set_opa(&lv_style_transp, LV_STYLE_BORDER_OPA, LV_OPA_TRANSP);

    lv_style_init(&lv_style_frame);
    lv_style_set_opa(&lv_style_frame, LV_STYLE_BG_OPA, LV_OPA_TRANSP);

    /*Padding styles*/
    lv_style_init(&lv_style_tight);
    lv_style_set_value(&lv_style_tight, LV_STYLE_PAD_LEFT, 0);
    lv_style_set_value(&lv_style_tight, LV_STYLE_PAD_RIGHT, 0);
    lv_style_set_value(&lv_style_tight, LV_STYLE_PAD_TOP, 0);
    lv_style_set_value(&lv_style_tight, LV_STYLE_PAD_BOTTOM, 0);
    lv_style_set_value(&lv_style_tight, LV_STYLE_PAD_INNER, 0);

    lv_style_init(&lv_style_fit);
    lv_style_set_value(&lv_style_fit, LV_STYLE_PAD_LEFT, 0);
    lv_style_set_value(&lv_style_fit, LV_STYLE_PAD_RIGHT, 0);
    lv_style_set_value(&lv_style_fit, LV_STYLE_PAD_TOP, 0);
    lv_style_set_value(&lv_style_fit, LV_STYLE_PAD_BOTTOM, 0);

    lv_mem_monitor(&mon);
    printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n", (int)mon.total_size - mon.free_size,
            mon.used_pct,
            mon.frag_pct,
            (int)mon.free_biggest_size);
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
    style->used_groups = 0;
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

    /*Set that group is used this style*/
    uint16_t group = (prop >> 4) & 0xF;
    style->used_groups |= 1 << group;
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

    /*Set that group is used this style*/
    uint16_t group = (prop >> 4) & 0xF;
    style->used_groups |= 1 << group;
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

    /*Set that group is used this style*/
    uint16_t group = (prop >> 4) & 0xF;
    style->used_groups |= 1 << group;
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

    /*Set that group is used this style*/
    uint16_t group = (prop >> 4) & 0xF;
    style->used_groups |= 1 << group;
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
    static uint32_t stat[256];


    static uint32_t s = 0;


    uint8_t id_to_find = prop & 0xFF;
    lv_style_attr_t attr;
    attr.full = (prop >> 8) & 0xFF;

    stat[id_to_find]++;

//    if(s > 1000) {
//        printf("\n\n");
//        s = 0;
//        uint32_t i;
//        for(i = 0; i < 256; i++) {
//            if(stat[i] == 0) continue;
//            printf("%02x;%d;\n", i, stat[i]);
//        }
//    }

    int16_t weight = -1;
    int16_t id_guess = -1;


    uint16_t group = (id_to_find >> 4) & 0xF;
    if((style->used_groups & (1 << group)) == 0) return id_guess;

    size_t i = 0;
    while(i < style->size) {
//        s++;

//        printf("style search:%d\n", s);
        lv_style_attr_t attr_act;
        attr_act.full = style->map[i + 1];
        if(style->map[i] == id_to_find) {
            /*If there the state has perfectly match return this property*/
            if(attr_act.bits.state == attr.bits.state) {
                return i;
            }
            /* Be sure the property not specifies other state the the requested.
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
