/**
 * @file lv_tab.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_TABVIEW != 0

#include "lv_tabview.h"
#include "lv_btnm.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/
#if USE_LV_ANIMATION
#  ifndef LV_TABVIEW_ANIM_TIME
#    define LV_TABVIEW_ANIM_TIME  300 /*Animation time of focusing to the a list element [ms] (0: no animation)  */
#  endif
#else
#  undef  LV_TABVIEW_ANIM_TIME
#  define LV_TABVIEW_ANIM_TIME	0	/*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_tabview_signal(lv_obj_t * tabview, lv_signal_t sign, void * param);
static lv_res_t tabpage_signal(lv_obj_t * tab_page, lv_signal_t sign, void * param);
static lv_res_t tabpage_scrl_signal(lv_obj_t * tab_scrl, lv_signal_t sign, void * param);

static void tabpage_pressed_handler(lv_obj_t * tabview, lv_obj_t * tabpage);
static void tabpage_pressing_handler(lv_obj_t * tabview, lv_obj_t * tabpage);
static void tabpage_press_lost_handler(lv_obj_t * tabview, lv_obj_t * tabpage);
static lv_res_t tab_btnm_action(lv_obj_t * tab_btnm, const char * tab_name);
static void tabview_realign(lv_obj_t * tabview);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_signal_func_t page_signal;
static lv_signal_func_t page_scrl_signal;
static const char * tab_def[] = {""};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a Tab view object
 * @param par pointer to an object, it will be the parent of the new tab
 * @param copy pointer to a tab object, if not NULL then the new object will be copied from it
 * @return pointer to the created tab
 */
lv_obj_t * lv_tabview_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of tab*/
    lv_obj_t * new_tabview = lv_obj_create(par, copy);
    lv_mem_assert(new_tabview);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_tabview);
    
    /*Allocate the tab type specific extended data*/
    lv_tabview_ext_t * ext = lv_obj_allocate_ext_attr(new_tabview, sizeof(lv_tabview_ext_t));
    lv_mem_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->drag_hor = 0;
    ext->draging = 0;
    ext->slide_enable = 1;
    ext->tab_cur = 0;
    ext->point_last.x = 0;
    ext->point_last.y = 0;
    ext->content = NULL;
    ext->indic = NULL;
    ext->btns = NULL;
    ext->tab_load_action = NULL;
    ext->anim_time = LV_TABVIEW_ANIM_TIME;
    ext->tab_name_ptr = lv_mem_alloc(sizeof(char*));
    ext->tab_name_ptr[0] = "";
    ext->tab_cnt = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_tabview, lv_tabview_signal);

    /*Init the new tab tab*/
    if(copy == NULL) {
        lv_obj_set_size(new_tabview, LV_HOR_RES, LV_VER_RES);

        ext->btns = lv_btnm_create(new_tabview, NULL);
        lv_obj_set_height(ext->btns, 3 * LV_DPI / 4);
        lv_btnm_set_map(ext->btns, tab_def);
        lv_btnm_set_action(ext->btns, tab_btnm_action);
        lv_btnm_set_toggle(ext->btns, true, 0);

        ext->indic = lv_obj_create(ext->btns, NULL);
        lv_obj_set_width(ext->indic, LV_DPI);
        lv_obj_align(ext->indic, ext->btns, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
        lv_obj_set_click(ext->indic, false);

        ext->content = lv_cont_create(new_tabview, NULL);
        lv_cont_set_fit(ext->content, true, false);
        lv_cont_set_layout(ext->content, LV_LAYOUT_ROW_T);
        lv_cont_set_style(ext->content, &lv_style_transp_tight);
        lv_obj_set_height(ext->content, LV_VER_RES - lv_obj_get_height(ext->btns));
        lv_obj_align(ext->content, ext->btns, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_BG, th->tabview.bg);
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_INDIC, th->tabview.indic);
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_BTN_BG, th->tabview.btn.bg);
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_BTN_REL, th->tabview.btn.rel);
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_BTN_PR, th->tabview.btn.pr);
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_BTN_TGL_REL, th->tabview.btn.tgl_rel);
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_BTN_TGL_PR, th->tabview.btn.tgl_pr);
        } else {
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_BG, &lv_style_plain);
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_BTN_BG, &lv_style_transp);
            lv_tabview_set_style(new_tabview, LV_TABVIEW_STYLE_INDIC, &lv_style_plain_color);
        }
    }
    /*Copy an existing tab view*/
    else {
    	lv_tabview_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->point_last.x = 0;
        ext->point_last.y = 0;
        ext->btns = lv_btnm_create(new_tabview, copy_ext->btns);
        ext->indic = lv_obj_create(ext->btns, copy_ext->indic);
        ext->content = lv_cont_create(new_tabview, copy_ext->content);
        ext->anim_time = copy_ext->anim_time;
        ext->tab_load_action = copy_ext->tab_load_action;

        ext->tab_name_ptr = lv_mem_alloc(sizeof(char*));
        ext->tab_name_ptr[0] = "";
        lv_btnm_set_map(ext->btns, ext->tab_name_ptr);

        uint16_t i;
        lv_obj_t *new_tab;
        lv_obj_t *copy_tab;
        for (i = 0; i < copy_ext->tab_cnt; i++) {
            new_tab = lv_tabview_add_tab(new_tabview, copy_ext->tab_name_ptr[i]);
            copy_tab = lv_tabview_get_tab(copy, i);
            lv_page_set_style(new_tab, LV_PAGE_STYLE_BG, lv_page_get_style(copy_tab, LV_PAGE_STYLE_BG));
            lv_page_set_style(new_tab, LV_PAGE_STYLE_SCRL, lv_page_get_style(copy_tab, LV_PAGE_STYLE_SCRL));
            lv_page_set_style(new_tab, LV_PAGE_STYLE_SB, lv_page_get_style(copy_tab, LV_PAGE_STYLE_SB));
        }

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_tabview);
    }
    
    return new_tabview;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a new tab with the given name
 * @param tabview pointer to Tab view object where to ass the new tab
 * @param name the text on the tab button
 * @return pointer to the created page object (lv_page). You can create your content here
 */
