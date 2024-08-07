/**
 * @file lv_btnmatrix.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_buttonmatrix_private.h"
#include "../../misc/lv_area_private.h"
#include "../../core/lv_obj_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_BUTTONMATRIX != 0

#include "../../misc/lv_assert.h"
#include "../../indev/lv_indev.h"
#include "../../core/lv_group.h"
#include "../../draw/lv_draw.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_text.h"
#include "../../misc/lv_text_ap.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_buttonmatrix_class)

#define BTN_EXTRA_CLICK_AREA_MAX (LV_DPI_DEF / 10)
#define LV_BUTTONMATRIX_WIDTH_MASK 0x000F

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_buttonmatrix_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_buttonmatrix_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_buttonmatrix_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);

static uint32_t get_button_width(lv_buttonmatrix_ctrl_t ctrl_bits);
static bool button_is_hidden(lv_buttonmatrix_ctrl_t ctrl_bits);
static bool button_is_checked(lv_buttonmatrix_ctrl_t ctrl_bits);
static bool button_is_repeat_disabled(lv_buttonmatrix_ctrl_t ctrl_bits);
static bool button_is_inactive(lv_buttonmatrix_ctrl_t ctrl_bits);
static bool button_is_click_trig(lv_buttonmatrix_ctrl_t ctrl_bits);
static bool button_is_popover(lv_buttonmatrix_ctrl_t ctrl_bits);
static bool button_is_checkable(lv_buttonmatrix_ctrl_t ctrl_bits);
static bool button_get_checked(lv_buttonmatrix_ctrl_t ctrl_bits);
static uint32_t get_button_from_point(lv_obj_t * obj, lv_point_t * p);
static void allocate_button_areas_and_controls(const lv_obj_t * obj, const char ** map);
static void invalidate_button_area(const lv_obj_t * obj, uint32_t btn_idx);
static void make_one_button_checked(lv_obj_t * obj, uint32_t btn_idx);
static bool has_popovers_in_top_row(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_WIDGETS_HAS_DEFAULT_VALUE
static const char * lv_buttonmatrix_def_map[] = {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""};
#endif

const lv_obj_class_t lv_buttonmatrix_class = {
    .constructor_cb = lv_buttonmatrix_constructor,
    .destructor_cb = lv_buttonmatrix_destructor,
    .event_cb = lv_buttonmatrix_event,
    .width_def = LV_DPI_DEF * 2,
    .height_def = LV_DPI_DEF,
    .instance_size = sizeof(lv_buttonmatrix_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .base_class = &lv_obj_class,
    .name = "btnmatrix",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_buttonmatrix_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_buttonmatrix_set_map(lv_obj_t * obj, const char * map[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(map == NULL) return;

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;

    /*Analyze the map and create the required number of buttons*/
    allocate_button_areas_and_controls(obj, map);
    btnm->map_p = map;

    lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);

    /*Set size and positions of the buttons*/
    int32_t sleft = lv_obj_get_style_space_left(obj, LV_PART_MAIN);
    int32_t stop = lv_obj_get_style_space_top(obj, LV_PART_MAIN);
    int32_t prow = lv_obj_get_style_pad_row(obj, LV_PART_MAIN);
    int32_t pcol = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

    int32_t max_w            = lv_obj_get_content_width(obj);
    int32_t max_h            = lv_obj_get_content_height(obj);

    /*Calculate the position of each row*/
    int32_t max_h_no_gap = max_h - (prow * (btnm->row_cnt - 1));

    /*Count the units and the buttons in a line
     *(A button can be 1,2,3... unit wide)*/
    uint32_t txt_tot_i = 0; /*Act. index in the str map*/
    uint32_t btn_tot_i = 0; /*Act. index of button areas*/
    const char ** map_row = map;

    /*Count the units and the buttons in a line*/
    uint32_t row;
    for(row = 0; row < btnm->row_cnt; row++) {
        uint32_t unit_cnt = 0;           /*Number of units in a row*/
        uint32_t btn_cnt = 0;            /*Number of buttons in a row*/
        /*Count the buttons and units in this row*/
        while(map_row[btn_cnt] && lv_strcmp(map_row[btn_cnt], "\n") != 0 && map_row[btn_cnt][0] != '\0') {
            unit_cnt += get_button_width(btnm->ctrl_bits[btn_tot_i + btn_cnt]);
            btn_cnt++;
        }

        /*Only deal with the non empty lines*/
        if(btn_cnt == 0) {
            map_row = &map_row[btn_cnt + 1];       /*Set the map to the next row*/
            continue;
        }

        int32_t row_y1 = stop + (max_h_no_gap * row) / btnm->row_cnt + row * prow;
        int32_t row_y2 = stop + (max_h_no_gap * (row + 1)) / btnm->row_cnt + row * prow - 1;

        /*Set the button size and positions*/
        int32_t max_w_no_gap = max_w - (pcol * (btn_cnt - 1));
        if(max_w_no_gap < 0) max_w_no_gap = 0;

        uint32_t row_unit_cnt = 0;  /*The current unit position in the row*/
        uint32_t btn;
        for(btn = 0; btn < btn_cnt; btn++, btn_tot_i++, txt_tot_i++) {
            uint32_t btn_u = get_button_width(btnm->ctrl_bits[btn_tot_i]);

            int32_t btn_x1 = (max_w_no_gap * row_unit_cnt) / unit_cnt + btn * pcol;
            int32_t btn_x2 = (max_w_no_gap * (row_unit_cnt + btn_u)) / unit_cnt + btn * pcol - 1;

            /*If RTL start from the right*/
            if(base_dir == LV_BASE_DIR_RTL) {
                int32_t tmp = btn_x1;
                btn_x1 = btn_x2;
                btn_x2 = tmp;

                btn_x1 = max_w - btn_x1;
                btn_x2 = max_w - btn_x2;
            }

            btn_x1 += sleft;
            btn_x2 += sleft;

            lv_area_set(&btnm->button_areas[btn_tot_i], btn_x1, row_y1, btn_x2, row_y2);

            row_unit_cnt += btn_u;
        }

        map_row = &map_row[btn_cnt + 1];       /*Set the map to the next line*/
    }

    /*Popovers in the top row will draw outside the widget and the extended draw size depends on
     *the row height which may have changed when setting the new map*/
    lv_obj_refresh_ext_draw_size(obj);

    lv_obj_invalidate(obj);
}

