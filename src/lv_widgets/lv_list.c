/**
 * @file lv_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_list.h"
#if LV_USE_LIST != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_list"

#define LV_LIST_LAYOUT_DEF LV_LAYOUT_COLUMN_MID

#if LV_USE_ANIMATION == 0
    #undef LV_LIST_DEF_ANIM_TIME
    #define LV_LIST_DEF_ANIM_TIME 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_list_signal(lv_obj_t * list, lv_signal_t sign, void * param);
static lv_res_t lv_list_btn_signal(lv_obj_t * btn, lv_signal_t sign, void * param);
static lv_style_list_t * lv_list_get_style(lv_obj_t * list, uint8_t part);
static bool lv_list_is_list_btn(lv_obj_t * list_btn);
static bool lv_list_is_list_img(lv_obj_t * list_btn);
static bool lv_list_is_list_label(lv_obj_t * list_btn);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_IMG
    static lv_signal_cb_t img_signal;
#endif
static lv_signal_cb_t label_signal;
static lv_signal_cb_t ancestor_page_signal;
static lv_signal_cb_t ancestor_btn_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a list objects
 * @param par pointer to an object, it will be the parent of the new list
 * @param copy pointer to a list object, if not NULL then the new object will be copied from it
 * @return pointer to the created list
 */
lv_obj_t * lv_list_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("list create started");

    /*Create the ancestor basic object*/
    lv_obj_t * list = lv_page_create(par, copy);
    LV_ASSERT_MEM(list);
    if(list == NULL) return NULL;

    if(ancestor_page_signal == NULL) ancestor_page_signal = lv_obj_get_signal_cb(list);

    lv_list_ext_t * ext = lv_obj_allocate_ext_attr(list, sizeof(lv_list_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(list);
        return NULL;
    }

#if LV_USE_GROUP
    ext->last_sel_btn = NULL;
#endif
    ext->act_sel_btn = NULL;

    lv_obj_set_signal_cb(list, lv_list_signal);

    /*Init the new list object*/
    if(copy == NULL) {
        lv_page_set_anim_time(list, LV_LIST_DEF_ANIM_TIME);
        lv_page_set_scrollable_fit2(list, LV_FIT_PARENT, LV_FIT_TIGHT);
        lv_obj_set_size(list, 2 * LV_DPI, 3 * LV_DPI);
        lv_page_set_scrl_layout(list, LV_LIST_LAYOUT_DEF);
        lv_list_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_DRAG);

        lv_theme_apply(list, LV_THEME_LIST);

    }
    else {
        lv_obj_t * copy_btn = lv_list_get_next_btn(copy, NULL);
        while(copy_btn) {
            const void * img_src = NULL;
#if LV_USE_IMG
            lv_obj_t * copy_img = lv_list_get_btn_img(copy_btn);
            if(copy_img) img_src = lv_img_get_src(copy_img);
#endif
            lv_list_add_btn(list, img_src, lv_list_get_btn_text(copy_btn));
            copy_btn = lv_list_get_next_btn(copy, copy_btn);
        }

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(list, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("list created");

    return list;
}

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param list pointer to an object
 */
void lv_list_clean(lv_obj_t * list)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    lv_obj_t * scrl = lv_page_get_scrollable(list);
    lv_obj_clean(scrl);
}

/*======================
 * Add/remove functions
 *=====================*/
/**
 * Add a list element to the list
 * @param list pointer to list object
 * @param img_fn file name of an image before the text (NULL if unused)
 * @param txt text of the list element (NULL if unused)
 * @return pointer to the new list element which can be customized (a button)
 */
