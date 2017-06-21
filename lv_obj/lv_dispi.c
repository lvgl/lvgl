/**
 * @file lv_dispi.c
 * 
 */

/*********************
 *      INCLUDES
 ********************/
#include "lv_conf.h"

#include "misc/os/ptask.h"
#include "misc/math/math_base.h"
#include "lv_dispi.h"
#include "../lv_draw/lv_draw_rbasic.h"
#include "hal/indev/indev.h"
#include "hal/systick/systick.h"
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void dispi_task(void * param);
static void dispi_proc_point(lv_dispi_t * dispi_p, cord_t x, cord_t y);
static void dispi_proc_press(lv_dispi_t * dispi_p);
static void disi_proc_release(lv_dispi_t * dispi_p);
static lv_obj_t * dispi_search_obj(const lv_dispi_t * dispi_p, lv_obj_t * obj);
static void dispi_drag(lv_dispi_t * dispi_p);
static void dispi_drag_throw(lv_dispi_t * dispi_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static ptask_t* dispi_task_p;
static bool lv_dispi_reset_qry;
static bool lv_dispi_reset_now;
static lv_dispi_t dispi_array[INDEV_NUM];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the display input subsystem
 */
void lv_dispi_init(void)
{
    lv_dispi_reset_qry = false;
    lv_dispi_reset_now = false;

#if LV_DISPI_READ_PERIOD != 0
    dispi_task_p = ptask_create(dispi_task, LV_DISPI_READ_PERIOD, PTASK_PRIO_MID, NULL);
#else
    dispi_task_p = ptask_create(dispi_task, 1, PTASK_PRIO_OFF); /*Not use lv_dispi*/
#endif
}

/**
 * Get an array with all the display inputs. Contains (INDEV_NUM elements)
 * @return pointer to a an lv_dispi_t array.
 */
lv_dispi_t * lv_dispi_get_array(void)
{
    return dispi_array;
}

/**
 * Reset all display inputs
 */
void lv_dispi_reset(void)
{
    lv_dispi_reset_qry = true;
}

/**
 * Reset the long press state of a display input
 * @param dispi pointer to a display input
 */
void lv_dispi_reset_lpr(lv_dispi_t * dispi)
{
    dispi->long_press_sent = 0;
    dispi->lpr_rep_time_stamp = systick_get();
    dispi->press_time_stamp = systick_get();
}

/**
 * Get the last point on display input
 * @param dispi pointer to a display input
 * @param point pointer to a point to store the result
 */
void lv_dispi_get_point(lv_dispi_t * dispi, point_t * point)
{
    point->x = dispi->act_point.x;
    point->y = dispi->act_point.y;
}

/**
 * Check if there is dragging on display input or not 
 * @param dispi pointer to a display input
 * @return true: drag is in progress
 */
bool lv_dispi_is_dragging(lv_dispi_t * dispi)
{
    return dispi->drag_in_prog == 0 ? false : true;
}

/**
 * Get the vector of dragging on a display input
 * @param dispi pointer to a display input
 * @param point pointer to a point to store the vector
 */
void lv_dispi_get_vect(lv_dispi_t * dispi, point_t * point)
{
    point->x = dispi->vect.x;
    point->y = dispi->vect.y;
}

/**
 * Do nothing until the next release
 * @param dispi pointer to a display input
 */
void lv_dispi_wait_release(lv_dispi_t * dispi)
{
    dispi->wait_release = 1;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called periodically to handle the display inputs
 * @param param unused
 */
static void dispi_task(void * param)
{
	cord_t x;
	cord_t y;
	uint8_t i;

	for (i = 0; i < INDEV_NUM; i++) {
		dispi_array[i].pressed = indev_get(i, &x, &y);
		dispi_proc_point(&dispi_array[i], x, y);
	}

    /*If reset query occurred in this round then set a flag to 
     * ask the dispis to reset themself in the next round  */
    if(lv_dispi_reset_qry != false) {
        lv_dispi_reset_qry = false;
        lv_dispi_reset_now = true;
    } 
    /*If now a reset occurred then clear the flag*/
    else if (lv_dispi_reset_now != false){
        lv_dispi_reset_now = false;
    }
}

/**
 * Process new points by a display input. dispi_p->pressed has to be set
 * @param dispi_p pointer to a display input
 * @param x x coordinate of the next point
 * @param y y coordinate of the next point
 */
static void dispi_proc_point(lv_dispi_t * dispi_p, cord_t x, cord_t y)
{
#if LV_ANTIALIAS != 0 && LV_VDB_SIZE != 0
    dispi_p->act_point.x = x * LV_DOWNSCALE;
    dispi_p->act_point.y = y * LV_DOWNSCALE;
#else
    dispi_p->act_point.x = x;
    dispi_p->act_point.y = y;
#endif
    /*Handle the reset query*/
    if(lv_dispi_reset_now != false) {
        dispi_p->act_obj = NULL;
        dispi_p->last_obj = NULL;
        dispi_p->drag_range_out = 0;
        dispi_p->drag_in_prog = 0;
        dispi_p->long_press_sent = 0;
        dispi_p->press_time_stamp = 0;
        dispi_p->lpr_rep_time_stamp = 0;
        dispi_p->vect_sum.x = 0;
        dispi_p->vect_sum.y = 0;
    }
    
    if(dispi_p->pressed != false){
#if LV_DISPI_TP_MARKER != 0
        area_t area;
        area.x1 = x - (LV_DISPI_TP_MARKER >> 1);
        area.y1 = y - (LV_DISPI_TP_MARKER >> 1);
        area.x2 = x + ((LV_DISPI_TP_MARKER >> 1) | 0x1);
        area.y2 = y + ((LV_DISPI_TP_MARKER >> 1) | 0x1);
        lv_rfill(&area, NULL, COLOR_MAKE(0xFF, 0, 0), OPA_COVER);
#endif
        dispi_proc_press(dispi_p);
    } else {
        disi_proc_release(dispi_p);
    }
    
    
    dispi_p->last_point.x = dispi_p->act_point.x;
    dispi_p->last_point.y = dispi_p->act_point.y;
}   

/**
 * Process the pressed state
 * @param dispi_p pointer to a display input
 */
static void dispi_proc_press(lv_dispi_t * dispi_p)
{
    lv_obj_t * pr_obj = dispi_p->act_obj;
    
    if(dispi_p->wait_release != 0) return;

    /*If there is no last object then search*/
    if(dispi_p->act_obj == NULL) {
        pr_obj = dispi_search_obj(dispi_p, lv_scr_act());
    }
    /*If there is last object but it is not dragged also search*/
    else if(dispi_p->drag_in_prog == 0) {/*Now act_obj != NULL*/
        pr_obj = dispi_search_obj(dispi_p, lv_scr_act());
    }
    /*If a dragable object was the last then keep it*/
    else {
        
    }
    
    /*If a new object was found reset some variables and send a pressed signal*/
    if(pr_obj != dispi_p->act_obj) {

        dispi_p->last_point.x = dispi_p->act_point.x;
        dispi_p->last_point.y = dispi_p->act_point.y;
        
        /*If a new object found the previous was lost, so send a signal*/
        if(dispi_p->act_obj != NULL) {
            dispi_p->act_obj->signal_f(dispi_p->act_obj,
                                          LV_SIGNAL_PRESS_LOST, dispi_p);
        }
        
        if(pr_obj != NULL) {
            /* Save the time when the obj pressed. 
             * It is necessary to count the long press time.*/
            dispi_p->press_time_stamp = systick_get();
            dispi_p->long_press_sent = 0;
            dispi_p->drag_range_out = 0;
            dispi_p->drag_in_prog = 0;
            dispi_p->vect_sum.x = 0;
            dispi_p->vect_sum.y = 0;

            /*Search for 'top' attribute*/
            lv_obj_t * i = pr_obj;
            lv_obj_t * last_top = NULL;
            while(i != NULL){
				if(i->top_en != 0) last_top = i;
				i = lv_obj_get_parent(i);
            }

            if(last_top != NULL) {
            	/*Move the last_top object to the foreground*/
            	lv_obj_t * par =lv_obj_get_parent(last_top);
            	/*After list change it will be the new head*/
                ll_chg_list(&par->child_ll, &par->child_ll, last_top);
                lv_obj_inv(last_top);
            }

            /*Send a signal about the press*/
            pr_obj->signal_f(pr_obj, LV_SIGNAL_PRESSED, dispi_p);
        }
    }
    
    /* The reset can be set in the signal function.
     * In case of reset query ignore the remaining parts.*/
    if(lv_dispi_reset_qry == false) {
        dispi_p->act_obj = pr_obj;      /*Save the pressed object*/
        dispi_p->last_obj = dispi_p->act_obj; /*Refresh the last_obj*/

        /*Calculate the vector*/
        dispi_p->vect.x = dispi_p->act_point.x - dispi_p->last_point.x;
        dispi_p->vect.y = dispi_p->act_point.y - dispi_p->last_point.y;

        /*If there is active object and it can be dragged run the drag*/
        if(dispi_p->act_obj != NULL) {
            dispi_p->act_obj->signal_f(dispi_p->act_obj, LV_SIGNAL_PRESSING, dispi_p);

            dispi_drag(dispi_p);
        
            /*If there is no drag then check for long press time*/
            if(dispi_p->drag_in_prog == 0 && dispi_p->long_press_sent == 0) {
                /*Send a signal about the long press if enough time elapsed*/
                if(systick_elaps(dispi_p->press_time_stamp) > LV_DISPI_LONG_PRESS_TIME) {
                    pr_obj->signal_f(pr_obj, LV_SIGNAL_LONG_PRESS, dispi_p);

                    /*Mark the signal sending to do not send it again*/
                    dispi_p->long_press_sent = 1;

                    /*Save the long press time stamp for the long press repeat handler*/
                    dispi_p->lpr_rep_time_stamp = systick_get();
                }
            }
            /*Send long press repeated signal*/
            if(dispi_p->drag_in_prog == 0 && dispi_p->long_press_sent == 1) {
            	/*Send a signal about the long press repeate if enough time elapsed*/
				if(systick_elaps(dispi_p->lpr_rep_time_stamp) > LV_DISPI_LONG_PRESS_REP_TIME) {
					pr_obj->signal_f(pr_obj, LV_SIGNAL_LONG_PRESS_REP, dispi_p);
                    dispi_p->lpr_rep_time_stamp = systick_get();

				}
            }
        }
    }
}

/**
 * Process the released state
 * @param dispi_p pointer to a display input
 */
static void disi_proc_release(lv_dispi_t * dispi_p)
{
    if(dispi_p->wait_release != 0) {
        dispi_p->act_obj = NULL;
        dispi_p->last_obj = NULL;
        dispi_p->press_time_stamp = 0;
        dispi_p->lpr_rep_time_stamp = 0;
        dispi_p->wait_release = 0;
    }

    /*Forgot the act obj and send a released signal */
    if(dispi_p->act_obj != NULL) {
        dispi_p->act_obj->signal_f(dispi_p->act_obj,
                                      LV_SIGNAL_RELEASED, dispi_p);
        dispi_p->act_obj = NULL;   
        dispi_p->press_time_stamp = 0;
        dispi_p->lpr_rep_time_stamp = 0;
    }
    
    /*The reset can be set in the signal function. 
     * In case of reset query ignore the remaining parts.*/
    if(dispi_p->last_obj != NULL && lv_dispi_reset_qry == false) {
        dispi_drag_throw(dispi_p);
    }

}

/**
 * Search the most top, clickable object on the last point of a display input
 * @param dispi_p pointer to a display input
 * @param obj pointer to a start object, typically the screen
 * @return pointer to the found object or NULL if there was no suitable object 
 */
static lv_obj_t * dispi_search_obj(const lv_dispi_t * dispi_p, lv_obj_t * obj)
{
    lv_obj_t * found_p = NULL;
    
    /*If the point is on this object*/
    /*Check its children too*/
    if(area_is_point_on(&obj->cords, &dispi_p->act_point)) {
        lv_obj_t * i;
    
        LL_READ(obj->child_ll, i) {
            found_p = dispi_search_obj(dispi_p, i);
            
            /*If a child was found then break*/
            if(found_p != NULL) {
                break;
            }
        }
        
        /*If then the children was not ok, but this obj is clickable
         * and it or its parent is not hidden then save this object*/
        if(found_p == NULL && lv_obj_get_click(obj) != false) {
        	lv_obj_t * i = obj;
        	while(i != NULL) {
        		if(lv_obj_get_hidden(i) == true) break;
        		i = lv_obj_get_parent(i);
        	}
        	/*No parent found with hidden == true*/
        	if(i == NULL) found_p = obj;
        }
        
    }
    
    return found_p;    
}

/**
 * Handle the dragging of dispi_p->act_obj
 * @param dispi_p pointer to a display input
 */
static void dispi_drag(lv_dispi_t * dispi_p)
{
    lv_obj_t * drag_obj = dispi_p->act_obj;
    
    /*If drag parent is active check recursively the drag_parent attribute*/
	while(lv_obj_get_drag_parent(drag_obj) != false &&
		  drag_obj != NULL) {
		drag_obj = lv_obj_get_parent(drag_obj);
	}

	if(drag_obj == NULL) return;
    
    if(lv_obj_get_drag(drag_obj) == false) return;

    /*If still there is no drag then count the movement*/
    if(dispi_p->drag_range_out == 0) {
        dispi_p->vect_sum.x += dispi_p->vect.x;
        dispi_p->vect_sum.y += dispi_p->vect.y;
        
        /*If a move is greater then LV_DRAG_LIMIT then begin the drag*/
        if(MATH_ABS(dispi_p->vect_sum.x) >= LV_DISPI_DRAG_LIMIT ||
           MATH_ABS(dispi_p->vect_sum.y) >= LV_DISPI_DRAG_LIMIT)
           {
                dispi_p->drag_range_out = 1;
           }
    }
    
    /*If the drag limit is stepped over then handle the dragging*/
    if(dispi_p->drag_range_out != 0) {
        /*Set new position if the vector is not zero*/
        if(dispi_p->vect.x != 0 ||
           dispi_p->vect.y != 0) {   
            /*Get the coordinates of the object end modify them*/
            cord_t act_x = lv_obj_get_x(drag_obj);
            cord_t act_y = lv_obj_get_y(drag_obj);

            lv_obj_set_pos(drag_obj, act_x + dispi_p->vect.x, act_y + dispi_p->vect.y);

            /*Set the drag in progress flag if the object is really moved*/
            if(lv_obj_get_x(drag_obj) != act_x || lv_obj_get_y(drag_obj) != act_y) {
                if(dispi_p->drag_range_out != 0) { /*Send the drag begin signal on first move*/
                    drag_obj->signal_f(drag_obj,  LV_SIGNAL_DRAG_BEGIN, dispi_p);
                }
                dispi_p->drag_in_prog = 1;
            }

        }
    }
}

/**
 * Handle throwing by drag if the drag is ended
 * @param dispi_p pointer to a display input
 */
static void dispi_drag_throw(lv_dispi_t * dispi_p)
{
	if(dispi_p->drag_in_prog == 0) return;

    /*Set new position if the vector is not zero*/
    lv_obj_t * drag_obj = dispi_p->last_obj;
    
    /*If drag parent is active check recursively the drag_parent attribute*/

	while(lv_obj_get_drag_parent(drag_obj) != false &&
		  drag_obj != NULL) {
		drag_obj = lv_obj_get_parent(drag_obj);
	}

	if(drag_obj == NULL) return;
    
    /*Return if the drag throw is not enabled*/
    if(lv_obj_get_drag_throw(drag_obj) == false ){
    	dispi_p->drag_in_prog = 0;
        drag_obj->signal_f(drag_obj, LV_SIGNAL_DRAG_END, dispi_p);
        return;
    }
    
    /*Reduce the vectors*/
    dispi_p->vect.x = dispi_p->vect.x * (100 -LV_DISPI_DRAG_THROW) / 100;
    dispi_p->vect.y = dispi_p->vect.y * (100 -LV_DISPI_DRAG_THROW) / 100;
    
    if(dispi_p->vect.x != 0 ||
       dispi_p->vect.y != 0)
    {
        /*Get the coordinates  and modify them*/
        cord_t act_x = lv_obj_get_x(drag_obj) + dispi_p->vect.x;
        cord_t act_y = lv_obj_get_y(drag_obj) + dispi_p->vect.y;
        lv_obj_set_pos(drag_obj, act_x, act_y);
    }
    /*If the vectors become 0 -> drag_in_prog = 0 and send a drag end signal*/
    else {
        dispi_p->drag_in_prog = 0;
        drag_obj->signal_f(drag_obj, LV_SIGNAL_DRAG_END, dispi_p);
    }
}
