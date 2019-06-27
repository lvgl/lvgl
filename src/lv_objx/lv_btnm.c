/**
 * @file lv_btnm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_btnm.h"
#if LV_USE_BTNM != 0

#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_refr.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_txt.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_btnm_signal(lv_obj_t * btnm, lv_signal_t sign, void * param);
static bool lv_btnm_design(lv_obj_t * btnm, const lv_area_t * mask, lv_design_mode_t mode);
static uint8_t get_button_width(lv_btnm_ctrl_t ctrl_bits);
static bool button_is_hidden(lv_btnm_ctrl_t ctrl_bits);
static bool button_is_repeat_disabled(lv_btnm_ctrl_t ctrl_bits);
static bool button_is_inactive(lv_btnm_ctrl_t ctrl_bits);
static bool button_is_click_trig(lv_btnm_ctrl_t ctrl_bits);
static bool button_is_tgl_enabled(lv_btnm_ctrl_t ctrl_bits);
static bool button_get_tgl_state(lv_btnm_ctrl_t ctrl_bits);
static uint16_t get_button_from_point(lv_obj_t * btnm, lv_point_t * p);
static void allocate_btn_areas_and_controls(const lv_obj_t * btnm, const char ** map);
static void invalidate_button_area(const lv_obj_t * btnm, uint16_t btn_idx);
static bool maps_are_identical(const char ** map1, const char ** map2);
static void make_one_button_toggled(lv_obj_t * btnm, uint16_t btn_idx);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * lv_btnm_def_map[] = {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""};

static lv_design_cb_t ancestor_design_f;
static lv_signal_cb_t ancestor_signal;

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
lv_obj_t * lv_btnm_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("button matrix create started");

    /*Create the ancestor object*/
    lv_obj_t * new_btnm = lv_obj_create(par, copy);
    lv_mem_assert(new_btnm);
    if(new_btnm == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_btnm);

    /*Allocate the object type specific extended data*/
    lv_btnm_ext_t * ext = lv_obj_allocate_ext_attr(new_btnm, sizeof(lv_btnm_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->btn_cnt                          = 0;
    ext->btn_id_pr                        = LV_BTNM_BTN_NONE;
    ext->btn_id_act                       = LV_BTNM_BTN_NONE;
    ext->button_areas                     = NULL;
    ext->ctrl_bits                        = NULL;
    ext->map_p                            = NULL;
    ext->recolor                          = 0;
    ext->one_toggle                       = 0;
    ext->styles_btn[LV_BTN_STATE_REL]     = &lv_style_btn_rel;
    ext->styles_btn[LV_BTN_STATE_PR]      = &lv_style_btn_pr;
    ext->styles_btn[LV_BTN_STATE_TGL_REL] = &lv_style_btn_tgl_rel;
    ext->styles_btn[LV_BTN_STATE_TGL_PR]  = &lv_style_btn_tgl_pr;
    ext->styles_btn[LV_BTN_STATE_INA]     = &lv_style_btn_ina;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_cb(new_btnm);

    lv_obj_set_signal_cb(new_btnm, lv_btnm_signal);
    lv_obj_set_design_cb(new_btnm, lv_btnm_design);

    /*Init the new button matrix object*/
    if(copy == NULL) {
        lv_obj_set_size(new_btnm, LV_DPI * 3, LV_DPI * 2);
        lv_btnm_set_map(new_btnm, lv_btnm_def_map);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BG, th->style.btnm.bg);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_REL, th->style.btnm.btn.rel);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_PR, th->style.btnm.btn.pr);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_TGL_REL, th->style.btnm.btn.tgl_rel);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_TGL_PR, th->style.btnm.btn.tgl_pr);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_INA, th->style.btnm.btn.ina);
        } else {
            lv_obj_set_style(new_btnm, &lv_style_pretty);
        }
    }
    /*Copy an existing object*/
    else {
        lv_btnm_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        memcpy(ext->styles_btn, copy_ext->styles_btn, sizeof(ext->styles_btn));
        lv_btnm_set_map(new_btnm, lv_btnm_get_map_array(copy));
    }

    LV_LOG_INFO("button matrix created");

    return new_btnm;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new map. Buttons will be created/deleted according to the map. The
 * button matrix keeps a reference to the map and so the string array must not
 * be deallocated during the life of the matrix.
 * @param btnm pointer to a button matrix object
 * @param map pointer a string array. The last string has to be: "". Use "\n" to make a line break.
 */
