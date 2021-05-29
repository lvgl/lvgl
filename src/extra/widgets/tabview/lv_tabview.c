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

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tabview_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_tabview_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_tabview_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void btns_value_changed_event_cb(lv_event_t * e);
static void cont_scroll_end_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_tabview_class = {
        .constructor_cb = lv_tabview_constructor,
        .destructor_cb = lv_tabview_destructor,
        .event_cb = lv_tabview_event,
        .width_def = LV_PCT(100),
        .height_def = LV_PCT(100),
        .base_class = &lv_obj_class,
        .instance_size = sizeof(lv_tabview_t)};

static lv_dir_t tabpos_create;
static lv_coord_t tabsize_create;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_tabview_create(lv_obj_t * parent, lv_dir_t tab_pos, lv_coord_t tab_size)
{
    LV_LOG_INFO("begin")
    tabpos_create = tab_pos;
    tabsize_create = tab_size;

    lv_obj_t * obj = lv_obj_class_create_obj(&lv_tabview_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_obj_t * lv_tabview_add_tab(lv_obj_t * obj, const char * name)
{
    lv_tabview_t * tabview = (lv_tabview_t *)obj;
    lv_obj_t * cont = lv_tabview_get_content(obj);

    lv_obj_t * page = lv_obj_create(cont);
    lv_obj_set_size(page, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(page, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    uint32_t tab_id = lv_obj_get_child_cnt(cont);

    lv_obj_t * btns = lv_tabview_get_tab_btns(obj);

    char ** old_map = tabview->map;
    char ** new_map;

    /*top or bottom dir*/
    if(tabview->tab_pos & LV_DIR_VER) {
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
        if(tabview->tab_cnt == 0) {
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
    tabview->map = new_map;
    lv_btnmatrix_set_map(btns, (const char **)new_map);
    lv_mem_free(old_map);

    lv_btnmatrix_set_btn_ctrl_all(btns, LV_BTNMATRIX_CTRL_CHECKABLE | LV_BTNMATRIX_CTRL_CLICK_TRIG | LV_BTNMATRIX_CTRL_NO_REPEAT);

    tabview->tab_cnt++;
    if(tabview->tab_cnt == 1) {
        lv_tabview_set_act(obj, 0, LV_ANIM_OFF);
    }

    lv_btnmatrix_set_btn_ctrl(btns, tabview->tab_cur, LV_BTNMATRIX_CTRL_CHECKED);

    return page;
}

void lv_tabview_set_act(lv_obj_t * obj, uint32_t id, lv_anim_enable_t anim_en)
{
    lv_tabview_t * tabview = (lv_tabview_t *)obj;

    if(id >= tabview->tab_cnt) {
        id = tabview->tab_cnt - 1;
    }

    /*To be sure lv_obj_get_content_width will return valid value*/
    lv_obj_update_layout(obj);

    lv_obj_t * cont = lv_tabview_get_content(obj);
    if(cont == NULL) return;
    lv_coord_t gap = lv_obj_get_style_pad_column(cont, LV_PART_MAIN);
    lv_coord_t w = lv_obj_get_content_width(obj);
    if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) != LV_BASE_DIR_RTL) {
        lv_obj_scroll_to_x(cont, id * (gap + w), anim_en);
    } else {
        int32_t id_rtl = -(int32_t)id;
        lv_obj_scroll_to_x(cont, (gap + w) * id_rtl, anim_en);
    }

    lv_obj_t * btns = lv_tabview_get_tab_btns(obj);
    lv_btnmatrix_set_btn_ctrl(btns, id, LV_BTNMATRIX_CTRL_CHECKED);
    tabview->tab_cur = id;
}

uint16_t lv_tabview_get_tab_act(lv_obj_t * obj)
{
    lv_tabview_t * tabview = (lv_tabview_t *)obj;
    return tabview->tab_cur;
}

lv_obj_t * lv_tabview_get_content(lv_obj_t * tv)
{
    return lv_obj_get_child(tv, 1);
}

lv_obj_t * lv_tabview_get_tab_btns(lv_obj_t * tv)
{
    return lv_obj_get_child(tv, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_tabview_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_tabview_t * tabview = (lv_tabview_t *)obj;

    tabview->tab_pos = tabpos_create;

    switch(tabview->tab_pos) {
    case LV_DIR_TOP:
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
        break;
    case LV_DIR_BOTTOM:
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN_REVERSE);
        break;
    case LV_DIR_LEFT:
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
        break;
    case LV_DIR_RIGHT:
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW_REVERSE);
        break;
    }

    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));

    lv_obj_t * btnm;
    lv_obj_t * cont;

    btnm = lv_btnmatrix_create(obj);
    cont = lv_obj_create(obj);

    lv_btnmatrix_set_one_checked(btnm, true);
    tabview->map = lv_mem_alloc(sizeof(const char *));
    tabview->map[0] = "";
    lv_btnmatrix_set_map(btnm, (const char **)tabview->map);
    lv_obj_add_event_cb(btnm, btns_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_flag(btnm, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_add_event_cb(cont, cont_scroll_end_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    switch(tabview->tab_pos) {
     case LV_DIR_TOP:
     case LV_DIR_BOTTOM:
         lv_obj_set_size(btnm, LV_PCT(100), tabsize_create);
         lv_obj_set_width(cont, LV_PCT(100));
         lv_obj_set_flex_grow(cont, 1);
         break;
     case LV_DIR_LEFT:
     case LV_DIR_RIGHT:
         lv_obj_set_size(btnm, tabsize_create, LV_PCT(100));
         lv_obj_set_height(cont, LV_PCT(100));
         lv_obj_set_flex_grow(cont, 1);
         break;
     }

    lv_group_t * g = lv_group_get_default();
    if(g) lv_group_add_obj(g, btnm);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
}

static void lv_tabview_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_tabview_t * tabview = (lv_tabview_t *)obj;

    uint32_t i;
    if(tabview->tab_pos & LV_DIR_VER) {
        for(i = 0; i < tabview->tab_cnt; i++) {
            lv_mem_free(tabview->map[i]);
            tabview->map[i] = NULL;
        }
    }
    if(tabview->tab_pos & LV_DIR_HOR) {
        for(i = 0; i < tabview->tab_cnt; i++) {
            lv_mem_free(tabview->map[i * 2]);
            tabview->map[i * 2] = NULL;
        }
    }


    lv_mem_free(tabview->map);
    tabview->map = NULL;
}

static void lv_tabview_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_res_t res = lv_obj_event_base(&lv_tabview_class, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if(code == LV_EVENT_SIZE_CHANGED) {
        lv_tabview_set_act(target, lv_tabview_get_tab_act(target), LV_ANIM_OFF);
    }
}


static void btns_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * btns = lv_event_get_target(e);

    lv_obj_t * tv = lv_obj_get_parent(btns);
    uint32_t id = lv_btnmatrix_get_selected_btn(btns);
    lv_tabview_set_act(tv, id, LV_ANIM_ON);

    lv_event_send(tv, LV_EVENT_VALUE_CHANGED, NULL);
}

static void cont_scroll_end_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    lv_obj_t * tv = lv_obj_get_parent(cont);
    if(code == LV_EVENT_LAYOUT_CHANGED) {
        lv_tabview_set_act(tv, lv_tabview_get_tab_act(tv), LV_ANIM_OFF);
    }
    else if(code == LV_EVENT_SCROLL_END) {
        lv_point_t p;
        lv_obj_get_scroll_end(cont, &p);

        lv_coord_t w = lv_obj_get_content_width(cont);
        lv_coord_t t;

        if(lv_obj_get_style_base_dir(tv, LV_PART_MAIN) == LV_BASE_DIR_RTL)  t = -(p.x - w/ 2) / w;
        else t = (p.x + w/ 2) / w;

        if(t < 0) t = 0;
        lv_tabview_set_act(tv, t, LV_ANIM_ON);
        lv_event_send(tv, LV_EVENT_VALUE_CHANGED, NULL);
    }
}
#endif /*LV_USE_TABVIEW*/
