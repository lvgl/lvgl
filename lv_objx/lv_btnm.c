/**
 * @file lv_btnm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_btnm.h"
#if USE_LV_BTNM != 0

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
static uint8_t get_button_width(const char * btn_str);
static bool button_is_hidden(const char * btn_str);
static bool button_is_repeat_disabled(const char * btn_str);
static bool button_is_inactive(const char * btn_str);
const char * cut_ctrl_byte(const char * btn_str);
static uint16_t get_button_from_point(lv_obj_t * btnm, lv_point_t * p);
static uint16_t get_button_text(lv_obj_t * btnm, uint16_t btn_id);
static void allocate_btn_areas(lv_obj_t * btnm, const char ** map);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * lv_btnm_def_map[] = {"Btn1", "Btn2", "Btn3", "\n",
                                         "\002Btn4", "Btn5", ""
                                        };

static lv_design_func_t ancestor_design_f;
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button matrix objects
 * @param par pointer to an object, it will be the parent of the new button matrix
 * @param copy pointer to a button matrix object, if not NULL then the new object will be copied from it
 * @return pointer to the created button matrix
 */
lv_obj_t * lv_btnm_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("button matrix create started");

    /*Create the ancestor object*/
    lv_obj_t * new_btnm = lv_obj_create(par, copy);
    lv_mem_assert(new_btnm);
    if(new_btnm == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_btnm);

    /*Allocate the object type specific extended data*/
    lv_btnm_ext_t * ext = lv_obj_allocate_ext_attr(new_btnm, sizeof(lv_btnm_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->btn_cnt = 0;
    ext->btn_id_pr = LV_BTNM_PR_NONE;
    ext->btn_id_tgl = LV_BTNM_PR_NONE;
    ext->button_areas = NULL;
    ext->action = NULL;
    ext->map_p = NULL;
    ext->toggle = 0;
    ext->recolor = 0;
    ext->styles_btn[LV_BTN_STATE_REL] = &lv_style_btn_rel;
    ext->styles_btn[LV_BTN_STATE_PR] = &lv_style_btn_pr;
    ext->styles_btn[LV_BTN_STATE_TGL_REL] = &lv_style_btn_tgl_rel;
    ext->styles_btn[LV_BTN_STATE_TGL_PR] = &lv_style_btn_tgl_pr;
    ext->styles_btn[LV_BTN_STATE_INA] = &lv_style_btn_ina;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_func(new_btnm);

    lv_obj_set_signal_func(new_btnm, lv_btnm_signal);
    lv_obj_set_design_func(new_btnm, lv_btnm_design);

    /*Init the new button matrix object*/
    if(copy == NULL) {
        lv_obj_set_size(new_btnm, LV_HOR_RES / 2, LV_VER_RES / 4);
        lv_btnm_set_map(new_btnm, lv_btnm_def_map);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BG, th->btnm.bg);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_REL, th->btnm.btn.rel);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_PR, th->btnm.btn.pr);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_TGL_REL, th->btnm.btn.tgl_rel);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_TGL_PR, th->btnm.btn.tgl_pr);
            lv_btnm_set_style(new_btnm, LV_BTNM_STYLE_BTN_INA, th->btnm.btn.ina);
        } else {
            lv_obj_set_style(new_btnm, &lv_style_pretty);
        }
    }
    /*Copy an existing object*/
    else {
        lv_btnm_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        memcpy(ext->styles_btn, copy_ext->styles_btn, sizeof(ext->styles_btn));
        ext->action = copy_ext->action;
        ext->toggle = copy_ext->toggle;
        ext->btn_id_tgl = copy_ext->btn_id_tgl;
        lv_btnm_set_map(new_btnm, lv_btnm_get_map(copy));
    }

    LV_LOG_INFO("button matrix created");

    return new_btnm;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new map. Buttons will be created/deleted according to the map.
 * @param btnm pointer to a button matrix object
 * @param map pointer a string array. The last string has to be: "".
 *            Use "\n" to begin a new line.
 *            The first byte can be a control data:
 *             - bit 7: always 1
 *             - bit 6: always 0
 *             - bit 5: inactive (disabled) (\24x)
 *             - bit 4: no repeat (on long press) (\22x)
 *             - bit 3: hidden (\21x)
 *             - bit 2..0: button relative width
 *             Example (practically use octal numbers): "\224abc": "abc" text with 4 width and no long press
 */
