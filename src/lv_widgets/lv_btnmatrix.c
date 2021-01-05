/**
 * @file lv_obj.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_btnmatrix.h"
#if LV_USE_BTNMATRIX != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_refr.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_txt_ap.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_btnmatrix"
#define BTN_EXTRA_CLICK_AREA_MAX (LV_DPI / 4)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_btnmatrix_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_btnmatrix_destructor(lv_obj_t * obj);

static lv_res_t lv_btnmatrix_signal(lv_obj_t * obj, lv_signal_t sign, void * param);
static lv_design_res_t lv_btnmatrix_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode);

static uint8_t get_button_width(lv_btnmatrix_ctrl_t ctrl_bits);
static bool button_is_hidden(lv_btnmatrix_ctrl_t ctrl_bits);
static bool button_is_repeat_disabled(lv_btnmatrix_ctrl_t ctrl_bits);
static bool button_is_inactive(lv_btnmatrix_ctrl_t ctrl_bits);
static bool button_is_click_trig(lv_btnmatrix_ctrl_t ctrl_bits);
static bool button_is_tgl_enabled(lv_btnmatrix_ctrl_t ctrl_bits);
static bool button_get_checked(lv_btnmatrix_ctrl_t ctrl_bits);
static uint16_t get_button_from_point(lv_obj_t * obj, lv_point_t * p);
static void allocate_btn_areas_and_controls(const lv_obj_t * obj, const char ** map);
static void invalidate_button_area(const lv_obj_t * obj, uint16_t btn_idx);
static void make_one_button_checked(lv_obj_t * obj, uint16_t btn_idx);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * lv_btnmatrix_def_map[] = {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""};

const lv_obj_class_t lv_btnmatrix = {
        .constructor = lv_btnmatrix_constructor,
        .destructor = lv_btnmatrix_destructor,
        .signal_cb = lv_btnmatrix_signal,
        .design_cb = lv_btnmatrix_design,
        .ext_size = sizeof(lv_btnmatrix_ext_t),
    };

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button matrix objects
 * @param par pointer to an object, it will be the parent of the new button matrix
 * @param copy pointer to a button matrix object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created button matrix
 */
lv_obj_t * lv_btnmatrix_create(lv_obj_t * parent, const lv_obj_t * copy)
{
  return lv_obj_create_from_class(&lv_btnmatrix, parent, copy);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new map. Buttons will be created/deleted according to the map. The
 * button matrix keeps a reference to the map and so the string array must not
 * be deallocated during the life of the matrix.
 * @param obj pointer to a button matrix object
 * @param map pointer a string array. The last string has to be: "". Use "\n" to make a line break.
 */
void lv_btnmatrix_set_map(lv_obj_t * obj, const char * map[])
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(map);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;

    /*Analyze the map and create the required number of buttons*/
    allocate_btn_areas_and_controls(obj, map);
    ext->map_p = map;

    lv_bidi_dir_t base_dir = lv_obj_get_base_dir(obj);

    /*Set size and positions of the buttons*/
    lv_coord_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t ptop = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t prow = lv_obj_get_style_pad_row(obj, LV_PART_MAIN);
    lv_coord_t pcol = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

    lv_coord_t max_w            = lv_obj_get_width_fit(obj);
    lv_coord_t max_h            = lv_obj_get_height_fit(obj);

    /*Count the lines to calculate button height*/
    uint8_t row_cnt = 1;
    uint32_t i;
    for(i = 0; strlen(map[i]) != 0; i++) {
        if(strcmp(map[i], "\n") == 0) row_cnt++;
    }

    /*Calculate the position of each row*/
    lv_coord_t max_h_no_gap = max_h - (prow * (row_cnt - 1));

    /* Count the units and the buttons in a line
     * (A button can be 1,2,3... unit wide)*/
    uint32_t txt_tot_i = 0; /*Act. index in the str map*/
    uint32_t btn_tot_i = 0; /*Act. index of button areas*/
    const char ** map_row = map;

    /*Count the units and the buttons in a line*/
    uint32_t row;
    for(row = 0; row < row_cnt; row++) {
        uint16_t unit_cnt = 0;           /*Number of units in a row*/
        uint16_t btn_cnt = 0;            /*Number of buttons in a row*/
        /*Count the buttons and units in this row*/
        while(strcmp(map_row[btn_cnt], "\n") != 0 && strlen(map_row[btn_cnt]) != '\0') {
            unit_cnt += get_button_width(ext->ctrl_bits[btn_tot_i + btn_cnt]);
            btn_cnt++;
        }

        /*Only deal with the non empty lines*/
        if(btn_cnt == 0) {
            map_row = &map_row[btn_cnt + 1];       /*Set the map to the next row*/
            continue;
        }

        lv_coord_t row_y1 = ptop + (max_h_no_gap * row) / row_cnt + row * prow;
        lv_coord_t row_y2 = ptop + (max_h_no_gap * (row + 1)) / row_cnt + row * prow - 1;

        /*Set the button size and positions*/
        lv_coord_t max_w_no_gap = max_w - (pcol * (btn_cnt - 1));
        if(max_w_no_gap < 0) max_w_no_gap = 0;

        uint32_t row_unit_cnt = 0;  /*The current unit position in the row*/
        uint32_t btn;
        for(btn = 0; btn < btn_cnt; btn++, btn_tot_i++, txt_tot_i++) {
            uint32_t btn_u = get_button_width(ext->ctrl_bits[btn_tot_i]);

            lv_coord_t btn_x1 = pleft + (max_w_no_gap * row_unit_cnt) / unit_cnt + btn * pcol;
            lv_coord_t btn_x2 = pleft + (max_w_no_gap * (row_unit_cnt + btn_u)) / unit_cnt + btn * pcol - 1;

            /*If RTL start from the right*/
            if(base_dir == LV_BIDI_DIR_RTL) {
                lv_coord_t tmp = btn_x1;
                btn_x1 = btn_x2;
                btn_x2 = tmp;
                btn_x1 = max_w - btn_x1;
                btn_x2 = max_w - btn_x2;
            }

            lv_area_set(&ext->button_areas[btn_tot_i], btn_x1, row_y1, btn_x2, row_y2);

            row_unit_cnt += btn_u;
        }

        map_row = &map_row[btn_cnt + 1];       /*Set the map to the next line*/
    }

    lv_obj_invalidate(obj);
}

