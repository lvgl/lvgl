/**
 * @file lv_cb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_cb.h"
#if LV_USE_CB != 0

#include "../lv_core/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_cb"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_cb_signal(lv_obj_t * cb, lv_signal_t sign, void * param);
static lv_style_dsc_t * lv_cb_get_style(lv_obj_t * cb, uint8_t type);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a check box objects
 * @param par pointer to an object, it will be the parent of the new check box
 * @param copy pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
lv_obj_t * lv_cb_create(lv_obj_t * par, const lv_obj_t * copy)
{

    LV_LOG_TRACE("check box create started");

    /*Create the ancestor basic object*/
    lv_obj_t * new_cb = lv_btn_create(par, copy);
    LV_ASSERT_MEM(new_cb);
    if(new_cb == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_cb);

    lv_cb_ext_t * ext = lv_obj_allocate_ext_attr(new_cb, sizeof(lv_cb_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(new_cb);
        return NULL;
    }

    ext->bullet = NULL;
    ext->label  = NULL;

    lv_obj_set_signal_cb(new_cb, lv_cb_signal);

    /*Init the new checkbox object*/
    if(copy == NULL) {
        ext->bullet = lv_obj_create(new_cb, NULL);
        lv_obj_set_click(ext->bullet, false);

        ext->label = lv_label_create(new_cb, NULL);

        lv_cb_set_text(new_cb, "Check box");
        lv_btn_set_layout(new_cb, LV_LAYOUT_ROW_M);
        lv_btn_set_fit(new_cb, LV_FIT_TIGHT);
        lv_btn_set_toggle(new_cb, true);
        lv_obj_set_protect(new_cb, LV_PROTECT_PRESS_LOST);

        lv_obj_reset_style(new_cb, LV_CB_PART_BG);
//        lv_obj_reset_style(new_cb, LV_CB_PART_BULLET);

        _ot(new_cb, LV_CB_PART_BG, CB);
        _ot(new_cb, LV_CB_PART_BULLET, CB_BULLET);

    } else {
        lv_cb_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->bullet            = lv_obj_create(new_cb, copy_ext->bullet);
        ext->label             = lv_label_create(new_cb, copy_ext->label);

        /*Refresh the style with new signal function*/
//        lv_obj_refresh_style(new_cb);
    }

    LV_LOG_INFO("check box created");

    return new_cb;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box. `txt` will be copied and may be deallocated
 * after this function returns.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void lv_cb_set_text(lv_obj_t * cb, const char * txt)
{
    LV_ASSERT_OBJ(cb, LV_OBJX_NAME);

    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    lv_label_set_text(ext->label, txt);
}

/**
 * Set the text of a check box. `txt` must not be deallocated during the life
 * of this checkbox.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void lv_cb_set_static_text(lv_obj_t * cb, const char * txt)
{
    LV_ASSERT_OBJ(cb, LV_OBJX_NAME);

    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    lv_label_set_static_text(ext->label, txt);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * lv_cb_get_text(const lv_obj_t * cb)
{
    LV_ASSERT_OBJ(cb, LV_OBJX_NAME);

    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    return lv_label_get_text(ext->label);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the check box
 * @param cb pointer to a check box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_cb_signal(lv_obj_t * cb, lv_signal_t sign, void * param)
{
    lv_res_t res;
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_cb_get_style(cb, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(cb, sign, param);
        return LV_RES_OK;
    }

    /* Include the ancient signal function */
    res = ancestor_signal(cb, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);

    if(sign == LV_SIGNAL_STYLE_CHG) {
        const lv_font_t * font = lv_obj_get_style_ptr(ext->label, LV_LABEL_PART_MAIN, LV_STYLE_FONT);
        lv_coord_t line_height = lv_font_get_line_height(font);
        lv_obj_set_size(ext->bullet, line_height, line_height);
        ext->bullet->state = cb->state;
        lv_obj_refresh_style(ext->bullet);
    } else if(sign == LV_SIGNAL_PRESSED || sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        lv_btn_set_state(ext->bullet, lv_btn_get_state(cb));
    } else if(sign == LV_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_DOWN || c == LV_KEY_LEFT || c == LV_KEY_UP) {
            /*Follow the backgrounds state with the bullet*/
            lv_btn_set_state(ext->bullet, lv_btn_get_state(cb));
        }
    }

    return res;
}


static lv_style_dsc_t * lv_cb_get_style(lv_obj_t * cb, uint8_t type)
{
    lv_style_dsc_t * style_dsc_p;

    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    switch(type) {
    case LV_CB_PART_BG:
        style_dsc_p = &cb->style_dsc;
        break;
    case LV_CB_PART_BULLET:
        style_dsc_p = lv_obj_get_style(ext->bullet, LV_BTN_PART_MAIN);
        break;
    default:
        style_dsc_p = NULL;
    }

    return style_dsc_p;
}

#endif