lv_obj_t * lv_tabview_add_tab(lv_obj_t * tabview, const char * name)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext_attr(tabview);

    /*Create the container page*/
    lv_obj_t * h = lv_page_create(ext->content, NULL);
    lv_obj_set_size(h, lv_obj_get_width(tabview), lv_obj_get_height(ext->content));
    lv_page_set_sb_mode(h, LV_SB_MODE_AUTO);
    lv_page_set_style(h, LV_PAGE_STYLE_BG, &lv_style_transp);
    lv_page_set_style(h, LV_PAGE_STYLE_SCRL, &lv_style_transp);

    if(page_signal == NULL) page_signal = lv_obj_get_signal_func(h);
    if(page_scrl_signal == NULL) page_scrl_signal = lv_obj_get_signal_func(lv_page_get_scrl(h));
    lv_obj_set_signal_func(h, tabpage_signal);
    lv_obj_set_signal_func(lv_page_get_scrl(h), tabpage_scrl_signal);

    /*Extend the button matrix map with the new name*/
    char *name_dm;
    if((name[0] & LV_BTNM_CTRL_MASK) == LV_BTNM_CTRL_CODE) { /*If control byte presented let is*/
        name_dm = lv_mem_alloc(strlen(name) + 1); /*+1 for the the closing '\0' */
        strcpy(name_dm, name);
    } else { /*Set a no long press control byte is not presented*/
        name_dm = lv_mem_alloc(strlen(name) + 2); /*+1 for the the closing '\0' and +1 for the control byte */
        name_dm[0] = '\221';
        strcpy(&name_dm[1], name);
    }
    ext->tab_cnt++;
    ext->tab_name_ptr = lv_mem_realloc(ext->tab_name_ptr, sizeof(char *) * (ext->tab_cnt + 1));
    ext->tab_name_ptr[ext->tab_cnt - 1] = name_dm;
    ext->tab_name_ptr[ext->tab_cnt] = "";

    lv_btnm_set_map(ext->btns, ext->tab_name_ptr);

    /*Modify the indicator size*/
    lv_style_t * style_tabs = lv_obj_get_style(ext->btns);
    lv_coord_t indic_width = (lv_obj_get_width(tabview) - style_tabs->body.padding.inner * (ext->tab_cnt - 1) - 2 * style_tabs->body.padding.hor) / ext->tab_cnt;
    lv_obj_set_width(ext->indic, indic_width);
    lv_obj_set_x(ext->indic, indic_width * ext->tab_cur + style_tabs->body.padding.inner * ext->tab_cur + style_tabs->body.padding.hor);

    /*Set the first btn as active*/
    if(ext->tab_cnt == 1) {
        ext->tab_cur = 0;
        lv_tabview_set_tab_act(tabview, 0, false);
        tabview_realign(tabview);       /*To set the proper btns height*/
    }

    return h;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new tab
 * @param tabview pointer to Tab view object
 * @param id index of a tab to load
 * @param anim_en true: set with sliding animation; false: set immediately
 */