lv_obj_t * lv_list_add_btn(lv_obj_t * list, const void * img_src, const char * txt)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    lv_obj_t * last_btn = lv_list_get_prev_btn(list, NULL);

    /*The coordinates may changed due to autofit so revert them at the end*/
    lv_coord_t pos_x_ori = lv_obj_get_x(list);
    lv_coord_t pos_y_ori = lv_obj_get_y(list);

    lv_obj_t * scrl =  lv_page_get_scrollable(list);
    lv_obj_add_protect(scrl, LV_PROTECT_CHILD_CHG);

    /*Create a list element with the image an the text*/
    lv_obj_t * btn;
    btn = lv_btn_create(list, NULL);

    lv_obj_add_protect(btn, LV_PROTECT_CHILD_CHG);

    /*Save the original signal function because it will be required in `lv_list_btn_signal`*/
    if(ancestor_btn_signal == NULL) ancestor_btn_signal = lv_obj_get_signal_cb(btn);

    /*Set the default styles*/
    lv_theme_apply(btn, LV_THEME_LIST_BTN);

    lv_page_glue_obj(btn, true);
    lv_btn_set_layout(btn, LV_LAYOUT_ROW_MID);

    lv_layout_t list_layout = lv_list_get_layout(list);
    bool layout_ver = false;
    if(list_layout == LV_LAYOUT_COLUMN_MID || list_layout == LV_LAYOUT_COLUMN_LEFT ||
       list_layout == LV_LAYOUT_COLUMN_RIGHT) {
        layout_ver = true;
    }

    if(layout_ver) {
        lv_btn_set_fit2(btn, LV_FIT_PARENT, LV_FIT_TIGHT);
    }
    else {
        lv_coord_t w = last_btn ? lv_obj_get_width(last_btn) : (LV_DPI * 3) / 2;
        lv_btn_set_fit2(btn, LV_FIT_NONE, LV_FIT_TIGHT);
        lv_obj_set_width(btn, w);
    }

    lv_obj_add_protect(btn, LV_PROTECT_PRESS_LOST);
    lv_obj_set_signal_cb(btn, lv_list_btn_signal);

#if LV_USE_IMG != 0
    lv_obj_t * img = NULL;
    if(img_src) {
        img = lv_img_create(btn, NULL);
        lv_img_set_src(img, img_src);
        lv_obj_set_click(img, false);
        if(img_signal == NULL) img_signal = lv_obj_get_signal_cb(img);
    }
#endif
    if(txt != NULL) {
        lv_obj_t * label = lv_label_create(btn, NULL);
        lv_label_set_text(label, txt);
        lv_obj_set_click(label, false);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL_CIRC);
        if(lv_obj_get_base_dir(btn) == LV_BIDI_DIR_RTL) {
            lv_coord_t pad = lv_obj_get_style_pad_left(btn, LV_BTN_PART_MAIN);
            lv_obj_set_width(label, label->coords.x2 - btn->coords.x1 - pad);
        }
        else {
            lv_coord_t pad = lv_obj_get_style_pad_right(btn, LV_BTN_PART_MAIN);
            lv_obj_set_width(label, btn->coords.x2 - label->coords.x1 - pad);
        }
        if(label_signal == NULL) label_signal = lv_obj_get_signal_cb(label);
    }

#if LV_USE_GROUP
    /* If this is the first item to be added to the list and the list is
     * focused, select it */
    {
        lv_group_t * g = lv_obj_get_group(list);
        if(lv_list_get_next_btn(list, NULL) == btn && lv_group_get_focused(g) == list) {
            lv_list_focus_btn(list, btn);
        }
    }
#endif

    lv_obj_clear_protect(scrl, LV_PROTECT_CHILD_CHG);
    lv_obj_clear_protect(btn, LV_PROTECT_CHILD_CHG);
    btn->signal_cb(btn, LV_SIGNAL_CHILD_CHG, NULL);

    lv_obj_set_pos(list, pos_x_ori, pos_y_ori);

    return btn;
}

/**
 * Remove the index of the button in the list
 * @param list pointer to a list object
 * @param index pointer to a the button's index in the list, index must be 0 <= index <
 * lv_list_ext_t.size
 * @return true: successfully deleted
 */
bool lv_list_remove(const lv_obj_t * list, uint16_t index)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
    uint16_t count = 0;
    lv_obj_t * e   = lv_list_get_next_btn(list, NULL);
    while(e != NULL) {
        if(count == index) {
#if LV_USE_GROUP
            if(e == ext->last_sel_btn) ext->last_sel_btn = NULL;
#endif
            if(e == ext->act_sel_btn) ext->act_sel_btn = NULL;

            lv_obj_del(e);
            return true;
        }
        e = lv_list_get_next_btn(list, e);
        count++;
    }
    return false;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Make a button selected
 * @param list pointer to a list object
 * @param btn pointer to a button to select
 *            NULL to not select any buttons
 */