void lv_btnm_set_map(lv_obj_t * btnm, const char ** map)
{
    if(map == NULL) return;

    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    ext->map_p = map;

    /*Analyze the map and create the required number of buttons*/
    allocate_btn_areas(btnm, map);

    /*Set size and positions of the buttons*/
    lv_style_t * style_bg = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BG);
    lv_coord_t max_w = lv_obj_get_width(btnm) - 2 * style_bg->body.padding.hor;
    lv_coord_t max_h = lv_obj_get_height(btnm) - 2 * style_bg->body.padding.ver;
    lv_coord_t act_y = style_bg->body.padding.ver;

    /*Count the lines to calculate button height*/
    uint8_t line_cnt = 1;
    uint8_t li;
    for(li = 0; strlen(map[li]) != 0; li++) {
        if(strcmp(map[li], "\n") == 0) line_cnt ++;
    }

    lv_coord_t btn_h = max_h - ((line_cnt - 1) * style_bg->body.padding.inner);
    btn_h = btn_h / line_cnt;
    btn_h --;                              /*-1 because e.g. height = 100 means 101 pixels (0..100)*/

    /* Count the units and the buttons in a line
     * (A button can be 1,2,3... unit wide)*/
    uint16_t unit_cnt;      /*Number of units in a row*/
    uint16_t unit_act_cnt;  /*Number of units currently put in a row*/
    uint16_t btn_cnt;       /*Number of buttons in a row*/
    uint16_t i_tot = 0;     /*Act. index in the str map*/
    uint16_t btn_i = 0;     /*Act. index of button areas*/
    const char  ** map_p_tmp = map;

    /*Count the units and the buttons in a line*/
    while(1) {
        unit_cnt = 0;
        btn_cnt = 0;
        /*Count the buttons in a line*/
        while(strcmp(map_p_tmp[btn_cnt], "\n") != 0 &&
                strlen(map_p_tmp[btn_cnt]) != 0) { /*Check a line*/
            unit_cnt += get_button_width(map_p_tmp[btn_cnt]);
            btn_cnt ++;
        }

        /*Make sure the last row is at the bottom of 'btnm'*/
        if(map_p_tmp[btn_cnt][0] == '\0') {         /*Last row?*/
            btn_h = max_h - act_y + style_bg->body.padding.ver - 1;
        }

        /*Only deal with the non empty lines*/
        if(btn_cnt != 0) {
            /*Calculate the width of all units*/
            lv_coord_t all_unit_w = max_w - ((btn_cnt - 1) * style_bg->body.padding.inner);

            /*Set the button size and positions and set the texts*/
            uint16_t i;
            lv_coord_t act_x = style_bg->body.padding.hor;
            lv_coord_t act_unit_w;
            unit_act_cnt = 0;
            for(i = 0; i < btn_cnt; i++) {
                /* one_unit_w = all_unit_w / unit_cnt
                 * act_unit_w = one_unit_w * button_width
                 * do this two operations but the multiply first to divide a greater number */
                act_unit_w = (all_unit_w * get_button_width(map_p_tmp[i])) / unit_cnt;
                act_unit_w --;                              /*-1 because e.g. width = 100 means 101 pixels (0..100)*/

                /*Always recalculate act_x because of rounding errors */
                act_x = (unit_act_cnt * all_unit_w) / unit_cnt + i * style_bg->body.padding.inner + style_bg->body.padding.hor;

                /* Set the button's area.
                 * If inner padding is zero then use the prev. button x2 as x1 to avoid rounding errors*/
                if(style_bg->body.padding.inner == 0 && act_x != style_bg->body.padding.hor) {
                    lv_area_set(&ext->button_areas[btn_i],  ext->button_areas[btn_i - 1].x2, act_y,
                                act_x + act_unit_w, act_y + btn_h);
                } else {
                    lv_area_set(&ext->button_areas[btn_i],  act_x, act_y,
                                act_x + act_unit_w, act_y + btn_h);
                }

                unit_act_cnt += get_button_width(map_p_tmp[i]);

                i_tot ++;
                btn_i ++;
            }
        }
        act_y += btn_h + style_bg->body.padding.inner;


        if(strlen(map_p_tmp[btn_cnt]) == 0) break; /*Break on end of map*/
        map_p_tmp = &map_p_tmp[btn_cnt + 1]; /*Set the map to the next line*/
        i_tot ++;   /*Skip the '\n'*/
    }

    lv_obj_invalidate(btnm);
}

