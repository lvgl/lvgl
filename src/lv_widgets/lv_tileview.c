/**
 * @file lv_tileview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tileview.h"
#if LV_USE_TILEVIEW != 0

#include <stdbool.h>
#include "lv_cont.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_tileview"

#if LV_USE_ANIMATION
    #ifndef LV_TILEVIEW_DEF_ANIM_TIME
        #define LV_TILEVIEW_DEF_ANIM_TIME 300 /*Animation time loading a tile [ms] (0: no animation)  */
    #endif
#else
    #undef LV_TILEVIEW_DEF_ANIM_TIME
    #define LV_TILEVIEW_DEF_ANIM_TIME 0 /*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_tileview_signal(lv_obj_t * tileview, lv_signal_t sign, void * param);
static lv_res_t lv_tileview_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
static void drag_end_handler(lv_obj_t * tileview);
static bool set_valid_drag_dirs(lv_obj_t * tileview);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_signal_cb_t ancestor_scrl_signal;
static lv_design_cb_t ancestor_design;

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
lv_obj_t * lv_tileview_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("tileview create started");

    /*Create the ancestor of tileview*/
    lv_obj_t * new_tileview = lv_page_create(par, copy);
    LV_ASSERT_MEM(new_tileview);
    if(new_tileview == NULL) return NULL;

    /*Allocate the tileview type specific extended data*/
    lv_tileview_ext_t * ext = lv_obj_allocate_ext_attr(new_tileview, sizeof(lv_tileview_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(new_tileview);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_tileview);
    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = lv_obj_get_signal_cb(lv_page_get_scrollable(new_tileview));
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_tileview);

    /*Initialize the allocated 'ext' */
#if LV_USE_ANIMATION
    ext->anim_time = LV_TILEVIEW_DEF_ANIM_TIME;