void lv_btnm_set_map(const lv_obj_t * btnm, const char * map[])
{
    if(map == NULL) return;

    /*
     * lv_btnm_set_map is called on receipt of signals such as
     * LV_SIGNAL_CORD_CHG regardless of whether the map has changed (e.g.
     * calling lv_obj_align on the map will trigger this).
     *
     * We check if the map has changed here to avoid overwriting changes made
     * to hidden/longpress/disabled states after the map was originally set.
     *
     * TODO: separate all map set/allocation from layout code below and skip
     * set/allocation when map hasn't changed.
     */
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    if(!maps_are_identical(ext->map_p, map)) {

        /*Analyze the map and create the required number of buttons*/
        allocate_btn_areas_and_controls(btnm, map);
    }
    ext->map_p = map;

    /*Set size and positions of the buttons*/
    const lv_style_t * style_bg = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BG);
    lv_coord_t max_w            = lv_obj_get_width(btnm) - style_bg->body.padding.left - style_bg->body.padding.right;
    lv_coord_t max_h            = lv_obj_get_height(btnm) - style_bg->body.padding.top - style_bg->body.padding.bottom;
    lv_coord_t act_y            = style_bg->body.padding.top;

    /*Count the lines to calculate button height*/
    uint8_t line_cnt = 1;
    uint8_t li;
    for(li = 0; strlen(map[li]) != 0; li++) {
        if(strcmp(map[li], "\n") == 0) line_cnt++;
    }

    lv_coord_t btn_h = max_h - ((line_cnt - 1) * style_bg->body.padding.inner);
    btn_h            = btn_h / line_cnt;
    btn_h--; /*-1 because e.g. height = 100 means 101 pixels (0..100)*/

    /* Count the units and the buttons in a line
     * (A button can be 1,2,3... unit wide)*/
    uint16_t unit_cnt;           /*Number of units in a row*/
    uint16_t unit_act_cnt;       /*Number of units currently put in a row*/
    uint16_t btn_cnt;            /*Number of buttons in a row*/
    uint16_t i_tot          = 0; /*Act. index in the str map*/
    uint16_t btn_i          = 0; /*Act. index of button areas*/
    const char ** map_p_tmp = map;

    /*Count the units and the buttons in a line*/
    while(1) {
        unit_cnt = 0;
        btn_cnt  = 0;
        /*Count the buttons in a line*/
        while(strcmp(map_p_tmp[btn_cnt], "\n") != 0 && strlen(map_p_tmp[btn_cnt]) != 0) { /*Check a line*/
            unit_cnt += get_button_width(ext->ctrl_bits[btn_i + btn_cnt]);
            btn_cnt++;
        }

        /*Make sure the last row is at the bottom of 'btnm'*/
        if(map_p_tmp[btn_cnt][0] == '\0') { /*Last row?*/
            btn_h = max_h - act_y + style_bg->body.padding.bottom - 1;
        }

        /*Only deal with the non empty lines*/
        if(btn_cnt != 0) {
            /*Calculate the width of all units*/
            lv_coord_t all_unit_w = max_w - ((btn_cnt - 1) * style_bg->body.padding.inner);

            /*Set the button size and positions and set the texts*/
            uint16_t i;
            lv_coord_t act_x = style_bg->body.padding.left;
            lv_coord_t act_unit_w;
            unit_act_cnt = 0;
            for(i = 0; i < btn_cnt; i++) {
                /* one_unit_w = all_unit_w / unit_cnt
                 * act_unit_w = one_unit_w * button_width
                 * do this two operations but the multiply first to divide a greater number */
                act_unit_w = (all_unit_w * get_button_width(ext->ctrl_bits[btn_i])) / unit_cnt;
                act_unit_w--; /*-1 because e.g. width = 100 means 101 pixels (0..100)*/

                /*Always recalculate act_x because of rounding errors */
                act_x = (unit_act_cnt * all_unit_w) / unit_cnt + i * style_bg->body.padding.inner +
                        style_bg->body.padding.left;

                /* Set the button's area.
                 * If inner padding is zero then use the prev. button x2 as x1 to avoid rounding
                 * errors*/
                if(style_bg->body.padding.inner == 0 && act_x != style_bg->body.padding.left) {
                    lv_area_set(&ext->button_areas[btn_i], ext->button_areas[btn_i - 1].x2, act_y, act_x + act_unit_w,
                                act_y + btn_h);
                } else {
                    lv_area_set(&ext->button_areas[btn_i], act_x, act_y, act_x + act_unit_w, act_y + btn_h);
                }

                unit_act_cnt += get_button_width(ext->ctrl_bits[btn_i]);

                i_tot++;
                btn_i++;
            }
        }
        act_y += btn_h + style_bg->body.padding.inner;

        if(strlen(map_p_tmp[btn_cnt]) == 0) break; /*Break on end of map*/
        map_p_tmp = &map_p_tmp[btn_cnt + 1];       /*Set the map to the next line*/
        i_tot++;                                   /*Skip the '\n'*/
    }

    lv_obj_invalidate(btnm);
}