void lv_buttonmatrix_set_ctrl_map(lv_obj_t * obj, const lv_buttonmatrix_ctrl_t ctrl_map[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    lv_memcpy(btnm->ctrl_bits, ctrl_map, sizeof(lv_buttonmatrix_ctrl_t) * btnm->btn_cnt);

    lv_buttonmatrix_set_map(obj, btnm->map_p);
}

void lv_buttonmatrix_set_selected_button(lv_obj_t * obj, uint32_t btn_id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;

    if(btn_id >= btnm->btn_cnt && btn_id != LV_BUTTONMATRIX_BUTTON_NONE) return;

    invalidate_button_area(obj, btnm->btn_id_sel);
    btnm->btn_id_sel = btn_id;
    invalidate_button_area(obj, btn_id);
}

void lv_buttonmatrix_set_button_ctrl(lv_obj_t * obj, uint32_t btn_id, lv_buttonmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;

    if(btn_id >= btnm->btn_cnt) return;

    if(btnm->one_check && (ctrl & LV_BUTTONMATRIX_CTRL_CHECKED)) {
        lv_buttonmatrix_clear_button_ctrl_all(obj, LV_BUTTONMATRIX_CTRL_CHECKED);
    }

    btnm->ctrl_bits[btn_id] |= ctrl;
    invalidate_button_area(obj, btn_id);

    if(ctrl & LV_BUTTONMATRIX_CTRL_POPOVER) {
        lv_obj_refresh_ext_draw_size(obj);
    }
}

void lv_buttonmatrix_clear_button_ctrl(lv_obj_t * obj, uint32_t btn_id, lv_buttonmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;

    if(btn_id >= btnm->btn_cnt) return;

    btnm->ctrl_bits[btn_id] &= (~ctrl);
    invalidate_button_area(obj, btn_id);

    if(ctrl & LV_BUTTONMATRIX_CTRL_POPOVER) {
        lv_obj_refresh_ext_draw_size(obj);
    }
}

void lv_buttonmatrix_set_button_ctrl_all(lv_obj_t * obj, lv_buttonmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    uint32_t i;
    for(i = 0; i < btnm->btn_cnt; i++) {
        lv_buttonmatrix_set_button_ctrl(obj, i, ctrl);
    }
}

void lv_buttonmatrix_clear_button_ctrl_all(lv_obj_t * obj, lv_buttonmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    uint32_t i;
    for(i = 0; i < btnm->btn_cnt; i++) {
        lv_buttonmatrix_clear_button_ctrl(obj, i, ctrl);
    }
}

void lv_buttonmatrix_set_button_width(lv_obj_t * obj, uint32_t btn_id, uint32_t width)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    if(btn_id >= btnm->btn_cnt) return;
    btnm->ctrl_bits[btn_id] &= (~LV_BUTTONMATRIX_WIDTH_MASK);
    btnm->ctrl_bits[btn_id] |= (LV_BUTTONMATRIX_WIDTH_MASK & width);

    lv_buttonmatrix_set_map(obj, btnm->map_p);
}

void lv_buttonmatrix_set_one_checked(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    btnm->one_check     = en;

    /*If more than one button is toggled only the first one should be*/
    make_one_button_checked(obj, 0);
}

/*=====================
 * Getter functions
 *====================*/

const char ** lv_buttonmatrix_get_map(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    return btnm->map_p;
}

uint32_t lv_buttonmatrix_get_selected_button(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    return btnm->btn_id_sel;
}

