/**
 * @file lv_disp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_disp.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if LV_USE_ANIMATION
    static void scr_load_anim_start(lv_anim_t * a);
    static void opa_scale_anim(lv_obj_t * obj, lv_anim_value_t v);
    static void scr_anim_ready(lv_anim_t * a);
#endif

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
 * Return with a pointer to the active screen
 * @param disp pointer to display which active screen should be get. (NULL to use the default
 * screen)
 * @return pointer to the active screen object (loaded by 'lv_scr_load()')
 */
lv_obj_t * lv_disp_get_scr_act(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered to get its active screen");
        return NULL;
    }

    return disp->act_scr;
}

/**
 * Return with a pointer to the previous screen. Only used during screen transitions.
 * @param disp pointer to display which previous screen should be get. (NULL to use the default
 * screen)
 * @return pointer to the previous screen object or NULL if not used now
 */
lv_obj_t * lv_disp_get_scr_prev(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered to get its previous screen");
        return NULL;
    }

    return disp->prev_scr;
}

/**
 * Make a screen active
 * @param scr pointer to a screen
 */
void lv_disp_load_scr(lv_obj_t * scr)
{
    lv_disp_t * d = lv_obj_get_disp(scr);
    if(!d) return;  /*Shouldn't happen, just to be sure*/
    d->act_scr = scr;

    lv_obj_invalidate(scr);
}

/**
 * Return with the top layer. (Same on every screen and it is above the normal screen layer)
 * @param disp pointer to display which top layer should be get. (NULL to use the default screen)
 * @return pointer to the top layer object  (transparent screen sized lv_obj)
 */
lv_obj_t * lv_disp_get_layer_top(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_layer_top: no display registered to get its top layer");
        return NULL;
    }

    return disp->top_layer;
}

/**
 * Return with the sys. layer. (Same on every screen and it is above the normal screen and the top
 * layer)
 * @param disp pointer to display which sys. layer  should be get. (NULL to use the default screen)
 * @return pointer to the sys layer object  (transparent screen sized lv_obj)
 */
lv_obj_t * lv_disp_get_layer_sys(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_layer_sys: no display registered to get its sys. layer");
        return NULL;
    }

    return disp->sys_layer;
}

/**
 * Assign a screen to a display.
 * @param disp pointer to a display where to assign the screen
 * @param scr pointer to a screen object to assign
 */
void lv_disp_assign_screen(lv_disp_t * disp, lv_obj_t * scr)
{
    if(lv_obj_get_parent(scr) != NULL) {
        LV_LOG_WARN("lv_disp_assign_screen: try to assign a non-screen object");
        return;
    }

    lv_disp_t * old_disp = lv_obj_get_disp(scr);

    if(old_disp == disp) return;

    _lv_ll_chg_list(&old_disp->scr_ll, &disp->scr_ll, scr, true);
}

/**
 * Set the background color of a display
 * @param disp pointer to a display
 * @param color color of the background
 */
void lv_disp_set_bg_color(lv_disp_t * disp, lv_color_t color)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered");
        return;
    }

    disp->bg_color = color;

    lv_area_t a;
    lv_area_set(&a, 0, 0, lv_disp_get_hor_res(disp) - 1, lv_disp_get_ver_res(disp) - 1);
    _lv_inv_area(disp, &a);

}

/**
 * Set the background image of a display
 * @param disp pointer to a display
 * @param img_src path to file or pointer to an `lv_img_dsc_t` variable
 */
void lv_disp_set_bg_image(lv_disp_t * disp, const void  * img_src)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered");
        return;
    }

    disp->bg_img = img_src;

    lv_area_t a;
    lv_area_set(&a, 0, 0, lv_disp_get_hor_res(disp) - 1, lv_disp_get_ver_res(disp) - 1);
    _lv_inv_area(disp, &a);
}


/**
 * Opacity of the background
 * @param disp pointer to a display
 * @param opa opacity (0..255)
 */
void lv_disp_set_bg_opa(lv_disp_t * disp, lv_opa_t opa)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("no display registered");
        return;
    }

    disp->bg_opa = opa;

    lv_area_t a;
    lv_area_set(&a, 0, 0, lv_disp_get_hor_res(disp) - 1, lv_disp_get_ver_res(disp) - 1);
    _lv_inv_area(disp, &a);
}