/**
 * Set the button control map (hidden, disabled etc.) for a button matrix. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param btnm pointer to a button matrix object
 * @param ctrl_map pointer to an array of `lv_btn_ctrl_t` control bytes. The
 *                 length of the array and position of the elements must match
 *                 the number and order of the individual buttons (i.e. excludes
 *                 newline entries).
 *                 An element of the map should look like e.g.:
 *                 `ctrl_map[0] = width | LV_BTNM_CTRL_NO_REPEAT |  LV_BTNM_CTRL_TGL_ENABLE`
 */
void lv_btnm_set_ctrl_map(const lv_obj_t * btnm, const lv_btnm_ctrl_t ctrl_map[])
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    memcpy(ext->ctrl_bits, ctrl_map, sizeof(lv_btnm_ctrl_t) * ext->btn_cnt);

    lv_btnm_set_map(btnm, ext->map_p);
}

/**
 * Set the pressed button i.e. visually highlight it.
 * Mainly used a when the btnm is in a group to show the selected button
 * @param btnm pointer to button matrix object
 * @param id index of the currently pressed button (`LV_BTNM_BTN_NONE` to unpress)
 */
void lv_btnm_set_pressed(const lv_obj_t * btnm, uint16_t id)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    if(id >= ext->btn_cnt && id != LV_BTNM_BTN_NONE) return;

    if(id == ext->btn_id_pr) return;

    ext->btn_id_pr = id;
    lv_obj_invalidate(btnm);
}

/**
 * Set a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_btnm_set_style(lv_obj_t * btnm, lv_btnm_style_t type, const lv_style_t * style)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    switch(type) {
        case LV_BTNM_STYLE_BG: lv_obj_set_style(btnm, style); break;
        case LV_BTNM_STYLE_BTN_REL:
            ext->styles_btn[LV_BTN_STATE_REL] = style;
            lv_obj_invalidate(btnm);
            break;
        case LV_BTNM_STYLE_BTN_PR:
            ext->styles_btn[LV_BTN_STATE_PR] = style;
            lv_obj_invalidate(btnm);
            break;
        case LV_BTNM_STYLE_BTN_TGL_REL:
            ext->styles_btn[LV_BTN_STATE_TGL_REL] = style;
            lv_obj_invalidate(btnm);
            break;
        case LV_BTNM_STYLE_BTN_TGL_PR:
            ext->styles_btn[LV_BTN_STATE_TGL_PR] = style;
            lv_obj_invalidate(btnm);
            break;
        case LV_BTNM_STYLE_BTN_INA:
            ext->styles_btn[LV_BTN_STATE_INA] = style;
            lv_obj_invalidate(btnm);
            break;
    }
}

/**
 * Enable recoloring of button's texts
 * @param btnm pointer to button matrix object
 * @param en true: enable recoloring; false: disable
 */
void lv_btnm_set_recolor(const lv_obj_t * btnm, bool en)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    ext->recolor = en;
    lv_obj_invalidate(btnm);
}

/**
 * Set the attributes of a button of the button matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void lv_btnm_set_btn_ctrl(const lv_obj_t * btnm, uint16_t btn_id, lv_btnm_ctrl_t ctrl)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    if(btn_id >= ext->btn_cnt) return;

    ext->ctrl_bits[btn_id] |= ctrl;
    invalidate_button_area(btnm, btn_id);
}

/**
 * Clear the attributes of a button of the button matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void lv_btnm_clear_btn_ctrl(const lv_obj_t * btnm, uint16_t btn_id, lv_btnm_ctrl_t ctrl)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    if(btn_id >= ext->btn_cnt) return;

    ext->ctrl_bits[btn_id] &= (~ctrl);
    invalidate_button_area(btnm, btn_id);
}

/**
 * Set the attributes of all buttons of a button matrix
 * @param btnm pointer to a button matrix object
 * @param ctrl attribute(s) to set from `lv_btnm_ctrl_t`. Values can be ORed.
 */
void lv_btnm_set_btn_ctrl_all(lv_obj_t * btnm, lv_btnm_ctrl_t ctrl)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    uint16_t i;
    for(i = 0; i < ext->btn_cnt; i++) {
        lv_btnm_set_btn_ctrl(btnm, i, ctrl);
    }
}

/**
 * Clear the attributes of all buttons of a button matrix
 * @param btnm pointer to a button matrix object
 * @param ctrl attribute(s) to set from `lv_btnm_ctrl_t`. Values can be ORed.
 * @param en true: set the attributes; false: clear the attributes
 */
void lv_btnm_clear_btn_ctrl_all(lv_obj_t * btnm, lv_btnm_ctrl_t ctrl)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    uint16_t i;
    for(i = 0; i < ext->btn_cnt; i++) {
        lv_btnm_clear_btn_ctrl(btnm, i, ctrl);
    }
}