const char * lv_buttonmatrix_get_button_text(const lv_obj_t * obj, uint32_t btn_id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(btn_id == LV_BUTTONMATRIX_BUTTON_NONE) return NULL;

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    if(btn_id > btnm->btn_cnt) return NULL;

    uint32_t txt_i = 0;
    uint32_t btn_i = 0;

    /*Search the text of btnm->btn_pr the buttons text in the map
     *Skip "\n"-s*/
    while(btn_i != btn_id) {
        btn_i++;
        txt_i++;
        if(lv_strcmp(btnm->map_p[txt_i], "\n") == 0) txt_i++;
    }

    if(btn_i == btnm->btn_cnt) return NULL;

    return btnm->map_p[txt_i];
}

bool lv_buttonmatrix_has_button_ctrl(lv_obj_t * obj, uint32_t btn_id, lv_buttonmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    if(btn_id >= btnm->btn_cnt) return false;

    return (btnm->ctrl_bits[btn_id] & ctrl) == ctrl;
}

bool lv_buttonmatrix_get_one_checked(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;

    return btnm->one_check;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_buttonmatrix_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    btnm->btn_cnt        = 0;
    btnm->row_cnt        = 0;
    btnm->btn_id_sel     = LV_BUTTONMATRIX_BUTTON_NONE;
    btnm->button_areas   = NULL;
    btnm->ctrl_bits      = NULL;
    btnm->map_p          = NULL;
    btnm->one_check      = 0;

#if LV_WIDGETS_HAS_DEFAULT_VALUE
    lv_buttonmatrix_set_map(obj, lv_buttonmatrix_def_map);
#endif

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_buttonmatrix_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_TRACE_OBJ_CREATE("begin");
    LV_UNUSED(class_p);
    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    lv_free(btnm->button_areas);
    lv_free(btnm->ctrl_bits);
    btnm->button_areas = NULL;
    btnm->ctrl_bits = NULL;
    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_buttonmatrix_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    lv_point_t p;

    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        if(has_popovers_in_top_row(obj)) {
            /*reserve one row worth of extra space to account for popovers in the top row*/
            int32_t s = btnm->row_cnt > 0 ? lv_obj_get_content_height(obj) / btnm->row_cnt : 0;
            lv_event_set_ext_draw_size(e, s);
        }
    }
    if(code == LV_EVENT_STYLE_CHANGED) {
        lv_buttonmatrix_set_map(obj, btnm->map_p);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        lv_buttonmatrix_set_map(obj, btnm->map_p);
    }
    else if(code == LV_EVENT_PRESSED) {
        lv_indev_t * indev = lv_event_get_indev(e);
        invalidate_button_area(obj, btnm->btn_id_sel);

        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_active());
        if(indev_type == LV_INDEV_TYPE_POINTER || indev_type == LV_INDEV_TYPE_BUTTON) {
            uint32_t btn_pr;
            /*Search the pressed area*/
            lv_indev_get_point(indev, &p);
            btn_pr = get_button_from_point(obj, &p);
            /*Handle the case where there is no button there*/
            btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
            if(btn_pr != LV_BUTTONMATRIX_BUTTON_NONE) {
                if(button_is_inactive(btnm->ctrl_bits[btn_pr]) == false &&
                   button_is_hidden(btnm->ctrl_bits[btn_pr]) == false) {
                    btnm->btn_id_sel = btn_pr;
                    invalidate_button_area(obj, btnm->btn_id_sel); /*Invalidate the new area*/
                }
            }
            else {
                btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
            }
        }

        if(btnm->btn_id_sel != LV_BUTTONMATRIX_BUTTON_NONE) {
            if(button_is_click_trig(btnm->ctrl_bits[btnm->btn_id_sel]) == false &&
               button_is_popover(btnm->ctrl_bits[btnm->btn_id_sel]) == false &&
               button_is_inactive(btnm->ctrl_bits[btnm->btn_id_sel]) == false &&
               button_is_hidden(btnm->ctrl_bits[btnm->btn_id_sel]) == false) {
                uint32_t b = btnm->btn_id_sel;
                res        = lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, &b);
                if(res != LV_RESULT_OK) return;
            }
        }
    }
    else if(code == LV_EVENT_PRESSING) {
        /*If a slid to a new button, discard the current button and don't press any buttons*/
        if(btnm->btn_id_sel != LV_BUTTONMATRIX_BUTTON_NONE) {
            lv_indev_t * indev = lv_event_get_indev(e);
            lv_indev_get_point(indev, &p);
            uint32_t btn_pr = get_button_from_point(obj, &p);
            if(btn_pr != btnm->btn_id_sel) {
                invalidate_button_area(obj, btnm->btn_id_sel); /*Invalidate the old area*/
                btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
            }
        }
    }
    else if(code == LV_EVENT_RELEASED) {
        if(btnm->btn_id_sel != LV_BUTTONMATRIX_BUTTON_NONE) {
            /*Toggle the button if enabled*/
            if(button_is_checkable(btnm->ctrl_bits[btnm->btn_id_sel]) &&
               !button_is_inactive(btnm->ctrl_bits[btnm->btn_id_sel])) {
                if(button_get_checked(btnm->ctrl_bits[btnm->btn_id_sel]) && !btnm->one_check) {
                    btnm->ctrl_bits[btnm->btn_id_sel] &= (~LV_BUTTONMATRIX_CTRL_CHECKED);
                }
                else {
                    btnm->ctrl_bits[btnm->btn_id_sel] |= LV_BUTTONMATRIX_CTRL_CHECKED;
                }
                if(btnm->one_check) make_one_button_checked(obj, btnm->btn_id_sel);
            }

            if((button_is_click_trig(btnm->ctrl_bits[btnm->btn_id_sel]) == true ||
                button_is_popover(btnm->ctrl_bits[btnm->btn_id_sel]) == true) &&
               button_is_inactive(btnm->ctrl_bits[btnm->btn_id_sel]) == false &&
               button_is_hidden(btnm->ctrl_bits[btnm->btn_id_sel]) == false) {
                uint32_t b = btnm->btn_id_sel;
                res        = lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, &b);
                if(res != LV_RESULT_OK) return;
            }
        }

        /*Invalidate to old pressed area*/;
        invalidate_button_area(obj, btnm->btn_id_sel);

    }
    else if(code == LV_EVENT_LONG_PRESSED_REPEAT) {
        if(btnm->btn_id_sel != LV_BUTTONMATRIX_BUTTON_NONE) {
            if(button_is_repeat_disabled(btnm->ctrl_bits[btnm->btn_id_sel]) == false &&
               button_is_inactive(btnm->ctrl_bits[btnm->btn_id_sel]) == false &&
               button_is_hidden(btnm->ctrl_bits[btnm->btn_id_sel]) == false) {
                uint32_t b = btnm->btn_id_sel;
                res        = lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, &b);
                if(res != LV_RESULT_OK) return;
            }
        }
    }
    else if(code == LV_EVENT_PRESS_LOST) {
        invalidate_button_area(obj, btnm->btn_id_sel);
        btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
    }
    else if(code == LV_EVENT_FOCUSED) {
        if(btnm->btn_cnt == 0) return;

        lv_indev_t * indev = lv_event_get_indev(e);
        lv_indev_type_t indev_type = lv_indev_get_type(indev);

        /*If not focused by an input device assume the last input device*/
        if(indev == NULL) {
            indev = lv_indev_get_next(NULL);
            indev_type = lv_indev_get_type(indev);
        }

        bool editing = lv_group_get_editing(lv_obj_get_group(obj));
        /*Focus the first button if there is not selected button*/
        if(btnm->btn_id_sel == LV_BUTTONMATRIX_BUTTON_NONE) {
            if(indev_type == LV_INDEV_TYPE_KEYPAD || (indev_type == LV_INDEV_TYPE_ENCODER && editing)) {
                uint32_t b = 0;
                if(btnm->one_check) {
                    while(b < btnm->btn_cnt &&
                          (button_is_hidden(btnm->ctrl_bits[b]) ||
                           button_is_inactive(btnm->ctrl_bits[b]) ||
                           button_is_checked(btnm->ctrl_bits[b]) == false)) {
                        b++;
                    }
                }
                else {
                    while(b < btnm->btn_cnt &&
                          (button_is_hidden(btnm->ctrl_bits[b]) ||
                           button_is_inactive(btnm->ctrl_bits[b]))) {
                        b++;
                    }
                }

                btnm->btn_id_sel = b;
            }
            else {
                btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
            }
        }
    }
    else if(code == LV_EVENT_DEFOCUSED || code == LV_EVENT_LEAVE) {
        //        TODO
        //        if(btnm->btn_id_sel != LV_BUTTONMATRIX_BUTTON_NONE) invalidate_button_area(obj, btnm->btn_id_sel);
        //        btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
    }
    else if(code == LV_EVENT_KEY) {

        invalidate_button_area(obj, btnm->btn_id_sel);

        uint32_t c = lv_event_get_key(e);
        if(c == LV_KEY_RIGHT) {
            if(btnm->btn_id_sel == LV_BUTTONMATRIX_BUTTON_NONE)  btnm->btn_id_sel = 0;
            else btnm->btn_id_sel++;
            if(btnm->btn_id_sel >= btnm->btn_cnt) btnm->btn_id_sel = 0;

            uint32_t btn_id_start = btnm->btn_id_sel;
            while(button_is_hidden(btnm->ctrl_bits[btnm->btn_id_sel]) || button_is_inactive(btnm->ctrl_bits[btnm->btn_id_sel])) {
                btnm->btn_id_sel++;
                if(btnm->btn_id_sel >= btnm->btn_cnt) btnm->btn_id_sel = 0;

                if(btnm->btn_id_sel == btn_id_start) {
                    btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
                    break;
                }
            }
        }
        else if(c == LV_KEY_LEFT) {
            if(btnm->btn_id_sel == LV_BUTTONMATRIX_BUTTON_NONE) btnm->btn_id_sel = 0;

            if(btnm->btn_id_sel == 0) btnm->btn_id_sel = btnm->btn_cnt - 1;
            else if(btnm->btn_id_sel > 0) btnm->btn_id_sel--;

            uint32_t btn_id_start = btnm->btn_id_sel;
            while(button_is_hidden(btnm->ctrl_bits[btnm->btn_id_sel]) || button_is_inactive(btnm->ctrl_bits[btnm->btn_id_sel])) {
                if(btnm->btn_id_sel > 0) btnm->btn_id_sel--;
                else btnm->btn_id_sel = btnm->btn_cnt - 1;

                if(btnm->btn_id_sel == btn_id_start) {
                    btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
                    break;
                }
            }
        }
        else if(c == LV_KEY_DOWN) {
            int32_t col_gap = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);
            /*Find the area below the current*/
            if(btnm->btn_id_sel == LV_BUTTONMATRIX_BUTTON_NONE) {
                btnm->btn_id_sel = 0;
                while(button_is_hidden(btnm->ctrl_bits[btnm->btn_id_sel]) || button_is_inactive(btnm->ctrl_bits[btnm->btn_id_sel])) {
                    btnm->btn_id_sel++;
                    if(btnm->btn_id_sel >= btnm->btn_cnt) {
                        btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
                        break;
                    }
                }
            }
            else {
                uint32_t area_below;
                int32_t pr_center =
                    btnm->button_areas[btnm->btn_id_sel].x1 + (lv_area_get_width(&btnm->button_areas[btnm->btn_id_sel]) >> 1);

                for(area_below = btnm->btn_id_sel; area_below < btnm->btn_cnt; area_below++) {
                    if(btnm->button_areas[area_below].y1 > btnm->button_areas[btnm->btn_id_sel].y1 &&
                       pr_center >= btnm->button_areas[area_below].x1 &&
                       pr_center <= btnm->button_areas[area_below].x2 + col_gap &&
                       button_is_inactive(btnm->ctrl_bits[area_below]) == false &&
                       button_is_hidden(btnm->ctrl_bits[area_below]) == false) {
                        break;
                    }
                }

                if(area_below < btnm->btn_cnt) btnm->btn_id_sel = area_below;
            }
        }
        else if(c == LV_KEY_UP) {
            int32_t col_gap = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);
            /*Find the area below the current*/
            if(btnm->btn_id_sel == LV_BUTTONMATRIX_BUTTON_NONE) {
                btnm->btn_id_sel = 0;
                while(button_is_hidden(btnm->ctrl_bits[btnm->btn_id_sel]) || button_is_inactive(btnm->ctrl_bits[btnm->btn_id_sel])) {
                    btnm->btn_id_sel++;
                    if(btnm->btn_id_sel >= btnm->btn_cnt) {
                        btnm->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
                        break;
                    }
                }
            }
            else {
                int16_t area_above;
                int32_t pr_center =
                    btnm->button_areas[btnm->btn_id_sel].x1 + (lv_area_get_width(&btnm->button_areas[btnm->btn_id_sel]) >> 1);

                for(area_above = btnm->btn_id_sel; area_above >= 0; area_above--) {
                    if(btnm->button_areas[area_above].y1 < btnm->button_areas[btnm->btn_id_sel].y1 &&
                       pr_center >= btnm->button_areas[area_above].x1 - col_gap &&
                       pr_center <= btnm->button_areas[area_above].x2 &&
                       button_is_inactive(btnm->ctrl_bits[area_above]) == false &&
                       button_is_hidden(btnm->ctrl_bits[area_above]) == false) {
                        break;
                    }
                }
                if(area_above >= 0) btnm->btn_id_sel = area_above;
            }
        }

        invalidate_button_area(obj, btnm->btn_id_sel);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }

}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    if(btnm->btn_cnt == 0) return;

    lv_layer_t * layer = lv_event_get_layer(e);
    obj->skip_trans = 1;

    lv_area_t area_obj;
    lv_obj_get_coords(obj, &area_obj);

    lv_area_t btn_area;

    uint32_t btn_i = 0;
    uint32_t txt_i = 0;

    lv_draw_rect_dsc_t draw_rect_dsc_act;
    lv_draw_label_dsc_t draw_label_dsc_act;

    lv_draw_rect_dsc_t draw_rect_dsc_def;
    lv_draw_label_dsc_t draw_label_dsc_def;

    lv_state_t state_ori = obj->state;
    obj->state = LV_STATE_DEFAULT;
    obj->skip_trans = 1;
    lv_draw_rect_dsc_init(&draw_rect_dsc_def);
    lv_draw_label_dsc_init(&draw_label_dsc_def);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_ITEMS, &draw_rect_dsc_def);
    lv_obj_init_draw_label_dsc(obj, LV_PART_ITEMS, &draw_label_dsc_def);
    obj->skip_trans = 0;
    obj->state = state_ori;

    int32_t ptop = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    int32_t pbottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    int32_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    int32_t pright = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);

