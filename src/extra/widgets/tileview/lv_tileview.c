/**
 * @file lv_tileview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tileview.h"
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
static void lv_tileview_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_tileview_tile_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void tileview_event_cb(lv_obj_t * tv, lv_event_t e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_tileview_class = {.constructor_cb = lv_tileview_constructor,
                                    .base_class = &lv_obj_class,
                                    .instance_size = sizeof(lv_tileview_t)};

const lv_obj_class_t lv_tileview_tile_class = {.constructor_cb = lv_tileview_tile_constructor,
                                         .base_class = &lv_obj_class,
                                         .instance_size = sizeof(lv_tileview_tile_t)};

static lv_dir_t create_dir;
static uint32_t create_col_id;
static uint32_t create_row_id;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a tileview object
 * @param par pointer to an object, it will be the parent of the new tileview
 * @param copy pointer to a tileview object, if not NULL then the new object will be copied from it
 * @return pointer to the created tileview
 */
lv_obj_t * lv_tileview_create(lv_obj_t * parent)
{
   return lv_obj_create_from_class(&lv_tileview_class, parent, NULL);
}

/*======================
 * Add/remove functions
 *=====================*/

lv_obj_t * lv_tileview_add_tile(lv_obj_t * tv, uint8_t col_id, uint8_t row_id, lv_dir_t dir)
{
    create_dir = dir;
    create_col_id = col_id;
    create_row_id = row_id;
    return lv_obj_create_from_class(&lv_tileview_tile_class, tv, NULL);
}

void lv_obj_set_tile(lv_obj_t * tv, lv_obj_t * tile_obj, lv_anim_enable_t anim_en)
{
    lv_coord_t tx = lv_obj_get_x(tile_obj);
    lv_coord_t ty = lv_obj_get_y(tile_obj);

    lv_tileview_tile_t * tile = (lv_tileview_tile_t *) tile_obj;
    lv_obj_set_scroll_dir(tv, tile->dir);
    lv_obj_scroll_to(tv, tx, ty, anim_en);
}

void lv_obj_set_tile_id(lv_obj_t * tv, uint32_t col_id, uint32_t row_id, lv_anim_enable_t anim_en)
{
    lv_coord_t w = lv_obj_get_width_fit(tv);
    lv_coord_t h = lv_obj_get_height_fit(tv);

    lv_coord_t tx = col_id * w;
    lv_coord_t ty = row_id * h;

    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(tv); i++) {
        lv_obj_t * tile_obj = lv_obj_get_child(tv, i);
        lv_coord_t x = lv_obj_get_x(tile_obj);
        lv_coord_t y = lv_obj_get_y(tile_obj);
        if(x == tx && y == ty) {
            lv_obj_set_tile(tv, tile_obj, anim_en);
            return;
        }
    }

    LV_LOG_WARN("No tile found with at (%d,%d) index", col_id, row_id);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_tileview_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    lv_obj_set_size(obj, LV_COORD_PCT(100), LV_COORD_PCT(100));
    lv_obj_add_event_cb(obj, tileview_event_cb, NULL);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_snap_align_x(obj, LV_SCROLL_SNAP_ALIGN_CENTER);
    lv_obj_set_snap_align_y(obj, LV_SCROLL_SNAP_ALIGN_CENTER);

}

static void lv_tileview_tile_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    lv_obj_set_size(obj, LV_COORD_PCT(100), LV_COORD_PCT(100));
    lv_obj_set_pos(obj, create_col_id * lv_obj_get_width_fit(parent),  create_row_id * lv_obj_get_height_fit(parent));

    lv_tileview_tile_t * tile = (lv_tileview_tile_t *) obj;
    tile->dir = create_dir;

    if(create_col_id == 0 && create_row_id == 0) {
        lv_obj_set_scroll_dir(parent, create_dir);
    }
}

static void tileview_event_cb(lv_obj_t * tv, lv_event_t e)
{
    if(e == LV_EVENT_SCROLL_END) {
        lv_coord_t w = lv_obj_get_width_fit(tv);
        lv_coord_t h = lv_obj_get_height_fit(tv);

        lv_point_t scroll_end;
        lv_obj_get_scroll_end(tv, &scroll_end);
        lv_coord_t left = scroll_end.x;
        lv_coord_t top = scroll_end.y;

        lv_coord_t tx = ((left + (w / 2)) / w) * w;
        lv_coord_t ty = ((top + (h / 2)) / h) * h;


        lv_dir_t dir = LV_DIR_ALL;
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(tv); i++) {
            lv_obj_t * tile_obj = lv_obj_get_child(tv, i);
            lv_coord_t x = lv_obj_get_x(tile_obj);
            lv_coord_t y = lv_obj_get_y(tile_obj);
            if(x == tx && y == ty) {
                lv_tileview_tile_t * tile = (lv_tileview_tile_t *) tile_obj;
                dir = tile->dir;
                break;
            }
        }
        lv_obj_set_scroll_dir(tv, dir);
    }
}
#endif /*LV_USE_TILEVIEW*/