void lv_tabview_set_tab_act(lv_obj_t * tabview, uint16_t id, bool anim_en)
{
#if USE_LV_ANIMATION == 0
    anim_en = false;
#endif
    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tabview);
    lv_style_t * style = lv_obj_get_style(ext->content);

    if(id >= ext->tab_cnt) id = ext->tab_cnt - 1;
    if(ext->tab_load_action) ext->tab_load_action(tabview, id);

    ext->tab_cur = id;

    lv_coord_t cont_x = -(lv_obj_get_width(tabview) * id + style->body.padding.inner * id + style->body.padding.hor);
    if(ext->anim_time == 0 || anim_en == false) {
        lv_obj_set_x(ext->content, cont_x);
    } else {
#if USE_LV_ANIMATION
        lv_anim_t a;
        a.var = ext->content;
        a.start = lv_obj_get_x(ext->content);
        a.end = cont_x;
        a.fp = (lv_anim_fp_t)lv_obj_set_x;
        a.path = lv_anim_path_linear;
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = ext->anim_time;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        lv_anim_create(&a);
#endif
    }

    /*Move the indicator*/
    lv_coord_t indic_width = lv_obj_get_width(ext->indic);
    lv_style_t * tabs_style = lv_obj_get_style(ext->btns);
    lv_coord_t indic_x = indic_width * id + tabs_style->body.padding.inner * id + tabs_style->body.padding.hor;

    if(ext->anim_time == 0 || anim_en == false ) {
        lv_obj_set_x(ext->indic, indic_x);
    } else {
#if USE_LV_ANIMATION
        lv_anim_t a;
        a.var = ext->indic;
        a.start = lv_obj_get_x(ext->indic);
        a.end = indic_x;
        a.fp = (lv_anim_fp_t)lv_obj_set_x;
        a.path = lv_anim_path_linear;
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = ext->anim_time;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        lv_anim_create(&a);
#endif
    }

    lv_btnm_set_toggle(ext->btns, true, ext->tab_cur);
}

/**
 * Set an action to call when a tab is loaded (Good to create content only if required)
 * lv_tabview_get_act() still gives the current (old) tab (to remove content from here)
 * @param tabview pointer to a tabview object
 * @param action pointer to a function to call when a btn is loaded
 */
void lv_tabview_set_tab_load_action(lv_obj_t *tabview, lv_tabview_action_t action)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext_attr(tabview);
    ext->tab_load_action = action;
}

/**
 * Enable horizontal sliding with touch pad
 * @param tabview pointer to Tab view object
 * @param en true: enable sliding; false: disable sliding
 */
void lv_tabview_set_sliding(lv_obj_t * tabview, bool en)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext_attr(tabview);
    ext->slide_enable = en == false ? 0 : 1;
}

/**
 * Set the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @param anim_time_ms time of animation in milliseconds
 */
void lv_tabview_set_anim_time(lv_obj_t * tabview, uint16_t anim_time)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext_attr(tabview);
#if USE_LV_ANIMATION == 0
    anim_time = 0;
#endif
    ext->anim_time = anim_time;
}

/**
 * Set the style of a tab view
 * @param tabview pointer to a tan view object
 * @param type which style should be set
 * @param style pointer to the new style
 */
