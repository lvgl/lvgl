/**
 * @file lv_scroll.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_scroll.h"
#include "lv_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_coord_t find_snap_point_x(const lv_obj_t * obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs);
static lv_coord_t find_snap_point_y(const lv_obj_t * obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs);
static void scroll_limit_diff(lv_indev_proc_t * proc, lv_coord_t * diff_x, lv_coord_t * diff_y);
static lv_coord_t scroll_throw_predict_y(lv_indev_proc_t * proc);
static lv_coord_t scroll_throw_predict_x(lv_indev_proc_t * proc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Handle scrolling. Called by LVGL during input device processing
 * @param proc pointer to an input device's proc field
 */
void _lv_scroll_handler(lv_indev_proc_t * proc)
{
    lv_obj_t * scroll_candidate_obj = NULL;
    lv_scroll_dir_t dirs_candidate = LV_SCROLL_DIR_NONE;

    lv_indev_t * indev_act = lv_indev_get_act();
    lv_coord_t scroll_limit =  indev_act->driver.scroll_limit;

    /*If there is no scroll object yet try to find one*/
    if(proc->types.pointer.scroll_obj == NULL) {
        proc->types.pointer.scroll_sum.x += proc->types.pointer.vect.x;
        proc->types.pointer.scroll_sum.y += proc->types.pointer.vect.y;

        proc->types.pointer.scroll_obj = proc->types.pointer.act_obj;

        /* Go until find an scrollable object in the current direction
         * More precisely:
         *  1. Check the pressed object and all of its ancestors and try to find an object which is scrollable
         *  2. Scrollable means it has some content out of it's area
         *  3. If an object can be scrolled into the current direction then use it ("real match"")
         *  4. If can be scrolled on the current axis (hor/ver) save it as candidate (at least show an elastic scroll effect)
         *  5. Use the last candidate. Always the "deepest" parent or the object from point 3 */
        while(proc->types.pointer.scroll_obj) {

            /*Decide if it's a horizontal or vertical scroll*/
            bool hor_en = false;
            bool ver_en = false;
            if(LV_MATH_ABS(proc->types.pointer.scroll_sum.x) > LV_MATH_ABS(proc->types.pointer.scroll_sum.y)) {
                hor_en = true;
            }
            else {
                ver_en = true;
            }

            /*Consider both up-down or left/right scrollable according to the current direction*/
            bool up_en = ver_en;
            bool down_en = ver_en;
            bool left_en = hor_en;
            bool right_en = hor_en;

            /*The object might have disabled some directions.*/
            lv_dir_t scroll_dir = proc->types.pointer.scroll_obj->scroll_dir;
            if((scroll_dir & LV_DIR_LEFT) == 0) left_en = false;
            if((scroll_dir & LV_DIR_RIGHT) == 0) right_en = false;
            if((scroll_dir & LV_DIR_TOP) == 0) up_en = false;
            if((scroll_dir & LV_DIR_BOTTOM) == 0) down_en = false;

            /*The object is scrollable to a direction if its content overflow in that direction. */
            lv_coord_t st = lv_obj_get_scroll_top(proc->types.pointer.scroll_obj);
            lv_coord_t sb = lv_obj_get_scroll_bottom(proc->types.pointer.scroll_obj);
            lv_coord_t sl = lv_obj_get_scroll_left(proc->types.pointer.scroll_obj);
            lv_coord_t sr = lv_obj_get_scroll_right(proc->types.pointer.scroll_obj);

            /* If this object is scrollable into the current scroll direction then save it as a candidate.
             * It's important only to be scrollable on the current axis (hor/ver) because if the scroll
             * is propagated to this object it can show at least elastic scroll effect.
             * But if not hor/ver scrollable do not scroll it at all (so it's not a good candidate) */
            if((st > 0 || sb > 0)  &&
              ((up_en    && proc->types.pointer.scroll_sum.y >= scroll_limit) ||
               (down_en  && proc->types.pointer.scroll_sum.y <= - scroll_limit)))
            {
                scroll_candidate_obj = proc->types.pointer.scroll_obj;
                dirs_candidate = LV_SCROLL_DIR_VER;
            }

            if((sl > 0 || sr > 0)  &&
              ((left_en    && proc->types.pointer.scroll_sum.x >=  scroll_limit) ||
               (right_en  && proc->types.pointer.scroll_sum.x <= - scroll_limit)))
            {
                scroll_candidate_obj = proc->types.pointer.scroll_obj;
                dirs_candidate = LV_SCROLL_DIR_HOR;
            }

            /**/
            if(st <= 0) up_en = false;
            if(sb <= 0) down_en = false;
            if(sl <= 0) left_en = false;
            if(sr <= 0) right_en = false;

            /*If the object really can be scrolled into the current direction the use it. */
            if((left_en  && proc->types.pointer.scroll_sum.x >=   scroll_limit) ||
               (right_en && proc->types.pointer.scroll_sum.x <= - scroll_limit) ||
               (up_en    && proc->types.pointer.scroll_sum.y >=   scroll_limit) ||
               (down_en  && proc->types.pointer.scroll_sum.y <= - scroll_limit))
            {
                proc->types.pointer.scroll_dir = hor_en ? LV_SCROLL_DIR_HOR : LV_SCROLL_DIR_VER;
                break;
            }

            /*Try the parent */
            proc->types.pointer.scroll_obj = lv_obj_get_parent(proc->types.pointer.scroll_obj);
        }

        /*Use the last candidate*/
        if(proc->types.pointer.scroll_obj == NULL) {
            if(scroll_candidate_obj) {
                proc->types.pointer.scroll_dir = dirs_candidate;
                proc->types.pointer.scroll_obj = scroll_candidate_obj;
            } else {
                return;
            }
        }

        proc->types.pointer.scroll_sum.x = 0;
        proc->types.pointer.scroll_sum.y = 0;

        lv_obj_t * obj = proc->types.pointer.scroll_obj;
        /*If there no STOP allow scrolling anywhere*/
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_STOP) == false) {
            lv_area_set(&proc->types.pointer.scroll_area, LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MAX, LV_COORD_MAX);
        }
        /*With STOP limit the scrolling to the perv and next snap point*/
        else {
            switch(obj->snap_align_y) {
            case LV_SCROLL_SNAP_ALIGN_START:
                proc->types.pointer.scroll_area.y1 = find_snap_point_y(obj, obj->coords.y1 + 1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.y2 = find_snap_point_y(obj, LV_COORD_MIN, obj->coords.y1 - 1, 0);
                break;
            case LV_SCROLL_SNAP_ALIGN_END:
                proc->types.pointer.scroll_area.y1 = find_snap_point_y(obj, obj->coords.y2, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.y2 = find_snap_point_y(obj, LV_COORD_MIN, obj->coords.y2, 0);
                break;
            case LV_SCROLL_SNAP_ALIGN_CENTER: {
                lv_coord_t y_mid = obj->coords.y1 + lv_area_get_height(&obj->coords) / 2;
                proc->types.pointer.scroll_area.y1 = find_snap_point_y(obj, y_mid + 1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.y2 = find_snap_point_y(obj, LV_COORD_MIN, y_mid - 1, 0);
                break;
            }
            default:
                proc->types.pointer.scroll_area.y1 = LV_COORD_MIN;
                proc->types.pointer.scroll_area.y2 = LV_COORD_MAX;
                break;
            }

            switch(obj->snap_align_x) {
            case LV_SCROLL_SNAP_ALIGN_START:
                proc->types.pointer.scroll_area.x1 = find_snap_point_x(obj, obj->coords.x1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.x2 = find_snap_point_x(obj, LV_COORD_MIN, obj->coords.x1, 0);
                break;
            case LV_SCROLL_SNAP_ALIGN_END:
                proc->types.pointer.scroll_area.x1 = find_snap_point_x(obj, obj->coords.x2, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.x2 = find_snap_point_x(obj, LV_COORD_MIN, obj->coords.x2, 0);
                break;
            case LV_SCROLL_SNAP_ALIGN_CENTER: {
                lv_coord_t x_mid = obj->coords.x1 + lv_area_get_width(&obj->coords) / 2;
                proc->types.pointer.scroll_area.x1 = find_snap_point_x(obj, x_mid + 1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.x2 = find_snap_point_x(obj, LV_COORD_MIN, x_mid - 1, 0);
                break;
            }
            default:
                proc->types.pointer.scroll_area.x1 = LV_COORD_MIN;
                proc->types.pointer.scroll_area.x2 = LV_COORD_MAX;
                break;
            }
        }
    }

    /*If there is no scroll object there is nothing to do*/
    lv_obj_t * scroll_obj = proc->types.pointer.scroll_obj;
    if(scroll_obj == NULL) return;

    /*Set new position or scroll if the vector is not zero*/
    if(proc->types.pointer.vect.x != 0 || proc->types.pointer.vect.y != 0) {
        lv_coord_t diff_x = 0;
        lv_coord_t diff_y = 0;

        if(proc->types.pointer.scroll_dir == LV_SCROLL_DIR_HOR) {
            diff_x = proc->types.pointer.vect.x;
            if(lv_obj_get_scroll_right(scroll_obj) < 0) diff_x = diff_x / 2;
            if(lv_obj_get_scroll_left(scroll_obj) < 0) diff_x = diff_x / 2;
        } else {
            diff_y = proc->types.pointer.vect.y;
            if(lv_obj_get_scroll_top(scroll_obj) < 0) diff_y = diff_y / 2;
            if(lv_obj_get_scroll_bottom(scroll_obj) < 0) diff_y = diff_y / 2;
        }


        if((scroll_obj->scroll_dir & LV_DIR_LEFT) == 0 && diff_x > 0) diff_x = 0;
        if((scroll_obj->scroll_dir & LV_DIR_RIGHT) == 0 && diff_x < 0) diff_x = 0;
        if((scroll_obj->scroll_dir & LV_DIR_TOP) == 0 && diff_y > 0) diff_y = 0;
        if((scroll_obj->scroll_dir & LV_DIR_BOTTOM) == 0 && diff_y < 0) diff_y = 0;

        /*Respect the scroll limit area*/
        scroll_limit_diff(proc, &diff_x, &diff_y);

        lv_obj_scroll_by_raw(scroll_obj, diff_x, diff_y);
        proc->types.pointer.scroll_sum.x += diff_x;
        proc->types.pointer.scroll_sum.y += diff_y;
    }
}


/**
 * Handle throwing after scrolling. Called by LVGL during input device processing
 * @param proc pointer to an input device's proc field
 */
void _lv_scroll_throw_handler(lv_indev_proc_t * proc)
{
    lv_obj_t * scroll_obj = proc->types.pointer.scroll_obj;
    if(scroll_obj == NULL) return;

    lv_indev_t * indev_act = lv_indev_get_act();
    lv_coord_t scroll_throw =  indev_act->driver.scroll_throw;

    if(proc->types.pointer.scroll_dir == LV_SCROLL_DIR_NONE) {
        return;
    }
    else if(proc->types.pointer.scroll_dir == LV_SCROLL_DIR_VER) {
        proc->types.pointer.scroll_throw_vect.x = 0;
        /*If no snapping "throw"*/
        if(scroll_obj->snap_align_y == LV_SCROLL_SNAP_ALIGN_NONE) {
            proc->types.pointer.scroll_throw_vect.y =
                    proc->types.pointer.scroll_throw_vect.y * (100 - scroll_throw) / 100;

            lv_coord_t st = lv_obj_get_scroll_top(scroll_obj);
            lv_coord_t sb = lv_obj_get_scroll_bottom(scroll_obj);

            /*If scrolled inside reduce faster*/
            if(st < 0 || sb < 0) {
                proc->types.pointer.scroll_throw_vect.y = proc->types.pointer.scroll_throw_vect.y >> 1;
            }

            lv_obj_scroll_by_raw(scroll_obj, 0, proc->types.pointer.scroll_throw_vect.y);
        }
        /*With snapping find the nearest snap point and scroll there*/
        else {
            lv_coord_t diff_y = scroll_throw_predict_y(proc);
            proc->types.pointer.scroll_throw_vect.y = 0;
            scroll_limit_diff(proc, NULL, &diff_y);
            lv_coord_t y = find_snap_point_y(scroll_obj, LV_COORD_MIN, LV_COORD_MAX, diff_y);
            lv_obj_scroll_by(scroll_obj, 0, diff_y + y, LV_ANIM_ON);
        }
    }
    else if(proc->types.pointer.scroll_dir == LV_SCROLL_DIR_HOR) {
        proc->types.pointer.scroll_throw_vect.y = 0;
        /*If no snapping "throw"*/
        if(scroll_obj->snap_align_x == LV_SCROLL_SNAP_ALIGN_NONE) {
            proc->types.pointer.scroll_throw_vect.x =
                    proc->types.pointer.scroll_throw_vect.x * (100 - scroll_throw) / 100;

            lv_coord_t sl = lv_obj_get_scroll_left(scroll_obj);
            lv_coord_t sr = lv_obj_get_scroll_right(scroll_obj);

            /*If scrolled inside reduce faster*/
            if(sl < 0 || sr < 0) {
                proc->types.pointer.scroll_throw_vect.x = proc->types.pointer.scroll_throw_vect.x >> 1;
            }

            lv_obj_scroll_by_raw(scroll_obj, proc->types.pointer.scroll_throw_vect.x, 0);
        }
        /*With snapping find the nearest snap point and scroll there*/
        else {
            lv_coord_t diff_x = scroll_throw_predict_x(proc);
            proc->types.pointer.scroll_throw_vect.x = 0;
            scroll_limit_diff(proc, NULL, &diff_x);
            lv_coord_t x = find_snap_point_x(scroll_obj, LV_COORD_MIN, LV_COORD_MAX, diff_x);
            lv_obj_scroll_by(scroll_obj, x + diff_x, 0, LV_ANIM_ON);
        }
    }


    /*Check if the scroll has finished */
    if(proc->types.pointer.scroll_throw_vect.x == 0 && proc->types.pointer.scroll_throw_vect.y == 0) {
        /*Revert if scrolled in*/
        /*If vertically scrollable and not controlled by snap*/
        if(scroll_obj->snap_align_y == LV_SCROLL_SNAP_ALIGN_NONE) {
            lv_coord_t st = lv_obj_get_scroll_top(scroll_obj);
            lv_coord_t sb = lv_obj_get_scroll_bottom(scroll_obj);
            if(st > 0 || sb > 0) {
                if(st < 0) {
                    lv_obj_scroll_by(scroll_obj, 0, st, LV_ANIM_ON);
                }
                else if(sb < 0) {
                    lv_obj_scroll_by(scroll_obj, 0, -sb, LV_ANIM_ON);
                }
            }
        }

        /*If horizontally scrollable and not controlled by snap*/
        if(scroll_obj->snap_align_x == LV_SCROLL_SNAP_ALIGN_NONE) {
            lv_coord_t sl = lv_obj_get_scroll_left(scroll_obj);
            lv_coord_t sr = lv_obj_get_scroll_right(scroll_obj);
            if (sl > 0 || sr > 0) {
                if(sl < 0) {
                    lv_obj_scroll_by(scroll_obj, sl, 0, LV_ANIM_ON);
                }
                else if(sr < 0) {
                    lv_obj_scroll_by(scroll_obj, -sr, 0, LV_ANIM_ON);
                }
            }
        }

        proc->types.pointer.scroll_dir = LV_SCROLL_DIR_NONE;
        proc->types.pointer.scroll_obj = NULL;
        lv_signal_send(scroll_obj, LV_SIGNAL_SCROLL_END, indev_act);
        if(proc->reset_query) return;
        lv_event_send(scroll_obj, LV_EVENT_SCROLL_END, indev_act);
        if(proc->reset_query) return;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_coord_t find_snap_point_x(const lv_obj_t * obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs)
{
    if(obj->snap_align_x == LV_SCROLL_SNAP_ALIGN_NONE) return 0;

    lv_coord_t dist = LV_COORD_MAX;

    lv_obj_t * child = lv_obj_get_child_back(obj, NULL);
    while(child) {
        lv_coord_t x_child = 0;
        lv_coord_t x_parent = 0;
        switch(obj->snap_align_x) {
        case LV_SCROLL_SNAP_ALIGN_START:
            x_child = child->coords.x1;
            x_parent = obj->coords.x1;
            break;
        case LV_SCROLL_SNAP_ALIGN_END:
            x_child = child->coords.x2;
            x_parent = obj->coords.x2;
            break;
        case LV_SCROLL_SNAP_ALIGN_CENTER:
            x_child = child->coords.x1 + lv_area_get_width(&child->coords) / 2;
            x_parent = obj->coords.x1 + lv_area_get_width(&obj->coords) / 2;
        }

        x_child += ofs;
        if(x_child >= min && x_child <= max) {
            lv_coord_t x = x_child -  x_parent;
            if(LV_MATH_ABS(x) < LV_MATH_ABS(dist)) dist = x;
        }

        child = lv_obj_get_child_back(obj, child);
    }

    return dist == LV_COORD_MAX ? 0: -dist;
}

/**
 * Search for snap point in the `min` - `max` range.
 * @param obj the object on which snap point should be found
 * @param min ignore snap points smaller then this. (Absolute coordinate)
 * @param max ignore snap points greater then this. (Absolute coordinate)
 * @param ofs offset to snap points. Useful the get a snap point in an imagined case
 *            what if children are already moved by this value
 * @return the distance of the snap point.
 */
static lv_coord_t find_snap_point_y(const lv_obj_t * obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs)
{
    if(obj->snap_align_y == LV_SCROLL_SNAP_ALIGN_NONE) return 0;

    lv_coord_t dist = LV_COORD_MAX;

    uint32_t cc = 0;
    lv_obj_t * child = lv_obj_get_child_back(obj, NULL);
    while(child) {
        cc++;
        lv_coord_t y_child = 0;
        lv_coord_t y_parent = 0;
        switch(obj->snap_align_y) {
        case LV_SCROLL_SNAP_ALIGN_START:
            y_child = child->coords.y1;
            y_parent = obj->coords.y1;
            break;
        case LV_SCROLL_SNAP_ALIGN_END:
            y_child = child->coords.y2;
            y_parent = obj->coords.y2;
            break;
        case LV_SCROLL_SNAP_ALIGN_CENTER:
            y_child = child->coords.y1 + lv_area_get_height(&child->coords) / 2;
            y_parent = obj->coords.y1 + lv_area_get_height(&obj->coords) / 2;
        }

        y_child += ofs;
        if(y_child >= min && y_child <= max) {
            lv_coord_t y = y_child -  y_parent;
            if(LV_MATH_ABS(y) < LV_MATH_ABS(dist)) dist = y;
        }

        child = lv_obj_get_child_back(obj, child);
    }

    return dist == LV_COORD_MAX ? 0 : -dist;
}

static void scroll_limit_diff(lv_indev_proc_t * proc, lv_coord_t * diff_x, lv_coord_t * diff_y)
{
    if(diff_y) {
        if(proc->types.pointer.scroll_sum.y + *diff_y < proc->types.pointer.scroll_area.y1) {
           *diff_y = proc->types.pointer.scroll_area.y1 - proc->types.pointer.scroll_sum.y;
        }

        if(proc->types.pointer.scroll_sum.y + *diff_y > proc->types.pointer.scroll_area.y2) {
            *diff_y = proc->types.pointer.scroll_area.y2 - proc->types.pointer.scroll_sum.y;
        }
    }

    if(diff_x) {
        if(proc->types.pointer.scroll_sum.x + *diff_x < proc->types.pointer.scroll_area.x1) {
            *diff_x = proc->types.pointer.scroll_area.x1 - proc->types.pointer.scroll_sum.x;
        }

        if(proc->types.pointer.scroll_sum.x + *diff_x > proc->types.pointer.scroll_area.x2) {
            *diff_x = proc->types.pointer.scroll_area.x2 - proc->types.pointer.scroll_sum.x;
        }
    }
}



static lv_coord_t scroll_throw_predict_y(lv_indev_proc_t * proc)
{
    lv_coord_t y =  proc->types.pointer.scroll_throw_vect.y;
    lv_coord_t move = 0;

    lv_indev_t * indev_act = lv_indev_get_act();
    lv_coord_t scroll_throw =  indev_act->driver.scroll_throw;

    while(y) {
        move += y;
        y = y * (100 - scroll_throw) / 100;
    }
    return move;
}


static lv_coord_t scroll_throw_predict_x(lv_indev_proc_t * proc)
{
    lv_coord_t x =  proc->types.pointer.scroll_throw_vect.x;
    lv_coord_t move = 0;

    lv_indev_t * indev_act = lv_indev_get_act();
    lv_coord_t scroll_throw =  indev_act->driver.scroll_throw;

    while(x) {
        move += x;
        x = x * (100 - scroll_throw) / 100;
    }
    return move;
}