void lv_list_focus_btn(lv_obj_t * list, lv_obj_t * btn)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);
    if(btn) {
        LV_ASSERT_OBJ(btn, "lv_btn");
    }

    lv_list_ext_t * ext = lv_obj_get_ext_attr(list);

    /*Defocus the current button*/
    if(ext->act_sel_btn) lv_obj_clear_state(ext->act_sel_btn, LV_STATE_FOCUSED);

#if LV_USE_GROUP
    /*Don't forget which button was selected.
     * It will be restored when the list is focused again.*/
    if(btn) ext->last_sel_btn = btn;
#endif
    /*Focus the new button*/
    ext->act_sel_btn = btn;

    if(ext->act_sel_btn) {
        lv_obj_add_state(ext->act_sel_btn, LV_STATE_FOCUSED);
        lv_page_focus(list, ext->act_sel_btn, LV_ANIM_ON);
    }
}

/**
 * Set layout of a list
 * @param list pointer to a list object
 * @param layout which layout should be used
 */
void lv_list_set_layout(lv_obj_t * list, lv_layout_t layout)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    /* Update list layout if necessary */
    if(layout == lv_list_get_layout(list)) return;

    /* Get the first button on the list */
    lv_obj_t * btn = lv_list_get_prev_btn(list, NULL);

    /* Visit all buttons on the list and update their layout  */
    while(btn != NULL) {
        /*If a column layout set the buttons' width to list width*/
        if(layout == LV_LAYOUT_COLUMN_MID || layout == LV_LAYOUT_COLUMN_LEFT || layout == LV_LAYOUT_COLUMN_RIGHT) {
            lv_btn_set_fit2(btn, LV_FIT_PARENT, LV_FIT_TIGHT);
        }
        /*If a row layout set the buttons' width according to the content*/
        else if(layout == LV_LAYOUT_ROW_MID || layout == LV_LAYOUT_ROW_TOP || layout == LV_LAYOUT_ROW_BOTTOM) {
            lv_btn_set_fit(btn, LV_FIT_TIGHT);
        }

        btn = lv_list_get_prev_btn(list, btn);
    }

    if(layout == LV_LAYOUT_COLUMN_MID || layout == LV_LAYOUT_COLUMN_LEFT || layout == LV_LAYOUT_COLUMN_RIGHT) {
        lv_page_set_scrollable_fit2(list, LV_FIT_PARENT, LV_FIT_TIGHT);
    }
    else if(layout == LV_LAYOUT_ROW_MID || layout == LV_LAYOUT_ROW_TOP || layout == LV_LAYOUT_ROW_BOTTOM) {
        lv_page_set_scrollable_fit2(list, LV_FIT_TIGHT, LV_FIT_TIGHT);
        lv_cont_set_fit2(list, LV_FIT_NONE, LV_FIT_TIGHT);
    }

    lv_page_set_scrl_layout(list, layout);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a list element
 * @param btn pointer to list element
 * @return pointer to the text
 */
const char * lv_list_get_btn_text(const lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, "lv_btn");

    lv_obj_t * label = lv_list_get_btn_label(btn);
    if(label == NULL) return "";
    return lv_label_get_text(label);
}

/**
 * Get the label object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the label from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_btn_label(const lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, "lv_btn");

    lv_obj_t * label = lv_obj_get_child_back(btn, NULL);
    if(label == NULL) return NULL;

    while(lv_list_is_list_label(label) == false) {
        label = lv_obj_get_child_back(btn, label);
        if(label == NULL) break;
    }

    return label;
}

/**
 * Get the image object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the image from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_btn_img(const lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, "lv_btn");

#if LV_USE_IMG != 0
    lv_obj_t * img = lv_obj_get_child_back(btn, NULL);
    if(img == NULL) return NULL;

    while(lv_list_is_list_img(img) == false) {
        img = lv_obj_get_child_back(btn, img);
        if(img == NULL) break;
    }

    return img;
#else
    return NULL;
#endif
}

/**
 * Get the previous button from list. (Starts from the bottom button)
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the previous before it.
 * @return pointer to the previous button or NULL when no more buttons
 */