#if LV_USE_ANIMATION

/**
 * Switch screen with animation
 * @param scr pointer to the new screen to load
 * @param anim_type type of the animation from `lv_scr_load_anim_t`. E.g.  `LV_SCR_LOAD_ANIM_MOVE_LEFT`
 * @param time time of the animation
 * @param delay delay before the transition
 * @param auto_del true: automatically delete the old screen
 */
void lv_scr_load_anim(lv_obj_t * new_scr, lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool auto_del)
{
    lv_disp_t * d = lv_obj_get_disp(new_scr);
    lv_obj_t * act_scr = lv_scr_act();


    if(d->del_prev && act_scr != d->scr_to_load && d->scr_to_load) {
        lv_obj_del(act_scr);
        lv_disp_load_scr(d->scr_to_load);
        lv_anim_del(d->scr_to_load, NULL);
        lv_obj_set_pos(d->scr_to_load, 0, 0);
        lv_style_remove_prop(lv_obj_get_local_style(d->scr_to_load, LV_OBJ_PART_MAIN), LV_STYLE_OPA_SCALE);

        act_scr = d->scr_to_load;
    }

    d->scr_to_load = new_scr;

    if(d->prev_scr && d->del_prev) {
        lv_obj_del(d->prev_scr);
        d->prev_scr = NULL;
    }

    d->del_prev = auto_del;

    /*Be sure there is no other animation on the screens*/
    lv_anim_del(new_scr, NULL);
    lv_anim_del(lv_scr_act(), NULL);

    /*Be sure both screens are in a normal position*/
    lv_obj_set_pos(new_scr, 0, 0);
    lv_obj_set_pos(lv_scr_act(), 0, 0);
    lv_style_remove_prop(lv_obj_get_local_style(new_scr, LV_OBJ_PART_MAIN), LV_STYLE_OPA_SCALE);
    lv_style_remove_prop(lv_obj_get_local_style(lv_scr_act(), LV_OBJ_PART_MAIN), LV_STYLE_OPA_SCALE);

    lv_anim_t a_new;
    lv_anim_init(&a_new);
    lv_anim_set_var(&a_new, new_scr);
    lv_anim_set_start_cb(&a_new, scr_load_anim_start);
    lv_anim_set_ready_cb(&a_new, scr_anim_ready);
    lv_anim_set_time(&a_new, time);
    lv_anim_set_delay(&a_new, delay);

    lv_anim_t a_old;
    lv_anim_init(&a_old);
    lv_anim_set_var(&a_old, d->act_scr);
    lv_anim_set_time(&a_old, time);
    lv_anim_set_delay(&a_old, delay);

    switch(anim_type) {
        case LV_SCR_LOAD_ANIM_NONE:
            /* Create a dummy animation to apply the delay*/
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) lv_obj_set_x);
            lv_anim_set_values(&a_new, 0, 0);
            break;
        case LV_SCR_LOAD_ANIM_OVER_LEFT:
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) lv_obj_set_x);
            lv_anim_set_values(&a_new, lv_disp_get_hor_res(d), 0);
            break;
        case LV_SCR_LOAD_ANIM_OVER_RIGHT:
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) lv_obj_set_x);
            lv_anim_set_values(&a_new, -lv_disp_get_hor_res(d), 0);
            break;
        case LV_SCR_LOAD_ANIM_OVER_TOP:
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) lv_obj_set_y);
            lv_anim_set_values(&a_new, lv_disp_get_ver_res(d), 0);
            break;
        case LV_SCR_LOAD_ANIM_OVER_BOTTOM:
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) lv_obj_set_y);
            lv_anim_set_values(&a_new, -lv_disp_get_ver_res(d), 0);
            break;
        case LV_SCR_LOAD_ANIM_MOVE_LEFT:
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) lv_obj_set_x);
            lv_anim_set_values(&a_new, lv_disp_get_hor_res(d), 0);

            lv_anim_set_exec_cb(&a_old, (lv_anim_exec_xcb_t) lv_obj_set_x);
            lv_anim_set_values(&a_old, 0, -lv_disp_get_hor_res(d));
            break;
        case LV_SCR_LOAD_ANIM_MOVE_RIGHT:
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) lv_obj_set_x);
            lv_anim_set_values(&a_new, -lv_disp_get_hor_res(d), 0);

            lv_anim_set_exec_cb(&a_old, (lv_anim_exec_xcb_t) lv_obj_set_x);
            lv_anim_set_values(&a_old, 0, lv_disp_get_hor_res(d));
            break;
        case LV_SCR_LOAD_ANIM_MOVE_TOP:
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) lv_obj_set_y);
            lv_anim_set_values(&a_new, lv_disp_get_ver_res(d), 0);

            lv_anim_set_exec_cb(&a_old, (lv_anim_exec_xcb_t) lv_obj_set_y);
            lv_anim_set_values(&a_old, 0, -lv_disp_get_ver_res(d));
            break;
        case LV_SCR_LOAD_ANIM_MOVE_BOTTOM:
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) lv_obj_set_y);
            lv_anim_set_values(&a_new, -lv_disp_get_ver_res(d), 0);

            lv_anim_set_exec_cb(&a_old, (lv_anim_exec_xcb_t) lv_obj_set_y);
            lv_anim_set_values(&a_old, 0, lv_disp_get_ver_res(d));
            break;

        case LV_SCR_LOAD_ANIM_FADE_ON:
            lv_anim_set_exec_cb(&a_new, (lv_anim_exec_xcb_t) opa_scale_anim);
            lv_anim_set_values(&a_new, LV_OPA_TRANSP, LV_OPA_COVER);
            break;
    }

    lv_anim_start(&a_new);
    lv_anim_start(&a_old);
}