/**
 * Set a single buttons relative width.
 * This method will cause the matrix be regenerated and is a relatively
 * expensive operation. It is recommended that initial width be specified using
 * `lv_btnm_set_ctrl_map` and this method only be used for dynamic changes.
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify.
 * @param width Relative width compared to the buttons in the same row. [1..7]
 */
void lv_btnm_set_btn_width(const lv_obj_t * btnm, uint16_t btn_id, uint8_t width)
{

    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    if(btn_id >= ext->btn_cnt) return;
    ext->ctrl_bits[btn_id] &= (~LV_BTNM_WIDTH_MASK);
    ext->ctrl_bits[btn_id] |= (LV_BTNM_WIDTH_MASK & width);

    lv_btnm_set_map(btnm, ext->map_p);
}

/**
 * Make the button matrix like a selector widget (only one button may be toggled at a time).
 *
 * Toggling must be enabled on the buttons you want to be selected with `lv_btnm_set_ctrl` or
 * `lv_btnm_set_btn_ctrl_all`.
 *
 * @param btnm Button matrix object
 * @param one_toggle Whether "one toggle" mode is enabled
 */
void lv_btnm_set_one_toggle(lv_obj_t * btnm, bool one_toggle)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    ext->one_toggle     = one_toggle;

    /*If more than one button is toggled only the first one should be*/
    make_one_button_toggled(btnm, 0);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current map of a button matrix
 * @param btnm pointer to a button matrix object
 * @return the current map
 */
const char ** lv_btnm_get_map_array(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    return ext->map_p;
}

/**
 * Check whether the button's text can use recolor or not
 * @param btnm pointer to button matrix object
 * @return true: text recolor enable; false: disabled
 */
bool lv_btnm_get_recolor(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    return ext->recolor;
}

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb` to get the text of the button, check if hidden etc.
 * @param btnm pointer to button matrix object
 * @return  index of the last released button (LV_BTNM_BTN_NONE: if unset)
 */
uint16_t lv_btnm_get_active_btn(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    return ext->btn_id_act;
}

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`
 * @param btnm pointer to button matrix object
 * @return text of the last released button (NULL: if unset)
 */
const char * lv_btnm_get_active_btn_text(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    if(ext->btn_id_act != LV_BTNM_BTN_NONE) {
        return lv_btnm_get_btn_text(btnm, ext->btn_id_act);
    } else {
        return NULL;
    }
}

/**
 * Get the pressed button's index.
 * The button be really pressed by the user or manually set to pressed with `lv_btnm_set_pressed`
 * @param btnm pointer to button matrix object
 * @return  index of the pressed button (LV_BTNM_BTN_NONE: if unset)
 */
uint16_t lv_btnm_get_pressed_btn(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    return ext->btn_id_pr;
}

/**
 * Get the button's text
 * @param btnm pointer to button matrix object
 * @param btn_id the index a button not counting new line characters. (The return value of
 * lv_btnm_get_pressed/released)
 * @return  text of btn_index` button
 */
const char * lv_btnm_get_btn_text(const lv_obj_t * btnm, uint16_t btn_id)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
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
 * @param btnm pointer to a button matrix object
 * @param btn_id the index a button not counting new line characters. (E.g. the return value of
 * lv_btnm_get_pressed/released)
 * @param ctrl control values to check (ORed value can be used)
 * @return true: long press repeat is disabled; false: long press repeat enabled
 */
bool lv_btnm_get_btn_ctrl(lv_obj_t * btnm, uint16_t btn_id, lv_btnm_ctrl_t ctrl)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    if(btn_id >= ext->btn_cnt) return false;

    return ext->ctrl_bits[btn_id] & ctrl ? true : false;
}

/**
 * Get a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_btnm_get_style(const lv_obj_t * btnm, lv_btnm_style_t type)
{
    const lv_style_t * style = NULL;
    lv_btnm_ext_t * ext      = lv_obj_get_ext_attr(btnm);

    switch(type) {
        case LV_BTNM_STYLE_BG: style = lv_obj_get_style(btnm); break;
        case LV_BTNM_STYLE_BTN_REL: style = ext->styles_btn[LV_BTN_STATE_REL]; break;
        case LV_BTNM_STYLE_BTN_PR: style = ext->styles_btn[LV_BTN_STATE_PR]; break;
        case LV_BTNM_STYLE_BTN_TGL_REL: style = ext->styles_btn[LV_BTN_STATE_TGL_REL]; break;
        case LV_BTNM_STYLE_BTN_TGL_PR: style = ext->styles_btn[LV_BTN_STATE_TGL_PR]; break;
        case LV_BTNM_STYLE_BTN_INA: style = ext->styles_btn[LV_BTN_STATE_INA]; break;
        default: style = NULL; break;
    }

    return style;
}

/**
 * Find whether "one toggle" mode is enabled.
 * @param btnm Button matrix object
 * @return whether "one toggle" mode is enabled
 */
