/**
 * @file lv_tileview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tileview_private.h"
#include "../../core/lv_obj_class_private.h"
#include "../../indev/lv_indev.h"
#include "../../indev/lv_indev_private.h"
#if LV_USE_TILEVIEW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tileview_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_tileview_tile_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void tileview_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_tileview_class = {
    .constructor_cb = lv_tileview_constructor,
    .base_class = &lv_obj_class,
    .instance_size = sizeof(lv_tileview_t),
    .name = "tileview",
};

const lv_obj_class_t lv_tileview_tile_class = {
    .constructor_cb = lv_tileview_tile_constructor,
    .base_class = &lv_obj_class,
    .instance_size = sizeof(lv_tileview_tile_t),
    .name = "tile",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_tileview_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_tileview_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*======================
 * Add/remove functions
 *=====================*/

lv_obj_t * lv_tileview_add_tile(lv_obj_t * tv, uint8_t col_id, uint8_t row_id, lv_dir_t dir)
{
    LV_LOG_INFO("begin");

    lv_obj_t * obj = lv_obj_class_create_obj(&lv_tileview_tile_class, tv);
    lv_obj_class_init_obj(obj);
    lv_obj_set_pos(obj, lv_pct(col_id * 100), lv_pct(row_id * 100));

    lv_tileview_tile_t * tile = (lv_tileview_tile_t *)obj;
    tile->dir = dir;

    if(col_id == 0 && row_id == 0) {
        lv_obj_set_scroll_dir(tv, dir);
    }
    return obj;
}

void lv_tileview_set_tile(lv_obj_t * obj, lv_obj_t * tile_obj, lv_anim_enable_t anim_en)
{
    int32_t tx = lv_obj_get_x(tile_obj);
    int32_t ty = lv_obj_get_y(tile_obj);

    lv_tileview_tile_t * tile = (lv_tileview_tile_t *)tile_obj;
    lv_tileview_t * tv = (lv_tileview_t *) obj;
    tv->tile_act = (lv_obj_t *)tile;

    lv_obj_set_scroll_dir(obj, tile->dir);
    lv_obj_scroll_to(obj, tx, ty, anim_en);
}

void lv_tileview_set_tile_by_index(lv_obj_t * tv, uint32_t col_id, uint32_t row_id, lv_anim_enable_t anim_en)
{
    lv_obj_update_layout(tv);

    int32_t w = lv_obj_get_content_width(tv);
    int32_t h = lv_obj_get_content_height(tv);

    int32_t tx = col_id * w;
    int32_t ty = row_id * h;

    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(tv); i++) {
        lv_obj_t * tile_obj = lv_obj_get_child(tv, i);
        int32_t x = lv_obj_get_x(tile_obj);
        int32_t y = lv_obj_get_y(tile_obj);
        if(x == tx && y == ty) {
            lv_tileview_set_tile(tv, tile_obj, anim_en);
            return;
        }
    }

    LV_LOG_WARN("No tile found with at (%d,%d) index", (int)col_id, (int)row_id);
}

lv_obj_t * lv_tileview_get_tile_active(lv_obj_t * obj)
{
    lv_tileview_t * tv = (lv_tileview_t *) obj;
    return tv->tile_act;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_tileview_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_add_event_cb(obj, tileview_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_scroll_snap_x(obj, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scroll_snap_y(obj, LV_SCROLL_SNAP_CENTER);

}

static void lv_tileview_tile_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{

    LV_UNUSED(class_p);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_update_layout(obj);  /*Be sure the size is correct*/
}

static void tileview_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_tileview_t * tv = (lv_tileview_t *) obj;

    if(code == LV_EVENT_SCROLL_END) {
        lv_indev_t * indev = lv_indev_active();
        if(indev && indev->state == LV_INDEV_STATE_PRESSED) {
            return;
        }

        int32_t w = lv_obj_get_content_width(obj);
        int32_t h = lv_obj_get_content_height(obj);

        lv_point_t scroll_end;
        lv_obj_get_scroll_end(obj, &scroll_end);
        int32_t left = scroll_end.x;
        int32_t top = scroll_end.y;

        int32_t tx = ((left + (w / 2)) / w) * w;
        int32_t ty = ((top + (h / 2)) / h) * h;

        lv_dir_t dir = LV_DIR_ALL;
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_count(obj); i++) {
            lv_obj_t * tile_obj = lv_obj_get_child(obj, i);
            int32_t x = lv_obj_get_x(tile_obj);
            int32_t y = lv_obj_get_y(tile_obj);
            if(x == tx && y == ty) {
                lv_tileview_tile_t * tile = (lv_tileview_tile_t *)tile_obj;
                tv->tile_act = (lv_obj_t *)tile;
                dir = tile->dir;
                lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
                break;
            }
        }
        lv_obj_set_scroll_dir(obj, dir);
    }
}
#endif /*LV_USE_TILEVIEW*/
