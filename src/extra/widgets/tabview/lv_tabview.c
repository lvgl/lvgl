/**
 * @file lv_tabview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tabview.h"
#if LV_USE_TABVIEW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint16_t tab_cnt;
    uint16_t tab_cur;
}lv_tabview_ext_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void btns_event_cb(lv_obj_t * btns, lv_event_t e);
static void cont_event_cb(lv_obj_t * cont, lv_event_t e);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool inited;
static lv_style_t style_bg;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_tabview_create(lv_obj_t * parent, lv_dir_t tab_pos, lv_coord_t tab_size)
{
    if(!inited) {
        lv_style_init(&style_bg);
        lv_style_set_radius(&style_bg, LV_STATE_DEFAULT, 0);

        inited = true;
    }

    lv_obj_t * tabview = lv_obj_create(parent, NULL);
    lv_tabview_ext_t * ext = lv_obj_allocate_ext_attr(tabview, sizeof(lv_tabview_ext_t));

    lv_memset_00(ext, sizeof(lv_tabview_ext_t));
    lv_flex_dir_t flex_dir;
    switch(tab_pos) {
    case LV_DIR_TOP:
        flex_dir = LV_FLEX_DIR_COLUMN;
        break;
    case LV_DIR_BOTTOM:
        flex_dir = LV_FLEX_DIR_COLUMN_REVERSE;
        break;
    case LV_DIR_LEFT:
        flex_dir = LV_FLEX_DIR_ROW;
        break;
    case LV_DIR_RIGHT:
        flex_dir = LV_FLEX_DIR_ROW_REVERSE;
        break;
    }

    lv_obj_reset_style_list(tabview, LV_OBJ_PART_MAIN);

    lv_obj_set_size(tabview, LV_COORD_PCT(100), LV_COORD_PCT(100));
    lv_obj_set_flex_dir(tabview, flex_dir);

    lv_obj_t * btnm;
    lv_obj_t * cont;


    btnm = lv_btnmatrix_create(tabview, NULL);
    cont = lv_obj_create(tabview, NULL);

    lv_btnmatrix_set_one_checked(btnm, true);
    const char ** map = lv_mem_alloc(sizeof(const char *));
    map[0] = "";
    lv_btnmatrix_set_map(btnm, map);
    lv_obj_add_style(btnm, LV_BTNMATRIX_PART_MAIN, &style_bg);
    lv_obj_set_event_cb(btnm, btns_event_cb);
    lv_obj_add_flag(btnm, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_set_event_cb(cont, cont_event_cb);
    lv_obj_reset_style_list(cont, LV_OBJ_PART_MAIN);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_flex_item(btnm, LV_FLEX_PLACE_START);
    lv_obj_set_flex_item(cont, LV_FLEX_PLACE_START);

    switch(tab_pos) {
     case LV_DIR_TOP:
     case LV_DIR_LEFT:
         lv_obj_set_size(btnm, LV_COORD_PCT(100), tab_size);
         lv_obj_set_size(cont, LV_COORD_PCT(100), LV_FLEX_GROW(1));
         break;
     case LV_DIR_BOTTOM:
     case LV_DIR_RIGHT:
         lv_obj_set_size(btnm, LV_COORD_PCT(100), tab_size);
         lv_obj_set_size(cont, LV_FLEX_GROW(1), LV_COORD_PCT(100));
         break;
     }

    lv_obj_set_flex_dir(cont, LV_FLEX_DIR_ROW);
    lv_obj_set_snap_align_x(cont, LV_SCROLL_SNAP_ALIGN_CENTER);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_STOP);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_FOCUS_SCROLL);

    return tabview;
}

lv_obj_t * lv_tabview_add_tab(lv_obj_t * tv, const char * name)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tv);
    lv_obj_t * cont = lv_tabview_get_content(tv);

    lv_obj_t * page = lv_obj_create(cont, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_style(page, LV_BTNMATRIX_PART_MAIN, &style_bg);
    uint32_t tab_id = lv_obj_count_children(cont);

    lv_obj_set_flex_item(page, true);
    lv_obj_set_size(page, LV_COORD_PCT(100), LV_COORD_PCT(100));

    lv_obj_t * btns = lv_tabview_get_tab_btns(tv);

    const char ** old_map = lv_btnmatrix_get_map_array(btns);
    const char ** new_map;

    /*top or bottom dir*/
    if(lv_obj_get_flex_dir(tv) == LV_FLEX_DIR_COLUMN) {
        new_map = lv_mem_alloc((tab_id + 1) * sizeof(const char *));
        lv_memcpy_small(new_map, old_map, sizeof(const char *) * (tab_id - 1));
        new_map[tab_id - 1] = lv_mem_alloc(strlen(name) + 1);
        strcpy((char *)new_map[tab_id - 1], name);
        new_map[tab_id] = "";
    }
    /*left or right dir*/
    else {
        new_map = lv_mem_alloc((tab_id * 2) * sizeof(const char *));
        lv_memcpy_small(new_map, old_map, sizeof(const char *) * tab_id * 2);
        if(ext->tab_cnt == 0) {
            new_map[0] = lv_mem_alloc(strlen(name) + 1);
            strcpy((char *)new_map[0], name);
            new_map[1] = "";
        } else {
            new_map[tab_id * 2 - 3] = "\n";
            new_map[tab_id * 2 - 2] = lv_mem_alloc(strlen(name) + 1);
            new_map[tab_id * 2 - 1] = "";
            strcpy((char *)new_map[(tab_id * 2) - 2], name);
        }
    }

    lv_btnmatrix_set_map(btns, new_map);
    lv_mem_free(old_map);

    lv_btnmatrix_set_btn_ctrl_all(btns, LV_BTNMATRIX_CTRL_CHECKABLE | LV_BTNMATRIX_CTRL_CLICK_TRIG | LV_BTNMATRIX_CTRL_NO_REPEAT);

    ext->tab_cnt++;
    if(ext->tab_cnt == 1) {
        lv_tabview_set_act(tv, 0);
    }

    lv_btnmatrix_set_btn_ctrl(btns, ext->tab_cur, LV_BTNMATRIX_CTRL_CHECKED);

    return page;
}

