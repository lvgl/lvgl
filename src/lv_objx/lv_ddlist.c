/**
 * @file lv_ddlist.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_ddlist.h"
#if LV_USE_DDLIST != 0

#include "../lv_core/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_group.h"
#include "../lv_core/lv_indev.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_font/lv_symbol_def.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_ddlist"

#if LV_USE_ANIMATION == 0
#undef LV_DDLIST_DEF_ANIM_TIME
#define LV_DDLIST_DEF_ANIM_TIME 0 /*No animation*/
#endif

#define LV_DDLIST_PR_NONE 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    lv_page_ext_t page;
    lv_obj_t * ddlist;      /*Pointer to the ddlist where the page belongs*/
}lv_ddlist_page_ext_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_ddlist_design(lv_obj_t * ddlist, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_design_res_t lv_ddlist_page_design(lv_obj_t * ddlist, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_ddlist_signal(lv_obj_t * ddlist, lv_signal_t sign, void * param);
static lv_res_t lv_ddlist_page_signal(lv_obj_t * page, lv_signal_t sign, void * param);
static lv_res_t lv_ddlist_page_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
static lv_style_list_t * lv_ddlist_get_style(lv_obj_t * ddlist, uint8_t part);
static void draw_box(lv_obj_t * ddlist, const lv_area_t * clip_area, uint16_t id, lv_obj_state_t state);
static void draw_box_label(lv_obj_t * ddlist, const lv_area_t * clip_area, uint16_t id, lv_obj_state_t state);
static lv_res_t page_release_handler(lv_obj_t * page);
static void page_press_handler(lv_obj_t * page);
static uint16_t get_id_on_point(lv_obj_t * ddlist, lv_coord_t x, lv_coord_t y);
static void pos_selected(lv_obj_t * ddlist);
static lv_obj_t * get_label(const lv_obj_t * ddlist);
static void list_anim(void * p, lv_anim_value_t v);
static void close_anim_ready(lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_signal_cb_t ancestor_page_signal;
static lv_signal_cb_t ancestor_page_scrl_signal;
static lv_design_cb_t ancestor_design;
static lv_design_cb_t ancestor_page_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a drop down list objects
 * @param par pointer to an object, it will be the parent of the new drop down list
 * @param copy pointer to a drop down list object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created drop down list
 */
lv_obj_t * lv_ddlist_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("drop down list create started");

    /*Create the ancestor drop down list*/
    lv_obj_t * ddlist = lv_btn_create(par, copy);
    LV_ASSERT_MEM(ddlist);
    if(ddlist == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(ddlist);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(ddlist);

    /*Allocate the drop down list type specific extended data*/
    lv_ddlist_ext_t * ext = lv_obj_allocate_ext_attr(ddlist, sizeof(lv_ddlist_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(ddlist);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->page          = NULL;
    ext->options     = NULL;
    ext->symbol         = LV_SYMBOL_DOWN;
    ext->text         = "Select";
    ext->show_selected   = 1;
    ext->sel_opt_id      = 0;
    ext->sel_opt_id_orig = 0;
    ext->pr_opt_id = LV_DDLIST_PR_NONE;
    ext->option_cnt      = 0;
    ext->dir = LV_DDLIST_DIR_DOWN;
    ext->max_height = LV_DPI * 2;
    ext->anim_time = LV_DDLIST_DEF_ANIM_TIME;
    lv_style_list_init(&ext->style_page);
    lv_style_list_init(&ext->style_scrlbar);
    lv_style_list_init(&ext->style_selected);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(ddlist, lv_ddlist_signal);
    lv_obj_set_design_cb(ddlist, lv_ddlist_design);

    /*Init the new drop down list drop down list*/
    if(copy == NULL) {
        lv_ddlist_set_options(ddlist, "Option 1\nOption 2\nOption 3");

        lv_theme_apply(ddlist, LV_THEME_DDLIST);
    }
    /*Copy an existing drop down list*/
    else {
//        lv_ddlist_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
//        ext->label                 = lv_label_create(ddlist, copy_ext->label);
//        lv_label_set_text(ext->label, lv_label_get_text(copy_ext->label));
//        ext->sel_opt_id     = copy_ext->sel_opt_id;
//        ext->sel_opt_id_ori = copy_ext->sel_opt_id;
//        ext->fix_height     = copy_ext->fix_height;
//        ext->option_cnt     = copy_ext->option_cnt;
//        ext->symbol           = copy_ext->symbol;
//        ext->stay_open      = copy_ext->stay_open;
    }

    LV_LOG_INFO("drop down list created");

    return ddlist;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set text of the ddlist (Displayed on the button if `show_selected = false`)
 * @param ddlist pointer to a drop down list object
 * @param txt the text as a string (Only it's pointer is saved)
 */
void lv_ddlist_set_text(lv_obj_t * ddlist, const char * txt)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    if(ext->text == txt) return;

    ext->text = txt;

    lv_obj_invalidate(ddlist);
}

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_ddlist_set_options(lv_obj_t * ddlist, const char * options)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);
    LV_ASSERT_STR(options);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    /*Count the '\n'-s to determine the number of options*/
    ext->option_cnt = 0;
    uint16_t i;
    for(i = 0; options[i] != '\0'; i++) {
        if(options[i] == '\n') ext->option_cnt++;
    }
    ext->option_cnt++;   /*Last option has no `\n`*/
    ext->sel_opt_id      = 0;
    ext->sel_opt_id_orig = 0;
    ext->options = options;
}

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void lv_ddlist_set_selected(lv_obj_t * ddlist, uint16_t sel_opt)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    if(ext->sel_opt_id == sel_opt) return;

    ext->sel_opt_id      = sel_opt < ext->option_cnt ? sel_opt : ext->option_cnt - 1;
    ext->sel_opt_id_orig = ext->sel_opt_id;
    /*Move the list to show the current option*/
    if(ext->page != NULL) {
        lv_obj_invalidate(ddlist);
    }
}

/**
 * Set a fix height for the drop down list
 * If 0 then the opened ddlist will be auto. sized else the set height will be applied.
 * @param ddlist pointer to a drop down list
 * @param h the height when the list is opened (0: auto size)
 */
void lv_ddlist_set_max_height(lv_obj_t * ddlist, lv_coord_t h)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    if(ext->max_height == h) return;

    ext->max_height = h;

    if(ext->page) {
        if(h == 0) {
            lv_cont_set_fit(ext->page, LV_FIT_TIGHT);
        } else {
            lv_cont_set_fit2(ext->page, LV_FIT_TIGHT, LV_FIT_NONE);
            lv_obj_set_height(ext->page, h);
        }
    }
}

/**
 * Set an arrow or other symbol to display when the drop-down list is closed.
 * @param ddlist pointer to drop down list object
 * @param symbol a text like `LV_SYMBOL_DOWN` or NULL to not draw icon
 */
void lv_ddlist_set_symbol(lv_obj_t * ddlist, const char * symbol)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->symbol = symbol;
    lv_obj_invalidate(ddlist);
}

/**
 * Set the direction of the a drop down list
 * @param ddlist pointer to a drop down list object
 * @param dir LV_DDLIST_DIR_LEF/RIGHT/TOP/BOTTOM
 */
void lv_ddlist_set_dir(lv_obj_t * ddlist, lv_ddlist_dir_t dir)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    if(ext->dir == dir) return;

    ext->dir = dir;

    lv_obj_invalidate(ddlist);
}

