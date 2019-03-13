/**
 * @file lv_tileview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tileview.h"
#if USE_LV_TILEVIEW != 0

#include <stdbool.h>
#include "lv_cont.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#if USE_LV_ANIMATION
#  ifndef LV_TILEVIEW_ANIM_TIME
#    define LV_TILEVIEW_ANIM_TIME  300 /*Animation time loading a tile [ms] (0: no animation)  */
#  endif
#else
#  undef  LV_TILEVIEW_ANIM_TIME
#  define LV_TILEVIEW_ANIM_TIME  0   /*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_tileview_signal(lv_obj_t * tileview, lv_signal_t sign, void * param);
static lv_res_t lv_tileview_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
static lv_res_t element_signal_func(lv_obj_t * element, lv_signal_t sign, void * param);
static void drag_end_handler(lv_obj_t * tileview);
static bool set_valid_drag_dirs(lv_obj_t * tileview);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_signal_func_t ancestor_scrl_signal;
static lv_design_func_t ancestor_design;

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
    lv_mem_assert(new_tileview);
    if(new_tileview == NULL) return NULL;

    /*Allocate the tileview type specific extended data*/
    lv_tileview_ext_t * ext = lv_obj_allocate_ext_attr(new_tileview, sizeof(lv_tileview_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_tileview);
    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = lv_obj_get_signal_func(lv_page_get_scrl(new_tileview));
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_tileview);

    /*Initialize the allocated 'ext' */
    ext->anim_time = LV_TILEVIEW_ANIM_TIME;
    ext->action = NULL;
    ext->act_id.x = 0;
    ext->act_id.y = 0;
    ext->valid_pos = NULL;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_tileview, lv_tileview_signal);
    lv_obj_set_signal_func(lv_page_get_scrl(new_tileview), lv_tileview_scrl_signal);

    /*Init the new tileview*/
    if(copy == NULL) {
        lv_obj_set_size(new_tileview, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_drag_throw(lv_page_get_scrl(new_tileview), false);
        lv_page_set_scrl_fit(new_tileview, true, true);
        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_page_set_style(new_tileview, LV_PAGE_STYLE_BG, th->tileview.bg);
            lv_page_set_style(new_tileview, LV_PAGE_STYLE_SCRL, th->tileview.scrl);
            lv_page_set_style(new_tileview, LV_PAGE_STYLE_SB, th->tileview.sb);
        } else {
            lv_page_set_style(new_tileview, LV_PAGE_STYLE_BG, &lv_style_transp_tight);
            lv_page_set_style(new_tileview, LV_PAGE_STYLE_SCRL, &lv_style_transp_tight);
        }
    }
    /*Copy an existing tileview*/
    else {
        lv_tileview_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->act_id.x = copy_ext->act_id.x;
        ext->act_id.y = copy_ext->act_id.y;
        ext->action = copy_ext->action;
        ext->anim_time = copy_ext->anim_time;


        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_tileview);
    }

    LV_LOG_INFO("tileview created");

    return new_tileview;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Register an object on the tileview. The register object will able to slide the tileview
 * @param element pointer to an object
 */
void lv_tileview_add_element(lv_obj_t * element)
{
    lv_obj_set_free_ptr(element, lv_obj_get_signal_func(element));
    lv_obj_set_signal_func(element, element_signal_func);
    lv_obj_set_drag_parent(element, true);
}


/*=====================
 * Setter functions
 *====================*/

/**
 * Set the valid position's indices. The scrolling will be possible only to these positions.
 * @param tileview pointer to a Tileview object
 * @param valid_pos array width the indices. E.g. `lv_point_t p[] = {{0,0}, {1,0}, {1,1}, {LV_COORD_MIN, LV_COORD_MIN}};`
 * Must be closed with `{LV_COORD_MIN, LV_COORD_MIN}`. Only the pointer is saved so can't be a local variable.
 */
void lv_tileview_set_valid_positions(lv_obj_t * tileview, const lv_point_t * valid_pos)
{
    lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);
    ext->valid_pos = valid_pos;
}

