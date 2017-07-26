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
static lv_action_res_t btn_next(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t btn_inc(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t btn_dec(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t btn_sel(lv_obj_t * btn, lv_dispi_t * dispi);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_group_t * g1;    /*Declare an Object Group*/
lv_group_t * g2;    /*Declare an Object Group*/

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
    /* Create a Container screen and use Pretty layout
     * to make the content responsive.
     * See the 'responsive' example for more information */
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_cont_set_layout(lv_page_get_scrl(scr), LV_CONT_LAYOUT_PRETTY);
    lv_scr_load(scr);

    g1 = lv_group_create();
    g2 = lv_group_create();

    lv_obj_t * title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "Channel 1");
    lv_obj_set_protect(title, LV_PROTECT_FOLLOW);   /*Make a line break in the layout*/


    /*Create a holder, a subtitle and a drop down list*/
    lv_obj_t * holder = lv_cont_create(scr, NULL);     /*Create a transparent holder to group some objects*/
    lv_cont_set_fit(holder, true, true);
    lv_cont_set_layout(holder, LV_CONT_LAYOUT_COL_L);
    lv_obj_set_style(holder, lv_style_get(LV_STYLE_TRANSP_TIGHT, NULL));

    lv_obj_t * subtitle = lv_label_create(holder, NULL);
    lv_label_set_text(subtitle, "Temperature");

    lv_obj_t * ddlist = lv_ddlist_create(holder, NULL);
    lv_ddlist_set_options_str(ddlist, "Low\nMedium\nHigh");
    lv_group_add_obj(g1, ddlist);   /*Add the object to the first group*/

    lv_ddlist_create(holder, ddlist);

    /*Copy the previous holder and subtitle and add check boxes*/
    holder = lv_cont_create(scr, holder);
    subtitle = lv_label_create(holder, subtitle);
    lv_label_set_text(subtitle, "Colors");
    lv_obj_t * cb = lv_cb_create(holder, NULL);
    lv_cb_set_text(cb, "Red");
    lv_group_add_obj(g1, cb);

    cb = lv_cb_create(holder, cb);
    lv_cb_set_text(cb, "Green");

    cb = lv_cb_create(holder, cb);
    lv_cb_set_text(cb, "Blue");

    /*Copy the previous holder and subtitle and add sliders*/
    holder = lv_cont_create(scr, holder);
    subtitle = lv_label_create(holder, subtitle);
    lv_label_set_text(subtitle, "Voltage");
    lv_obj_t * slider = lv_slider_create(holder, NULL);
    lv_obj_set_size_us(slider, 180, 30);

    subtitle = lv_label_create(holder, subtitle);
    lv_label_set_text(subtitle, "Current");
    slider = lv_slider_create(holder, slider);

    lv_obj_t * list = lv_list_create(lv_scr_act(), NULL);
    lv_list_add(list, "", "List1", NULL);
    lv_list_add(list, "", "List2", NULL);
    lv_list_add(list, "", "List3", NULL);
    lv_list_add(list, "", "List4", NULL);
    lv_list_add(list, "", "List5", NULL);
    lv_list_add(list, "", "List6", NULL);
    lv_list_add(list, "", "List7", NULL);
    lv_list_add(list, "", "List8", NULL);
    lv_group_add_obj(g1, list);

    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t * l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "Button1");
    lv_group_add_obj(g1, btn);


    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_tgl(btn, true);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "Button\nToggle");
    lv_group_add_obj(g1, btn);

    btn = lv_btn_create(lv_scr_act(), btn);
    lv_btn_set_tgl(btn, true);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "Button\nToggle\nCopy");


    l = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(l, "Single\nlabel");
    lv_obj_set_protect(l, LV_PROTECT_FOLLOW);
    lv_group_add_obj(g1, l);

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_rel_action(btn, btn_next);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "Next");

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_rel_action(btn, btn_dec);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "Dec");

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_rel_action(btn, btn_inc);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "Inc");

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_rel_action(btn, btn_sel);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "Sel");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_action_res_t btn_next(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_focus_next(g1);

    return LV_ACTION_RES_OK;
}

static lv_action_res_t btn_prev(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_focus_prev(g1);

    return LV_ACTION_RES_OK;
}


static lv_action_res_t btn_inc(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_send(g1, LV_GROUP_KEY_RIGHT);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t btn_dec(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_send(g1, LV_GROUP_KEY_LEFT);

    return LV_ACTION_RES_OK;
}

static lv_action_res_t btn_sel(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_send(g1, LV_GROUP_KEY_ENTER);

    return LV_ACTION_RES_OK;
}

#endif /*USE_LV_EXAMPLE != 0*/