/**
 * Set a new callback function for the buttons (It will be called when a button is released)
 * @param btnm: pointer to button matrix object
 * @param cb pointer to a callback function
 */
void lv_btnm_set_action(lv_obj_t * btnm, lv_btnm_action_t action)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    ext->action = action;
}

/**
 * Enable or disable button toggling
 * @param btnm pointer to button matrix object
 * @param en true: enable toggling; false: disable toggling
 * @param id index of the currently toggled button (ignored if 'en' == false)
 */
void lv_btnm_set_toggle(lv_obj_t * btnm, bool en, uint16_t id)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    ext->toggle = en == false ? 0 : 1;
    if(ext->toggle != 0) {
        if(id >= ext->btn_cnt) id = ext->btn_cnt - 1;
        ext->btn_id_tgl = id;
    } else {
        ext->btn_id_tgl = LV_BTNM_PR_NONE;
    }

    lv_obj_invalidate(btnm);
}

/**
 * Set a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_btnm_set_style(lv_obj_t * btnm, lv_btnm_style_t type, lv_style_t * style)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    switch(type) {
        case LV_BTNM_STYLE_BG:
            lv_obj_set_style(btnm, style);
            break;
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

void lv_btnm_set_recolor(const lv_obj_t * btnm, bool en)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    ext->recolor = en;
    lv_obj_invalidate(btnm);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current map of a button matrix
 * @param btnm pointer to a button matrix object
 * @return the current map
 */
const char ** lv_btnm_get_map(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    return ext->map_p;
}

/**
 * Get a the callback function of the buttons on a button matrix
 * @param btnm: pointer to button matrix object
 * @return pointer to the callback function
 */
lv_btnm_action_t lv_btnm_get_action(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    return ext->action;
}

/**
 * Get the pressed button
 * @param btnm pointer to button matrix object
 * @return  index of the currently pressed button (LV_BTNM_PR_NONE: if unset)
 */
uint16_t lv_btnm_get_pressed(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    return ext->btn_id_pr;
}

/**
 * Get the toggled button
 * @param btnm pointer to button matrix object
 * @return  index of the currently toggled button (LV_BTNM_PR_NONE: if unset)
 */
uint16_t lv_btnm_get_toggled(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    if(ext->toggle == 0) return LV_BTNM_PR_NONE;
    else return ext->btn_id_tgl;
}