#endif
    ext->act_id.x      = 0;
    ext->act_id.y      = 0;
    ext->valid_pos     = NULL;
    ext->valid_pos_cnt = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_tileview, lv_tileview_signal);
    lv_obj_set_signal_cb(lv_page_get_scrollable(new_tileview), lv_tileview_scrl_signal);

    /*Init the new tileview*/
    if(copy == NULL) {
        /* Set a size which fits into the parent.
         * Don't use `par` directly because if the tileview is created on a page it is moved to the
         * scrollable so the parent has changed */
        lv_coord_t w;
        lv_coord_t h;
        if(par) {
            w = lv_obj_get_width_fit(lv_obj_get_parent(new_tileview));
            h = lv_obj_get_height_fit(lv_obj_get_parent(new_tileview));
        }
        else {
            w = lv_disp_get_hor_res(NULL);
            h = lv_disp_get_ver_res(NULL);
        }

        lv_obj_set_size(new_tileview, w, h);
        lv_obj_set_drag_throw(lv_page_get_scrollable(new_tileview), true);
        lv_obj_set_drag_dir(lv_page_get_scrollable(new_tileview), LV_DRAG_DIR_ONE);

        lv_page_set_scrollable_fit(new_tileview, LV_FIT_MAX);

        lv_obj_reset_style_list(new_tileview, LV_PAGE_PART_SCROLLABLE);
        lv_theme_apply(new_tileview, LV_THEME_TILEVIEW);
    }
    /*Copy an existing tileview*/
    else {
        lv_tileview_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->act_id.x                = copy_ext->act_id.x;
        ext->act_id.y                = copy_ext->act_id.y;
        ext->valid_pos               = copy_ext->valid_pos;
        ext->valid_pos_cnt           = copy_ext->valid_pos_cnt;
#if LV_USE_ANIMATION
        ext->anim_time = copy_ext->anim_time;
#endif

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_tileview, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("tileview created");

    return new_tileview;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Register an object on the tileview. The register object will able to slide the tileview
 * @param tileview pointer to a Tileview object
 * @param element pointer to an object
 */
void lv_tileview_add_element(lv_obj_t * tileview, lv_obj_t * element)
{
    LV_UNUSED(tileview);
    LV_ASSERT_OBJ(tileview, LV_OBJX_NAME);
    LV_ASSERT_NULL(tileview);

    lv_page_glue_obj(element, true);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the valid position's indices. The scrolling will be possible only to these positions.
 * @param tileview pointer to a Tileview object
 * @param valid_pos array width the indices. E.g. `lv_point_t p[] = {{0,0}, {1,0}, {1,1}`.
 * Only the pointer is saved so can't be a local variable.
 * @param valid_pos_cnt number of elements in `valid_pos` array
 */
void lv_tileview_set_valid_positions(lv_obj_t * tileview, const lv_point_t valid_pos[], uint16_t valid_pos_cnt)
{
    LV_ASSERT_OBJ(tileview, LV_OBJX_NAME);
    LV_ASSERT_NULL(valid_pos);

    lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);
    ext->valid_pos          = valid_pos;
    ext->valid_pos_cnt      = valid_pos_cnt;

    set_valid_drag_dirs(tileview);

    /*If valid pos. is selected do nothing*/
    uint16_t i;
    for(i = 0; i < valid_pos_cnt; i++) {
        if(valid_pos[i].x == ext->act_id.x && valid_pos[i].y == ext->act_id.y) {
            return;
        }
    }

    /*Set a valid position if now an invalid is selected*/
    if(valid_pos_cnt > 0) {
        lv_tileview_set_tile_act(tileview, valid_pos[0].x, valid_pos[0].y, LV_ANIM_OFF);
    }
}

/**
 * Set the tile to be shown
 * @param tileview pointer to a tileview object
 * @param x column id (0, 1, 2...)
 * @param y line id (0, 1, 2...)
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_tileview_set_tile_act(lv_obj_t * tileview, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(tileview, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif

    lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);

    uint32_t tile_id;
    bool valid = false;
    for(tile_id = 0; tile_id < ext->valid_pos_cnt; tile_id++) {
        if(ext->valid_pos[tile_id].x == x && ext->valid_pos[tile_id].y == y) {
            valid = true;
            break;
        }
    }

    if(valid == false) return; /*Don't load not valid tiles*/

    ext->act_id.x = x;
    ext->act_id.y = y;

    lv_coord_t x_coord = -x * lv_obj_get_width(tileview);
    lv_coord_t y_coord = -y * lv_obj_get_height(tileview);
    lv_obj_t * scrl    = lv_page_get_scrollable(tileview);
    if(anim) {
#if LV_USE_ANIMATION
        lv_coord_t x_act = lv_obj_get_x(scrl);
        lv_coord_t y_act = lv_obj_get_y(scrl);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, scrl);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_anim_set_time(&a, ext->anim_time);


        if(x_coord != x_act) {
            lv_anim_set_values(&a, x_act, x_coord);
            lv_anim_start(&a);
        }

        if(y_coord != y_act) {
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_values(&a, y_act, y_coord);
            lv_anim_start(&a);
        }
#endif
    }
    else {
        lv_obj_set_pos(scrl, x_coord, y_coord);
    }

    lv_res_t res;
    res          = lv_event_send(tileview, LV_EVENT_VALUE_CHANGED, &tile_id);
    if(res != LV_RES_OK) return; /*Prevent the tile loading*/

    set_valid_drag_dirs(tileview);
}

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */
/**
* Get the tile to be shown
* @param tileview pointer to a tileview object
* @param x column id (0, 1, 2...)
* @param y line id (0, 1, 2...)
*/
void lv_tileview_get_tile_act(lv_obj_t * tileview, lv_coord_t * x, lv_coord_t * y)
{
    lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);

    *x = ext->act_id.x;
    *y = ext->act_id.y;
}

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the tileview
 * @param tileview pointer to a tileview object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_tileview_signal(lv_obj_t * tileview, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(tileview, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }

    return res;
}