lv_obj_t * lv_list_get_prev_btn(const lv_obj_t * list, lv_obj_t * prev_btn)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    /* Not a good practice but user can add/create objects to the lists manually.
     * When getting the next button try to be sure that it is at least a button */

    lv_obj_t * btn;
    lv_obj_t * scrl = lv_page_get_scrollable(list);

    btn = lv_obj_get_child(scrl, prev_btn);
    if(btn == NULL) return NULL;

    while(lv_list_is_list_btn(btn) == false) {
        btn = lv_obj_get_child(scrl, btn);
        if(btn == NULL) break;
    }

    return btn;
}

/**
 * Get the next button from list. (Starts from the top button)
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the next after it.
 * @return pointer to the next button or NULL when no more buttons
 */
lv_obj_t * lv_list_get_next_btn(const lv_obj_t * list, lv_obj_t * prev_btn)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    /* Not a good practice but user can add/create objects to the lists manually.
     * When getting the next button try to be sure that it is at least a button */

    lv_obj_t * btn;
    lv_obj_t * scrl = lv_page_get_scrollable(list);

    btn = lv_obj_get_child_back(scrl, prev_btn);
    if(btn == NULL) return NULL;

    while(lv_list_is_list_btn(btn) == false) {
        btn = lv_obj_get_child_back(scrl, btn);
        if(btn == NULL) break;
    }

    return btn;
}

/**
 * Get the index of the button in the list
 * @param list pointer to a list object. If NULL, assumes btn is part of a list.
 * @param btn pointer to a list element (button)
 * @return the index of the button in the list, or -1 of the button not in this list
 */
int32_t lv_list_get_btn_index(const lv_obj_t * list, const lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, "lv_btn");

    int index = 0;
    if(list == NULL) {
        /* no list provided, assuming btn is part of a list */
        list = lv_obj_get_parent(lv_obj_get_parent(btn));
    }
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    lv_obj_t * e = lv_list_get_next_btn(list, NULL);
    while(e != NULL) {
        if(e == btn) {
            return index;
        }
        index++;
        e = lv_list_get_next_btn(list, e);
    }
    return -1;
}

/**
 * Get the number of buttons in the list
 * @param list pointer to a list object
 * @return the number of buttons in the list
 */
uint16_t lv_list_get_size(const lv_obj_t * list)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    uint16_t size = 0;
    lv_obj_t * btn = lv_list_get_next_btn(list, NULL);
    while(btn) {
        size++;
        btn = lv_list_get_next_btn(list, btn);
    }
    return size;
}

#if LV_USE_GROUP
/**
 * Get the currently selected button
 * @param list pointer to a list object
 * @return pointer to the selected button
 */
lv_obj_t * lv_list_get_btn_selected(const lv_obj_t * list)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
    return ext->act_sel_btn;
}
#endif

/**
 * Get layout of a list
 * @param list pointer to a list object
 * @return layout of the list object
 */
lv_layout_t lv_list_get_layout(lv_obj_t * list)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    return lv_page_get_scrl_layout(list);
}

/*=====================
 * Other functions
 *====================*/

/**
 * Move the list elements up by one
 * @param list pointer a to list object
 */
void lv_list_up(const lv_obj_t * list)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    /*Search the first list element which 'y' coordinate is below the parent
     * and position the list to show this element on the bottom*/
    lv_obj_t * scrl = lv_page_get_scrollable(list);
    lv_obj_t * e;
    lv_obj_t * e_prev = NULL;

    e                 = lv_list_get_prev_btn(list, NULL);
    while(e != NULL) {
        if(e->coords.y2 <= list->coords.y2) {
            if(e_prev != NULL) {
                lv_coord_t new_y = lv_obj_get_height(list) - (lv_obj_get_y(e_prev) + lv_obj_get_height(e_prev));
                if(lv_list_get_anim_time(list) == 0) {
                    lv_obj_set_y(scrl, new_y);
                }
                else {
#if LV_USE_ANIMATION
                    lv_anim_t a;
                    lv_anim_init(&a);
                    lv_anim_set_var(&a, scrl);
                    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
                    lv_anim_set_values(&a, lv_obj_get_y(scrl), new_y);
                    lv_anim_set_time(&a, LV_LIST_DEF_ANIM_TIME);
                    lv_anim_start(&a);
#endif
                }
            }
            break;
        }
        e_prev = e;
        e      = lv_list_get_prev_btn(list, e);
    }
}

