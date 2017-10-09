/**
 * @file lv_indev_proc.c
 * 
 */

/*********************
 *      INCLUDES
 ********************/
#include <lvgl/lv_obj/lv_indev.h>
#include "lv_conf.h"

#include "../lv_hal/lv_hal_tick.h"
#include "misc/os/ptask.h"
#include "misc/math/math_base.h"
#include "../lv_draw/lv_draw_rbasic.h"
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
static void indev_proc_task(void * param);
static void indev_proc_point(lv_indev_state_t * indev, cord_t x, cord_t y);
static void indev_proc_press(lv_indev_state_t * state);
static void disi_proc_release(lv_indev_state_t * state);
static lv_obj_t * indev_search_obj(const lv_indev_state_t * indev, lv_obj_t * obj);
static void indev_drag(lv_indev_state_t * state);
static void indev_drag_throw(lv_indev_state_t * state);

/**********************
 *  STATIC VARIABLES
 **********************/
static ptask_t *indev_proc_task_p;
static lv_indev_t *indev_act;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the display input device subsystem
 */
void lv_indev_proc_init(void)
{
#if LV_indev_proc_READ_PERIOD != 0
    indev_proc_task_p = ptask_create(indev_proc_task, LV_indev_proc_READ_PERIOD, PTASK_PRIO_MID, NULL);
#else
    indev_proc_task_p = ptask_create(indev_proc_task, 1, PTASK_PRIO_OFF); /*Not use lv_indev_proc*/
#endif

    lv_indev_reset(NULL);   /*Reset all input devices*/
}

/**
 * Enable input devices device by type
 * @param type Input device type
 * @param enable true: enable this type; false: disable this type
 */
void lv_indev_enable(lv_hal_indev_type_t type, bool enable)
{
    lv_indev_t *i = lv_indev_next(NULL);

    while (i) {
        if (i->drv.type == type) i->state.disable = enable == false ? 1 : 0;
        i = lv_indev_next(i);
    }
}

/**
 * Get the currently processed input device. Can be used in action functions too.
 * @return pointer to the currently processed input device or NULL if no input device processing right now
 */
lv_indev_t * lv_indev_get_act(void)
{
    return indev_act;
}

/**
 * Reset one or all input devices
 * @param indev
 */
void lv_indev_reset(lv_indev_t * indev)
{
    if(indev) indev->state.reset_qry = 1;
    else {
        lv_indev_t * i = lv_indev_next(NULL);
        while(i) {
            i->state.reset_qry = 1;
            i = lv_indev_next(i);
        }
    }
}

/**
 * Reset the long press state of an input device
 * @param indev_proc pointer to an input device
 */
void lv_indev_reset_lpr(lv_indev_t * indev_proc)
{
    indev_proc->state.long_press_sent = 0;
    indev_proc->state.lpr_rep_time_stamp = lv_tick_get();
    indev_proc->state.press_time_stamp = lv_tick_get();
}

/**
 * Get the last point of an input device
 * @param indev pointer to an input device
 * @param point pointer to a point to store the result
 */
void lv_indev_get_point(lv_indev_t * indev, point_t * point)
{
    point->x = indev->state.act_point.x;
    point->y = indev->state.act_point.y;
}

/**
 * Check if there is dragging with an input device or not
 * @param indev pointer to an input device
 * @return true: drag is in progress
 */
bool lv_indev_is_dragging(lv_indev_t * indev)
{
    return indev->state.drag_in_prog == 0 ? false : true;
}

/**
 * Get the vector of dragging of an input device
 * @param indev pointer to an input device
 * @param point pointer to a point to store the vector
 */
void lv_indev_get_vect(lv_indev_t * indev, point_t * point)
{
    point->x = indev->state.vect.x;
    point->y = indev->state.vect.y;
}

/**
 * Do nothing until the next release
 * @param indev pointer to an input device
 */