/**
 * Set the button control map (hidden, disabled etc.) for a button matrix. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param obj pointer to a button matrix object
 * @param ctrl_map pointer to an array of `lv_btn_ctrl_t` control bytes. The
 *                 length of the array and position of the elements must match
 *                 the number and order of the individual buttons (i.e. excludes
 *                 newline entries).
 *                 An element of the map should look like e.g.:
 *                 `ctrl_map[0] = width | LV_BTNMATRIX_CTRL_NO_REPEAT |  LV_BTNMATRIX_CTRL_TGL_ENABLE`
 */
void lv_btnmatrix_set_ctrl_map(lv_obj_t * obj, const lv_btnmatrix_ctrl_t ctrl_map[])
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;
    _lv_memcpy(ext->ctrl_bits, ctrl_map, sizeof(lv_btnmatrix_ctrl_t) * ext->btn_cnt);

    lv_btnmatrix_set_map(obj, ext->map_p);
}

/**
 * Set the focused button i.e. visually highlight it.
 * @param obj pointer to button matrix object
 * @param id index of the button to focus(`LV_BTNMATRIX_BTN_NONE` to remove focus)
 */
void lv_btnmatrix_set_focused_btn(lv_obj_t * obj, uint16_t id)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;

    if(id >= ext->btn_cnt && id != LV_BTNMATRIX_BTN_NONE) return;

    if(id == ext->btn_id_focused) return;

    ext->btn_id_focused = id;
    lv_obj_invalidate(obj);
}


/**
 * Enable recoloring of button's texts
 * @param obj pointer to button matrix object
 * @param en true: enable recoloring; false: disable
 */
void lv_btnmatrix_set_recolor(const lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;

    ext->recolor = en;
    lv_obj_invalidate(obj);
}

/**
 * Set the attributes of a button of the button matrix
 * @param obj pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void lv_btnmatrix_set_btn_ctrl(lv_obj_t * obj, uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;

    if(btn_id >= ext->btn_cnt) return;

    if(ext->one_check && (ctrl & LV_BTNMATRIX_CTRL_CHECKED)) {
        lv_btnmatrix_clear_btn_ctrl_all(obj, LV_BTNMATRIX_CTRL_CHECKED);
    }

    ext->ctrl_bits[btn_id] |= ctrl;
    invalidate_button_area(obj, btn_id);
}

/**
 * Clear the attributes of a button of the button matrix
 * @param obj pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void lv_btnmatrix_clear_btn_ctrl(const lv_obj_t * obj, uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;

    if(btn_id >= ext->btn_cnt) return;

    ext->ctrl_bits[btn_id] &= (~ctrl);
    invalidate_button_area(obj, btn_id);
}

/**
 * Set the attributes of all buttons of a button matrix
 * @param obj pointer to a button matrix object
 * @param ctrl attribute(s) to set from `lv_btnmatrix_ctrl_t`. Values can be ORed.
 */
void lv_btnmatrix_set_btn_ctrl_all(lv_obj_t * obj, lv_btnmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    uint16_t i;
    for(i = 0; i < ext->btn_cnt; i++) {
        lv_btnmatrix_set_btn_ctrl(obj, i, ctrl);
    }
}

/**
 * Clear the attributes of all buttons of a button matrix
 * @param obj pointer to a button matrix object
 * @param ctrl attribute(s) to set from `lv_btnmatrix_ctrl_t`. Values can be ORed.
 * @param en true: set the attributes; false: clear the attributes
 */
void lv_btnmatrix_clear_btn_ctrl_all(lv_obj_t * obj, lv_btnmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    uint16_t i;
    for(i = 0; i < ext->btn_cnt; i++) {
        lv_btnmatrix_clear_btn_ctrl(obj, i, ctrl);
    }
}

/**
 * Set a single buttons relative width.
 * This method will cause the matrix be regenerated and is a relatively
 * expensive operation. It is recommended that initial width be specified using
 * `lv_btnmatrix_set_ctrl_map` and this method only be used for dynamic changes.
 * @param obj pointer to button matrix object
 * @param btn_id 0 based index of the button to modify.
 * @param width Relative width compared to the buttons in the same row. [1..7]
 */
void lv_btnmatrix_set_btn_width(lv_obj_t * obj, uint16_t btn_id, uint8_t width)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);


    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    if(btn_id >= ext->btn_cnt) return;
    ext->ctrl_bits[btn_id] &= (~LV_BTNMATRIX_WIDTH_MASK);
    ext->ctrl_bits[btn_id] |= (LV_BTNMATRIX_WIDTH_MASK & width);

    lv_btnmatrix_set_map(obj, ext->map_p);
}

/**
 * Make the button matrix like a selector widget (only one button may be checked at a time).
 * `Checkable` must be enabled on the buttons you want to be selected with `lv_btnmatrix_set_ctrl` or
 * `lv_btnmatrix_set_btn_ctrl_all`.
 * @param obj pointer to a button matrix object
 * @param one_chk whether "one check" mode is enabled
 */