void lv_tabview_set_style(lv_obj_t *tabview, lv_tabview_style_t type, lv_style_t *style)
{
    lv_tabview_ext_t *ext = lv_obj_get_ext_attr(tabview);

    switch(type) {
        case LV_TABVIEW_STYLE_BG:
            lv_obj_set_style(tabview, style);
            break;
        case LV_TABVIEW_STYLE_BTN_BG:
            lv_btnm_set_style(ext->btns, LV_BTNM_STYLE_BG, style);
            tabview_realign(tabview);
            break;
        case LV_TABVIEW_STYLE_BTN_REL:
            lv_btnm_set_style(ext->btns, LV_BTNM_STYLE_BTN_REL, style);
            tabview_realign(tabview);
            break;
        case LV_TABVIEW_STYLE_BTN_PR:
            lv_btnm_set_style(ext->btns, LV_BTNM_STYLE_BTN_PR, style);
            break;
        case LV_TABVIEW_STYLE_BTN_TGL_REL:
            lv_btnm_set_style(ext->btns, LV_BTNM_STYLE_BTN_TGL_REL, style);
            break;
        case LV_TABVIEW_STYLE_BTN_TGL_PR:
            lv_btnm_set_style(ext->btns, LV_BTNM_STYLE_BTN_TGL_PR, style);
            break;
        case LV_TABVIEW_STYLE_INDIC:
            lv_obj_set_style(ext->indic, style);
            lv_obj_set_height(ext->indic, style->body.padding.inner);
            tabview_realign(tabview);
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the index of the currently active tab
 * @param tabview pointer to Tab view object
 * @return the active btn index
 */
uint16_t lv_tabview_get_tab_act(lv_obj_t * tabview)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext_attr(tabview);
    return ext->tab_cur;
}

/**
 * Get the number of tabs
 * @param tabview pointer to Tab view object
 * @return btn count
 */
uint16_t lv_tabview_get_tab_count(lv_obj_t * tabview)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext_attr(tabview);
    return ext->tab_cnt;
}

/**
 * Get the page (content area) of a tab
 * @param tabview pointer to Tab view object
 * @param id index of the btn (>= 0)
 * @return pointer to page (lv_page) object
 */
lv_obj_t * lv_tabview_get_tab(lv_obj_t * tabview, uint16_t id)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tabview);
    uint16_t i = 0;
    lv_obj_t * page = lv_obj_get_child_back(ext->content, NULL);

    while(page != NULL && i != id) {
        i++;
        page = lv_obj_get_child_back(ext->content, page);
    }

    if(i == id) return page;

    return NULL;
}

/**
 * Get the tab load action
 * @param tabview pointer to a tabview object
 * @param return the current btn load action
 */
lv_tabview_action_t lv_tabview_get_tab_load_action(lv_obj_t *tabview)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext_attr(tabview);
    return ext->tab_load_action;
}

/**
 * Get horizontal sliding is enabled or not
 * @param tabview pointer to Tab view object
 * @return true: enable sliding; false: disable sliding
 */
bool lv_tabview_get_sliding(lv_obj_t * tabview)
{
    lv_tabview_ext_t *ext = lv_obj_get_ext_attr(tabview);
    return ext->slide_enable ? true : false;
}

/**
 * Get the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @return time of animation in milliseconds
 */
uint16_t lv_tabview_get_anim_time(lv_obj_t * tabview)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext_attr(tabview);
    return ext->anim_time;
}

/**
 * Get a style of a tab view
 * @param tabview pointer to a ab view object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_tabview_get_style(lv_obj_t *tabview, lv_tabview_style_t type)
{
    lv_tabview_ext_t *ext = lv_obj_get_ext_attr(tabview);

    switch (type) {
        case LV_TABVIEW_STYLE_BG:           return lv_obj_get_style(tabview);
        case LV_TABVIEW_STYLE_BTN_BG:       return lv_btnm_get_style(ext->btns, LV_BTNM_STYLE_BG);
        case LV_TABVIEW_STYLE_BTN_REL:      return lv_btnm_get_style(ext->btns, LV_BTNM_STYLE_BTN_REL);
        case LV_TABVIEW_STYLE_BTN_PR:       return lv_btnm_get_style(ext->btns, LV_BTNM_STYLE_BTN_PR);
        case LV_TABVIEW_STYLE_BTN_TGL_REL:  return lv_btnm_get_style(ext->btns, LV_BTNM_STYLE_BTN_TGL_REL);
        case LV_TABVIEW_STYLE_BTN_TGL_PR:   return lv_btnm_get_style(ext->btns, LV_BTNM_STYLE_BTN_TGL_PR);
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the Tab view
 * @param tabview pointer to a Tab view object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_tabview_signal(lv_obj_t * tabview, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(tabview, sign, param);
    if(res != LV_RES_OK) return res;

    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tabview);
    if(sign == LV_SIGNAL_CLEANUP) {
        uint8_t i;
        for(i = 0; ext->tab_name_ptr[i][0] != '\0'; i++) lv_mem_free(ext->tab_name_ptr[i]);

        lv_mem_free(ext->tab_name_ptr);
        ext->tab_name_ptr = NULL;
        ext->btns = NULL;     /*These objects were children so they are already invalid*/
        ext->content= NULL;
    }
    else if(sign == LV_SIGNAL_CORD_CHG) {
        if(ext->content != NULL &&
          (lv_obj_get_width(tabview) != lv_area_get_width(param) ||
           lv_obj_get_height(tabview) != lv_area_get_height(param)))
        {
            tabview_realign(tabview);
        }
    }
    else if(sign == LV_SIGNAL_FOCUS || sign == LV_SIGNAL_DEFOCUS || sign == LV_SIGNAL_CONTROLL) {
        if(ext->btns) {
            ext->btns->signal_func(ext->btns, sign, param);
        }
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_tabview";
    }

    return res;
}


