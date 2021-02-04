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
typedef struct
{
    lv_dir_t dir;
}lv_tile_ext_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void tileview_event_cb(lv_obj_t * tv, lv_event_t e);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool inited;
static lv_style_t style_bg;
static lv_style_t style_tile;

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
    if(!inited) {
        lv_style_init(&style_bg);
        lv_style_set_radius(&style_bg, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_all(&style_bg, LV_STATE_DEFAULT, 0);

        lv_style_init(&style_tile);
        lv_style_set_radius(&style_tile, LV_STATE_DEFAULT, 0);
    }

    lv_obj_t * tileview = lv_obj_create(parent, NULL);
    LV_ASSERT_MEM(tileview);
    if(tileview == NULL) return NULL;

    lv_obj_add_style(tileview, LV_OBJ_PART_MAIN, &style_bg);
    lv_obj_set_size(tileview, LV_COORD_PCT(100), LV_COORD_PCT(100));
    lv_obj_set_event_cb(tileview, tileview_event_cb);
    lv_obj_add_flag(tileview, LV_OBJ_FLAG_SCROLL_STOP);
    lv_obj_set_snap_align_x(tileview, LV_SCROLL_SNAP_ALIGN_CENTER);
    lv_obj_set_snap_align_y(tileview, LV_SCROLL_SNAP_ALIGN_CENTER);
    return tileview;
}

/*======================
 * Add/remove functions
 *=====================*/

lv_obj_t * lv_tileview_add_tile(lv_obj_t * tv, uint8_t col_id, uint8_t row_id, lv_dir_t dir)
{
    lv_obj_t * tile = lv_obj_create(tv, NULL);
    lv_obj_set_size(tile, LV_COORD_PCT(100), LV_COORD_PCT(100));
    lv_obj_set_pos(tile, col_id * lv_obj_get_width_fit(tv),  row_id * lv_obj_get_height_fit(tv));
    lv_obj_add_style(tile, LV_OBJ_PART_MAIN, &style_tile);

    lv_tile_ext_t * ext = lv_obj_allocate_ext_attr(tile, sizeof(lv_tile_ext_t));

    ext->dir = dir;

    if(col_id == 0 && row_id == 0) {
        lv_obj_set_scroll_dir(tv, dir);
    }

    return tile;
}

void lv_obj_set_tile(lv_obj_t * tv, lv_obj_t * tile, lv_anim_enable_t anim_en)
{
    lv_coord_t tx = lv_obj_get_x(tile);
    lv_coord_t ty = lv_obj_get_y(tile);

    lv_tile_ext_t * ext = lv_obj_get_ext_attr(tile);
    lv_obj_set_scroll_dir(tv, ext->dir);
    lv_obj_scroll_to(tv, tx, ty, anim_en);
}

void lv_obj_set_tile_id(lv_obj_t * tv, uint32_t col_id, uint32_t row_id, lv_anim_enable_t anim_en)
{
    lv_coord_t w = lv_obj_get_width_fit(tv);
    lv_coord_t h = lv_obj_get_height_fit(tv);

    lv_coord_t tx = col_id * w;
    lv_coord_t ty = row_id * h;

    lv_dir_t dir = LV_DIR_ALL;
    lv_obj_t * tile = lv_obj_get_child(tv, NULL);
    while(tile) {
        lv_coord_t x = lv_obj_get_x(tile);
        lv_coord_t y = lv_obj_get_y(tile);
        if(x == tx && y == ty) {
            lv_tile_ext_t * ext = lv_obj_get_ext_attr(tile);
            dir = ext->dir;
            break;
        }
        tile = lv_obj_get_child(tv, tile);
    }
    lv_obj_set_scroll_dir(tv, dir);
    lv_obj_scroll_to(tv, tx, ty, anim_en);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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
        lv_obj_t * tile = lv_obj_get_child(tv, NULL);
        while(tile) {
            lv_coord_t x = lv_obj_get_x(tile);
            lv_coord_t y = lv_obj_get_y(tile);
            if(x == tx && y == ty) {
                lv_tile_ext_t * ext = lv_obj_get_ext_attr(tile);
                dir = ext->dir;
                break;
            }
            tile = lv_obj_get_child(tv, tile);
        }

        lv_obj_set_scroll_dir(tv, dir);
    }
}
#endif /*LV_USE_TILEVIEW*/
