/**
 * @file encoder_ctrl.c
 *
 */

/*
 * Create a simple GUI to demonstrate how to control it with an encoder
 * using 'lv_group'.
 *
 * lv_group:
 * - you can create groups and add object to them
 * - it can be a focused object within a group
 * - the style of the focused object will be automatically modified
 * - different style modifier functions can be applied in each groups
 * - you can focus on the next or previous object (lv_group_focus_next/prev)
 * - letters can be sent to the focused object to do something (lv_group_send):
 *    - LV_GROUP_KEY_RIGHT/UP: increment action in the object
 *    - LV_GROUP_KEY_LEFT/DOWN: decrement action in the object
 *    - LV_GROUP_KEY_ENTER: ok or select action in the object
 *    - LV_GROUP_KEY_ESC: close or back action action in the object
 *    - or any character for example to a text area
 *
 * The encoder is replaced by 4 button on the screen:
 * - [>] Next (lv_group_focus_next): focus on the next object in the group (simulates encoder press)
 * - [+] IncrementNext (LV_GROUP_KEY_RIGHT): increment signal to the object (simulates rotate right)
 * - [-] DecrementNext (LV_GROUP_KEY_LEFT): increment signal to the object (simulates rotate left)
 * - [!] SelectNext (LV_GROUP_KEY_ENTER): Select something (simulates encoder long press or an 'Select' button)
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
static void gui_create(void);
static void enc_create(void);
static lv_action_res_t mbox_yes_action(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t mbox_no_action(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t enable_action(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t enc_next(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t enc_inc(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t enc_dec(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t enc_sel(lv_obj_t * btn, lv_dispi_t * dispi);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * scr;              /*The screen for the demo*/
static lv_obj_t * btn_enable;       /*An enable button*/
static lv_style_t style_mbox_bg;    /*Black bg. style with opacity*/
static lv_group_t * g;              /*An Object Group*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a simple GUI to demonstrate encoder control capability
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

    /*Create an object group for objects to focus*/
    g = lv_group_create();

    /* Create a dark plain style for a message box's background*/
    lv_style_get(LV_STYLE_PLAIN, &style_mbox_bg);
    style_mbox_bg.mcolor = COLOR_BLACK;
    style_mbox_bg.gcolor = COLOR_BLACK;
    style_mbox_bg.opa = OPA_50;

    /*Create a demo GUI*/
    gui_create();

    /*Create virtual encoder*/
    enc_create();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Create a demo GUI
 */
static void gui_create(void)
{
    /*Create a title*/
    lv_obj_t * title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "Encoder control");
    lv_obj_set_protect(title, LV_PROTECT_FOLLOW);   /*Make a line break in the layout*/

    /*Create a drop down list*/
    lv_obj_t * ddlist = lv_ddlist_create(scr, NULL);
    lv_ddlist_set_options_str(ddlist, "Low\nMedium\nHigh");
    lv_group_add_obj(g, ddlist);                    /*Add the object to the group*/

    /*Create a holder an check boxes on it*/
    lv_obj_t * holder = lv_cont_create(scr, NULL);   /*Create a transparent holder*/
    lv_cont_set_fit(holder, true, true);
    lv_cont_set_layout(holder, LV_CONT_LAYOUT_COL_L);
    lv_obj_set_style(holder, lv_style_get(LV_STYLE_TRANSP, NULL));

    lv_obj_t * cb = lv_cb_create(holder, NULL);     /*First check box*/
    lv_cb_set_text(cb, "Red");
    lv_group_add_obj(g, cb);                        /*Add to the group*/

    cb = lv_cb_create(holder, cb);                  /*Copy the first check box. Automatically added to the same group*/
    lv_cb_set_text(cb, "Green");

    cb = lv_cb_create(holder, cb);                  /*Copy the second check box. Automatically added to the same group*/
    lv_cb_set_text(cb, "Blue");

    /*Create a sliders*/
    lv_obj_t * slider = lv_slider_create(scr, NULL);
    lv_obj_set_size_us(slider, 180, 30);
    lv_bar_set_range(slider, 0, 20);
    lv_group_add_obj(g, slider);                    /*Add to the group*/

    /*Create a button*/
    btn_enable = lv_btn_create(scr, NULL);
    lv_btn_set_rel_action(btn_enable, enable_action);
    lv_cont_set_fit(btn_enable, true, true);
    lv_group_add_obj(g, btn_enable);                /*Add to the group*/
    lv_obj_t * l = lv_label_create(btn_enable, NULL);
    lv_label_set_text(l, "Enable");
    lv_obj_set_protect(btn_enable, LV_PROTECT_FOLLOW); /*Make a line break in the layout*/
}

/**
 * Create virtual encoder using 4 buttons:
 * - [>] Next: focus on the next object in the group (simulates encoder press)
 * - [+] Increment: increment signal to the object (simulates rotate right)
 * - [-] Decrement: increment signal to the object (simulates rotate left)
 * - [!] Select: Select something (simulates encoder long press or an 'Select' button)
 */