/**
 * Signal function of a tab's page
 * @param tab pointer to a tab page object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t tabpage_signal(lv_obj_t * tab_page, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = page_signal(tab_page, sign, param);
    if(res != LV_RES_OK) return res;

    lv_obj_t * cont = lv_obj_get_parent(tab_page);
    lv_obj_t * tabview = lv_obj_get_parent(cont);

    if(lv_tabview_get_sliding(tabview) == false) return res;

    if(sign == LV_SIGNAL_PRESSED) {
        tabpage_pressed_handler(tabview, tab_page);
    }
    else if(sign == LV_SIGNAL_PRESSING) {
        tabpage_pressing_handler(tabview, tab_page);
    }
    else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        tabpage_press_lost_handler(tabview, tab_page);
    }

    return res;
}
/**
 * Signal function of the tab page's scrollable object
 * @param tab_scrl pointer to a tab page's scrollable object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t tabpage_scrl_signal(lv_obj_t * tab_scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = page_scrl_signal(tab_scrl, sign, param);
    if(res != LV_RES_OK) return res;

    lv_obj_t * tab_page = lv_obj_get_parent(tab_scrl);
    lv_obj_t * cont = lv_obj_get_parent(tab_page);
    lv_obj_t * tabview = lv_obj_get_parent(cont);

    if(lv_tabview_get_sliding(tabview) == false) return res;

    if(sign == LV_SIGNAL_PRESSED) {
        tabpage_pressed_handler(tabview, tab_page);
    }
    else if(sign == LV_SIGNAL_PRESSING) {
        tabpage_pressing_handler(tabview, tab_page);
    }
    else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        tabpage_press_lost_handler(tabview, tab_page);
    }

    return res;
}

/**
 * Called when a tab's page or scrollable object is pressed
 * @param tabview pointer to the btn view object
 * @param tabpage pointer to the page of a btn
 */
static void tabpage_pressed_handler(lv_obj_t * tabview, lv_obj_t * tabpage)
{
    (void)tabpage;

    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tabview);
    lv_indev_t * indev = lv_indev_get_act();
    lv_indev_get_point(indev, &ext->point_last);
}

/**
 * Called when a tab's page or scrollable object is being pressed
 * @param tabview pointer to the btn view object
 * @param tabpage pointer to the page of a btn
 */
static void tabpage_pressing_handler(lv_obj_t * tabview, lv_obj_t * tabpage)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tabview);
    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t point_act;
    lv_indev_get_point(indev, &point_act);
    lv_coord_t x_diff = point_act.x - ext->point_last.x;
    lv_coord_t y_diff = point_act.y - ext->point_last.y;

    if(ext->draging == 0) {
        if(x_diff >= LV_INDEV_DRAG_LIMIT || x_diff<= -LV_INDEV_DRAG_LIMIT) {
            ext->drag_hor = 1;
            ext->draging = 1;
            lv_obj_set_drag(lv_page_get_scrl(tabpage), false);
        } else if(y_diff >= LV_INDEV_DRAG_LIMIT || y_diff <= -LV_INDEV_DRAG_LIMIT) {
            ext->drag_hor = 0;
            ext->draging = 1;
        }
    }
    if(ext->drag_hor) {
        lv_obj_set_x(ext->content, lv_obj_get_x(ext->content) + point_act.x - ext->point_last.x);
        ext->point_last.x = point_act.x;
        ext->point_last.y = point_act.y;

        /*Move the indicator*/
        lv_coord_t indic_width = lv_obj_get_width(ext->indic);
        lv_style_t * tabs_style = lv_obj_get_style(ext->btns);
        lv_style_t * indic_style = lv_obj_get_style(ext->indic);
        lv_coord_t p = ((tabpage->coords.x1 - tabview->coords.x1) * (indic_width + tabs_style->body.padding.inner)) / lv_obj_get_width(tabview);

        lv_obj_set_x(ext->indic, indic_width * ext->tab_cur + tabs_style->body.padding.inner * ext->tab_cur + indic_style->body.padding.hor - p);
    }
}