#if LV_USE_ARABIC_PERSIAN_CHARS
    char txt_ap[256];
#endif

    for(btn_i = 0; btn_i < btnm->btn_cnt; btn_i++, txt_i++) {
        /*Search the next valid text in the map*/
        while(lv_strcmp(btnm->map_p[txt_i], "\n") == 0) {
            txt_i++;
        }

        /*Skip hidden buttons*/
        if(button_is_hidden(btnm->ctrl_bits[btn_i])) continue;

        /*Get the state of the button*/
        lv_state_t btn_state = LV_STATE_DEFAULT;
        if(button_get_checked(btnm->ctrl_bits[btn_i])) btn_state |= LV_STATE_CHECKED;

        if(button_is_inactive(btnm->ctrl_bits[btn_i])) btn_state |= LV_STATE_DISABLED;
        else if(btn_i == btnm->btn_id_sel) {
            if(state_ori & LV_STATE_PRESSED) btn_state |= LV_STATE_PRESSED;
            if(state_ori & LV_STATE_FOCUSED) btn_state |= LV_STATE_FOCUSED;
            if(state_ori & LV_STATE_FOCUS_KEY) btn_state |= LV_STATE_FOCUS_KEY;
            if(state_ori & LV_STATE_EDITED) btn_state |= LV_STATE_EDITED;
        }

        /*Get the button's area*/
        lv_area_copy(&btn_area, &btnm->button_areas[btn_i]);
        btn_area.x1 += area_obj.x1;
        btn_area.y1 += area_obj.y1;
        btn_area.x2 += area_obj.x1;
        btn_area.y2 += area_obj.y1;

        /*Set up the draw descriptors*/
        if(btn_state == LV_STATE_DEFAULT) {
            lv_memcpy(&draw_rect_dsc_act, &draw_rect_dsc_def, sizeof(lv_draw_rect_dsc_t));
            lv_memcpy(&draw_label_dsc_act, &draw_label_dsc_def, sizeof(lv_draw_label_dsc_t));
        }
        /*In other cases get the styles directly without caching them*/
        else {
            obj->state = btn_state;
            obj->skip_trans = 1;
            lv_draw_rect_dsc_init(&draw_rect_dsc_act);
            lv_draw_label_dsc_init(&draw_label_dsc_act);
            lv_obj_init_draw_rect_dsc(obj, LV_PART_ITEMS, &draw_rect_dsc_act);
            lv_obj_init_draw_label_dsc(obj, LV_PART_ITEMS, &draw_label_dsc_act);
            obj->state = state_ori;
            obj->skip_trans = 0;
        }

        draw_rect_dsc_act.base.id1 = btn_i;

        /*Remove borders on the edges if `LV_BORDER_SIDE_INTERNAL`*/
        if(draw_rect_dsc_act.border_side & LV_BORDER_SIDE_INTERNAL) {
            draw_rect_dsc_act.border_side = LV_BORDER_SIDE_FULL;
            if(btn_area.x1 == obj->coords.x1 + pleft) draw_rect_dsc_act.border_side &= ~LV_BORDER_SIDE_LEFT;
            if(btn_area.x2 == obj->coords.x2 - pright) draw_rect_dsc_act.border_side &= ~LV_BORDER_SIDE_RIGHT;
            if(btn_area.y1 == obj->coords.y1 + ptop) draw_rect_dsc_act.border_side &= ~LV_BORDER_SIDE_TOP;
            if(btn_area.y2 == obj->coords.y2 - pbottom) draw_rect_dsc_act.border_side &= ~LV_BORDER_SIDE_BOTTOM;
        }

        int32_t btn_height = lv_area_get_height(&btn_area);

        if((btn_state & LV_STATE_PRESSED) && (btnm->ctrl_bits[btn_i] & LV_BUTTONMATRIX_CTRL_POPOVER)) {
            /*Push up the upper boundary of the btn area to create the popover*/
            btn_area.y1 -= btn_height;
        }

        /*Draw the background*/
        lv_draw_rect(layer, &draw_rect_dsc_act, &btn_area);

        /*Calculate the size of the text*/
        const lv_font_t * font = draw_label_dsc_act.font;
        int32_t letter_space = draw_label_dsc_act.letter_space;
        int32_t line_space = draw_label_dsc_act.line_space;
        const char * txt = btnm->map_p[txt_i];

#if LV_USE_ARABIC_PERSIAN_CHARS
        /*Get the size of the Arabic text and process it*/
        size_t len_ap = lv_text_ap_calc_bytes_count(txt);
        if(len_ap < sizeof(txt_ap)) {
            lv_text_ap_proc(txt, txt_ap);
            txt = txt_ap;
        }
#endif
        lv_point_t txt_size;
        lv_text_get_size(&txt_size, txt, font, letter_space,
                         line_space, lv_area_get_width(&area_obj), draw_label_dsc_act.flag);

        btn_area.x1 += (lv_area_get_width(&btn_area) - txt_size.x) / 2;
        btn_area.y1 += (lv_area_get_height(&btn_area) - txt_size.y) / 2;
        btn_area.x2 = btn_area.x1 + txt_size.x;
        btn_area.y2 = btn_area.y1 + txt_size.y;

        if((btn_state & LV_STATE_PRESSED) && (btnm->ctrl_bits[btn_i] & LV_BUTTONMATRIX_CTRL_POPOVER)) {
            /*Push up the button text into the popover*/
            btn_area.y1 -= btn_height / 2;
            btn_area.y2 -= btn_height / 2;
        }

        /*Draw the text*/
        draw_label_dsc_act.text = txt;
        draw_label_dsc_act.text_local = true;
        draw_label_dsc_act.base.id1 = btn_i;
        lv_draw_label(layer, &draw_label_dsc_act, &btn_area);
    }

    obj->skip_trans = 0;
}
/**
 * Create the required number of buttons and control bytes according to a map
 * @param obj pointer to button matrix object
 * @param map_p pointer to a string array
 */