void lv_btnmatrix_set_one_checked(lv_obj_t * obj, bool one_chk)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    ext->one_check     = one_chk;

    /*If more than one button is toggled only the first one should be*/
    make_one_button_checked(obj, 0);
}

/**
 * Set the align of the map text (left, right or center)
 * @param obj pointer to a btnmatrix object
 * @param align LV_LABEL_ALIGN_LEFT, LV_LABEL_ALIGN_RIGHT or LV_LABEL_ALIGN_CENTER
 */
void lv_btnmatrix_set_text_align(lv_obj_t * obj, lv_label_align_t align)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    if(ext->align == align) return;

    ext->align = align;

    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current map of a button matrix
 * @param obj pointer to a button matrix object
 * @return the current map
 */
const char ** lv_btnmatrix_get_map_array(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    return ext->map_p;
}

/**
 * Check whether the button's text can use recolor or not
 * @param obj pointer to button matrix object
 * @return true: text recolor enable; false: disabled
 */
bool lv_btnmatrix_get_recolor(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;

    return ext->recolor;
}

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb` to get the text of the button, check if hidden etc.
 * @param obj pointer to button matrix object
 * @return  index of the last released button (LV_BTNMATRIX_BTN_NONE: if unset)
 */
uint16_t lv_btnmatrix_get_active_btn(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    return ext->btn_id_act;
}

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`
 * @param obj pointer to button matrix object
 * @return text of the last released button (NULL: if unset)
 */
const char * lv_btnmatrix_get_active_btn_text(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    if(ext->btn_id_act != LV_BTNMATRIX_BTN_NONE) {
        return lv_btnmatrix_get_btn_text(obj, ext->btn_id_act);
    }
    else {
        return NULL;
    }
}

/**
 * Get the pressed button's index.
 * The button be really pressed by the user or manually set to pressed with `lv_btnmatrix_set_pressed`
 * @param obj pointer to button matrix object
 * @return  index of the pressed button (LV_BTNMATRIX_BTN_NONE: if unset)
 */
uint16_t lv_btnmatrix_get_focused_btn(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    return ext->btn_id_focused;
}

/**
 * Get the button's text
 * @param obj pointer to button matrix object
 * @param btn_id the index a button not counting new line characters. (The return value of
 * lv_btnmatrix_get_pressed/released)
 * @return  text of btn_index` button
 */
const char * lv_btnmatrix_get_btn_text(const lv_obj_t * obj, uint16_t btn_id)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    if(btn_id > ext->btn_cnt) return NULL;

    uint16_t txt_i = 0;
    uint16_t btn_i = 0;

    /* Search the text of ext->btn_pr the buttons text in the map
     * Skip "\n"-s*/
    while(btn_i != btn_id) {
        btn_i++;
        txt_i++;
        if(strcmp(ext->map_p[txt_i], "\n") == 0) txt_i++;
    }

    if(btn_i == ext->btn_cnt) return NULL;

    return ext->map_p[txt_i];
}

/**
 * Get the whether a control value is enabled or disabled for button of a button matrix
 * @param obj pointer to a button matrix object
 * @param btn_id the index a button not counting new line characters. (E.g. the return value of
 * lv_btnmatrix_get_pressed/released)
 * @param ctrl control values to check (ORed value can be used)
 * @return true: long press repeat is disabled; false: long press repeat enabled
 */
bool lv_btnmatrix_get_btn_ctrl(lv_obj_t * obj, uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    if(btn_id >= ext->btn_cnt) return false;

    return (ext->ctrl_bits[btn_id] & ctrl) ? true : false;
}


/**
 * Find whether "one check" mode is enabled.
 * @param obj Button matrix object
 * @return whether "one check" mode is enabled
 */
bool lv_btnmatrix_get_one_checked(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;

    return ext->one_check;
}

/**
 * Get the align attribute
 * @param obj pointer to a btnmatrix object
 * @return LV_LABEL_ALIGN_LEFT, LV_LABEL_ALIGN_RIGHT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_btnmatrix_get_align(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;

    lv_label_align_t align = ext->align;

    if(align == LV_LABEL_ALIGN_AUTO) {
#if LV_USE_BIDI
        lv_bidi_dir_t base_dir = lv_obj_get_base_dir(obj);
        if(base_dir == LV_BIDI_DIR_RTL) align = LV_LABEL_ALIGN_RIGHT;
        else align = LV_LABEL_ALIGN_LEFT;
#else
        align = LV_LABEL_ALIGN_LEFT;
#endif
    }

    return align;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_btnmatrix_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("lv_bar create started");

    LV_CLASS_CONSTRUCTOR_BEGIN(obj, lv_btnmatrix)
    lv_obj.constructor(obj, parent, copy);

    LV_LOG_TRACE("button matrix create started");

    lv_btnmatrix_ext_t * ext = obj->ext_attr;
    ext->btn_cnt        = 0;
    ext->btn_id_pr      = LV_BTNMATRIX_BTN_NONE;
    ext->btn_id_focused = LV_BTNMATRIX_BTN_NONE;
    ext->btn_id_act     = LV_BTNMATRIX_BTN_NONE;
    ext->button_areas   = NULL;
    ext->ctrl_bits      = NULL;
    ext->map_p          = NULL;
    ext->recolor        = 0;
    ext->one_check      = 0;

    /*Init the new button matrix object*/
    if(copy == NULL) {
        lv_btnmatrix_set_map(obj, lv_btnmatrix_def_map);
        lv_obj_set_size(obj, LV_DPI * 2, LV_DPI * 1);
    }
    /*Copy an existing object*/
    else {
        lv_btnmatrix_ext_t * copy_ext = copy->ext_attr;
        lv_btnmatrix_set_map(obj, copy_ext->map_p);
        lv_btnmatrix_set_ctrl_map(obj, copy_ext->ctrl_bits);
    }


    LV_CLASS_CONSTRUCTOR_END(obj, lv_bar)
    LV_LOG_INFO("button matrix created");
}