/**
 * Called when a tab's page or scrollable object is released or the press id lost
 * @param tabview pointer to the btn view object
 * @param tabpage pointer to the page of a btn
 */
static void tabpage_press_lost_handler(lv_obj_t * tabview, lv_obj_t * tabpage)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tabview);
    ext->drag_hor = 0;
    ext->draging = 0;

    lv_obj_set_drag(lv_page_get_scrl(tabpage), true);

    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t point_act;
    lv_indev_get_point(indev, &point_act);
    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);
    lv_coord_t x_predict = 0;

    while(vect.x != 0)   {
        x_predict += vect.x;
        vect.x = vect.x * (100 - LV_INDEV_DRAG_THROW) / 100;
    }

    lv_coord_t page_x1 = tabpage->coords.x1 - tabview->coords.x1 + x_predict;
    lv_coord_t page_x2 = page_x1 + lv_obj_get_width(tabpage);
    lv_coord_t treshold = lv_obj_get_width(tabview) / 2;

    uint16_t tab_cur = ext->tab_cur;
    if(page_x1 > treshold) {
        if(tab_cur != 0) tab_cur--;
    } else if(page_x2 < treshold) {
        if(tab_cur < ext->tab_cnt - 1) tab_cur++;
    }

    lv_tabview_set_tab_act(tabview, tab_cur, true);
}

/**
 * Called when a tab button is released
 * @param tab_btnm pointer to the tab's button matrix object
 * @param id the id of the tab (>= 0)
 * @return LV_ACTION_RES_OK because the button matrix in not deleted in the function
 */
static lv_res_t tab_btnm_action(lv_obj_t * tab_btnm, const char * tab_name)
{
    lv_obj_t * tab = lv_obj_get_parent(tab_btnm);
    const char ** tabs_map = lv_btnm_get_map(tab_btnm);

    uint8_t i = 0;

    while(tabs_map[i][0] != '\0') {
        if(strcmp(&tabs_map[i][1], tab_name) == 0) break;   /*[1] to skip the control byte*/
        i++;
    }

    lv_tabview_set_tab_act(tab, i, true);

    return LV_RES_OK;
}

/**
 * Realign and resize the elements of Tab view
 * @param tabview pointer to a Tab view object
 */
static void tabview_realign(lv_obj_t * tabview)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tabview);

    lv_obj_set_width(ext->btns, lv_obj_get_width(tabview));

    if(ext->tab_cnt != 0) {
        lv_style_t * style_btn_bg = lv_tabview_get_style(tabview, LV_TABVIEW_STYLE_BTN_BG);
        lv_style_t * style_btn_rel = lv_tabview_get_style(tabview, LV_TABVIEW_STYLE_BTN_REL);

        /*Set the indicator widths*/
        lv_coord_t indic_width = (lv_obj_get_width(tabview) - style_btn_bg->body.padding.inner * (ext->tab_cnt - 1) -
                2 * style_btn_bg->body.padding.hor) / ext->tab_cnt;
        lv_obj_set_width(ext->indic, indic_width);

        /*Set the tabs height*/
        lv_coord_t btns_height = lv_font_get_height(style_btn_rel->text.font) +
                              2 * style_btn_rel->body.padding.ver +
                              2 * style_btn_bg->body.padding.ver;
        lv_obj_set_height(ext->btns, btns_height);
    }

    lv_obj_set_height(ext->content, lv_obj_get_height(tabview) - lv_obj_get_height(ext->btns));
    lv_obj_align(ext->content, ext->btns, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    lv_obj_t * pages = lv_obj_get_child(ext->content, NULL);
    while(pages != NULL) {
        if(lv_obj_get_signal_func(pages) == tabpage_signal) {  /*Be sure adjust only the pages (user can other things)*/
            lv_obj_set_size(pages, lv_obj_get_width(tabview), lv_obj_get_height(ext->content));
        }
        pages = lv_obj_get_child(ext->content, pages);
    }


    lv_obj_align(ext->indic, ext->btns, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

    lv_tabview_set_tab_act(tabview, ext->tab_cur, false);
}
#endif