/**
 * Move the list elements down by one
 * @param list pointer to a list object
 */
void lv_list_down(const lv_obj_t * list)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    /*Search the first list element which 'y' coordinate is above the parent
     * and position the list to show this element on the top*/
    lv_obj_t * scrl = lv_page_get_scrollable(list);
    lv_obj_t * e;
    e = lv_list_get_prev_btn(list, NULL);
    while(e != NULL) {
        if(e->coords.y1 < list->coords.y1) {
            lv_coord_t new_y = -lv_obj_get_y(e);
            if(lv_list_get_anim_time(list) == 0) {
                lv_obj_set_y(scrl, new_y);
            }
            else {
#if LV_USE_ANIMATION
                lv_anim_t a;
                lv_anim_init(&a);
                lv_anim_set_var(&a, scrl);
                lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
                lv_anim_set_values(&a, lv_obj_get_y(scrl), new_y);
                lv_anim_set_time(&a, LV_LIST_DEF_ANIM_TIME);
                lv_anim_start(&a);
#endif
            }
            break;
        }
        e = lv_list_get_prev_btn(list, e);
    }
}

/**
 * Focus on a list button. It ensures that the button will be visible on the list.
 * @param btn pointer to a list button to focus
 * @param anim_en LV_ANIM_ON: scroll with animation, LV_ANOM_OFF: without animation
 */