static void lv_btnmatrix_destructor(lv_obj_t * obj)
{

//    lv_mem_free(ext->button_areas);
//    lv_mem_free(ext->ctrl_bits);
}
/**
 * Handle the drawing related tasks of the button matrix
 * @param obj pointer to a button matrix object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_btnmatrix_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return lv_obj.design_cb(obj, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_obj.design_cb(obj, clip_area, mode);

        lv_btnmatrix_ext_t * ext         = obj->ext_attr;;
        if(ext->btn_cnt == 0) return LV_DESIGN_RES_OK;

        obj->style_list.skip_trans = 1;

        lv_area_t area_obj;
        lv_obj_get_coords(obj, &area_obj);

        lv_area_t btn_area;

        uint16_t btn_i = 0;
        uint16_t txt_i = 0;
        lv_txt_flag_t txt_flag = LV_TXT_FLAG_NONE;
        if(ext->recolor) txt_flag |= LV_TXT_FLAG_RECOLOR;
        lv_label_align_t align = lv_btnmatrix_get_align(obj);
        if(align == LV_LABEL_ALIGN_CENTER) txt_flag |= LV_TXT_FLAG_CENTER;
        if(align == LV_LABEL_ALIGN_RIGHT) txt_flag |= LV_TXT_FLAG_RIGHT;

        lv_draw_rect_dsc_t draw_rect_def_dsc;
        lv_draw_label_dsc_t draw_label_def_dsc;

        lv_draw_rect_dsc_t draw_rect_tmp_dsc;
        lv_draw_label_dsc_t draw_label_tmp_dsc;

        lv_state_t state_ori = obj->state;
        obj->state = LV_STATE_DEFAULT;
        obj->style_list.skip_trans = 1;
        lv_draw_rect_dsc_init(&draw_rect_def_dsc);
        lv_draw_label_dsc_init(&draw_label_def_dsc);
        lv_obj_init_draw_rect_dsc(obj, LV_PART_ITEM, &draw_rect_def_dsc);
        lv_obj_init_draw_label_dsc(obj, LV_PART_ITEM, &draw_label_def_dsc);
        draw_label_def_dsc.flag = txt_flag;
        obj->style_list.skip_trans = 0;
        obj->state = state_ori;

        lv_coord_t ptop = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        lv_coord_t pbottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
        lv_coord_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        lv_coord_t pright = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);

#if LV_USE_ARABIC_PERSIAN_CHARS
        const size_t txt_ap_size = 256 ;
        char * txt_ap = _lv_mem_buf_get(txt_ap_size);
#endif

        for(btn_i = 0; btn_i < ext->btn_cnt; btn_i++, txt_i++) {
            /*Search the next valid text in the map*/
            while(strcmp(ext->map_p[txt_i], "\n") == 0) {
                txt_i++;
            }

            /*Skip hidden buttons*/
            if(button_is_hidden(ext->ctrl_bits[btn_i])) continue;

            /*Get the state of the button*/
            lv_draw_rect_dsc_t * draw_rect_dsc_act;
            lv_draw_label_dsc_t * draw_label_dsc_act;
            lv_state_t btn_state = LV_STATE_DEFAULT;
            if(button_get_checked(ext->ctrl_bits[btn_i])) btn_state |= LV_STATE_CHECKED;
            if(button_is_inactive(ext->ctrl_bits[btn_i])) btn_state |= LV_STATE_DISABLED;
            if(btn_i == ext->btn_id_pr) btn_state |= LV_STATE_PRESSED;
            if(btn_i == ext->btn_id_focused) {
                btn_state |= LV_STATE_FOCUSED;
                if(state_ori & LV_STATE_EDITED) btn_state |= LV_STATE_EDITED;
            }

            /*Get the button's area*/
            lv_area_copy(&btn_area, &ext->button_areas[btn_i]);
            btn_area.x1 += area_obj.x1;
            btn_area.y1 += area_obj.y1;
            btn_area.x2 += area_obj.x1;
            btn_area.y2 += area_obj.y1;

            /*Use the custom drawer if any*/
            if(ext->custom_drawer_cb) {
                obj->state = btn_state;
                bool drawn = ext->custom_drawer_cb(obj, btn_i, &btn_area, clip_area);
                obj->state = state_ori;
                if(drawn) continue;
            }

            /*Set up the draw descriptors*/
            if(btn_state == LV_STATE_DEFAULT) {
                obj->state = btn_state;
                draw_rect_dsc_act = &draw_rect_def_dsc;
                draw_label_dsc_act = &draw_label_def_dsc;
                obj->state = state_ori;
            }
            /*In other cases get the styles directly without caching them*/
            else {
                obj->state = btn_state;
                lv_draw_rect_dsc_init(&draw_rect_tmp_dsc);
                lv_draw_label_dsc_init(&draw_label_tmp_dsc);
                lv_obj_init_draw_rect_dsc(obj, LV_PART_ITEM, &draw_rect_tmp_dsc);
                lv_obj_init_draw_label_dsc(obj, LV_PART_ITEM, &draw_label_tmp_dsc);
                draw_label_tmp_dsc.flag = txt_flag;
                draw_rect_dsc_act = &draw_rect_tmp_dsc;
                draw_label_dsc_act = &draw_label_tmp_dsc;
                obj->state = state_ori;
            }

            lv_coord_t border_part_ori = draw_rect_dsc_act->border_side;

            /*Remove borders on the edges if `LV_BORDER_INTERNAL`*/
            if(border_part_ori & LV_BORDER_SIDE_INTERNAL) {
                if(btn_area.x1 == obj->coords.x1 + pleft) draw_rect_dsc_act->border_side &= ~LV_BORDER_SIDE_LEFT;
                if(btn_area.y2 == obj->coords.x2 - pright) draw_rect_dsc_act->border_side &= ~LV_BORDER_SIDE_RIGHT;
                if(btn_area.y1 == obj->coords.y1 + ptop) draw_rect_dsc_act->border_side &= ~LV_BORDER_SIDE_TOP;
                if(btn_area.y2 == obj->coords.y2 - pbottom) draw_rect_dsc_act->border_side &= ~LV_BORDER_SIDE_BOTTOM;
            }

            /*Draw the background*/
            lv_draw_rect(&btn_area, clip_area, draw_rect_dsc_act);

            draw_rect_dsc_act->border_side = border_part_ori;   /*REstore the original border for the next button*/

            /*Calculate the size of the text*/
            const lv_font_t * font = draw_label_dsc_act->font;
            lv_coord_t letter_space = draw_label_dsc_act->letter_space;
            lv_coord_t line_space = draw_label_dsc_act->line_space;
            const char * txt = ext->map_p[txt_i];