static void allocate_button_areas_and_controls(const lv_obj_t * obj, const char ** map)
{
    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    btnm->row_cnt = 1;
    /*Count the buttons in the map*/
    uint32_t btn_cnt = 0;
    uint32_t i       = 0;
    while(map[i] && map[i][0] != '\0') {
        if(lv_strcmp(map[i], "\n") != 0) { /*Do not count line breaks*/
            btn_cnt++;
        }
        else {
            btnm->row_cnt++;
        }
        i++;
    }

    /*Do not allocate memory for the same amount of buttons*/
    if(btn_cnt == btnm->btn_cnt) return;

    if(btnm->button_areas != NULL) {
        lv_free(btnm->button_areas);
        btnm->button_areas = NULL;
    }
    if(btnm->ctrl_bits != NULL) {
        lv_free(btnm->ctrl_bits);
        btnm->ctrl_bits = NULL;
    }

    btnm->button_areas = lv_malloc(sizeof(lv_area_t) * btn_cnt);
    LV_ASSERT_MALLOC(btnm->button_areas);
    btnm->ctrl_bits = lv_malloc(sizeof(lv_buttonmatrix_ctrl_t) * btn_cnt);
    LV_ASSERT_MALLOC(btnm->ctrl_bits);
    if(btnm->button_areas == NULL || btnm->ctrl_bits == NULL) btn_cnt = 0;

    lv_memzero(btnm->ctrl_bits, sizeof(lv_buttonmatrix_ctrl_t) * btn_cnt);

    btnm->btn_cnt = btn_cnt;
}