#endif

/**
 * Get elapsed time since last user activity on a display (e.g. click)
 * @param disp pointer to an display (NULL to get the overall smallest inactivity)
 * @return elapsed ticks (milliseconds) since the last activity
 */
uint32_t lv_disp_get_inactive_time(const lv_disp_t * disp)
{
    if(disp) return lv_tick_elaps(disp->last_activity_time);

    lv_disp_t * d;
    uint32_t t = UINT32_MAX;
    d          = lv_disp_get_next(NULL);
    while(d) {
        uint32_t elaps = lv_tick_elaps(d->last_activity_time);
        t = LV_MATH_MIN(t, elaps);
        d = lv_disp_get_next(d);
    }

    return t;
}

/**
 * Manually trigger an activity on a display
 * @param disp pointer to an display (NULL to use the default display)
 */
void lv_disp_trig_activity(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_disp_trig_activity: no display registered");
        return;
    }

    disp->last_activity_time = lv_tick_get();
}

/**
 * Clean any CPU cache that is related to the display.
 * @param disp pointer to an display (NULL to use the default display)
 */
void lv_disp_clean_dcache(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_disp_clean_dcache: no display registered");
        return;
    }

    if(disp->driver.clean_dcache_cb)
        disp->driver.clean_dcache_cb(&disp->driver);
}

/**
 * Get a pointer to the screen refresher task to
 * modify its parameters with `lv_task_...` functions.
 * @param disp pointer to a display
 * @return pointer to the display refresher task. (NULL on error)
 */
lv_task_t * _lv_disp_get_refr_task(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_disp_get_refr_task: no display registered");
        return NULL;
    }

    return disp->refr_task;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_USE_ANIMATION
static void scr_load_anim_start(lv_anim_t * a)
{
    lv_disp_t * d = lv_obj_get_disp(a->var);
    d->prev_scr = lv_scr_act();

    lv_disp_load_scr(a->var);
}

static void opa_scale_anim(lv_obj_t * obj, lv_anim_value_t v)
{
    lv_obj_set_style_local_opa_scale(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, v);
}

static void scr_anim_ready(lv_anim_t * a)
{
    lv_disp_t * d = lv_obj_get_disp(a->var);

    if(d->prev_scr && d->del_prev) lv_obj_del(d->prev_scr);
    d->prev_scr = NULL;
    d->scr_to_load = NULL;
    lv_style_remove_prop(lv_obj_get_local_style(a->var, LV_OBJ_PART_MAIN), LV_STYLE_OPA_SCALE);
}
#endif