void lv_list_focus(const lv_obj_t * btn, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(btn, "");

#if LV_USE_ANIMATION == 0
    anim = false;
#endif

    lv_obj_t * list = lv_obj_get_parent(lv_obj_get_parent(btn));

    lv_page_focus(list, btn, anim == LV_ANIM_OFF ? 0 : lv_list_get_anim_time(list));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the list
 * @param list pointer to a list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_list_signal(lv_obj_t * list, lv_signal_t sign, void * param)
{
    lv_res_t res;
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_list_get_style(list, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_page_signal(list, sign, param);
    }

    /* Include the ancient signal function */
    res = ancestor_page_signal(list, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESSED || sign == LV_SIGNAL_PRESSING ||
       sign == LV_SIGNAL_LONG_PRESS || sign == LV_SIGNAL_LONG_PRESS_REP) {
#if LV_USE_GROUP
        /*If pressed/released etc by a KEYPAD or ENCODER delegate signal and events to the button*/
        lv_indev_t * indev         = lv_indev_get_act();
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type == LV_INDEV_TYPE_KEYPAD ||
           (indev_type == LV_INDEV_TYPE_ENCODER && lv_group_get_editing(lv_obj_get_group(list)))) {
            lv_list_ext_t * ext = lv_obj_get_ext_attr(list);

            /*The page receives the key presses so the events should be propagated to the selected
             * button*/
            if(ext->act_sel_btn) {
                res = lv_signal_send(ext->act_sel_btn, sign, param);
                if(res != LV_RES_OK) return res;
                if(sign == LV_SIGNAL_PRESSED) {
                    res = lv_event_send(ext->act_sel_btn, LV_EVENT_PRESSED, NULL);
                    if(res != LV_RES_OK) return res;
                }
                else if(sign == LV_SIGNAL_PRESSING) {
                    res = lv_event_send(ext->act_sel_btn, LV_EVENT_PRESSING, NULL);
                    if(res != LV_RES_OK) return res;
                }
                else if(sign == LV_SIGNAL_LONG_PRESS) {
                    res = lv_event_send(ext->act_sel_btn, LV_EVENT_LONG_PRESSED, NULL);
                    if(res != LV_RES_OK) return res;
                }
                else if(sign == LV_SIGNAL_LONG_PRESS_REP) {
                    res = lv_event_send(ext->act_sel_btn, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
                    if(res != LV_RES_OK) return res;
                }
                else if(sign == LV_SIGNAL_RELEASED) {
                    if(indev->proc.long_pr_sent == 0) {
                        res = lv_event_send(ext->act_sel_btn, LV_EVENT_SHORT_CLICKED, NULL);
                        if(res != LV_RES_OK) return res;
                    }
                    if(lv_indev_is_dragging(indev) == false) {
                        res = lv_event_send(ext->act_sel_btn, LV_EVENT_CLICKED, NULL);
                        if(res != LV_RES_OK) return res;
                    }

                    res = lv_event_send(ext->act_sel_btn, LV_EVENT_RELEASED, NULL);
                    if(res != LV_RES_OK) return res;
                }
            }
        }
#endif
    }
    else if(sign == LV_SIGNAL_FOCUS) {

#if LV_USE_GROUP
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        /*With ENCODER focus the button only in edit mode*/
        lv_group_t * g = lv_obj_get_group(list);
        if((indev_type == LV_INDEV_TYPE_KEYPAD) ||
           (indev_type == LV_INDEV_TYPE_ENCODER && lv_group_get_editing(g))) {
            lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
            /* Select the last used button, or use the first no last button */
            if(ext->last_sel_btn) lv_list_focus_btn(list, ext->last_sel_btn);
            else lv_list_focus_btn(list, lv_list_get_next_btn(list, NULL));
        }
        if(indev_type == LV_INDEV_TYPE_ENCODER && lv_group_get_editing(g) == false) {
            lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
            if(ext->act_sel_btn) lv_obj_clear_state(ext->act_sel_btn, LV_STATE_PRESSED);
            if(ext->last_sel_btn) lv_obj_clear_state(ext->last_sel_btn, LV_STATE_PRESSED);
        }
#endif
    }
    else if(sign == LV_SIGNAL_DEFOCUS) {

#if LV_USE_GROUP
        /*De-select the selected btn*/
        lv_list_focus_btn(list, NULL);
        lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
        ext->act_sel_btn   = NULL;
#endif
    }
    else if(sign == LV_SIGNAL_GET_EDITABLE) {
#if LV_USE_GROUP
        bool * editable = (bool *)param;
        *editable       = true;
#endif
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_DOWN) {
            lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
            /*If there is a valid selected button the make the previous selected*/
            if(ext->act_sel_btn) {
                lv_obj_t * btn_prev = lv_list_get_next_btn(list, ext->act_sel_btn);
                if(btn_prev) lv_list_focus_btn(list, btn_prev);
            }
            /*If there is no selected button the make the first selected*/
            else {
                lv_obj_t * btn = lv_list_get_next_btn(list, NULL);
                if(btn)
                    lv_list_focus_btn(list,
                                      btn); /*If there are no buttons on the list then there is no first button*/
            }
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_UP) {
            lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
            /*If there is a valid selected button the make the next selected*/
            if(ext->act_sel_btn != NULL) {
                lv_obj_t * btn_next = lv_list_get_prev_btn(list, ext->act_sel_btn);
                if(btn_next) lv_list_focus_btn(list, btn_next);
            }
            /*If there is no selected button the make the first selected*/
            else {
                lv_obj_t * btn = lv_list_get_next_btn(list, NULL);
                if(btn) lv_list_focus_btn(list, btn);
            }
        }
        else if(c == LV_KEY_ESC) {
            lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
            /* Handle ESC/Cancel event */
            res = lv_event_send(ext->act_sel_btn, LV_EVENT_CANCEL, NULL);
        }
#endif
    }
    return res;
}