bool lv_btnm_get_one_toggle(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    return ext->one_toggle;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the button matrixs
 * @param btnm pointer to a button matrix object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_btnm_design(lv_obj_t * btnm, const lv_area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design_f(btnm, mask, mode);
        /*Return false if the object is not covers the mask_p area*/
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

        ancestor_design_f(btnm, mask, mode);

        lv_btnm_ext_t * ext         = lv_obj_get_ext_attr(btnm);
        const lv_style_t * bg_style = lv_obj_get_style(btnm);
        const lv_style_t * btn_style;
        lv_opa_t opa_scale = lv_obj_get_opa_scale(btnm);

        lv_area_t area_btnm;
        lv_obj_get_coords(btnm, &area_btnm);

        lv_area_t area_tmp;
        lv_coord_t btn_w;
        lv_coord_t btn_h;

        uint16_t btn_i = 0;
        uint16_t txt_i = 0;
        lv_style_t style_tmp;
        lv_txt_flag_t txt_flag = LV_TXT_FLAG_NONE;

        if(ext->recolor) txt_flag = LV_TXT_FLAG_RECOLOR;

        for(btn_i = 0; btn_i < ext->btn_cnt; btn_i++, txt_i++) {
            /*Search the next valid text in the map*/
            while(strcmp(ext->map_p[txt_i], "\n") == 0) {
                txt_i++;
            }

            /*Skip hidden buttons*/
            if(button_is_hidden(ext->ctrl_bits[btn_i])) continue;

            lv_area_copy(&area_tmp, &ext->button_areas[btn_i]);
            area_tmp.x1 += area_btnm.x1;
            area_tmp.y1 += area_btnm.y1;
            area_tmp.x2 += area_btnm.x1;
            area_tmp.y2 += area_btnm.y1;

            btn_w = lv_area_get_width(&area_tmp);
            btn_h = lv_area_get_height(&area_tmp);

            /*Load the style*/
            bool tgl_state = button_get_tgl_state(ext->ctrl_bits[btn_i]);
            if(button_is_inactive(ext->ctrl_bits[btn_i]))
                btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_INA);
            else if(btn_i != ext->btn_id_pr && tgl_state == false)
                btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_REL);
            else if(btn_i == ext->btn_id_pr && tgl_state == false)
                btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_PR);
            else if(btn_i != ext->btn_id_pr && tgl_state == true)
                btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_TGL_REL);
            else if(btn_i == ext->btn_id_pr && tgl_state == true)
                btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_TGL_PR);
            else
                btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_REL); /*Not possible option, just to be sure*/

            lv_style_copy(&style_tmp, btn_style);

            /*Remove borders on the edges if `LV_BORDER_INTERNAL`*/
            if(style_tmp.body.border.part & LV_BORDER_INTERNAL) {
                if(area_tmp.y1 == btnm->coords.y1 + bg_style->body.padding.top) {
                    style_tmp.body.border.part &= ~LV_BORDER_TOP;
                }
                if(area_tmp.y2 == btnm->coords.y2 - bg_style->body.padding.bottom) {
                    style_tmp.body.border.part &= ~LV_BORDER_BOTTOM;
                }

                if(txt_i == 0) {
                    style_tmp.body.border.part &= ~LV_BORDER_LEFT;
                } else if(strcmp(ext->map_p[txt_i - 1], "\n") == 0) {
                    style_tmp.body.border.part &= ~LV_BORDER_LEFT;
                }

                if(ext->map_p[txt_i + 1][0] == '\0' || strcmp(ext->map_p[txt_i + 1], "\n") == 0) {
                    style_tmp.body.border.part &= ~LV_BORDER_RIGHT;
                }
            }
            lv_draw_rect(&area_tmp, mask, &style_tmp, opa_scale);

            /*Calculate the size of the text*/
            if(btn_style->glass) btn_style = bg_style;
            const lv_font_t * font = btn_style->text.font;
            lv_point_t txt_size;
            lv_txt_get_size(&txt_size, ext->map_p[txt_i], font, btn_style->text.letter_space,
                            btn_style->text.line_space, lv_area_get_width(&area_btnm), txt_flag);

            area_tmp.x1 += (btn_w - txt_size.x) / 2;
            area_tmp.y1 += (btn_h - txt_size.y) / 2;
            area_tmp.x2 = area_tmp.x1 + txt_size.x;
            area_tmp.y2 = area_tmp.y1 + txt_size.y;

            lv_draw_label(&area_tmp, mask, btn_style, opa_scale, ext->map_p[txt_i], txt_flag, NULL, -1, -1, NULL);
        }
    }
    return true;
}