void lv_tabview_set_act(lv_obj_t * tv, uint32_t id)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tv);
    if(id >= ext->tab_cnt) {
        id = ext->tab_cnt - 1;
    }

    lv_obj_t * cont = lv_tabview_get_content(tv);
    lv_obj_t * tab = lv_obj_get_child(cont, NULL);
    lv_obj_scroll_to_x(cont, id * lv_obj_get_width(tab), LV_ANIM_ON);

    lv_obj_t * btns = lv_tabview_get_tab_btns(tv);
    lv_btnmatrix_set_btn_ctrl(btns, id, LV_BTNMATRIX_CTRL_CHECKED);
    ext->tab_cur = id;
}

uint16_t lv_tabview_get_tab_act(lv_obj_t * tv)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tv);
    return ext->tab_cur;

}

lv_obj_t * lv_tabview_get_content(lv_obj_t * tv)
{
    return lv_obj_get_child(tv, NULL);
}

lv_obj_t * lv_tabview_get_tab_btns(lv_obj_t * tv)
{
    return lv_obj_get_child_back(tv, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void btns_event_cb(lv_obj_t * btns, lv_event_t e)
{
    if(e == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * tv = lv_obj_get_parent(btns);
        uint32_t id = lv_btnmatrix_get_active_btn(btns);
        lv_tabview_set_act(tv, id);
    }
}
static void cont_event_cb(lv_obj_t * cont, lv_event_t e)
{
    if(e == LV_EVENT_SCROLL_END) {
        lv_obj_t * tv = lv_obj_get_parent(cont);

        lv_point_t p;
        lv_obj_get_scroll_end(cont, &p);

        lv_coord_t w = lv_obj_get_width_fit(cont);
        lv_coord_t t = (p.x + w/ 2) / w;
        if(t < 0) t = 0;
        lv_tabview_set_act(tv, t);
    }
}
#endif /*LV_USE_TABVIEW*/