/**
 * Set the tile to be shown
 * @param tileview pointer to a tileview object
 * @param x column id (0, 1, 2...)
 * @param y line id (0, 1, 2...)
 * @param anim_en true: move with animation
 */
void lv_tileview_set_tile_act(lv_obj_t * tileview, lv_coord_t x, lv_coord_t y, bool anim_en)
{
#if USE_LV_ANIMATION == 0
    anim_en = false;
#endif

    lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);


    uint16_t i;
    bool valid = false;
    for(i = 0; ext->valid_pos[i].x != LV_COORD_MIN; i++) {
        if(ext->valid_pos[i].x == x && ext->valid_pos[i].y == y) {
            valid = true;
        }
    }

    if(valid == false) return;  /*Don't load not valid tiles*/

    lv_res_t res = LV_RES_OK;
    if(ext->action) res = ext->action(tileview, x, y);
    if(res != LV_RES_OK) return;        /*Prevent the tile loading*/

    ext->act_id.x = x;
    ext->act_id.y = y;

    lv_coord_t x_coord = -x * lv_obj_get_width(tileview);
    lv_coord_t y_coord = -y * lv_obj_get_height(tileview);
    lv_obj_t * scrl = lv_page_get_scrl(tileview);
    if(anim_en) {
#if USE_LV_ANIMATION
        lv_coord_t x_act = lv_obj_get_x(scrl);
        lv_coord_t y_act = lv_obj_get_y(scrl);

        lv_anim_t a;
        a.var = scrl;
        a.fp = (lv_anim_fp_t)lv_obj_set_x;
        a.path = lv_anim_path_linear;
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = ext->anim_time;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;

        if(x_coord != x_act) {
            a.start = x_act;
            a.end = x_coord;
            lv_anim_create(&a);
        }

        if(y_coord != y_act) {
            a.start = y_act;
            a.end = y_coord;
            a.fp = (lv_anim_fp_t)lv_obj_set_y;
            lv_anim_create(&a);
        }
#endif
    } else {
        lv_obj_set_pos(scrl, x_coord, y_coord);
    }
}

void lv_tileview_set_tile_load_action(lv_obj_t * tileview, lv_tileview_action_t action)
{
    lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);
    ext->action = action;

}

/**
 * Set a style of a tileview.
 * @param tileview pointer to tileview object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_tileview_set_style(lv_obj_t * tileview, lv_tileview_style_t type, lv_style_t * style)
{

    switch(type) {
        case LV_TILEVIEW_STYLE_BG:
            lv_obj_set_style(tileview, style);
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */

/**
 * Get style of a tileview.
 * @param tileview pointer to tileview object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_tileview_get_style(const lv_obj_t * tileview, lv_tileview_style_t type)
{
    lv_style_t * style = NULL;
    switch(type) {
        case LV_TILEVIEW_STYLE_BG:
            style = lv_obj_get_style(tileview);
            break;
        default:
            style =  NULL;
    }

    return style;
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


    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_tileview";
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

    lv_obj_t * tileview = lv_obj_get_parent(scrl);
    lv_style_t * style_bg = lv_tileview_get_style(tileview, LV_TILEVIEW_STYLE_BG);


    /*Apply constraint on moving of the tileview*/
    if(sign == LV_SIGNAL_CORD_CHG) {
        lv_indev_t * indev = lv_indev_get_act();
        if(indev) {
            lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);

            /*Set horizontal drag constraint if no vertical constraint an dragged to valid x direction */
            if(ext->drag_ver == 0 &&
                    ((ext->drag_right_en && indev->proc.drag_sum.x <= -LV_INDEV_DRAG_LIMIT) ||
                     (ext->drag_left_en  && indev->proc.drag_sum.x >= LV_INDEV_DRAG_LIMIT))) {
                ext->drag_hor = 1;
            }
            /*Set vertical drag constraint if no horizontal constraint an dragged to valid y direction */
            if(ext->drag_hor == 0 &&
                    ((ext->drag_bottom_en && indev->proc.drag_sum.y <= -LV_INDEV_DRAG_LIMIT) ||
                     (ext->drag_top_en && indev->proc.drag_sum.y >= LV_INDEV_DRAG_LIMIT))) {
                ext->drag_ver = 1;
            }

            if(ext->drag_hor) {
                ext->page.edge_flash.top_ip = 0;
                ext->page.edge_flash.bottom_ip = 0;
            }

            if(ext->drag_ver) {
                ext->page.edge_flash.right_ip = 0;
                ext->page.edge_flash.left_ip = 0;
            }

            lv_coord_t x = lv_obj_get_x(scrl);
            lv_coord_t y = lv_obj_get_y(scrl);
            lv_coord_t h = lv_obj_get_height(tileview);
            lv_coord_t w = lv_obj_get_width(tileview);
            if(ext->drag_top_en == 0) {
                if(y > -(ext->act_id.y * h) && indev->proc.vect.y > 0 && ext->drag_hor == 0) {
                    if(ext->page.edge_flash.enabled &&
                            ext->page.edge_flash.left_ip == 0 && ext->page.edge_flash.right_ip == 0 &&
                            ext->page.edge_flash.top_ip == 0 && ext->page.edge_flash.bottom_ip == 0) {
                        ext->page.edge_flash.top_ip = 1;
                        lv_page_start_edge_flash(tileview);
                    }

                    lv_obj_set_y(scrl, -ext->act_id.y * h + style_bg->body.padding.ver);
                }
            }
            if(ext->drag_bottom_en == 0 && indev->proc.vect.y < 0 && ext->drag_hor == 0) {
                if(y < -(ext->act_id.y * h)) {
                    if(ext->page.edge_flash.enabled &&
                            ext->page.edge_flash.left_ip == 0 && ext->page.edge_flash.right_ip == 0 &&
                            ext->page.edge_flash.top_ip == 0 && ext->page.edge_flash.bottom_ip == 0) {
                        ext->page.edge_flash.bottom_ip = 1;
                        lv_page_start_edge_flash(tileview);
                    }
                }

                lv_obj_set_y(scrl, -ext->act_id.y * h + style_bg->body.padding.ver);
            }
            if(ext->drag_left_en == 0) {
                if(x > -(ext->act_id.x * w) && indev->proc.vect.x > 0 && ext->drag_ver == 0) {
                    if(ext->page.edge_flash.enabled &&
                            ext->page.edge_flash.left_ip == 0 && ext->page.edge_flash.right_ip == 0 &&
                            ext->page.edge_flash.top_ip == 0 && ext->page.edge_flash.bottom_ip == 0) {
                        ext->page.edge_flash.left_ip = 1;
                        lv_page_start_edge_flash(tileview);
                    }

                    lv_obj_set_x(scrl, -ext->act_id.x * w + style_bg->body.padding.hor);
                }
            }
            if(ext->drag_right_en == 0 && indev->proc.vect.x < 0 && ext->drag_ver == 0) {
                if(x < -(ext->act_id.x * w)) {
                    if(ext->page.edge_flash.enabled &&
                            ext->page.edge_flash.left_ip == 0 && ext->page.edge_flash.right_ip == 0 &&
                            ext->page.edge_flash.top_ip == 0 && ext->page.edge_flash.bottom_ip == 0) {
                        ext->page.edge_flash.right_ip = 1;
                        lv_page_start_edge_flash(tileview);
                    }
                }

                lv_obj_set_x(scrl, -ext->act_id.x * w + style_bg->body.padding.hor);
            }

            /*Apply the drag constraints*/
            if(ext->drag_ver == 0) lv_obj_set_y(scrl, - ext->act_id.y * lv_obj_get_height(tileview) + style_bg->body.padding.ver);
            if(ext->drag_hor == 0) lv_obj_set_x(scrl, - ext->act_id.x * lv_obj_get_width(tileview) + style_bg->body.padding.hor);
        }
    }

    return res;

}

/**
 * This function is applied called for the elements of the tileview. Used when the element is
 * @param element
 * @param sign
 * @param param
 * @return
 */