/**
 * Signal function of the button matrix
 * @param btnm pointer to a button matrix object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_btnm_signal(lv_obj_t * btnm, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(btnm, sign, param);
    if(res != LV_RES_OK) return res;

    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    lv_point_t p;
    if(sign == LV_SIGNAL_CLEANUP) {
        lv_mem_free(ext->button_areas);
        lv_mem_free(ext->ctrl_bits);
    } else if(sign == LV_SIGNAL_STYLE_CHG || sign == LV_SIGNAL_CORD_CHG) {
        lv_btnm_set_map(btnm, ext->map_p);
    } else if(sign == LV_SIGNAL_PRESSED) {
        lv_indev_t * indev = lv_indev_get_act();
        if(lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER || lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON) {
            uint16_t btn_pr;
            /*Search the pressed area*/
            lv_indev_get_point(param, &p);
            btn_pr = get_button_from_point(btnm, &p);

            invalidate_button_area(btnm, ext->btn_id_pr) /*Invalidate the old area*/;
            ext->btn_id_pr  = btn_pr;
            ext->btn_id_act = btn_pr;
            invalidate_button_area(btnm, ext->btn_id_pr); /*Invalidate the new area*/
        }
        if(ext->btn_id_act != LV_BTNM_BTN_NONE) {
            if(button_is_click_trig(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_inactive(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_hidden(ext->ctrl_bits[ext->btn_id_act]) == false) {
                uint32_t b = ext->btn_id_act;
                res        = lv_event_send(btnm, LV_EVENT_VALUE_CHANGED, &b);
            }
        }
    } else if(sign == LV_SIGNAL_PRESSING) {
        uint16_t btn_pr;
        /*Search the pressed area*/
        lv_indev_get_point(param, &p);
        btn_pr = get_button_from_point(btnm, &p);
        /*Invalidate to old and the new areas*/;
        if(btn_pr != ext->btn_id_pr) {
            lv_indev_reset_long_press(param); /*Start the log press time again on the new button*/
            if(ext->btn_id_pr != LV_BTNM_BTN_NONE) {
                invalidate_button_area(btnm, ext->btn_id_pr);
            }
            if(btn_pr != LV_BTNM_BTN_NONE) {
                uint32_t b = ext->btn_id_act;
                res        = lv_event_send(btnm, LV_EVENT_VALUE_CHANGED, &b);
                if(res == LV_RES_OK) {
                    invalidate_button_area(btnm, btn_pr);
                }
            }
        }

        ext->btn_id_pr  = btn_pr;
        ext->btn_id_act = btn_pr;
    } else if(sign == LV_SIGNAL_RELEASED) {
        if(ext->btn_id_pr != LV_BTNM_BTN_NONE) {
            /*Toggle the button if enabled*/
            if(button_is_tgl_enabled(ext->ctrl_bits[ext->btn_id_pr])) {
                if(button_get_tgl_state(ext->ctrl_bits[ext->btn_id_pr])) {
                    ext->ctrl_bits[ext->btn_id_pr] &= (~LV_BTNM_CTRL_TGL_STATE);
                } else {
                    ext->ctrl_bits[ext->btn_id_pr] |= LV_BTNM_CTRL_TGL_STATE;
                }
                if(ext->one_toggle) make_one_button_toggled(btnm, ext->btn_id_pr);
            }

            /*Invalidate to old pressed area*/;
            invalidate_button_area(btnm, ext->btn_id_pr);

#if LV_USE_GROUP
            /*Leave the clicked button when releases if this not the focused object in a group*/
            lv_group_t * g = lv_obj_get_group(btnm);
            if(lv_group_get_focused(g) != btnm) {
                ext->btn_id_pr = LV_BTNM_BTN_NONE;
            }
#else
            ext->btn_id_pr = LV_BTNM_BTN_NONE;
#endif

            if(button_is_click_trig(ext->ctrl_bits[ext->btn_id_act]) == true &&
               button_is_inactive(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_hidden(ext->ctrl_bits[ext->btn_id_act]) == false) {
                uint32_t b = ext->btn_id_act;
                res        = lv_event_send(btnm, LV_EVENT_VALUE_CHANGED, &b);
            }
        }
    } else if(sign == LV_SIGNAL_LONG_PRESS_REP) {
        if(ext->btn_id_act != LV_BTNM_BTN_NONE) {
            if(button_is_repeat_disabled(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_inactive(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_hidden(ext->ctrl_bits[ext->btn_id_act]) == false) {
                uint32_t b = ext->btn_id_act;
                res        = lv_event_send(btnm, LV_EVENT_VALUE_CHANGED, &b);
            }
        }
    } else if(sign == LV_SIGNAL_PRESS_LOST || sign == LV_SIGNAL_DEFOCUS) {
        ext->btn_id_pr  = LV_BTNM_BTN_NONE;
        ext->btn_id_act = LV_BTNM_BTN_NONE;
        lv_obj_invalidate(btnm);
    } else if(sign == LV_SIGNAL_FOCUS) {
#if LV_USE_GROUP
        lv_indev_t * indev         = lv_indev_get_act();
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type == LV_INDEV_TYPE_POINTER) {
            /*Select the clicked button*/
            lv_point_t p1;
            lv_indev_get_point(indev, &p1);
            uint16_t btn_i = get_button_from_point(btnm, &p1);
            ext->btn_id_pr = btn_i;

        } else if(indev_type == LV_INDEV_TYPE_ENCODER) {
            /*In navigation mode don't select any button but in edit mode select the fist*/
            if(lv_group_get_editing(lv_obj_get_group(btnm)))
                ext->btn_id_pr = 0;
            else
                ext->btn_id_pr = LV_BTNM_BTN_NONE;
        } else {
            ext->btn_id_pr = 0;
        }
#else
        ext->btn_id_pr = 0;
#endif

        ext->btn_id_act = ext->btn_id_pr;
        lv_obj_invalidate(btnm);
    } else if(sign == LV_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT) {
            if(ext->btn_id_pr == LV_BTNM_BTN_NONE)
                ext->btn_id_pr = 0;
            else
                ext->btn_id_pr++;
            if(ext->btn_id_pr >= ext->btn_cnt - 1) ext->btn_id_pr = ext->btn_cnt - 1;
            ext->btn_id_act = ext->btn_id_pr;
            lv_obj_invalidate(btnm);
        } else if(c == LV_KEY_LEFT) {
            if(ext->btn_id_pr == LV_BTNM_BTN_NONE) ext->btn_id_pr = 0;
            if(ext->btn_id_pr > 0) ext->btn_id_pr--;
            ext->btn_id_act = ext->btn_id_pr;
            lv_obj_invalidate(btnm);
        } else if(c == LV_KEY_DOWN) {
            const lv_style_t * style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BG);
            /*Find the area below the the current*/
            if(ext->btn_id_pr == LV_BTNM_BTN_NONE) {
                ext->btn_id_pr = 0;
            } else {
                uint16_t area_below;
                lv_coord_t pr_center =
                    ext->button_areas[ext->btn_id_pr].x1 + (lv_area_get_width(&ext->button_areas[ext->btn_id_pr]) >> 1);

                for(area_below = ext->btn_id_pr; area_below < ext->btn_cnt; area_below++) {
                    if(ext->button_areas[area_below].y1 > ext->button_areas[ext->btn_id_pr].y1 &&
                       pr_center >= ext->button_areas[area_below].x1 &&
                       pr_center <= ext->button_areas[area_below].x2 + style->body.padding.left) {
                        break;
                    }
                }

                if(area_below < ext->btn_cnt) ext->btn_id_pr = area_below;
            }
            ext->btn_id_act = ext->btn_id_pr;
            lv_obj_invalidate(btnm);
        } else if(c == LV_KEY_UP) {
            const lv_style_t * style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BG);
            /*Find the area below the the current*/
            if(ext->btn_id_pr == LV_BTNM_BTN_NONE) {
                ext->btn_id_pr = 0;
            } else {
                int16_t area_above;
                lv_coord_t pr_center =
                    ext->button_areas[ext->btn_id_pr].x1 + (lv_area_get_width(&ext->button_areas[ext->btn_id_pr]) >> 1);

                for(area_above = ext->btn_id_pr; area_above >= 0; area_above--) {
                    if(ext->button_areas[area_above].y1 < ext->button_areas[ext->btn_id_pr].y1 &&
                       pr_center >= ext->button_areas[area_above].x1 - style->body.padding.left &&
                       pr_center <= ext->button_areas[area_above].x2) {
                        break;
                    }
                }
                if(area_above >= 0) ext->btn_id_pr = area_above;
            }
            ext->btn_id_act = ext->btn_id_pr;
            lv_obj_invalidate(btnm);
        }
    } else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = true;
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_btnm";
    }

    return res;
}