/**
 * Signal function of the tileview scrollable
 * @param tileview pointer to the scrollable part of the tileview object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_tileview_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{

    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_scrl_signal(scrl, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, "");

    lv_obj_t * tileview         = lv_obj_get_parent(scrl);

    if(sign == LV_SIGNAL_DRAG_BEGIN) {
        set_valid_drag_dirs(tileview);
    }
    else if(sign == LV_SIGNAL_DRAG_THROW_BEGIN) {
        drag_end_handler(tileview);

        res = lv_indev_finish_drag(lv_indev_get_act());
        if(res != LV_RES_OK) return res;
    }
    /*Apply constraint on moving of the tileview*/
    else if(sign == LV_SIGNAL_COORD_CHG) {
        lv_indev_t * indev = lv_indev_get_act();
        if(indev) {
            lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);

            lv_coord_t x = lv_obj_get_x(scrl);
            lv_coord_t y = lv_obj_get_y(scrl);
            lv_coord_t h = lv_obj_get_height(tileview);
            lv_coord_t w = lv_obj_get_width(tileview);
            lv_coord_t top = lv_obj_get_style_pad_top(tileview, LV_TILEVIEW_PART_BG);
            lv_coord_t left = lv_obj_get_style_pad_left(tileview, LV_TILEVIEW_PART_BG);
            if(!ext->drag_top_en && y > -(ext->act_id.y * h) && indev->proc.types.pointer.vect.y > 0)  {
                lv_page_start_edge_flash(tileview, LV_PAGE_EDGE_TOP);
                lv_obj_set_y(scrl, -ext->act_id.y * h + top);
            }
            if(!ext->drag_bottom_en && indev->proc.types.pointer.vect.y < 0 && y < -(ext->act_id.y * h)) {
                lv_page_start_edge_flash(tileview, LV_PAGE_EDGE_BOTTOM);
                lv_obj_set_y(scrl, -ext->act_id.y * h + top);
            }

            if(!ext->drag_left_en && x > -(ext->act_id.x * w) && indev->proc.types.pointer.vect.x > 0) {
                lv_page_start_edge_flash(tileview, LV_PAGE_EDGE_LEFT);
                lv_obj_set_x(scrl, -ext->act_id.x * w + left);
            }

            if(!ext->drag_right_en && indev->proc.types.pointer.vect.x < 0 && x < -(ext->act_id.x * w)) {
                lv_page_start_edge_flash(tileview, LV_PAGE_EDGE_RIGHT);
                lv_obj_set_x(scrl, -ext->act_id.x * w + left);
            }

            /*Apply the drag constraints*/
            lv_drag_dir_t drag_dir = indev->proc.types.pointer.drag_dir;
            if(drag_dir == LV_DRAG_DIR_HOR)
                lv_obj_set_y(scrl, -ext->act_id.y * lv_obj_get_height(tileview) + top);
            else if(drag_dir == LV_DRAG_DIR_VER)
                lv_obj_set_x(scrl, -ext->act_id.x * lv_obj_get_width(tileview) + left);
        }
    }
    return res;
}

/**
 * Called when the user releases an element of the tileview after dragging it.
 * @param tileview pointer to a tileview object
 */
static void drag_end_handler(lv_obj_t * tileview)
{
    lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);
    lv_indev_t * indev      = lv_indev_get_act();
    lv_point_t point_act;
    lv_indev_get_point(indev, &point_act);
    lv_obj_t * scrl = lv_page_get_scrollable(tileview);
    lv_point_t p;

    p.x = -(lv_obj_get_x(scrl) - lv_obj_get_width(tileview) / 2);
    p.y = -(lv_obj_get_y(scrl) - lv_obj_get_height(tileview) / 2);

    lv_drag_dir_t drag_dir = indev->proc.types.pointer.drag_dir;
    /*From the drag vector (drag throw) predict the end position*/
    if(drag_dir & LV_DRAG_DIR_HOR) {
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        lv_coord_t predict = 0;

        while(vect.x != 0) {
            predict += vect.x;
            vect.x = vect.x * (100 - LV_INDEV_DEF_DRAG_THROW) / 100;
        }

        p.x -= predict;

    }
    else if(drag_dir & LV_DRAG_DIR_VER) {
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        lv_coord_t predict = 0;

        while(vect.y != 0) {
            predict += vect.y;
            vect.y = vect.y * (100 - LV_INDEV_DEF_DRAG_THROW) / 100;
        }

        p.y -= predict;
    }

    /*Get the index of the tile*/
    p.x = p.x / lv_obj_get_width(tileview);
    p.y = p.y / lv_obj_get_height(tileview);

    /*Max +- move*/
    lv_coord_t x_move = p.x - ext->act_id.x;
    lv_coord_t y_move = p.y - ext->act_id.y;
    if(x_move < -1) x_move = -1;
    if(x_move > 1) x_move = 1;
    if(y_move < -1) y_move = -1;
    if(y_move > 1) y_move = 1;

    /*Set the new tile*/
    lv_tileview_set_tile_act(tileview, ext->act_id.x + x_move, ext->act_id.y + y_move, true);
}

static bool set_valid_drag_dirs(lv_obj_t * tileview)
{

    lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);
    if(ext->valid_pos == NULL) return false;

    ext->drag_bottom_en = 0;
    ext->drag_top_en    = 0;
    ext->drag_left_en   = 0;
    ext->drag_right_en  = 0;

    uint16_t i;
    for(i = 0; i < ext->valid_pos_cnt; i++) {
        if(ext->valid_pos[i].x == ext->act_id.x && ext->valid_pos[i].y == ext->act_id.y - 1) ext->drag_top_en = 1;
        if(ext->valid_pos[i].x == ext->act_id.x && ext->valid_pos[i].y == ext->act_id.y + 1) ext->drag_bottom_en = 1;
        if(ext->valid_pos[i].x == ext->act_id.x - 1 && ext->valid_pos[i].y == ext->act_id.y) ext->drag_left_en = 1;
        if(ext->valid_pos[i].x == ext->act_id.x + 1 && ext->valid_pos[i].y == ext->act_id.y) ext->drag_right_en = 1;
    }

    return true;
}

#endif