#if LV_USE_ARABIC_PERSIAN_CHARS
            /*Get the size of the Arabic text and process it*/
            size_t len_ap = _lv_txt_ap_calc_bytes_cnt(txt);
            if(len_ap < txt_ap_size) {
                _lv_txt_ap_proc(txt, txt_ap);
                txt = txt_ap;
            }
#endif

            lv_point_t txt_size;
            _lv_txt_get_size(&txt_size, txt, font, letter_space,
                             line_space, lv_area_get_width(&area_obj), txt_flag);

            btn_area.x1 += (lv_area_get_width(&btn_area) - txt_size.x) / 2;
            btn_area.y1 += (lv_area_get_height(&btn_area) - txt_size.y) / 2;
            btn_area.x2 = btn_area.x1 + txt_size.x;
            btn_area.y2 = btn_area.y1 + txt_size.y;

            /*Draw the text*/
            lv_draw_label(&btn_area, clip_area, draw_label_dsc_act, txt, NULL);
        }

        obj->style_list.skip_trans = 0;
#if LV_USE_ARABIC_PERSIAN_CHARS
        _lv_mem_buf_release(txt_ap);
#endif
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        lv_obj.design_cb(obj, clip_area, mode);
    }
    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the button matrix
 * @param obj pointer to a button matrix object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_btnmatrix_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_obj.signal_cb(obj, sign, param);
    if(res != LV_RES_OK) return res;

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    lv_point_t p;

    if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_btnmatrix_set_map(obj, ext->map_p);
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        if(param &&
           (lv_obj_get_width(obj) != lv_area_get_width(param) || lv_obj_get_height(obj) != lv_area_get_height(param)))
        {
            lv_btnmatrix_set_map(obj, ext->map_p);
        }
    }
    else if(sign == LV_SIGNAL_PRESSED) {
        invalidate_button_area(obj, ext->btn_id_pr);

        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if(indev_type == LV_INDEV_TYPE_POINTER || indev_type == LV_INDEV_TYPE_BUTTON) {
            uint16_t btn_pr;
            /*Search the pressed area*/
            lv_indev_get_point(param, &p);
            btn_pr = get_button_from_point(obj, &p);
            /*Handle the case where there is no button there*/
            if(btn_pr != LV_BTNMATRIX_BTN_NONE) {
                if(button_is_inactive(ext->ctrl_bits[btn_pr]) == false &&
                   button_is_hidden(ext->ctrl_bits[btn_pr]) == false) {
                    invalidate_button_area(obj, ext->btn_id_pr) /*Invalidate the old area*/;
                    ext->btn_id_pr = btn_pr;
                    ext->btn_id_act = btn_pr;
                    invalidate_button_area(obj, ext->btn_id_pr); /*Invalidate the new area*/
                }
            }
        }
#if LV_USE_GROUP
        else if(indev_type == LV_INDEV_TYPE_KEYPAD || (indev_type == LV_INDEV_TYPE_ENCODER &&
                                                       lv_group_get_editing(lv_obj_get_group(obj)))) {
            ext->btn_id_pr = ext->btn_id_focused;
            invalidate_button_area(obj, ext->btn_id_focused);
        }
#endif

        if(ext->btn_id_pr != LV_BTNMATRIX_BTN_NONE) {
            if(button_is_click_trig(ext->ctrl_bits[ext->btn_id_pr]) == false &&
               button_is_inactive(ext->ctrl_bits[ext->btn_id_pr]) == false &&
               button_is_hidden(ext->ctrl_bits[ext->btn_id_pr]) == false) {
                uint32_t b = ext->btn_id_pr;
                res        = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &b);
            }
        }
    }
    else if(sign == LV_SIGNAL_PRESSING) {
        uint16_t btn_pr = LV_BTNMATRIX_BTN_NONE;
        /*Search the pressed area*/
        lv_indev_t * indev = lv_indev_get_act();
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type == LV_INDEV_TYPE_ENCODER || indev_type == LV_INDEV_TYPE_KEYPAD) return LV_RES_OK;

        lv_indev_get_point(indev, &p);
        btn_pr = get_button_from_point(obj, &p);
        /*Invalidate to old and the new areas*/
        if(btn_pr != ext->btn_id_pr) {
            if(ext->btn_id_pr != LV_BTNMATRIX_BTN_NONE) {
                invalidate_button_area(obj, ext->btn_id_pr);
            }

            ext->btn_id_pr  = btn_pr;
            ext->btn_id_act = btn_pr;

            lv_indev_reset_long_press(param); /*Start the log press time again on the new button*/
            if(btn_pr != LV_BTNMATRIX_BTN_NONE &&
               button_is_inactive(ext->ctrl_bits[btn_pr]) == false &&
               button_is_hidden(ext->ctrl_bits[btn_pr]) == false) {
                invalidate_button_area(obj, btn_pr);
                /* Send VALUE_CHANGED for the newly pressed button */
                if(button_is_click_trig(ext->ctrl_bits[btn_pr]) == false) {
                    uint32_t b = btn_pr;
                    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &b);
                }
            }
        }
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        if(ext->btn_id_pr != LV_BTNMATRIX_BTN_NONE) {
            /*Toggle the button if enabled*/
            if(button_is_tgl_enabled(ext->ctrl_bits[ext->btn_id_pr]) &&
               !button_is_inactive(ext->ctrl_bits[ext->btn_id_pr])) {
                if(button_get_checked(ext->ctrl_bits[ext->btn_id_pr]) && !ext->one_check) {
                    ext->ctrl_bits[ext->btn_id_pr] &= (~LV_BTNMATRIX_CTRL_CHECKED);
                }
                else {
                    ext->ctrl_bits[ext->btn_id_pr] |= LV_BTNMATRIX_CTRL_CHECKED;
                }
                if(ext->one_check) make_one_button_checked(obj, ext->btn_id_pr);
            }

            /*Invalidate to old pressed area*/;
            invalidate_button_area(obj, ext->btn_id_pr);
            invalidate_button_area(obj, ext->btn_id_focused);

            lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
            if(indev_type == LV_INDEV_TYPE_KEYPAD || indev_type == LV_INDEV_TYPE_ENCODER) {
                ext->btn_id_focused = ext->btn_id_pr;
            }

            ext->btn_id_pr = LV_BTNMATRIX_BTN_NONE;

            if(button_is_click_trig(ext->ctrl_bits[ext->btn_id_act]) == true &&
               button_is_inactive(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_hidden(ext->ctrl_bits[ext->btn_id_act]) == false) {
                uint32_t b = ext->btn_id_act;
                res        = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &b);
            }
        }
    }
    else if(sign == LV_SIGNAL_LONG_PRESS_REP) {
        if(ext->btn_id_act != LV_BTNMATRIX_BTN_NONE) {
            if(button_is_repeat_disabled(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_inactive(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_hidden(ext->ctrl_bits[ext->btn_id_act]) == false) {
                uint32_t b = ext->btn_id_act;
                res        = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &b);
            }
        }
    }
    else if(sign == LV_SIGNAL_PRESS_LOST) {
        ext->btn_id_pr  = LV_BTNMATRIX_BTN_NONE;
        ext->btn_id_act = LV_BTNMATRIX_BTN_NONE;
        lv_obj_invalidate(obj);
    }
    else if(sign == LV_SIGNAL_FOCUS) {
#if LV_USE_GROUP
        lv_indev_t * indev         = lv_indev_get_act();
        lv_indev_type_t indev_type = lv_indev_get_type(indev);

        /*If not focused by an input device assume the last input device*/
        if(indev == NULL) {
            indev = lv_indev_get_next(NULL);
            indev_type = lv_indev_get_type(indev);
        }

        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            /*In navigation mode don't select any button but in edit mode select the fist*/
            if(lv_group_get_editing(lv_obj_get_group(obj))) {
                uint32_t b = 0;
                while(button_is_hidden(ext->ctrl_bits[b]) || button_is_inactive(ext->ctrl_bits[b])) b++;
                ext->btn_id_focused = b;
                ext->btn_id_act = b;
            }
            else {
                ext->btn_id_focused = LV_BTNMATRIX_BTN_NONE;
            }
        }
        else if(indev_type == LV_INDEV_TYPE_KEYPAD) {
            uint32_t b = 0;
            while(button_is_hidden(ext->ctrl_bits[b]) || button_is_inactive(ext->ctrl_bits[b])) {
                b++;
            }
            ext->btn_id_focused = b;
            ext->btn_id_act = b;
        }

#endif
    }
    else if(sign == LV_SIGNAL_DEFOCUS || sign == LV_SIGNAL_LEAVE) {
        if(ext->btn_id_focused != LV_BTNMATRIX_BTN_NONE) invalidate_button_area(obj, ext->btn_id_focused);
        if(ext->btn_id_pr != LV_BTNMATRIX_BTN_NONE) invalidate_button_area(obj, ext->btn_id_pr);
        ext->btn_id_focused = LV_BTNMATRIX_BTN_NONE;
        ext->btn_id_pr = LV_BTNMATRIX_BTN_NONE;
        ext->btn_id_act = LV_BTNMATRIX_BTN_NONE;
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT) {
            if(ext->btn_id_focused == LV_BTNMATRIX_BTN_NONE)  ext->btn_id_focused = 0;
            else ext->btn_id_focused++;
            if(ext->btn_id_focused >= ext->btn_cnt) ext->btn_id_focused = 0;

            while(button_is_hidden(ext->ctrl_bits[ext->btn_id_focused]) || button_is_inactive(ext->ctrl_bits[ext->btn_id_focused])) {
                ext->btn_id_focused++;
                if(ext->btn_id_focused >= ext->btn_cnt) ext->btn_id_focused = 0;
            }

            ext->btn_id_act = ext->btn_id_focused;
            lv_obj_invalidate(obj);
        }
        else if(c == LV_KEY_LEFT) {
            if(ext->btn_id_focused == LV_BTNMATRIX_BTN_NONE) ext->btn_id_focused = 0;
            if(ext->btn_id_focused > 0) ext->btn_id_focused--;

            while(button_is_hidden(ext->ctrl_bits[ext->btn_id_focused]) || button_is_inactive(ext->ctrl_bits[ext->btn_id_focused])) {
                if(ext->btn_id_focused > 0) ext->btn_id_focused--;
                else ext->btn_id_focused = ext->btn_cnt - 1;
            }

            ext->btn_id_act = ext->btn_id_focused;
            lv_obj_invalidate(obj);
        }
        else if(c == LV_KEY_DOWN) {
            lv_coord_t col_gap = LV_MATH_MAX(lv_obj_get_style_margin_left(obj, LV_BTNMATRIX_PART_BTN), lv_obj_get_style_margin_right(obj, LV_BTNMATRIX_PART_BTN));

            /*Find the area below the the current*/
            if(ext->btn_id_focused == LV_BTNMATRIX_BTN_NONE) {
                ext->btn_id_focused = 0;
            }
            else {
                uint16_t area_below;
                lv_coord_t pr_center =
                    ext->button_areas[ext->btn_id_focused].x1 + (lv_area_get_width(&ext->button_areas[ext->btn_id_focused]) >> 1);

                for(area_below = ext->btn_id_focused; area_below < ext->btn_cnt; area_below++) {
                    if(ext->button_areas[area_below].y1 > ext->button_areas[ext->btn_id_focused].y1 &&
                       pr_center >= ext->button_areas[area_below].x1 &&
                       pr_center <= ext->button_areas[area_below].x2 + col_gap &&
                       button_is_inactive(ext->ctrl_bits[area_below]) == false &&
                       button_is_hidden(ext->ctrl_bits[area_below]) == false) {
                        break;
                    }
                }

                if(area_below < ext->btn_cnt) ext->btn_id_focused = area_below;
            }
            ext->btn_id_act = ext->btn_id_focused;
            lv_obj_invalidate(obj);
        }
        else if(c == LV_KEY_UP) {
            lv_coord_t col_gap = LV_MATH_MAX(lv_obj_get_style_margin_left(obj, LV_BTNMATRIX_PART_BTN), lv_obj_get_style_margin_right(obj, LV_BTNMATRIX_PART_BTN));
            /*Find the area below the the current*/
            if(ext->btn_id_focused == LV_BTNMATRIX_BTN_NONE) {
                ext->btn_id_focused = 0;
            }
            else {
                int16_t area_above;
                lv_coord_t pr_center =
                    ext->button_areas[ext->btn_id_focused].x1 + (lv_area_get_width(&ext->button_areas[ext->btn_id_focused]) >> 1);

                for(area_above = ext->btn_id_focused; area_above >= 0; area_above--) {
                    if(ext->button_areas[area_above].y1 < ext->button_areas[ext->btn_id_focused].y1 &&
                       pr_center >= ext->button_areas[area_above].x1 - col_gap &&
                       pr_center <= ext->button_areas[area_above].x2 &&
                       button_is_inactive(ext->ctrl_bits[area_above]) == false &&
                       button_is_hidden(ext->ctrl_bits[area_above]) == false) {
                        break;
                    }
                }
                if(area_above >= 0) ext->btn_id_focused = area_above;
            }
            ext->btn_id_act = ext->btn_id_focused;
            lv_obj_invalidate(obj);
        }