/**
 * Create the required number of buttons and control bytes according to a map
 * @param btnm pointer to button matrix object
 * @param map_p pointer to a string array
 */
static void allocate_btn_areas_and_controls(const lv_obj_t * btnm, const char ** map)
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

    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    if(ext->button_areas != NULL) {
        lv_mem_free(ext->button_areas);
        ext->button_areas = NULL;
    }
    if(ext->ctrl_bits != NULL) {
        lv_mem_free(ext->ctrl_bits);
        ext->ctrl_bits = NULL;
    }

    ext->button_areas = lv_mem_alloc(sizeof(lv_area_t) * btn_cnt);
    lv_mem_assert(ext->button_areas);
    ext->ctrl_bits = lv_mem_alloc(sizeof(lv_btnm_ctrl_t) * btn_cnt);
    lv_mem_assert(ext->ctrl_bits);
    if(ext->button_areas == NULL || ext->ctrl_bits == NULL) btn_cnt = 0;

    memset(ext->ctrl_bits, 0, sizeof(lv_btnm_ctrl_t) * btn_cnt);

    ext->btn_cnt = btn_cnt;
}

/**
 * Get the width of a button in units (default is 1).
 * @param ctrl_bits least significant 3 bits used (1..7 valid values)
 * @return the width of the button in units
 */