/**
 * Get a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_btnm_get_style(const lv_obj_t * btnm, lv_btnm_style_t type)
{
    lv_style_t * style = NULL;
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    switch(type) {
        case LV_BTNM_STYLE_BG:
            style = lv_obj_get_style(btnm);
            break;
        case LV_BTNM_STYLE_BTN_REL:
            style = ext->styles_btn[LV_BTN_STATE_REL];
            break;
        case LV_BTNM_STYLE_BTN_PR:
            style = ext->styles_btn[LV_BTN_STATE_PR];
            break;
        case LV_BTNM_STYLE_BTN_TGL_REL:
            style = ext->styles_btn[LV_BTN_STATE_TGL_REL];
            break;
        case LV_BTNM_STYLE_BTN_TGL_PR:
            style = ext->styles_btn[LV_BTN_STATE_TGL_PR];
            break;
        case LV_BTNM_STYLE_BTN_INA:
            style = ext->styles_btn[LV_BTN_STATE_INA];
            break;
        default:
            style = NULL;
            break;
    }

    return style;
}

bool lv_btnm_get_recolor(const lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    return ext->recolor;
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

        lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
        lv_style_t * bg_style = lv_obj_get_style(btnm);
        lv_style_t * btn_style;
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

        for(btn_i = 0; btn_i < ext->btn_cnt; btn_i ++, txt_i ++) {
            /*Search the next valid text in the map*/
            while(strcmp(ext->map_p[txt_i], "\n") == 0) {
                txt_i ++;
            }

            /*Skip hidden buttons*/
            if(button_is_hidden(ext->map_p[txt_i])) continue;

            lv_area_copy(&area_tmp, &ext->button_areas[btn_i]);
            area_tmp.x1 += area_btnm.x1;
            area_tmp.y1 += area_btnm.y1;
            area_tmp.x2 += area_btnm.x1;
            area_tmp.y2 += area_btnm.y1;

            btn_w = lv_area_get_width(&area_tmp);
            btn_h = lv_area_get_height(&area_tmp);

            /*Load the style*/
            if(button_is_inactive(ext->map_p[txt_i])) btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_INA);
            else if(btn_i != ext->btn_id_pr && btn_i != ext->btn_id_tgl) btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_REL);
            else if(btn_i == ext->btn_id_pr && btn_i != ext->btn_id_tgl) btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_PR);
            else if(btn_i != ext->btn_id_pr && btn_i == ext->btn_id_tgl) btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_TGL_REL);
            else if(btn_i == ext->btn_id_pr && btn_i == ext->btn_id_tgl) btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_TGL_PR);
            else btn_style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BTN_REL);    /*Not possible option, just to be sure*/

            lv_style_copy(&style_tmp, btn_style);

            /*Remove borders on the edges if `LV_BORDER_INTERNAL`*/
            if(style_tmp.body.border.part & LV_BORDER_INTERNAL) {
                if(area_tmp.y1 == btnm->coords.y1 + bg_style->body.padding.ver) {
                    style_tmp.body.border.part &= ~LV_BORDER_TOP;
                }
                if(area_tmp.y2 == btnm->coords.y2 - bg_style->body.padding.ver) {
                    style_tmp.body.border.part &= ~LV_BORDER_BOTTOM;
                }

                if(txt_i == 0) {
                    style_tmp.body.border.part &= ~LV_BORDER_LEFT;
                }
                else if(strcmp(ext->map_p[txt_i - 1],"\n") == 0) {
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
            lv_txt_get_size(&txt_size, ext->map_p[txt_i], font,
                            btn_style->text.letter_space, btn_style->text.line_space,
                            lv_area_get_width(&area_btnm), txt_flag);

            area_tmp.x1 += (btn_w - txt_size.x) / 2;
            area_tmp.y1 += (btn_h - txt_size.y) / 2;
            area_tmp.x2 = area_tmp.x1 + txt_size.x;
            area_tmp.y2 = area_tmp.y1 + txt_size.y;

            lv_draw_label(&area_tmp, mask, btn_style, opa_scale,  ext->map_p[txt_i], txt_flag, NULL);
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
    lv_area_t btnm_area;
    lv_area_t btn_area;
    lv_point_t p;
    if(sign == LV_SIGNAL_CLEANUP) {
        lv_mem_free(ext->button_areas);
    } else if(sign == LV_SIGNAL_STYLE_CHG || sign == LV_SIGNAL_CORD_CHG) {
        lv_btnm_set_map(btnm, ext->map_p);
    } else if(sign == LV_SIGNAL_PRESSING) {
        uint16_t btn_pr;
        /*Search the pressed area*/
        lv_indev_get_point(param, &p);
        btn_pr = get_button_from_point(btnm, &p);
        /*Invalidate to old and the new areas*/;
        lv_obj_get_coords(btnm, &btnm_area);
        if(btn_pr != ext->btn_id_pr) {
            lv_indev_reset_lpr(param);
            if(ext->btn_id_pr != LV_BTNM_PR_NONE) {
                lv_area_copy(&btn_area, &ext->button_areas[ext->btn_id_pr]);
                btn_area.x1 += btnm_area.x1;
                btn_area.y1 += btnm_area.y1;
                btn_area.x2 += btnm_area.x1;
                btn_area.y2 += btnm_area.y1;
                lv_inv_area(&btn_area);
            }
            if(btn_pr != LV_BTNM_PR_NONE) {
                lv_area_copy(&btn_area, &ext->button_areas[btn_pr]);
                btn_area.x1 += btnm_area.x1;
                btn_area.y1 += btnm_area.y1;
                btn_area.x2 += btnm_area.x1;
                btn_area.y2 += btnm_area.y1;
                lv_inv_area(&btn_area);
            }
        }

        ext->btn_id_pr = btn_pr;
    }

    else if(sign == LV_SIGNAL_LONG_PRESS_REP) {
        if(ext->action && ext->btn_id_pr != LV_BTNM_PR_NONE) {
            uint16_t txt_i = get_button_text(btnm, ext->btn_id_pr);
            if(txt_i != LV_BTNM_PR_NONE) {
                if(button_is_repeat_disabled(ext->map_p[txt_i]) == false &&
                        button_is_inactive(ext->map_p[txt_i]) == false) {
                    res = ext->action(btnm, cut_ctrl_byte(ext->map_p[txt_i]));
                }
            }
        }
    } else if(sign == LV_SIGNAL_RELEASED) {
        if(ext->btn_id_pr != LV_BTNM_PR_NONE) {
            uint16_t txt_i = get_button_text(btnm, ext->btn_id_pr);
            if(button_is_inactive(ext->map_p[txt_i]) == false && txt_i != LV_BTNM_PR_NONE) {        /*Ignore the inactive buttons anf click between the buttons*/
                if(ext->action) res = ext->action(btnm, cut_ctrl_byte(ext->map_p[txt_i]));
                if(res == LV_RES_OK) {

                    /*Invalidate to old pressed area*/;
                    lv_obj_get_coords(btnm, &btnm_area);
                    lv_area_copy(&btn_area, &ext->button_areas[ext->btn_id_pr]);
                    btn_area.x1 += btnm_area.x1;
                    btn_area.y1 += btnm_area.y1;
                    btn_area.x2 += btnm_area.x1;
                    btn_area.y2 += btnm_area.y1;
                    lv_inv_area(&btn_area);

                    if(ext->toggle != 0) {
                        /*Invalidate to old toggled area*/;
                        lv_area_copy(&btn_area, &ext->button_areas[ext->btn_id_tgl]);
                        btn_area.x1 += btnm_area.x1;
                        btn_area.y1 += btnm_area.y1;
                        btn_area.x2 += btnm_area.x1;
                        btn_area.y2 += btnm_area.y1;
                        lv_inv_area(&btn_area);
                        ext->btn_id_tgl = ext->btn_id_pr;

                    }

        #if USE_LV_GROUP
                    /*Leave the clicked button when releases if this not the focused object in a group*/
                    lv_group_t * g = lv_obj_get_group(btnm);
                    if(lv_group_get_focused(g) != btnm) {
                        ext->btn_id_pr = LV_BTNM_PR_NONE;
                    }
        #else
                    ext->btn_id_pr = LV_BTNM_PR_NONE;
        #endif

                }
            }
        }
    } else if(sign == LV_SIGNAL_PRESS_LOST || sign == LV_SIGNAL_DEFOCUS) {
        ext->btn_id_pr = LV_BTNM_PR_NONE;
        lv_obj_invalidate(btnm);
    } else if(sign == LV_SIGNAL_FOCUS) {
#if USE_LV_GROUP
        lv_indev_t * indev = lv_indev_get_act();
        lv_hal_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type == LV_INDEV_TYPE_POINTER) {
            /*Select the clicked button*/
            lv_point_t p1;
            lv_indev_get_point(indev, &p1);
            uint16_t btn_i = get_button_from_point(btnm, &p1);
            ext->btn_id_pr = btn_i;
        } else  if(indev_type == LV_INDEV_TYPE_ENCODER) {
            /*In navigation mode don't select any button but in edit mode select the fist*/
            if(lv_group_get_editing(lv_obj_get_group(btnm))) ext->btn_id_pr = 0;
            else ext->btn_id_pr = LV_BTNM_PR_NONE;
        } else {
            ext->btn_id_pr = 0;
        }
#else
        ext->btn_id_pr = 0;
#endif
        lv_obj_invalidate(btnm);
    } else if(sign == LV_SIGNAL_CONTROLL) {
        char c = *((char *)param);
        if(c == LV_GROUP_KEY_RIGHT) {
            if(ext->btn_id_pr  == LV_BTNM_PR_NONE) ext->btn_id_pr = 0;
            else ext->btn_id_pr++;
            if(ext->btn_id_pr >= ext->btn_cnt - 1) ext->btn_id_pr = ext->btn_cnt - 1;
            lv_obj_invalidate(btnm);
        } else if(c == LV_GROUP_KEY_LEFT) {
            if(ext->btn_id_pr  == LV_BTNM_PR_NONE) ext->btn_id_pr = 0;
            if(ext->btn_id_pr > 0) ext->btn_id_pr--;
            lv_obj_invalidate(btnm);
        } else if(c == LV_GROUP_KEY_DOWN) {
            lv_style_t * style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BG);
            /*Find the area below the the current*/
            if(ext->btn_id_pr  == LV_BTNM_PR_NONE) {
                ext->btn_id_pr = 0;
            } else {
                uint16_t area_below;
                lv_coord_t pr_center = ext->button_areas[ext->btn_id_pr].x1 + (lv_area_get_width(&ext->button_areas[ext->btn_id_pr]) >> 1);

                for(area_below = ext->btn_id_pr; area_below < ext->btn_cnt; area_below ++) {
                    if(ext->button_areas[area_below].y1 >  ext->button_areas[ext->btn_id_pr].y1 &&
                            pr_center >=  ext->button_areas[area_below].x1 &&
                            pr_center <=  ext->button_areas[area_below].x2 + style->body.padding.hor) {
                        break;
                    }
                }

                if(area_below < ext->btn_cnt) ext->btn_id_pr = area_below;
            }
            lv_obj_invalidate(btnm);
        } else if(c == LV_GROUP_KEY_UP) {
            lv_style_t * style = lv_btnm_get_style(btnm, LV_BTNM_STYLE_BG);
            /*Find the area below the the current*/
            if(ext->btn_id_pr  == LV_BTNM_PR_NONE) {
                ext->btn_id_pr = 0;
            } else {
                int16_t area_above;
                lv_coord_t pr_center = ext->button_areas[ext->btn_id_pr].x1 + (lv_area_get_width(&ext->button_areas[ext->btn_id_pr]) >> 1);

                for(area_above = ext->btn_id_pr; area_above >= 0; area_above --) {
                    if(ext->button_areas[area_above].y1 < ext->button_areas[ext->btn_id_pr].y1 &&
                            pr_center >=  ext->button_areas[area_above].x1 - style->body.padding.hor &&
                            pr_center <=  ext->button_areas[area_above].x2) {
                        break;
                    }
                }
                if(area_above >= 0) ext->btn_id_pr = area_above;

            }
            lv_obj_invalidate(btnm);
        } else if(c == LV_GROUP_KEY_ENTER) {
            if(ext->action != NULL) {
                uint16_t txt_i = get_button_text(btnm, ext->btn_id_pr);
                if(txt_i != LV_BTNM_PR_NONE) {
                    res = ext->action(btnm, cut_ctrl_byte(ext->map_p[txt_i]));
                }
            }
        }
    } else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable = true;
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_btnm";
    }


    return res;
}