#endif
    }
    else if(sign == LV_SIGNAL_GET_EDITABLE) {
#if LV_USE_GROUP
        bool * editable = (bool *)param;
        *editable       = true;
#endif
    }
    return res;
}

/**
 * Create the required number of buttons and control bytes according to a map
 * @param obj pointer to button matrix object
 * @param map_p pointer to a string array
 */
static void allocate_btn_areas_and_controls(const lv_obj_t * obj, const char ** map)
{
    /*Count the buttons in the map*/
    uint16_t btn_cnt = 0;
    uint16_t i       = 0;
    while(strlen(map[i]) != 0) {
        if(strcmp(map[i], "\n") != 0) { /*Do not count line breaks*/
            btn_cnt++;
        }
        i++;
    }

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;

    /*Do not allocate memory for the same amount of buttons*/
    if(btn_cnt == ext->btn_cnt) return;


    if(ext->button_areas != NULL) {
        lv_mem_free(ext->button_areas);
        ext->button_areas = NULL;
    }
    if(ext->ctrl_bits != NULL) {
        lv_mem_free(ext->ctrl_bits);
        ext->ctrl_bits = NULL;
    }

    ext->button_areas = lv_mem_alloc(sizeof(lv_area_t) * btn_cnt);
    LV_ASSERT_MEM(ext->button_areas);
    ext->ctrl_bits = lv_mem_alloc(sizeof(lv_btnmatrix_ctrl_t) * btn_cnt);
    LV_ASSERT_MEM(ext->ctrl_bits);
    if(ext->button_areas == NULL || ext->ctrl_bits == NULL) btn_cnt = 0;

    _lv_memset_00(ext->ctrl_bits, sizeof(lv_btnmatrix_ctrl_t) * btn_cnt);

    ext->btn_cnt = btn_cnt;
}