static lv_res_t element_signal_func(lv_obj_t * element, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    lv_signal_func_t sign_func = lv_obj_get_free_ptr(element);
    res = sign_func(element, sign, param);
    if(res != LV_RES_OK) return res;

    /*Initialize some variables on PRESS*/
    if(sign == LV_SIGNAL_PRESSED) {
        /*Get the tileview from the element*/
        lv_obj_t * tileview = lv_obj_get_parent(element);
        while(tileview) {
            if(lv_obj_get_signal_func(tileview) != lv_tileview_signal) tileview = lv_obj_get_parent(tileview);
            else break;
        }

        if(tileview) {
            lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);
            ext->drag_hor = 0;
            ext->drag_ver = 0;
            set_valid_drag_dirs(tileview);
        }
    }

    /*Animate the tabview to the correct location on RELEASE*/
    else if(sign == LV_SIGNAL_PRESS_LOST || sign == LV_SIGNAL_RELEASED) {

        /*Get the tileview from the element*/
        lv_obj_t * tileview = lv_obj_get_parent(element);
        while(tileview) {
            if(lv_obj_get_signal_func(tileview) != lv_tileview_signal) tileview = lv_obj_get_parent(tileview);
            else break;
        }

        if(tileview) {
            /* If the element was dragged and it moved the tileview finish the drag manually to
             * let the tileview to finish the move.*/
            lv_indev_t * indev = lv_indev_get_act();
            lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);
            if(indev->proc.drag_in_prog && (ext->drag_hor || ext->drag_ver)) {

                lv_obj_t * drag_obj = element;
                while(lv_obj_get_drag_parent(drag_obj)) {
                    drag_obj = lv_obj_get_parent(drag_obj);
                    if(drag_obj == NULL) break;
                }
                indev->proc.drag_in_prog = 0;
                if(drag_obj) drag_obj->signal_func(drag_obj, LV_SIGNAL_DRAG_END, NULL);
            }

             drag_end_handler(tileview);
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
    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t point_act;
    lv_indev_get_point(indev, &point_act);
    lv_obj_t * scrl = lv_page_get_scrl(tileview);
    lv_point_t p;

    p.x = - (scrl->coords.x1 - LV_HOR_RES / 2);
    p.y = - (scrl->coords.y1 - LV_VER_RES / 2);

    /*From the drag vector (drag throw) predict the end position*/
    if(ext->drag_hor) {
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        lv_coord_t predict = 0;

        while(vect.x != 0) {
            predict += vect.x;
            vect.x = vect.x * (100 - LV_INDEV_DRAG_THROW) / 100;
        }

        p.x -= predict;
    }
    else if(ext->drag_ver) {
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        lv_coord_t predict = 0;

        while(vect.y != 0) {
            predict += vect.y;
            vect.y = vect.y * (100 - LV_INDEV_DRAG_THROW) / 100;
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
    if(x_move > 1)  x_move =  1;
    if(y_move < -1) y_move = -1;
    if(y_move > 1) y_move = 1;

    /*Set the new tile*/
    lv_tileview_set_tile_act(tileview, ext->act_id.x + x_move, ext->act_id.y + y_move,true);
}

static bool set_valid_drag_dirs(lv_obj_t * tileview)
{

    lv_tileview_ext_t * ext = lv_obj_get_ext_attr(tileview);
    if(ext->valid_pos == NULL) return false;

    ext->drag_bottom_en = 0;
    ext->drag_top_en = 0;
    ext->drag_left_en = 0;
    ext->drag_right_en = 0;

    uint16_t i;
    for(i = 0; ext->valid_pos[i].x != LV_COORD_MIN; i++) {
        if(ext->valid_pos[i].x == ext->act_id.x && ext->valid_pos[i].y == ext->act_id.y - 1) ext->drag_top_en = 1;
        if(ext->valid_pos[i].x == ext->act_id.x && ext->valid_pos[i].y == ext->act_id.y + 1) ext->drag_bottom_en = 1;
        if(ext->valid_pos[i].x == ext->act_id.x - 1 && ext->valid_pos[i].y == ext->act_id.y) ext->drag_left_en = 1;
        if(ext->valid_pos[i].x == ext->act_id.x + 1 && ext->valid_pos[i].y == ext->act_id.y) ext->drag_right_en = 1;
    }

    return true;
}


#endif