/**
 * Signal function of the list buttons
 * @param btn pointer to a button on the list
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_list_btn_signal(lv_obj_t * btn, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_btn_signal(btn, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, "");

    if((sign == LV_SIGNAL_COORD_CHG && (lv_obj_get_width(btn) != lv_area_get_width(param) ||
                                        lv_obj_get_height(btn) != lv_area_get_height(param))) ||
       sign == LV_SIGNAL_STYLE_CHG) {
        lv_obj_t * label = lv_list_get_btn_label(btn);
        if(label) {
            const lv_font_t * font = lv_obj_get_style_text_font(label, LV_LABEL_PART_MAIN);
            lv_coord_t font_h = lv_font_get_line_height(font);
            if(lv_obj_get_base_dir(btn) == LV_BIDI_DIR_RTL) {
                lv_coord_t pad = lv_obj_get_style_pad_left(btn, LV_BTN_PART_MAIN);
                lv_obj_set_size(label, label->coords.x2 - btn->coords.x1 - pad, font_h);
            }
            else {
                lv_coord_t pad = lv_obj_get_style_pad_right(btn, LV_BTN_PART_MAIN);
                lv_obj_set_size(label, btn->coords.x2 - label->coords.x1 - pad + 1, font_h);
            }
        }
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        if(lv_indev_is_dragging(lv_indev_get_act()) == false) {
            lv_obj_t * list = lv_obj_get_parent(lv_obj_get_parent(btn));
            lv_list_focus_btn(list, btn);
#if LV_USE_GROUP
            if(lv_obj_get_group(list)) {
                lv_group_focus_obj(list);
            }
#endif
        }
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
#if LV_USE_GROUP
        lv_obj_t * list = lv_obj_get_parent(lv_obj_get_parent(btn));
        lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
        lv_obj_t * sel  = lv_list_get_btn_selected(list);
        if(sel == btn) lv_list_focus_btn(list, lv_list_get_next_btn(list, btn));
        if(ext->last_sel_btn == btn) ext->last_sel_btn = NULL;
#endif
    }
    else if(sign == LV_SIGNAL_FOCUS) {
#if LV_USE_GROUP
        lv_obj_t * list = lv_obj_get_parent(lv_obj_get_parent(btn));
        lv_list_focus_btn(list, btn);
#endif
    }

    return res;
}

/**
 * Get the style descriptor of a part of the object
 * @param list pointer the object
 * @param part the part from `lv_page_list_t`. (LV_LIST_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_list_get_style(lv_obj_t * list, uint8_t part)
{
    LV_ASSERT_OBJ(list, LV_OBJX_NAME);

    lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_LIST_PART_BG:
            style_dsc_p = &list->style_list;
            break;
        case LV_LIST_PART_SCROLLABLE:
            style_dsc_p = &ext->page.scrl->style_list;
            break;
        case LV_LIST_PART_SCROLLBAR:
            style_dsc_p = &ext->page.scrlbar.style;
            break;
#if LV_USE_ANIMATION
        case LV_LIST_PART_EDGE_FLASH:
            style_dsc_p = &ext->page.edge_flash.style;
            break;
#endif
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}

/**
 * Check if this is really a list button or another object.
 * @param list_btn List button
 */
static bool lv_list_is_list_btn(lv_obj_t * list_btn)
{
    lv_obj_type_t type;

    lv_obj_get_type(list_btn, &type);
    uint8_t cnt;
    for(cnt = 0; cnt < LV_MAX_ANCESTOR_NUM; cnt++) {
        if(type.type[cnt] == NULL) break;
        if(!strcmp(type.type[cnt], "lv_btn")) return true;
    }
    return false;
}

/**
 * Check if this is really a list label or another object.
 * @param list_label List label
 */
static bool lv_list_is_list_label(lv_obj_t * list_label)
{
    lv_obj_type_t type;

    lv_obj_get_type(list_label, &type);
    uint8_t cnt;
    for(cnt = 0; cnt < LV_MAX_ANCESTOR_NUM; cnt++) {
        if(type.type[cnt] == NULL) break;
        if(!strcmp(type.type[cnt], "lv_label")) return true;
    }
    return false;
}

/**
 * Check if this is really a list image or another object.
 * @param list_image List image
 */
static bool lv_list_is_list_img(lv_obj_t * list_img)
{
    lv_obj_type_t type;

    lv_obj_get_type(list_img, &type);
    uint8_t cnt;
    for(cnt = 0; cnt < LV_MAX_ANCESTOR_NUM; cnt++) {
        if(type.type[cnt] == NULL) break;
        if(!strcmp(type.type[cnt], "lv_img")) return true;
    }
    return false;
}

#endif