/**
 * Get the width of a button in units (default is 1).
 * @param ctrl_bits least significant 3 bits used (1..7 valid values)
 * @return the width of the button in units
 */
static uint8_t get_button_width(lv_btnmatrix_ctrl_t ctrl_bits)
{
    uint8_t w = ctrl_bits & LV_BTNMATRIX_WIDTH_MASK;
    return w != 0 ? w : 1;
}

static bool button_is_hidden(lv_btnmatrix_ctrl_t ctrl_bits)
{
    return (ctrl_bits & LV_BTNMATRIX_CTRL_HIDDEN) ? true : false;
}

static bool button_is_repeat_disabled(lv_btnmatrix_ctrl_t ctrl_bits)
{
    return (ctrl_bits & LV_BTNMATRIX_CTRL_NO_REPEAT) ? true : false;
}

static bool button_is_inactive(lv_btnmatrix_ctrl_t ctrl_bits)
{
    return (ctrl_bits & LV_BTNMATRIX_CTRL_DISABLED) ? true : false;
}

static bool button_is_click_trig(lv_btnmatrix_ctrl_t ctrl_bits)
{
    return (ctrl_bits & LV_BTNMATRIX_CTRL_CLICK_TRIG) ? true : false;
}

static bool button_is_tgl_enabled(lv_btnmatrix_ctrl_t ctrl_bits)
{
    return (ctrl_bits & LV_BTNMATRIX_CTRL_CHECKABLE) ? true : false;
}