/**
 * Create the required number of buttons according to a map
 * @param btnm pointer to button matrix object
 * @param map_p pointer to a string array
 */
static void allocate_btn_areas(lv_obj_t * btnm, const char ** map)
{
    /*Count the buttons in the map*/
    uint16_t btn_cnt = 0;
    uint16_t i = 0;
    while(strlen(map[i]) != 0) {
        if(strcmp(map[i], "\n") != 0) { /*Do not count line breaks*/
            btn_cnt ++;
        }
        i++;
    }

    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);

    if(ext->button_areas != NULL) {
        lv_mem_free(ext->button_areas);
        ext->button_areas = NULL;
    }

    ext->button_areas = lv_mem_alloc(sizeof(lv_area_t) * btn_cnt);
    lv_mem_assert(ext->button_areas);
    if(ext->button_areas == NULL) btn_cnt = 0;

    ext->btn_cnt = btn_cnt;
}

/**
 * Get the width of a button in units. It comes from the first "letter".
 * @param btn_str The descriptor string of a button. E.g. "apple" or "\004banana"
 * @return the width of the button in units
 */
static uint8_t get_button_width(const char * btn_str)
{
    if((btn_str[0] & LV_BTNM_CTRL_MASK) == LV_BTNM_CTRL_CODE) {
        return btn_str[0] & LV_BTNM_WIDTH_MASK;
    }

    return 1;   /*Default width is 1*/
}