/**
 * Set whether the ddlist highlight the last selected option and display its text or not
 * @param ddlist pointer to a drop down list object
 * @param show true/false
 */
void lv_ddlist_set_show_selected(lv_obj_t * ddlist, bool show)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    if(ext->show_selected == show) return;

    ext->show_selected = show;

    lv_obj_invalidate(ddlist);
}
/*=====================
 * Getter functions
 *====================*/

/**
 * Get text of the ddlist (Displayed on the button if `show_selected = false`)
 * @param ddlist pointer to a drop down list object
 * @return the text string
 */
const char * lv_ddlist_get_text(lv_obj_t * ddlist)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    return ext->text;
}

/**
 * Get the options of a drop down list
 * @param ddlist pointer to drop down list object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_ddlist_get_options(const lv_obj_t * ddlist)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    return ext->options;
}

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_ddlist_get_selected(const lv_obj_t * ddlist)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    return ext->sel_opt_id;
}

/**
 * Get the total number of options
 * @param ddlist pointer to drop down list object
 * @return the total number of options in the list
 */
uint16_t lv_ddlist_get_option_cnt(const lv_obj_t * ddlist)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    return ext->option_cnt;
}

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
void lv_ddlist_get_selected_str(const lv_obj_t * ddlist, char * buf, uint16_t buf_size)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    uint16_t i;
    uint16_t line        = 0;
    size_t txt_len     = strlen(ext->options);

    for(i = 0; i < txt_len && line != ext->sel_opt_id_orig; i++) {
        if(ext->options[i] == '\n') line++;
    }

    uint16_t c;
    for(c = 0; ext->options[i] != '\n' && i < txt_len; c++, i++) {
        if(buf_size && c >= buf_size - 1) {
            LV_LOG_WARN("lv_ddlist_get_selected_str: the buffer was too small")
            break;
        }
        buf[c] = ext->options[i];
    }

    buf[c] = '\0';
}

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
lv_coord_t lv_ddlist_get_max_height(const lv_obj_t * ddlist)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    return ext->max_height;
}