static bool button_get_checked(lv_btnmatrix_ctrl_t ctrl_bits)
{
    return (ctrl_bits & LV_BTNMATRIX_CTRL_CHECKED) ? true : false;
}

/**
 * Gives the button id of a button under a given point
 * @param obj pointer to a button matrix object
 * @param p a point with absolute coordinates
 * @return the id of the button or LV_BTNMATRIX_BTN_NONE.
 */
static uint16_t get_button_from_point(lv_obj_t * obj, lv_point_t * p)
{
    lv_area_t obj_cords;
    lv_area_t btn_area;
    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    uint16_t i;
    lv_obj_get_coords(obj, &obj_cords);

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_coord_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t pright = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t ptop = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t pbottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    lv_coord_t prow = lv_obj_get_style_pad_row(obj, LV_PART_MAIN);
    lv_coord_t pcol = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

    /*Get the half gap. Button look larger with this value. (+1 for rounding error)*/
    prow = (prow / 2) + 1 + (prow & 1);
    pcol = (pcol / 2) + 1 + (pcol & 1);

    prow = LV_MATH_MIN(prow, BTN_EXTRA_CLICK_AREA_MAX);
    pcol = LV_MATH_MIN(pcol, BTN_EXTRA_CLICK_AREA_MAX);
    pright = LV_MATH_MIN(pright, BTN_EXTRA_CLICK_AREA_MAX);
    ptop = LV_MATH_MIN(ptop, BTN_EXTRA_CLICK_AREA_MAX);
    pbottom = LV_MATH_MIN(pbottom, BTN_EXTRA_CLICK_AREA_MAX);

    for(i = 0; i < ext->btn_cnt; i++) {
        lv_area_copy(&btn_area, &ext->button_areas[i]);
        if(btn_area.x1 <= pleft) btn_area.x1 += obj_cords.x1 - LV_MATH_MIN(pleft, BTN_EXTRA_CLICK_AREA_MAX);
        else btn_area.x1 += obj_cords.x1 - pcol;

        if(btn_area.y1 <= ptop) btn_area.y1 += obj_cords.y1 - LV_MATH_MIN(ptop, BTN_EXTRA_CLICK_AREA_MAX);
        else btn_area.y1 += obj_cords.y1 - prow;

        if(btn_area.x2 >= w - pright - 2) btn_area.x2 += obj_cords.x1 + LV_MATH_MIN(pright,
                                                                                         BTN_EXTRA_CLICK_AREA_MAX);  /*-2 for rounding error*/
        else btn_area.x2 += obj_cords.x1 + pcol;

        if(btn_area.y2 >= h - pbottom - 2) btn_area.y2 += obj_cords.y1 + LV_MATH_MIN(pbottom,
                                                                                          BTN_EXTRA_CLICK_AREA_MAX); /*-2 for rounding error*/
        else btn_area.y2 += obj_cords.y1 + prow;

        if(_lv_area_is_point_on(&btn_area, p, 0) != false) {
            break;
        }
    }

    if(i == ext->btn_cnt) i = LV_BTNMATRIX_BTN_NONE;

    return i;
}

static void invalidate_button_area(const lv_obj_t * obj, uint16_t btn_idx)
{
    if(btn_idx == LV_BTNMATRIX_BTN_NONE) return;

    lv_area_t btn_area;
    lv_area_t obj_area;

    lv_btnmatrix_ext_t * ext = obj->ext_attr;;
    lv_area_copy(&btn_area, &ext->button_areas[btn_idx]);
    lv_obj_get_coords(obj, &obj_area);

    /* Convert relative coordinates to absolute */
    btn_area.x1 += obj_area.x1;
    btn_area.y1 += obj_area.y1;
    btn_area.x2 += obj_area.x1;
    btn_area.y2 += obj_area.y1;

    lv_obj_invalidate_area(obj, &btn_area);
}

/**
 * Enforces a single button being toggled on the button matrix.
 * It simply clears the toggle flag on other buttons.
 * @param obj Button matrix object
 * @param btn_idx Button that should remain toggled
 */
static void make_one_button_checked(lv_obj_t * obj, uint16_t btn_idx)
{
    /*Save whether the button was toggled*/
    bool was_toggled = lv_btnmatrix_get_btn_ctrl(obj, btn_idx, LV_BTNMATRIX_CTRL_CHECKED);

    lv_btnmatrix_clear_btn_ctrl_all(obj, LV_BTNMATRIX_CTRL_CHECKED);

    if(was_toggled) lv_btnmatrix_set_btn_ctrl(obj, btn_idx, LV_BTNMATRIX_CTRL_CHECKED);
}

#endif
