/**
 * @file encoder_ctrl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "encoder_ctrl.h"
#if USE_LV_EXAMPLE != 0

#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void channel_create(uint8_t ch_id, lv_group_t * g);

static lv_action_res_t mbox_yes_action(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t mbox_no_action(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t enable_action(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t btn_next(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t btn_inc(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t btn_dec(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t btn_sel(lv_obj_t * btn, lv_dispi_t * dispi);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * scr;
static lv_style_t style_mbox_bg;

static lv_group_t * g;    /*Declare an Object Group*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 */
void encoder_ctrl_init(void)
{
    /* Create a Page screen (to make it scrollable)
     * and use Pretty layout to make the content responsive.
     * See the 'responsive' example for more information */
    scr = lv_page_create(NULL, NULL);
    lv_cont_set_layout(lv_page_get_scrl(scr), LV_CONT_LAYOUT_PRETTY);
    lv_page_set_sb_mode(scr, LV_PAGE_SB_MODE_AUTO);
    lv_scr_load(scr);

    g = lv_group_create();

    lv_style_get(LV_STYLE_PLAIN, &style_mbox_bg);
    style_mbox_bg.mcolor = COLOR_BLACK;
    style_mbox_bg.gcolor = COLOR_BLACK;
    style_mbox_bg.opa = OPA_50;

    lv_obj_t * title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "Encoder control");
    lv_obj_set_protect(title, LV_PROTECT_FOLLOW);   /*Make a line break in the layout*/


    /*Create a holder, a subtitle and a drop down list*/
    lv_obj_t * ddlist = lv_ddlist_create(scr, NULL);
    lv_ddlist_set_options_str(ddlist, "Low\nMedium\nHigh");
    lv_group_add_obj(g, ddlist);   /*Add the object to the first group*/

    /*Copy the previous holder and subtitle and add check boxes*/
    lv_obj_t * holder = lv_cont_create(scr, NULL);     /*Create a transparent holder to group some objects*/
    lv_cont_set_fit(holder, true, true);
    lv_cont_set_layout(holder, LV_CONT_LAYOUT_COL_L);
    lv_obj_set_style(holder, lv_style_get(LV_STYLE_TRANSP, NULL));

    lv_obj_t * cb = lv_cb_create(holder, NULL);
    lv_cb_set_text(cb, "Red");
    lv_group_add_obj(g, cb);

    cb = lv_cb_create(holder, cb);
    lv_cb_set_text(cb, "Green");

    cb = lv_cb_create(holder, cb);
    lv_cb_set_text(cb, "Blue");

    /*Copy the previous holder and subtitle and add sliders*/
    lv_obj_t * slider = lv_slider_create(scr, NULL);
    lv_obj_set_size_us(slider, 180, 30);
    lv_group_add_obj(g, slider);

    lv_obj_t * btn = lv_btn_create(scr, NULL);
    lv_btn_set_rel_action(btn, enable_action);
    lv_cont_set_fit(btn, true, true);
    lv_group_add_obj(g, btn);
    lv_obj_t * l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "Enable");

    lv_obj_set_protect(btn, LV_PROTECT_FOLLOW);


    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_rel_action(btn, btn_next);
    lv_cont_set_fit(btn, true, true);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, ">");

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_rel_action(btn, btn_dec);
    lv_cont_set_fit(btn, true, true);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "-");

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_rel_action(btn, btn_inc);
    lv_cont_set_fit(btn, true, true);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "+");

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_rel_action(btn, btn_sel);
    lv_cont_set_fit(btn, true, true);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "!");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_action_res_t enable_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
    if(lv_btn_get_state(btn) == LV_BTN_STATE_REL) {
        lv_obj_t * bg = lv_obj_create(scr, NULL);
        lv_obj_set_protect(bg, LV_PROTECT_PARENT);
        lv_obj_set_parent(bg, scr);
        lv_obj_set_style(bg, &style_mbox_bg);
        lv_obj_set_size(bg, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_pos(bg, 0, 0);
        lv_obj_set_click(bg, false);

        lv_obj_t * mbox = lv_mbox_create(bg, NULL);
        lv_mbox_set_text(mbox, "Really Enable the outputs?");
        lv_group_add_obj(g, mbox);

        lv_mbox_add_btn(mbox, "Yes", mbox_yes_action);
        lv_mbox_add_btn(mbox, "No", mbox_no_action);

        lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, - LV_DPI / 2);

        lv_group_focus_obj(mbox);
        lv_group_focus_freeze(g, true);
    }
    return LV_ACTION_RES_OK;
}

static lv_action_res_t mbox_yes_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_obj_t * mbox = lv_mbox_get_from_btn(btn);
    lv_group_focus_freeze(g, false);
    lv_obj_del(lv_obj_get_parent(mbox));
}

static lv_action_res_t mbox_no_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_obj_t * mbox = lv_mbox_get_from_btn(btn);
    lv_group_focus_freeze(g, false);
    lv_obj_del(lv_obj_get_parent(mbox));
}

static lv_action_res_t btn_next(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_focus_next(g);

    return LV_ACTION_RES_OK;
}



static lv_action_res_t btn_inc(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_send(g, LV_GROUP_KEY_RIGHT);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t btn_dec(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_send(g, LV_GROUP_KEY_LEFT);

    return LV_ACTION_RES_OK;
}

static lv_action_res_t btn_sel(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_send(g, LV_GROUP_KEY_ENTER);

    return LV_ACTION_RES_OK;
}

#endif /*USE_LV_EXAMPLE != 0*/