/**
 * Get the symbol to draw when the drop-down list is closed
 * @param ddlist pointer to drop down list object
 * @return the symbol or NULL if not enabled
 */
const char * lv_ddlist_get_symbol(lv_obj_t * ddlist)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    return ext->symbol;
}

/**
 * Get the direction of the drop down list
 * @param ddlist pointer to a drop down list object
 * @return LV_DDLIST_DIR_LEF/RIGHT/TOP/BOTTOM
 */
lv_ddlist_dir_t lv_ddlist_get_dir(const lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    return ext->dir;
}

/**
 * Get whether the ddlist highlight the last selected option and display its text or not
 * @param ddlist pointer to a drop down list object
 * @return true/false
 */
bool lv_ddlist_get_show_selected(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    return ext->show_selected ? true : false;

}

/*=====================
 * Other functions
 *====================*/

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 * @param anim_en LV_ANIM_EN: use animation; LV_ANIM_OFF: not use animations
 */
void lv_ddlist_open(lv_obj_t * ddlist, lv_anim_enable_t anim)
{
#if LV_USE_ANIMATION == 0
    anim = false;
#endif
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    if(ext->page) return;

    ext->page = lv_page_create(lv_obj_get_screen(ddlist), NULL);
    lv_obj_set_protect(ext->page, LV_PROTECT_POS | LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_protect(lv_page_get_scrl(ext->page), LV_PROTECT_CLICK_FOCUS);

    if(ancestor_page_signal == NULL) ancestor_page_signal = lv_obj_get_signal_cb(ext->page);
    if(ancestor_page_scrl_signal == NULL) ancestor_page_scrl_signal = lv_obj_get_signal_cb(lv_page_get_scrl(ext->page));
    if(ancestor_page_design == NULL) ancestor_page_design = lv_obj_get_design_cb(ext->page);

    lv_ddlist_page_ext_t * page_ext = lv_obj_allocate_ext_attr(ext->page, sizeof(lv_ddlist_page_ext_t));
    page_ext->ddlist = ddlist;

    lv_obj_set_design_cb(ext->page, lv_ddlist_page_design);
    lv_obj_set_signal_cb(ext->page, lv_ddlist_page_signal);
    lv_obj_set_signal_cb(lv_page_get_scrl(ext->page), lv_ddlist_page_scrl_signal);

    lv_style_list_copy(lv_obj_get_style_list(ext->page, LV_PAGE_PART_BG), &ext->style_page);
    lv_style_list_copy(lv_obj_get_style_list(ext->page, LV_PAGE_PART_SCRLBAR), &ext->style_scrlbar);
    lv_style_list_reset(lv_obj_get_style_list(ext->page, LV_PAGE_PART_SCRL));
    lv_obj_refresh_style(ext->page);

    lv_page_set_scrl_fit(ext->page, LV_FIT_TIGHT);

    lv_obj_t * label = lv_label_create(ext->page, NULL);
    lv_label_set_static_text(label, ext->options);

    lv_cont_set_fit2(ext->page, LV_FIT_TIGHT, LV_FIT_NONE);
    lv_coord_t label_h = lv_obj_get_height(label);
    lv_style_int_t top = lv_obj_get_style_pad_top(ddlist, LV_DDLIST_PART_LIST);
    lv_style_int_t bottom = lv_obj_get_style_pad_bottom(ddlist, LV_DDLIST_PART_LIST);

    lv_coord_t list_h = label_h + top + bottom;

    if(list_h > ext->max_height) list_h = ext->max_height;

    lv_obj_set_height(ext->page, list_h);

    pos_selected(ddlist);

    lv_disp_t * disp  = lv_obj_get_disp(ddlist);
    lv_coord_t vres = lv_disp_get_ver_res(disp);
    lv_coord_t hres = lv_disp_get_hor_res(disp);

    if(ext->dir == LV_DDLIST_DIR_DOWN)      lv_obj_align(ext->page, ddlist, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    else if(ext->dir == LV_DDLIST_DIR_UP)   lv_obj_align(ext->page, ddlist, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    else if(ext->dir == LV_DDLIST_DIR_LEFT) lv_obj_align(ext->page, ddlist, LV_ALIGN_OUT_LEFT_TOP, 0, 0);
    else if(ext->dir == LV_DDLIST_DIR_RIGHT)lv_obj_align(ext->page, ddlist, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);

    lv_ddlist_dir_t dir = ext->dir;
    if(dir == LV_DDLIST_DIR_DOWN && ext->page->coords.y2 >= vres) dir = LV_DDLIST_DIR_UP;
    else if(dir == LV_DDLIST_DIR_UP && ext->page->coords.y1 <= 0) dir = LV_DDLIST_DIR_DOWN;
    else if(dir == LV_DDLIST_DIR_RIGHT && ext->page->coords.x2 >= hres) dir = LV_DDLIST_DIR_LEFT;
    else if(dir == LV_DDLIST_DIR_LEFT && ext->page->coords.x1 <= 0) dir = LV_DDLIST_DIR_RIGHT;

    if(dir != ext->dir) {
        if(dir == LV_DDLIST_DIR_DOWN)      lv_obj_align(ext->page, ddlist, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
        else if(dir == LV_DDLIST_DIR_UP)   lv_obj_align(ext->page, ddlist, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
        else if(dir == LV_DDLIST_DIR_LEFT) lv_obj_align(ext->page, ddlist, LV_ALIGN_OUT_LEFT_TOP, 0, 0);
        else if(dir == LV_DDLIST_DIR_RIGHT)lv_obj_align(ext->page, ddlist, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    }

    lv_obj_t * scr = lv_scr_act();
    if(ext->page->coords.y2 > scr->coords.y2) {
        lv_obj_set_height(ext->page, lv_obj_get_height(ext->page) - (ext->page->coords.y2 - scr->coords.y2));
    }

    if(dir != LV_DDLIST_DIR_UP) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_exec_cb(&a, ddlist, list_anim);
        lv_anim_set_values(&a, 0, lv_obj_get_height(ext->page));
        lv_anim_set_time(&a, ext->anim_time, 0);
        lv_anim_create(&a);
    }
}

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 * @param anim_en LV_ANIM_ON: use animation; LV_ANIM_OFF: not use animations
 */
void lv_ddlist_close(lv_obj_t * ddlist, lv_anim_enable_t anim)
{
#if LV_USE_ANIMATION == 0
    anim = false;
#endif
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    if(ext->page == NULL) return;

    ext->pr_opt_id = LV_DDLIST_PR_NONE;

    if(ext->anim_time == 0 || anim == LV_ANIM_OFF) {
        lv_obj_del(ext->page);
        ext->page = NULL;
    } else {
//    if(dir != LV_DDLIST_DIR_UP) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_exec_cb(&a, ddlist, list_anim);
        lv_anim_set_values(&a, lv_obj_get_height(ext->page), 0);
        lv_anim_set_time(&a, ext->anim_time, 0);
        lv_anim_set_ready_cb(&a, close_anim_ready);
        lv_anim_create(&a);

//    }
    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the drop down list
 * @param ddlist pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_ddlist_design(lv_obj_t * ddlist, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design(ddlist, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(ddlist, clip_area, mode);

        lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

        lv_style_int_t left = lv_obj_get_style_pad_left(ddlist, LV_DDLIST_PART_BTN);
        lv_style_int_t right = lv_obj_get_style_pad_right(ddlist, LV_DDLIST_PART_BTN);
        lv_style_int_t top = lv_obj_get_style_pad_top(ddlist, LV_DDLIST_PART_BTN);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        lv_obj_init_draw_label_dsc(ddlist, LV_DDLIST_PART_BTN, &label_dsc);

        lv_area_t txt_area;
        lv_point_t txt_size;

        const char * opt_txt = ext->text;
        if(ext->show_selected) {
            char * buf = lv_mem_buf_get(128);
            lv_ddlist_get_selected_str(ddlist, buf, 128);
            opt_txt = buf;
        }

        const char * txt;

        txt = ext->dir != LV_DDLIST_DIR_LEFT ? opt_txt : ext->symbol;
        if(txt) {
            lv_txt_get_size(&txt_size, txt, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);

            txt_area.y1 = ddlist->coords.y1 + top;
            txt_area.y2 = txt_area.y1 + txt_size.y;

            /*Center align the text if no symbol*/
            if(ext->symbol == NULL && txt == opt_txt) {
                txt_area.x1 = ddlist->coords.x1 + (lv_obj_get_width(ddlist) - txt_size.x) / 2;
                txt_area.x2 = txt_area.x1 + txt_size.x;
            } else {
                txt_area.x1 = ddlist->coords.x1 + left;
                txt_area.x2 = txt_area.x1 + txt_size.x;
            }
            lv_draw_label(&txt_area, clip_area, &label_dsc, txt, NULL);
        }

        txt = ext->dir != LV_DDLIST_DIR_LEFT ? ext->symbol : opt_txt;
        if(txt) {
            lv_txt_get_size(&txt_size, txt, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);
            txt_area.y1 = ddlist->coords.y1 + top;
            txt_area.y2 = txt_area.y1 + txt_size.y;

            /*Center align the text if no symbol*/
            if(ext->symbol == NULL && txt == opt_txt) {
                txt_area.x1 = ddlist->coords.x1 + (lv_obj_get_width(ddlist) - txt_size.x) / 2;
                txt_area.x2 = txt_area.x1 + txt_size.x;
            } else {
                txt_area.x1 = txt_area.x2 - txt_size.x;
                txt_area.x2 = ddlist->coords.x2 - right;
            }

            lv_draw_label(&txt_area, clip_area, &label_dsc, txt, NULL);
        }

        if(ext->show_selected && ext->sel_opt_id_orig >= 0) {
            lv_mem_buf_release((char *)opt_txt);
        }

    } else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(ddlist, clip_area, mode);
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Handle the drawing related tasks of the drop down list's page
 * @param page pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_ddlist_page_design(lv_obj_t * page, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_page_design(page, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_page_design(page, clip_area, mode);

        lv_ddlist_page_ext_t * page_ext = lv_obj_get_ext_attr(page);
        lv_obj_t * ddlist = page_ext->ddlist;
        lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

        /*Draw the boxes if the page is not being deleted*/
        if(ext->page) {
            if(ext->pr_opt_id != LV_DDLIST_PR_NONE) {
                draw_box(ddlist, clip_area, ext->pr_opt_id, LV_OBJ_STATE_PRESSED);
            }

            if(ext->show_selected) {
                draw_box(ddlist, clip_area, ext->sel_opt_id, LV_OBJ_STATE_NORMAL);
            }
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        /*Draw the scrollbar in the ancestor page design function*/
        ancestor_page_design(page, clip_area, mode);

        /*Redraw the text on the selected area with a different color*/
        lv_ddlist_page_ext_t * page_ext = lv_obj_get_ext_attr(page);
        lv_obj_t * ddlist = page_ext->ddlist;
        lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

        /*Draw the box labels if the page is not being deleted*/
        if(ext->page) {
            if(ext->pr_opt_id != LV_DDLIST_PR_NONE) {
                draw_box_label(ddlist, clip_area, ext->pr_opt_id, LV_OBJ_STATE_PRESSED);
            }

            if(ext->show_selected) {
                draw_box_label(ddlist, clip_area, ext->sel_opt_id, LV_OBJ_STATE_NORMAL);
            }
        }
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the drop down list
 * @param ddlist pointer to a drop down list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_ddlist_signal(lv_obj_t * ddlist, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(ddlist, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_ddlist_get_style(ddlist, info->part);
        if(info->result != NULL) return LV_RES_OK;
        return LV_RES_OK;
    }
    else if(sign == LV_SIGNAL_GET_STATE_DSC) {
        lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
        lv_get_state_info_t * info = param;
        if(info->part == LV_DDLIST_PART_LIST ||
            info->part == LV_DDLIST_PART_SCRLBAR ||
            info->part == LV_DDLIST_PART_SELECTED) {
        info->result = lv_obj_get_state_dsc(ext->page, LV_PAGE_PART_BG);
        }
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
        lv_ddlist_close(ddlist, LV_ANIM_OFF);
    }
    else if(sign == LV_SIGNAL_FOCUS) {
#if LV_USE_GROUP
        lv_group_t * g             = lv_obj_get_group(ddlist);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        /*Encoders need special handling*/
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            /*Open the list if editing*/
            if(editing) lv_ddlist_open(ddlist, LV_ANIM_ON);
            /*Close the list if navigating*/
            else
                lv_ddlist_close(ddlist, LV_ANIM_ON);
        }
#endif
    } else if(sign == LV_SIGNAL_DEFOCUS || sign == LV_SIGNAL_LEAVE) {
        lv_ddlist_close(ddlist, LV_ANIM_ON);
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        if(lv_indev_is_dragging(lv_indev_get_act()) == false) {
            if(ext->page) {
                lv_ddlist_close(ddlist, LV_ANIM_ON);
                if(ext->sel_opt_id_orig != ext->sel_opt_id) {
                    ext->sel_opt_id_orig = ext->sel_opt_id;
                    lv_obj_invalidate(ddlist);
                }
            } else {
                lv_ddlist_open(ddlist, LV_ANIM_ON);
            }
        }
        else {
            ext->sel_opt_id = ext->sel_opt_id_orig;
            lv_obj_invalidate(ddlist);
        }
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        if(ext->page) lv_ddlist_close(ddlist, LV_ANIM_OFF);
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_style_int_t top = lv_obj_get_style_pad_top(ddlist, LV_DDLIST_PART_BTN);
        lv_style_int_t bottom = lv_obj_get_style_pad_bottom(ddlist, LV_DDLIST_PART_BTN);
        const lv_font_t * font = lv_obj_get_style_font(ddlist, LV_DDLIST_PART_BTN);
        lv_obj_set_height(ddlist, top + bottom + lv_font_get_line_height(font));

        if(ext->page) lv_obj_refresh_style(ext->page);
    }
    else if(sign == LV_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_DOWN) {
            if(ext->page == NULL) {
                lv_ddlist_open(ddlist, LV_ANIM_ON);
            } else if(ext->sel_opt_id + 1 < ext->option_cnt) {
                ext->sel_opt_id++;
                pos_selected(ddlist);
            }
        } else if(c == LV_KEY_LEFT || c == LV_KEY_UP) {

            if(ext->page == NULL) {
                lv_ddlist_open(ddlist, LV_ANIM_ON);
            } else if(ext->sel_opt_id > 0) {
                ext->sel_opt_id--;
                pos_selected(ddlist);
            }
        } else if(c == LV_KEY_ESC) {
                ext->sel_opt_id = ext->sel_opt_id_orig;
                lv_ddlist_close(ddlist, LV_ANIM_ON);
        }
    }
else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = true;
    }

    return res;
}

/**
 * Signal function of the drop down list's scrollable part
 * @param scrl pointer to a drop down list's scrollable part
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_ddlist_page_signal(lv_obj_t * page, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_page_signal(page, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, "");

    lv_ddlist_page_ext_t * page_ext = lv_obj_get_ext_attr(page);
    lv_obj_t * ddlist = page_ext->ddlist;

    if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        /* Make possible to draw on the full width of the background to redraw the selected rectangle
         * when the ddlist is scrolled in fix height mode.
         * (The scrollabel is scrolled the "select rectangle" is drawn on the bg too)*/
        lv_style_int_t left = lv_obj_get_style_pad_left(ddlist, LV_DDLIST_PART_LIST);
        lv_style_int_t right = lv_obj_get_style_pad_right(ddlist, LV_DDLIST_PART_LIST);
        lv_obj_t * scrl = lv_page_get_scrl(page);
        scrl->ext_draw_pad = LV_MATH_MAX(scrl->ext_draw_pad, LV_MATH_MAX(left, right));
    } else if(sign == LV_SIGNAL_RELEASED) {
        if(lv_indev_is_dragging(lv_indev_get_act()) == false) {
            page_release_handler(page);
        }
    } else if(sign == LV_SIGNAL_PRESSED) {
         page_press_handler(page);
    } else if(sign == LV_SIGNAL_CLEANUP) {
        lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
        ext->page  = NULL; /*The page is just being deleted*/
    }

    return res;
}

/**
 * Signal function of the drop down list's scrollable part
 * @param scrl pointer to a drop down list's scrollable part
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_ddlist_page_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_page_scrl_signal(scrl, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, "");

    lv_obj_t * page = lv_obj_get_parent(scrl);
    lv_ddlist_page_ext_t * page_ext = lv_obj_get_ext_attr(page);
    lv_obj_t * ddlist = page_ext->ddlist;
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

   if(sign == LV_SIGNAL_RELEASED) {
        if(lv_indev_is_dragging(lv_indev_get_act()) == false) {
            page_release_handler(page);
        }
   } else if(sign == LV_SIGNAL_PRESSED) {
       page_press_handler(page);
   } else if(sign == LV_SIGNAL_DRAG_BEGIN) {
       ext->pr_opt_id = LV_DDLIST_PR_NONE;
       lv_obj_invalidate(page);
   }
   else  if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
       /* Make possible to draw on the full width of the background to redraw the selected rectangle
        * when the ddlist is scrolled in fix height mode.
        * (The scrollabel is scrolled the "select rectangle" is drawn on the bg too)*/
       lv_style_int_t left = lv_obj_get_style_pad_left(ddlist, LV_DDLIST_PART_LIST);
       lv_style_int_t right = lv_obj_get_style_pad_right(ddlist, LV_DDLIST_PART_LIST);
       scrl->ext_draw_pad = LV_MATH_MAX(scrl->ext_draw_pad, LV_MATH_MAX(left, right));
   }

    return res;
}


/**
 * Get the style descriptor of a part of the object
 * @param page pointer the object
 * @param part the part from `lv_ddlist_part_t`. (LV_DDLIST_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_ddlist_get_style(lv_obj_t * ddlist, uint8_t part)
{
    LV_ASSERT_OBJ(ddlist, LV_OBJX_NAME);

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    lv_style_list_t * style_dsc_p;

    switch(part) {
    case LV_DDLIST_PART_BTN:
        style_dsc_p = &ddlist->style_list;
        break;
    case LV_DDLIST_PART_LIST:
        style_dsc_p = &ext->style_page;
        break;
    case LV_DDLIST_PART_SCRLBAR:
        style_dsc_p = &ext->style_scrlbar;
        break;
    case LV_DDLIST_PART_SELECTED:
        style_dsc_p = &ext->style_selected;
        break;
    default:
        style_dsc_p = NULL;
    }

    return style_dsc_p;
}

static void draw_box(lv_obj_t * ddlist, const lv_area_t * clip_area, uint16_t id, lv_obj_state_t state)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    lv_obj_t * page = ext->page;
    lv_obj_state_dsc_t state_orig = page->state_dsc;

    page->state_dsc.act = LV_OBJ_STATE_NORMAL;
    page->state_dsc.act |= state;
    page->state_dsc.prev = page->state_dsc.act;

    /*Draw a rectangle under the selected item*/
    const lv_font_t * font    = lv_obj_get_style_font(ddlist, LV_DDLIST_PART_LIST);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(ddlist, LV_DDLIST_PART_LIST);
    lv_coord_t font_h         = lv_font_get_line_height(font);

    /*Draw the selected*/
    lv_obj_t * label = get_label(ddlist);
    lv_area_t rect_area;
    rect_area.y1 = label->coords.y1;
    rect_area.y1 += id * (font_h + line_space);
    rect_area.y1 -= line_space / 2;

    rect_area.y2 = rect_area.y1 + font_h + line_space - 1;
    rect_area.x1 = ext->page->coords.x1;
    rect_area.x2 = ext->page->coords.x2;

    lv_draw_rect_dsc_t sel_rect;
    lv_draw_rect_dsc_init(&sel_rect);
    lv_obj_init_draw_rect_dsc(ddlist, LV_DDLIST_PART_SELECTED, &sel_rect);
    lv_draw_rect(&rect_area, clip_area, &sel_rect);

    page->state_dsc = state_orig;
}



static void draw_box_label(lv_obj_t * ddlist, const lv_area_t * clip_area, uint16_t id, lv_obj_state_t state)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    lv_obj_t * page = ext->page;
    lv_obj_state_dsc_t state_orig = page->state_dsc;

    page->state_dsc.act = LV_OBJ_STATE_NORMAL;
    page->state_dsc.act |= state;
    page->state_dsc.prev = page->state_dsc.act;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(ddlist, LV_DDLIST_PART_SELECTED, &label_dsc);
    label_dsc.line_space = lv_obj_get_style_text_line_space(ddlist, LV_DDLIST_PART_LIST);  /*Line space should come from the page*/
    lv_coord_t font_h        = lv_font_get_line_height(label_dsc.font);

    lv_obj_t * label = get_label(ddlist);
    lv_area_t area_sel;
    area_sel.y1 = label->coords.y1;
    area_sel.y1 += id * (font_h + label_dsc.line_space);
    area_sel.y1 -= label_dsc.line_space / 2;

    area_sel.y2 = area_sel.y1 + font_h + label_dsc.line_space - 1;
    area_sel.x1 = page->coords.x1;
    area_sel.x2 = page->coords.x2;
    lv_area_t mask_sel;
    bool area_ok;
    area_ok = lv_area_intersect(&mask_sel, clip_area, &area_sel);
    if(area_ok) {
        lv_draw_label(&label->coords, &mask_sel, &label_dsc, lv_label_get_text(label), NULL);
    }
    page->state_dsc = state_orig;
}

/**
 * Called when a drop down list is released to open it or set new option
 * @param page pointer to the drop down list's page
 * @return LV_RES_INV if the page is not being deleted in the user callback. Else LV_RES_OK
 */
static lv_res_t page_release_handler(lv_obj_t * page)
{
    lv_ddlist_page_ext_t * page_ext = lv_obj_get_ext_attr(page);
    lv_obj_t * ddlist = page_ext->ddlist;

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    lv_indev_t * indev = lv_indev_get_act();
#if LV_USE_GROUP
    /*Leave edit mode once a new item is selected*/
    if(lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
        ext->sel_opt_id_orig = ext->sel_opt_id;
        lv_group_t * g      = lv_obj_get_group(ddlist);
        if(lv_group_get_editing(g)) {
            lv_group_set_editing(g, false);
        }
    }
#endif

    /*Search the clicked option (For KEYPAD and ENCODER the new value should be already set)*/
    if(lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER || lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON) {
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        ext->sel_opt_id     = get_id_on_point(ddlist, p.x, p.y);
        ext->sel_opt_id_orig = ext->sel_opt_id;
    }

    lv_ddlist_close(ddlist, LV_ANIM_ON);

    /*Invalidate to refresh the text*/
    if(ext->show_selected) lv_obj_invalidate(ddlist);

    uint32_t id  = ext->sel_opt_id; /*Just to use uint32_t in event data*/
    lv_res_t res = lv_event_send(ddlist, LV_EVENT_VALUE_CHANGED, &id);
    if(res != LV_RES_OK) return res;
    if(ext->page == NULL) return LV_RES_INV;

    return LV_RES_OK;
}

static void page_press_handler(lv_obj_t * page)
{
    lv_ddlist_page_ext_t * page_ext = lv_obj_get_ext_attr(page);
    lv_obj_t * ddlist = page_ext->ddlist;

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    lv_indev_t * indev = lv_indev_get_act();
    if(indev && (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER || lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON)) {
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        ext->pr_opt_id = get_id_on_point(ddlist, p.x, p.y);
        lv_obj_invalidate(page);
    }
}

static uint16_t get_id_on_point(lv_obj_t * ddlist, lv_coord_t x, lv_coord_t y)
{
    lv_obj_t * label = get_label(ddlist);
    x -= label->coords.x1;
    y -= label->coords.y1;
    uint16_t letter_i;

    lv_point_t p = {x, y};
    letter_i = lv_label_get_letter_on(label, &p);
    uint16_t opt  = 0;
    const char * txt  = lv_label_get_text(label);
    uint32_t i        = 0;
    uint32_t i_prev   = 0;

    uint32_t letter_cnt = 0;
    uint32_t letter;
    for(letter_cnt = 0; letter_cnt < letter_i; letter_cnt++) {
        letter = lv_txt_encoded_next(txt, &i);
        /*Count he lines to reach the clicked letter. But ignore the last '\n' because it
         * still belongs to the clicked line*/
        if(letter == '\n' && i_prev != letter_i) opt++;
        i_prev = i;
    }

    return opt;
}

/**
 * Set the position of list when it is closed to show the selected item
 * @param ddlist pointer to a drop down list
 */
static void pos_selected(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext          = lv_obj_get_ext_attr(ddlist);

    const lv_font_t * font         = lv_obj_get_style_font(ddlist, LV_DDLIST_PART_LIST);
    lv_coord_t font_h              = lv_font_get_line_height(font);
    lv_obj_t * scrl                = lv_page_get_scrl(ext->page);
    lv_obj_t * label = get_label(ddlist);

    lv_coord_t h = lv_obj_get_height(ext->page);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(ddlist, LV_DDLIST_PART_LIST);

    lv_coord_t line_y1 = ext->sel_opt_id * (font_h + line_space) + label->coords.y1 - scrl->coords.y1;

    lv_obj_set_y(scrl, -line_y1 + (h - font_h) / 2);
    lv_obj_invalidate(ext->page);
}



static lv_obj_t * get_label(const lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    if(ext->page == NULL) return NULL;

    return lv_obj_get_child(lv_page_get_scrl(ext->page), NULL);
}

static void list_anim(void * p, lv_anim_value_t v)
{
    lv_obj_t * ddlist = p;
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    lv_obj_set_height(ext->page, v);
}

static void close_anim_ready(lv_anim_t * a)
{
    lv_obj_t * ddlist = a->var;
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    lv_obj_del(ext->page);
    ext->page = NULL;
}

#endif