/**
 * Get the width of a button in units (default is 1).
 * @param ctrl_bits least significant 3 bits used (1..7 valid values)
 * @return the width of the button in units
 */
static uint32_t get_button_width(lv_buttonmatrix_ctrl_t ctrl_bits)
{
    uint32_t w = ctrl_bits & LV_BUTTONMATRIX_WIDTH_MASK;
    return w != 0 ? w : 1;
}

static bool button_is_hidden(lv_buttonmatrix_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BUTTONMATRIX_CTRL_HIDDEN;
}

static bool button_is_checked(lv_buttonmatrix_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BUTTONMATRIX_CTRL_CHECKED;
}

static bool button_is_repeat_disabled(lv_buttonmatrix_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BUTTONMATRIX_CTRL_NO_REPEAT;
}

static bool button_is_inactive(lv_buttonmatrix_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BUTTONMATRIX_CTRL_DISABLED;
}

static bool button_is_click_trig(lv_buttonmatrix_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BUTTONMATRIX_CTRL_CLICK_TRIG;
}

static bool button_is_popover(lv_buttonmatrix_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BUTTONMATRIX_CTRL_POPOVER;
}

static bool button_is_checkable(lv_buttonmatrix_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BUTTONMATRIX_CTRL_CHECKABLE;
}