void lv_indev_wait_release(lv_indev_t * indev)
{
    indev->state.wait_release = 1;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called periodically to handle the input devices
 * @param param unused
 */
static void indev_proc_task(void * param)
{
    lv_hal_indev_data_t data;
    lv_indev_t * i;
    i = lv_indev_next(NULL);

    while(i) {
        indev_act = i;

        if(i->state.reset_qry) {
            i->state.act_obj = NULL;
            i->state.last_obj = NULL;
            i->state.drag_range_out = 0;
            i->state.drag_in_prog = 0;
            i->state.long_press_sent = 0;
            i->state.press_time_stamp = 0;
            i->state.lpr_rep_time_stamp = 0;
            i->state.vect_sum.x = 0;
            i->state.vect_sum.y = 0;
            i->state.reset_qry = 0;
        }

        lv_indev_get(i, &data);
        i->state.pressed = data.state;
        indev_proc_point(&i->state, data.point.x , data.point.y);



        i = lv_indev_next(i);
    }

    indev_act = NULL;
}

/**
 * Process new points from a input device. indev->state.pressed has to be set
 * @param indev pointer to an input device state
 * @param x x coordinate of the next point
 * @param y y coordinate of the next point
 */
static void indev_proc_point(lv_indev_state_t * indev, cord_t x, cord_t y)
{
#if LV_ANTIALIAS != 0 && LV_VDB_SIZE != 0
    indev->act_point.x = x * LV_DOWNSCALE;
    indev->act_point.y = y * LV_DOWNSCALE;
#else
    indev->act_point.x = x;
    indev->act_point.y = y;
#endif
    
    if(indev->pressed != false){
#if LV_indev_proc_TP_MARKER != 0
        area_t area;
        area.x1 = x - (LV_indev_proc_TP_MARKER >> 1);
        area.y1 = y - (LV_indev_proc_TP_MARKER >> 1);
        area.x2 = x + ((LV_indev_proc_TP_MARKER >> 1) | 0x1);
        area.y2 = y + ((LV_indev_proc_TP_MARKER >> 1) | 0x1);
        lv_rfill(&area, NULL, COLOR_MAKE(0xFF, 0, 0), OPA_COVER);
#endif
        indev_proc_press(indev);
    } else {
        disi_proc_release(indev);
    }
    
    
    indev->last_point.x = indev->act_point.x;
    indev->last_point.y = indev->act_point.y;
}   

/**
 * Process the pressed state
 * @param indev pointer to an input device state
 */
static void indev_proc_press(lv_indev_state_t * state)
{
    lv_obj_t * pr_obj = state->act_obj;
    
    if(state->wait_release != 0) return;

    /*If there is no last object then search*/
    if(state->act_obj == NULL) {
        pr_obj = indev_search_obj(state, lv_scr_act());
    }
    /*If there is last object but it is not dragged also search*/
    else if(state->drag_in_prog == 0) {/*Now act_obj != NULL*/
        pr_obj = indev_search_obj(state, lv_scr_act());
    }
    /*If a dragable object was the last then keep it*/
    else {
        
    }
    
    /*If a new object was found reset some variables and send a pressed signal*/
    if(pr_obj != state->act_obj) {

        state->last_point.x = state->act_point.x;
        state->last_point.y = state->act_point.y;
        
        /*If a new object found the previous was lost, so send a signal*/
        if(state->act_obj != NULL) {
            state->act_obj->signal_f(state->act_obj, LV_SIGNAL_PRESS_LOST, indev_act);
        }
        
        if(pr_obj != NULL) {
            /* Save the time when the obj pressed. 
             * It is necessary to count the long press time.*/
            state->press_time_stamp = lv_tick_get();
            state->long_press_sent = 0;
            state->drag_range_out = 0;
            state->drag_in_prog = 0;
            state->vect_sum.x = 0;
            state->vect_sum.y = 0;

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
            pr_obj->signal_f(pr_obj, LV_SIGNAL_PRESSED, indev_act);
        }
    }
    
    /* The reset can be set in the signal function.
     * In case of reset query ignore the remaining parts.*/
    if(state->reset_qry == 0) {
        state->act_obj = pr_obj;      /*Save the pressed object*/
        state->last_obj = state->act_obj; /*Refresh the last_obj*/

        /*Calculate the vector*/
        state->vect.x = state->act_point.x - state->last_point.x;
        state->vect.y = state->act_point.y - state->last_point.y;

        /*If there is active object and it can be dragged run the drag*/
        if(state->act_obj != NULL) {
            state->act_obj->signal_f(state->act_obj, LV_SIGNAL_PRESSING, indev_act);

            indev_drag(state);
        
            /*If there is no drag then check for long press time*/
            if(state->drag_in_prog == 0 && state->long_press_sent == 0) {
                /*Send a signal about the long press if enough time elapsed*/
                if(lv_tick_elaps(state->press_time_stamp) > LV_indev_proc_LONG_PRESS_TIME) {
                    pr_obj->signal_f(pr_obj, LV_SIGNAL_LONG_PRESS, indev_act);

                    /*Mark the signal sending to do not send it again*/
                    state->long_press_sent = 1;

                    /*Save the long press time stamp for the long press repeat handler*/
                    state->lpr_rep_time_stamp = lv_tick_get();
                }
            }
            /*Send long press repeated signal*/
            if(state->drag_in_prog == 0 && state->long_press_sent == 1) {
            	/*Send a signal about the long press repeate if enough time elapsed*/
				if(lv_tick_elaps(state->lpr_rep_time_stamp) > LV_indev_proc_LONG_PRESS_REP_TIME) {
					pr_obj->signal_f(pr_obj, LV_SIGNAL_LONG_PRESS_REP, indev_act);
                    state->lpr_rep_time_stamp = lv_tick_get();

				}
            }
        }
    }
}

/**
 * Process the released state
 * @param indev_proc_p pointer to an input device state
 */
static void disi_proc_release(lv_indev_state_t * state)
{
    if(state->wait_release != 0) {
        state->act_obj = NULL;
        state->last_obj = NULL;
        state->press_time_stamp = 0;
        state->lpr_rep_time_stamp = 0;
        state->wait_release = 0;
    }

    /*Forgot the act obj and send a released signal */
    if(state->act_obj != NULL) {
        state->act_obj->signal_f(state->act_obj, LV_SIGNAL_RELEASED, indev_act);
        state->act_obj = NULL;
        state->press_time_stamp = 0;
        state->lpr_rep_time_stamp = 0;
    }
    
    /*The reset can be set in the signal function. 
     * In case of reset query ignore the remaining parts.*/
    if(state->last_obj != NULL && state->reset_qry == 0) {
        indev_drag_throw(state);
    }
}

/**
 * Search the most top, clickable object on the last point of an input device
 * @param indev pointer to  an input device
 * @param obj pointer to a start object, typically the screen
 * @return pointer to the found object or NULL if there was no suitable object 
 */
static lv_obj_t * indev_search_obj(const lv_indev_state_t * indev, lv_obj_t * obj)
{
    lv_obj_t * found_p = NULL;
    
    /*If the point is on this object*/
    /*Check its children too*/
    if(area_is_point_on(&obj->cords, &indev->act_point)) {
        lv_obj_t * i;
    
        LL_READ(obj->child_ll, i) {
            found_p = indev_search_obj(indev, i);
            
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
 * Handle the dragging of indev_proc_p->act_obj
 * @param indev pointer to a input device state
 */
static void indev_drag(lv_indev_state_t * state)
{
    lv_obj_t * drag_obj = state->act_obj;
    
    /*If drag parent is active check recursively the drag_parent attribute*/
	while(lv_obj_get_drag_parent(drag_obj) != false &&
		  drag_obj != NULL) {
		drag_obj = lv_obj_get_parent(drag_obj);
	}

	if(drag_obj == NULL) return;
    
    if(lv_obj_get_drag(drag_obj) == false) return;

    /*If still there is no drag then count the movement*/
    if(state->drag_range_out == 0) {
        state->vect_sum.x += state->vect.x;
        state->vect_sum.y += state->vect.y;
        
        /*If a move is greater then LV_DRAG_LIMIT then begin the drag*/
        if(MATH_ABS(state->vect_sum.x) >= LV_indev_proc_DRAG_LIMIT ||
           MATH_ABS(state->vect_sum.y) >= LV_indev_proc_DRAG_LIMIT)
           {
                state->drag_range_out = 1;
           }
    }
    
    /*If the drag limit is stepped over then handle the dragging*/
    if(state->drag_range_out != 0) {
        /*Set new position if the vector is not zero*/
        if(state->vect.x != 0 ||
           state->vect.y != 0) {
            /*Get the coordinates of the object end modify them*/
            cord_t act_x = lv_obj_get_x(drag_obj);
            cord_t act_y = lv_obj_get_y(drag_obj);

            lv_obj_set_pos(drag_obj, act_x + state->vect.x, act_y + state->vect.y);

            /*Set the drag in progress flag if the object is really moved*/
            if(lv_obj_get_x(drag_obj) != act_x || lv_obj_get_y(drag_obj) != act_y) {
                if(state->drag_range_out != 0) { /*Send the drag begin signal on first move*/
                    drag_obj->signal_f(drag_obj,  LV_SIGNAL_DRAG_BEGIN, indev_act);
                }
                state->drag_in_prog = 1;
            }

        }
    }
}

/**
 * Handle throwing by drag if the drag is ended
 * @param indev pointer to an input device state
 */
static void indev_drag_throw(lv_indev_state_t * state)
{
	if(state->drag_in_prog == 0) return;

    /*Set new position if the vector is not zero*/
    lv_obj_t * drag_obj = state->last_obj;
    
    /*If drag parent is active check recursively the drag_parent attribute*/

	while(lv_obj_get_drag_parent(drag_obj) != false &&
		  drag_obj != NULL) {
		drag_obj = lv_obj_get_parent(drag_obj);
	}

	if(drag_obj == NULL) return;
    
    /*Return if the drag throw is not enabled*/
    if(lv_obj_get_drag_throw(drag_obj) == false ){
    	state->drag_in_prog = 0;
        drag_obj->signal_f(drag_obj, LV_SIGNAL_DRAG_END, indev_act);
        return;
    }
    
    /*Reduce the vectors*/
    state->vect.x = state->vect.x * (100 -LV_indev_proc_DRAG_THROW) / 100;
    state->vect.y = state->vect.y * (100 -LV_indev_proc_DRAG_THROW) / 100;
    
    if(state->vect.x != 0 ||
       state->vect.y != 0)
    {
        /*Get the coordinates  and modify them*/
        cord_t act_x = lv_obj_get_x(drag_obj) + state->vect.x;
        cord_t act_y = lv_obj_get_y(drag_obj) + state->vect.y;
        lv_obj_set_pos(drag_obj, act_x, act_y);
    }
    /*If the vectors become 0 -> drag_in_prog = 0 and send a drag end signal*/
    else {
        state->drag_in_prog = 0;
        drag_obj->signal_f(drag_obj, LV_SIGNAL_DRAG_END, indev_act);
    }
}