static bool button_is_hidden(const char * btn_str)
{
    /*If control byte presents and hidden bit is '1' then the button is hidden*/
    if(((btn_str[0] & LV_BTNM_CTRL_MASK) == LV_BTNM_CTRL_CODE) &&
            (btn_str[0] & LV_BTNM_HIDE_MASK)) {
        return true;
    }

    return false;
}

static bool button_is_repeat_disabled(const char * btn_str)
{
    /*If control byte presents and hidden bit is '1' then the button is hidden*/
    if(((btn_str[0] & LV_BTNM_CTRL_MASK) == LV_BTNM_CTRL_CODE) &&
            (btn_str[0] & LV_BTNM_REPEAT_DISABLE_MASK)) {
        return true;
    }

    return false;
}

static bool button_is_inactive(const char * btn_str)
{
    /*If control byte presents and hidden bit is '1' then the button is hidden*/
    if(((btn_str[0] & LV_BTNM_CTRL_MASK) == LV_BTNM_CTRL_CODE) &&
            (btn_str[0] & LV_BTNM_INACTIVE_MASK)) {
        return true;
    }

    return false;
}


const char * cut_ctrl_byte(const char * btn_str)
{
    /*Cut the control byte if present*/
    if((btn_str[0] & LV_BTNM_CTRL_MASK) == LV_BTNM_CTRL_CODE) return &btn_str[1];
    else return btn_str;
}

/**
 * Gives the button id of a button under a given point
 * @param btnm pointer to a button matrix object
 * @param p a point with absolute coordinates
 * @return the id of the button or LV_BTNM_PR_NONE.
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

    if(i == ext->btn_cnt) i = LV_BTNM_PR_NONE;

    return i;
}

/**
 * Get the text of a button
 * @param btnm pointer to a button matrix object
 * @param btn_id button id
 * @return text id in ext->map_p or LV_BTNM_PR_NONE if 'btn_id' was invalid
 */
static uint16_t get_button_text(lv_obj_t * btnm, uint16_t btn_id)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext_attr(btnm);
    if(btn_id > ext->btn_cnt) return LV_BTNM_PR_NONE;

    uint16_t txt_i = 0;
    uint16_t btn_i = 0;

    /* Search the text of ext->btn_pr the buttons text in the map
     * Skip "\n"-s*/
    while(btn_i != btn_id) {
        btn_i ++;
        txt_i ++;
        if(strcmp(ext->map_p[txt_i], "\n") == 0) txt_i ++;
    }

    if(btn_i == ext->btn_cnt) return  LV_BTNM_PR_NONE;

    return txt_i;
}


#endif