static bool button_get_checked(lv_buttonmatrix_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BUTTONMATRIX_CTRL_CHECKED;
}

/**
 * Gives the button id of a button under a given point
 * @param obj pointer to a button matrix object
 * @param p a point with absolute coordinates
 * @return the id of the button or LV_BUTTONMATRIX_BUTTON_NONE.
 */
static uint32_t get_button_from_point(lv_obj_t * obj, lv_point_t * p)
{
    lv_area_t obj_cords;
    lv_area_t btn_area;
    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    uint32_t i;
    lv_obj_get_coords(obj, &obj_cords);

    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    int32_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    int32_t pright = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    int32_t ptop = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    int32_t pbottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    int32_t prow = lv_obj_get_style_pad_row(obj, LV_PART_MAIN);
    int32_t pcol = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

    /*Get the half gap. Button look larger with this value. (+1 for rounding error)*/
    prow = (prow / 2) + 1 + (prow & 1);
    pcol = (pcol / 2) + 1 + (pcol & 1);

    prow = LV_MIN(prow, BTN_EXTRA_CLICK_AREA_MAX);
    pcol = LV_MIN(pcol, BTN_EXTRA_CLICK_AREA_MAX);
    pright = LV_MIN(pright, BTN_EXTRA_CLICK_AREA_MAX);
    ptop = LV_MIN(ptop, BTN_EXTRA_CLICK_AREA_MAX);
    pbottom = LV_MIN(pbottom, BTN_EXTRA_CLICK_AREA_MAX);

    for(i = 0; i < btnm->btn_cnt; i++) {
        lv_area_copy(&btn_area, &btnm->button_areas[i]);
        if(btn_area.x1 <= pleft) btn_area.x1 += obj_cords.x1 - LV_MIN(pleft, BTN_EXTRA_CLICK_AREA_MAX);
        else btn_area.x1 += obj_cords.x1 - pcol;

        if(btn_area.y1 <= ptop) btn_area.y1 += obj_cords.y1 - LV_MIN(ptop, BTN_EXTRA_CLICK_AREA_MAX);
        else btn_area.y1 += obj_cords.y1 - prow;

        if(btn_area.x2 >= w - pright - 2) btn_area.x2 += obj_cords.x1 + LV_MIN(pright,
                                                                                   BTN_EXTRA_CLICK_AREA_MAX);  /*-2 for rounding error*/
        else btn_area.x2 += obj_cords.x1 + pcol;

        if(btn_area.y2 >= h - pbottom - 2) btn_area.y2 += obj_cords.y1 + LV_MIN(pbottom,
                                                                                    BTN_EXTRA_CLICK_AREA_MAX); /*-2 for rounding error*/
        else btn_area.y2 += obj_cords.y1 + prow;

        if(lv_area_is_point_on(&btn_area, p, 0) != false) {
            break;
        }
    }

    if(i == btnm->btn_cnt) i = LV_BUTTONMATRIX_BUTTON_NONE;

    return i;
}

