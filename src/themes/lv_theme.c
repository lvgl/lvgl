/**
 * @file lv_theme.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme_private.h"
#include "../core/lv_obj_private.h"
#include "../core/lv_obj_style_private.h"
#include "../core/lv_obj_class_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void apply_theme(lv_theme_t * th, lv_obj_t * obj);
static void apply_theme_recursion(lv_theme_t * th, lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_theme_t * lv_theme_get_from_obj(lv_obj_t * obj)
{
    if(obj == NULL) return lv_display_get_theme(lv_display_get_default());

    LV_CHECK_OBJ(obj, &lv_obj_class, return NULL);
    return lv_display_get_theme(lv_obj_get_display(obj));
}

lv_theme_t * lv_theme_create(void)
{
    lv_theme_t * theme = lv_zalloc(sizeof(*theme));
    LV_ASSERT_MALLOC(theme);
    return theme;
}
void lv_theme_delete(lv_theme_t * theme)
{
    lv_free(theme);
}

void lv_theme_copy(lv_theme_t * dst, const lv_theme_t * src)
{
    LV_CHECK_ARG(dst != NULL, return);
    LV_CHECK_ARG(src != NULL, return);
    lv_memcpy(dst, src, sizeof(*src));
}

void lv_theme_apply(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);

    lv_theme_t * th = lv_theme_get_from_obj(obj);
    lv_obj_remove_style_all(obj);

    if(th == NULL) return;

    apply_theme_recursion(th, obj);    /*Apply the theme including the base theme(s)*/
}

void lv_theme_set_parent(lv_theme_t * theme, lv_theme_t * parent)
{
    LV_CHECK_ARG(theme != NULL, return);

    theme->parent = parent;
}

void lv_theme_set_apply_cb(lv_theme_t * theme, lv_theme_apply_cb_t apply_cb)
{
    LV_CHECK_ARG(theme != NULL, return);

    theme->apply_cb = apply_cb;
}

void lv_theme_set_user_data(lv_theme_t * theme, void * user_data)
{
    if(theme == NULL) return;
    theme->user_data = user_data;
}

const lv_font_t * lv_theme_get_font_small(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->font_small : LV_FONT_DEFAULT;
}

const lv_font_t * lv_theme_get_font_normal(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->font_normal : LV_FONT_DEFAULT;
}

const lv_font_t * lv_theme_get_font_large(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->font_large : LV_FONT_DEFAULT;
}

lv_color_t lv_theme_get_color_primary(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->color_primary : lv_palette_main(LV_PALETTE_BLUE_GREY);
}

lv_color_t lv_theme_get_color_secondary(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->color_secondary : lv_palette_main(LV_PALETTE_BLUE);
}

void * lv_theme_get_user_data(const lv_theme_t * theme)
{
    if(theme == NULL) return NULL;
    return theme->user_data;
}

#if LV_USE_EXT_DATA
void lv_theme_set_external_data(lv_theme_t * theme, void * user_data, void (* free_cb)(void * data))
{
    LV_CHECK_ARG_MSG(theme != NULL, return, "can't attach external data to a NULL theme");

    theme->ext_data.data = user_data;
    theme->ext_data.free_cb = free_cb;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void apply_theme(lv_theme_t * th, lv_obj_t * obj)
{
    if(th->parent) apply_theme(th->parent, obj);
    if(th->apply_cb) {
        th->apply_cb(th, obj);
        for(uint32_t i = 0; i < obj->style_cnt; i++) {
            obj->styles[i].is_theme = 1;
        }
    }
}

static void apply_theme_recursion(lv_theme_t * th, lv_obj_t * obj)
{
    const lv_obj_class_t * original_class_p = obj->class_p;

    if(obj->class_p->base_class && obj->class_p->theme_inheritable == LV_OBJ_CLASS_THEME_INHERITABLE_TRUE) {
        /*Apply the base class theme in obj*/
        obj->class_p = obj->class_p->base_class;

        /*apply the base first*/
        apply_theme_recursion(th, obj);
    }

    /*Restore the original class*/
    obj->class_p = original_class_p;

    apply_theme(th, obj);
}