static uint8_t get_button_width(lv_btnm_ctrl_t ctrl_bits)
{
    uint8_t w = ctrl_bits & LV_BTNM_WIDTH_MASK;
    return w != 0 ? w : 1;
}

static bool button_is_hidden(lv_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BTNM_CTRL_HIDDEN ? true : false;
}

static bool button_is_repeat_disabled(lv_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BTNM_CTRL_NO_REPEAT ? true : false;
}

static bool button_is_inactive(lv_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BTNM_CTRL_INACTIVE ? true : false;
}

static bool button_is_click_trig(lv_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BTNM_CTRL_CLICK_TRIG ? true : false;
}

static bool button_is_tgl_enabled(lv_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BTNM_CTRL_TGL_ENABLE ? true : false;
}

static bool button_get_tgl_state(lv_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & LV_BTNM_CTRL_TGL_STATE ? true : false;
}

/**
 * Gives the button id of a button under a given point
 * @param btnm pointer to a button matrix object
 * @param p a point with absolute coordinates
 * @return the id of the button or LV_BTNM_BTN_NONE.
 */
static uint16_t get_button_from_point(lv_obj_t * btnm, lv_point_t * p)
{
    lv_area_t btnm_cords;
    lv_area_t btn_area;
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    uint16_t i;
    lv_obj_get_coords(btnm, &btnm_cords);

    for(i = 0; i < ext->btn_cnt; i++) {
        lv_area_copy(&btn_area, &ext->button_areas[i]);
        btn_area.x1 += btnm_cords.x1;
        btn_area.y1 += btnm_cords.y1;
        btn_area.x2 += btnm_cords.x1;
        btn_area.y2 += btnm_cords.y1;
        if(lv_area_is_point_on(&btn_area, p) != false) {
            break;
        }
    }

    if(i == ext->btn_cnt) i = LV_BTNM_BTN_NONE;

    return i;
}

static void invalidate_button_area(const lv_obj_t * btnm, uint16_t btn_idx)
{
    if(btn_idx == LV_BTNM_BTN_NONE) return;

    lv_area_t btn_area;
    lv_area_t btnm_area;

    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    lv_area_copy(&btn_area, &ext->button_areas[btn_idx]);
    lv_obj_get_coords(btnm, &btnm_area);

    /* Convert relative coordinates to absolute */
    btn_area.x1 += btnm_area.x1;
    btn_area.y1 += btnm_area.y1;
    btn_area.x2 += btnm_area.x1;
    btn_area.y2 += btnm_area.y1;

    lv_inv_area(lv_obj_get_disp(btnm), &btn_area);
}

/**
 * Compares two button matrix maps for equality
 * @param map1 map to compare
 * @param map2 map to compare
 * @return true if maps are identical in length and content
 */
static bool maps_are_identical(const char ** map1, const char ** map2)
{
    if(map1 == map2) return true;
    if(map1 == NULL || map2 == NULL) return map1 == map2;

    uint16_t i = 0;
    while(map1[i][0] != '\0' && map2[i][0] != '\0') {
        if(strcmp(map1[i], map2[i]) != 0) return false;
        i++;
    }
    return map1[i][0] == '\0' && map2[i][0] == '\0';
}

/**
 * Enforces a single button being toggled on the button matrix.
 * It simply clears the toggle flag on other buttons.
 * @param btnm Button matrix object
 * @param btn_idx Button that should remain toggled
 */
static void make_one_button_toggled(lv_obj_t * btnm, uint16_t btn_idx)
{
    /*Save whether the button was toggled*/
    bool was_toggled = lv_btnm_get_btn_ctrl(btnm, btn_idx, LV_BTNM_CTRL_TGL_STATE);

    lv_btnm_clear_btn_ctrl_all(btnm, LV_BTNM_CTRL_TGL_STATE);

    if(was_toggled) lv_btnm_set_btn_ctrl(btnm, btn_idx, LV_BTNM_CTRL_TGL_STATE);
}

#endif