static void invalidate_button_area(const lv_obj_t * obj, uint32_t btn_idx)
{
    if(btn_idx == LV_BUTTONMATRIX_BUTTON_NONE) return;

    lv_area_t btn_area;
    lv_area_t obj_area;

    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
    if(btn_idx >= btnm->btn_cnt) return;

    lv_area_copy(&btn_area, &btnm->button_areas[btn_idx]);
    lv_obj_get_coords(obj, &obj_area);

    /*The buttons might have outline and shadow so make the invalidation larger with the gaps between the buttons.
     *It assumes that the outline or shadow is smaller than the gaps*/
    int32_t row_gap = lv_obj_get_style_pad_row(obj, LV_PART_MAIN);
    int32_t col_gap = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

    /*Be sure to have a minimal extra space if row/col_gap is small*/
    int32_t dpi = lv_display_get_dpi(lv_obj_get_display(obj));
    row_gap = LV_MAX(row_gap, dpi / 10);
    col_gap = LV_MAX(col_gap, dpi / 10);

    /*Convert relative coordinates to absolute*/
    btn_area.x1 += obj_area.x1 - row_gap;
    btn_area.y1 += obj_area.y1 - col_gap;
    btn_area.x2 += obj_area.x1 + row_gap;
    btn_area.y2 += obj_area.y1 + col_gap;

    if((btn_idx == btnm->btn_id_sel) && (btnm->ctrl_bits[btn_idx] & LV_BUTTONMATRIX_CTRL_POPOVER)) {
        /*Push up the upper boundary of the btn area to also invalidate the popover*/
        btn_area.y1 -= lv_area_get_height(&btn_area);
    }

    lv_obj_invalidate_area(obj, &btn_area);
}

/**
 * Enforces a single button being toggled on the button matrix.
 * It simply clears the toggle flag on other buttons.
 * @param obj Button matrix object
 * @param btn_idx Button that should remain toggled
 */
static void make_one_button_checked(lv_obj_t * obj, uint32_t btn_idx)
{
    /*Save whether the button was toggled*/
    bool was_toggled = lv_buttonmatrix_has_button_ctrl(obj, btn_idx, LV_BUTTONMATRIX_CTRL_CHECKED);

    lv_buttonmatrix_clear_button_ctrl_all(obj, LV_BUTTONMATRIX_CTRL_CHECKED);

    if(was_toggled) lv_buttonmatrix_set_button_ctrl(obj, btn_idx, LV_BUTTONMATRIX_CTRL_CHECKED);
}

/**
 * Check if any of the buttons in the first row has the LV_BUTTONMATRIX_CTRL_POPOVER control flag set.
 * @param obj Button matrix object
 * @return true if at least one button has the flag, false otherwise
 */
static bool has_popovers_in_top_row(lv_obj_t * obj)
{
    lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;

    if(btnm->row_cnt <= 0) {
        return false;
    }

    const char ** map_row = btnm->map_p;
    uint32_t btn_cnt = 0;

    while(map_row[btn_cnt] && lv_strcmp(map_row[btn_cnt], "\n") != 0 && map_row[btn_cnt][0] != '\0') {
        if(button_is_popover(btnm->ctrl_bits[btn_cnt])) {
            return true;
        }
        btn_cnt++;
    }

    return false;
}

#endif