static void enc_create(void)
{
    /*Next button*/
    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_rel_action(btn, enc_next);
    lv_cont_set_fit(btn, true, true);
    lv_obj_t * l = lv_label_create(btn, NULL);
    lv_label_set_text(l, ">");

    /*Increment button*/
    btn = lv_btn_create(lv_scr_act(), btn);
    lv_btn_set_rel_action(btn, enc_dec);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "-");

    /*Decrement button*/
    btn = lv_btn_create(lv_scr_act(), btn);
    lv_btn_set_rel_action(btn, enc_inc);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "+");

    /*Select button*/
    btn = lv_btn_create(lv_scr_act(), btn);
    lv_btn_set_rel_action(btn, enc_sel);
    l = lv_label_create(btn, NULL);
    lv_label_set_text(l, "!");
}

/**
 * Called when the Enable button is released. Show a message box to really enable or not?
 * @param btn pointer to the Enable button
 * @param dispi pointer to the caller display input or NULL if the encoder used
 * @return LV_ACTION_RES_OK: because the button is not deleted
 */
static lv_action_res_t enable_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
    /*If the butto nsi released the show message box to be sure about the Enable*/
    if(lv_btn_get_state(btn) == LV_BTN_STATE_REL) {
        /* Create a dark screen sized bg. with opacity to show
         * the other objects are not available now*/
        lv_obj_t * bg = lv_obj_create(scr, NULL);
        lv_obj_set_protect(bg, LV_PROTECT_PARENT);          /*The page screen move it to scrollable area*/
        lv_obj_set_parent(bg, scr);                         /*So movi it back ater protected*/
        lv_obj_set_style(bg, &style_mbox_bg);
        lv_obj_set_size(bg, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_pos(bg, 0, 0);
        lv_obj_set_click(bg, false);                        /*For test disable click there fore buttons under it remain  clickable*/

        /*Create a message box*/
        lv_obj_t * mbox = lv_mbox_create(bg, NULL);
        lv_mbox_set_text(mbox, "Really Enable the outputs?");
        lv_group_add_obj(g, mbox);          /*Add to he group*/

        /*Add two buttons*/
        lv_mbox_add_btn(mbox, "Yes", mbox_yes_action);
        lv_mbox_add_btn(mbox, "No", mbox_no_action);

        lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, - LV_DPI / 2);

        /*Focus on the new message box, can freeze focus on it*/
        lv_group_focus_obj(mbox);
        lv_group_focus_freeze(g, true);
    }
    /*Disable is not dangerous so just change the button state*/
    else {
        lv_btn_set_state(btn_enable, LV_BTN_STATE_REL);
    }
    return LV_ACTION_RES_OK;
}

/**
 * Called when the message box's 'Yes' button is released
 * @param btn pointer to the 'Yes' button
 * @param dispi pointer to the caller display input or NULL if the encoder used
 * @return LV_ACTION_RES_INV: because the button along with the message box will be deleted
 */
static lv_action_res_t mbox_yes_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_focus_freeze(g, false);        /*Release the freeze*/
    lv_obj_t * mbox = lv_mbox_get_from_btn(btn);
    lv_obj_del(lv_obj_get_parent(mbox));    /*Delete the black background. (it will delete the mbox too)*/

    /*Mark the enabled state by toggling the button*/
    lv_btn_set_state(btn_enable, LV_BTN_STATE_TREL);

    /* In a real case you can add some specific actions here
     * to really enable something */

    return LV_ACTION_RES_INV;
}

/**
 * Called when the message box's 'No' button is released
 * @param btn pointer to the 'No' button
 * @param dispi pointer to the caller display input or NULL if the encoder used
 * @return LV_ACTION_RES_INV: because the button along with the message box will be deleted
 */
static lv_action_res_t mbox_no_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_group_focus_freeze(g, false);        /*Release the freeze*/
    lv_obj_t * mbox = lv_mbox_get_from_btn(btn);
    lv_obj_del(lv_obj_get_parent(mbox));    /*Delete the black background. (it will delete the mbox too)*/

    return LV_ACTION_RES_INV;
}

/**
 * Called when the Encoder emulator Next button is released
 * @param btn pointer to the button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK: because the button is not deleted
 */
static lv_action_res_t enc_next(lv_obj_t * btn, lv_dispi_t * dispi)
{
    /*Focus on the next object in the group*/
    lv_group_focus_next(g);

    return LV_ACTION_RES_OK;
}

/**
 * Called when the Encoder emulator Increment button is released
 * @param btn pointer to the button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK: because the button is not deleted
 */
static lv_action_res_t enc_inc(lv_obj_t * btn, lv_dispi_t * dispi)
{
    /* Send RIGHT key when rotate to right.
     * It will trigger an increment like action in the object */
    lv_group_send(g, LV_GROUP_KEY_RIGHT);
    return LV_ACTION_RES_OK;
}

/**
 * Called when the Encoder emulator Increment button is released
 * @param btn pointer to the button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK: because the button is not deleted
 */
static lv_action_res_t enc_dec(lv_obj_t * btn, lv_dispi_t * dispi)
{
    /* Send LEFT key when rotate to left.
     * It will trigger a decrement like action in the object */
    lv_group_send(g, LV_GROUP_KEY_LEFT);

    return LV_ACTION_RES_OK;
}
/**
 * Called when the Encoder emulator Send button is released
 * @param btn pointer to the button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK: because the button is not deleted
 */
static lv_action_res_t enc_sel(lv_obj_t * btn, lv_dispi_t * dispi)
{
    /* Send ENTER key.
     * It will trigger an 'OK' or 'Select' action in the object */
    lv_group_send(g, LV_GROUP_KEY_ENTER);

    return LV_ACTION_RES_OK;
}

#endif /*USE_LV_EXAMPLE != 0*/
