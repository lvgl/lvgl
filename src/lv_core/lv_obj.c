/**
 * @file lv_base_obj.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "lv_indev.h"
#include "lv_refr.h"
#include "lv_group.h"
#include "lv_disp.h"
#include "../lv_core/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_task.h"
#include "../lv_misc/lv_async.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_gc.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_gc.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_log.h"
#include "../lv_hal/lv_hal.h"
#include <stdint.h>
#include <string.h>

#if defined(LV_GC_INCLUDE)
#include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_obj"
#define LV_OBJ_DEF_WIDTH    (LV_DPI)
#define LV_OBJ_DEF_HEIGHT   (2 * LV_DPI / 3)
#define LV_DRAW_RECT_CACHE_SIZE     0

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_event_temp_data
{
    lv_obj_t * obj;
    bool deleted;
    struct _lv_event_temp_data * prev;
} lv_event_temp_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void refresh_children_position(lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff);
static void report_style_mod_core(void * style_p, lv_obj_t * obj);
static void refresh_children_style(lv_obj_t * obj);
static void delete_children(lv_obj_t * obj);
static void base_dir_refr_children(lv_obj_t * obj);
static void obj_state_anim_cb(void * p, lv_anim_value_t value);
static void lv_event_mark_deleted(lv_obj_t * obj);
static void lv_obj_del_async_cb(void * obj);
static lv_design_res_t lv_obj_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_obj_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool lv_initialized = false;
static lv_event_temp_data_t * event_temp_data_head;
static const void * event_act_data;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init. the 'lv' library.
 */
void lv_init(void)
{
    /* Do nothing if already initialized */
    if(lv_initialized) {
        LV_LOG_WARN("lv_init: already inited");
        return;
    }

    LV_LOG_TRACE("lv_init started");

    /*Initialize the lv_misc modules*/
    lv_mem_init();
    lv_task_core_init();

#if LV_USE_FILESYSTEM
    lv_fs_init();
#endif

#if LV_USE_ANIMATION
    lv_anim_core_init();
#endif

#if LV_USE_GROUP
    lv_group_init();
#endif



    lv_theme_t * th = LV_THEME_DEFAULT_INIT(LV_THEME_DEFAULT_COLOR_PRIMARY, LV_THEME_DEFAULT_COLOR_SECONDARY,
            LV_THEME_DEFAULT_FLAGS,
            LV_THEME_DEFAULT_FONT_SMALL, LV_THEME_DEFAULT_FONT_NORMAL, LV_THEME_DEFAULT_FONT_SUBTITLE, LV_THEME_DEFAULT_FONT_TITLE);
    lv_theme_set_act(th);

    /*Initialize the screen refresh system*/
    lv_refr_init();

    lv_ll_init(&LV_GC_ROOT(_lv_disp_ll), sizeof(lv_disp_t));
    lv_ll_init(&LV_GC_ROOT(_lv_indev_ll), sizeof(lv_indev_t));

    /*Init the input device handling*/
    lv_indev_init();

    lv_img_decoder_init();
    lv_img_cache_set_size(LV_IMG_CACHE_DEF_SIZE);

    lv_initialized = true;
    LV_LOG_INFO("lv_init ready");
}

#if LV_ENABLE_GC || !LV_MEM_CUSTOM

/**
 * Deinit the 'lv' library
 * Currently only implemented when not using custom allocators, or GC is enabled.
 */
void lv_deinit(void)
{
    lv_gc_clear_roots();
#if LV_USE_LOG
    lv_log_register_print_cb(NULL);
#endif
    lv_disp_set_default(NULL);
    lv_mem_deinit();
    lv_initialized = false;
#if LV_USE_LOG
    lv_log_register_print_cb(NULL);
#endif
    LV_LOG_INFO("lv_deinit done");
}
#endif

/*--------------------
 * Create and delete
 *-------------------*/

/**
 * Create a basic object
 * @param parent pointer to a parent object.
 *                  If NULL then a screen will be created
 * @param copy pointer to a base object, if not NULL then the new object will be copied from it
 * @return pointer to the new object
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    lv_obj_t * new_obj = NULL;

    /*Create a screen*/
    if(parent == NULL) {
        LV_LOG_TRACE("Screen create started");
        lv_disp_t * disp = lv_disp_get_default();
        if(!disp) {
            LV_LOG_WARN("lv_obj_create: not display created to so far. No place to assign the new screen");
            return NULL;
        }

        new_obj = lv_ll_ins_head(&disp->scr_ll);
        LV_ASSERT_MEM(new_obj);
        if(new_obj == NULL) return NULL;

        memset(new_obj, 0x00, sizeof(lv_obj_t));

        /*Set coordinates to full screen size*/
        new_obj->coords.x1    = 0;
        new_obj->coords.y1    = 0;
        new_obj->coords.x2    = lv_disp_get_hor_res(NULL) - 1;
        new_obj->coords.y2    = lv_disp_get_ver_res(NULL) - 1;
    }
    /*Create a normal object*/
    else {
        LV_LOG_TRACE("Object create started");
        LV_ASSERT_OBJ(parent, LV_OBJX_NAME);

        new_obj = lv_ll_ins_head(&parent->child_ll);
        LV_ASSERT_MEM(new_obj);
        if(new_obj == NULL) return NULL;

        memset(new_obj, 0x00, sizeof(lv_obj_t));

        new_obj->coords.y1    = parent->coords.y1;
        new_obj->coords.y2    = parent->coords.y1 + LV_OBJ_DEF_HEIGHT;
        if(lv_obj_get_base_dir(new_obj) == LV_BIDI_DIR_RTL) {
            new_obj->coords.x2    = parent->coords.x2;
            new_obj->coords.x1    = parent->coords.x2 - LV_OBJ_DEF_WIDTH;
        } else {
            new_obj->coords.x1    = parent->coords.x1;
            new_obj->coords.x2    = parent->coords.x1 + LV_OBJ_DEF_WIDTH;
        }
    }

    new_obj->parent = parent;

    lv_ll_init(&(new_obj->child_ll), sizeof(lv_obj_t));

    /*Set the callbacks*/
    new_obj->signal_cb = lv_obj_signal;
    new_obj->design_cb = lv_obj_design;
    new_obj->event_cb = NULL;

    new_obj->ext_draw_pad = 0;

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    memset(&new_obj->ext_click_pad, 0, sizeof(new_obj->ext_click_pad));
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    new_obj->ext_click_pad_hor = 0;
    new_obj->ext_click_pad_ver = 0;
#endif

    /*Init realign*/
#if LV_USE_OBJ_REALIGN
    new_obj->realign.align        = LV_ALIGN_CENTER;
    new_obj->realign.xofs         = 0;
    new_obj->realign.yofs         = 0;
    new_obj->realign.base         = NULL;
    new_obj->realign.auto_realign = 0;
#endif

    /*Init. user date*/
#if LV_USE_USER_DATA
    memset(&new_obj->user_data, 0, sizeof(lv_obj_user_data_t));
#endif


#if LV_USE_GROUP
    new_obj->group_p = NULL;
#endif

    /*Set attributes*/
    new_obj->adv_hittest  = 0;
    new_obj->click        = 1;
    new_obj->drag         = 0;
    new_obj->drag_throw   = 0;
    new_obj->drag_parent  = 0;
    new_obj->drag_dir     = LV_DRAG_DIR_BOTH;
    new_obj->hidden       = 0;
    new_obj->top          = 0;
    new_obj->protect      = LV_PROTECT_NONE;
    new_obj->parent_event = 0;
    new_obj->gesture_parent = 1;
    new_obj->state_dsc.act = LV_STATE_NORMAL;
    new_obj->state_dsc.prev = LV_STATE_NORMAL;
    new_obj->state_dsc.anim = 0;

#if LV_USE_BIDI
    if(parent == NULL) new_obj->base_dir     = LV_BIDI_BASE_DIR_DEF;
    else new_obj->base_dir     = LV_BIDI_DIR_INHERIT;
#else
    new_obj->base_dir     = LV_BIDI_DIR_LTR;
#endif

    new_obj->ext_attr = NULL;

    lv_style_list_init(&new_obj->style_list);
    if(copy == NULL) {
        if(parent != NULL) lv_theme_apply(new_obj, LV_THEME_OBJ);
        else  lv_theme_apply(new_obj, LV_THEME_SCR);
    } else {
        lv_style_list_copy(&new_obj->style_list, &copy->style_list);
    }
    /*Copy the attributes if required*/
    if(copy != NULL) {
        lv_area_copy(&new_obj->coords, &copy->coords);
        new_obj->ext_draw_pad = copy->ext_draw_pad;

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
        lv_area_copy(&new_obj->ext_click_pad, &copy->ext_click_pad);
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
        new_obj->ext_click_pad_hor = copy->ext_click_pad_hor;
        new_obj->ext_click_pad_ver = copy->ext_click_pad_ver;
#endif

        /*Set user data*/
#if LV_USE_USER_DATA
        memcpy(&new_obj->user_data, &copy->user_data, sizeof(lv_obj_user_data_t));
#endif

        /*Copy realign*/
#if LV_USE_OBJ_REALIGN
        new_obj->realign.align        = copy->realign.align;
        new_obj->realign.xofs         = copy->realign.xofs;
        new_obj->realign.yofs         = copy->realign.yofs;
        new_obj->realign.base         = copy->realign.base;
        new_obj->realign.auto_realign = copy->realign.auto_realign;
#endif

        /*Only copy the `event_cb`. `signal_cb` and `design_cb` will be copied in the derived
         * object type (e.g. `lv_btn`)*/
        new_obj->event_cb = copy->event_cb;

        /*Copy attributes*/
        new_obj->adv_hittest  = copy->adv_hittest;
        new_obj->click        = copy->click;
        new_obj->drag         = copy->drag;
        new_obj->drag_dir     = copy->drag_dir;
        new_obj->drag_throw   = copy->drag_throw;
        new_obj->drag_parent  = copy->drag_parent;
        new_obj->hidden       = copy->hidden;
        new_obj->top          = copy->top;
        new_obj->parent_event = copy->parent_event;

        new_obj->protect      = copy->protect;
        new_obj->gesture_parent = copy->gesture_parent;

#if LV_USE_GROUP
        /*Add to the same group*/
        if(copy->group_p != NULL) {
            lv_group_add_obj(copy->group_p, new_obj);
        }
#endif

        /*Set the same coordinates for non screen objects*/
        if(lv_obj_get_parent(copy) != NULL && parent != NULL) {
            lv_obj_set_pos(new_obj, lv_obj_get_x(copy), lv_obj_get_y(copy));
        } else {
            lv_obj_set_pos(new_obj, 0, 0);
        }

    }

    /*Send a signal to the parent to notify it about the new child*/
    if(parent != NULL) {
        parent->signal_cb(parent, LV_SIGNAL_CHILD_CHG, new_obj);

        /*Invalidate the area if not screen created*/
        lv_obj_invalidate(new_obj);
    }

    LV_LOG_INFO("Object create ready");

    return new_obj;
}

/**
 * Delete 'obj' and all of its children
 * @param obj pointer to an object to delete
 * @return LV_RES_INV because the object is deleted
 */
lv_res_t lv_obj_del(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_obj_invalidate(obj);

    /*Delete from the group*/
#if LV_USE_GROUP
    lv_group_t * group = lv_obj_get_group(obj);
    if(group) lv_group_remove_obj(obj);
#endif

        /*Remove the animations from this object*/
#if LV_USE_ANIMATION
    lv_anim_del(obj, NULL);
#endif

    /*Recursively delete the children*/
    lv_obj_t * i;
    lv_obj_t * i_next;
    i = lv_ll_get_head(&(obj->child_ll));
    while(i != NULL) {
        /*Get the next object before delete this*/
        i_next = lv_ll_get_next(&(obj->child_ll), i);

        /*Call the recursive del to the child too*/
        delete_children(i);

        /*Set i to the next node*/
        i = i_next;
    }

    /*Let the user free the resources used in `LV_EVENT_DELETE`*/
    lv_event_send(obj, LV_EVENT_DELETE, NULL);

    lv_event_mark_deleted(obj);

    /* Reset all input devices if the object to delete is used*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(indev->proc.types.pointer.act_obj == obj || indev->proc.types.pointer.last_obj == obj) {
            lv_indev_reset(indev, obj);
        }
        if(indev->proc.types.pointer.last_pressed == obj) {
            indev->proc.types.pointer.last_pressed = NULL;
        }

#if LV_USE_GROUP
        if(indev->group == group && obj == lv_indev_get_obj_act()) {
            lv_indev_reset(indev, obj);
        }
#endif
        indev = lv_indev_get_next(indev);
    }

    /* All children deleted.
     * Now clean up the object specific data*/
    obj->signal_cb(obj, LV_SIGNAL_CLEANUP, NULL);

    /*Remove the object from parent's children list*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par == NULL) { /*It is a screen*/
        lv_disp_t * d = lv_obj_get_disp(obj);
        lv_ll_remove(&d->scr_ll, obj);
    } else {
        lv_ll_remove(&(par->child_ll), obj);
    }

    /*Delete the base objects*/
    if(obj->ext_attr != NULL) lv_mem_free(obj->ext_attr);
    lv_mem_free(obj); /*Free the object itself*/

    /*Send a signal to the parent to notify it about the child delete*/
    if(par != NULL) {
        par->signal_cb(par, LV_SIGNAL_CHILD_CHG, NULL);
    }

    return LV_RES_INV;
}

/**
 * Helper function for asynchronously deleting objects.
 * Useful for cases where you can't delete an object directly in an `LV_EVENT_DELETE` handler (i.e. parent).
 * @param obj object to delete
 * @see lv_async_call
 */
void lv_obj_del_async(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_async_call(lv_obj_del_async_cb, obj);
}

/**
 * Delete all children of an object
 * @param obj pointer to an object
 */
void lv_obj_clean(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    lv_obj_t * child_next;
    while(child) {
        /* Read the next child before deleting the current
         * because the next couldn't be read from a deleted (invalid) node*/
        child_next = lv_obj_get_child(obj, child);
        lv_obj_del(child);
        child = child_next;
    }
}

/**
 * Mark an area of an object as invalid.
 * This area will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 * @param area the area to redraw
 */
void lv_obj_invalidate_area(const lv_obj_t * obj, const lv_area_t * area)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(lv_obj_get_hidden(obj)) return;

    /*Invalidate the object only if it belongs to the 'LV_GC_ROOT(_lv_act_scr)'*/
    lv_obj_t * obj_scr = lv_obj_get_screen(obj);
    lv_disp_t * disp   = lv_obj_get_disp(obj_scr);
    if(obj_scr == lv_disp_get_scr_act(disp) || obj_scr == lv_disp_get_layer_top(disp) ||
       obj_scr == lv_disp_get_layer_sys(disp)) {

        /*Truncate the area to the object*/
        lv_area_t obj_coords;
        lv_coord_t ext_size = obj->ext_draw_pad;
        lv_area_copy(&obj_coords, &obj->coords);
        obj_coords.x1 -= ext_size;
        obj_coords.y1 -= ext_size;
        obj_coords.x2 += ext_size;
        obj_coords.y2 += ext_size;

        bool is_common;
        lv_area_t area_trunc;

        is_common = lv_area_intersect(&area_trunc, area, &obj_coords);
        if(is_common == false) return;  /*The area is not on the object*/

        /*Truncate recursively to the parents*/
        lv_obj_t * par = lv_obj_get_parent(obj);
        while(par != NULL) {
            is_common = lv_area_intersect(&area_trunc, &area_trunc, &par->coords);
            if(is_common == false) break;       /*If no common parts with parent break;*/
            if(lv_obj_get_hidden(par)) return; /*If the parent is hidden then the child is hidden and won't be drawn*/

            par = lv_obj_get_parent(par);
        }

        if(is_common) lv_inv_area(disp, &area_trunc);
    }
}

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_invalidate(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    /*Truncate the area to the object*/
    lv_area_t obj_coords;
    lv_coord_t ext_size = obj->ext_draw_pad;
    lv_area_copy(&obj_coords, &obj->coords);
    obj_coords.x1 -= ext_size;
    obj_coords.y1 -= ext_size;
    obj_coords.x2 += ext_size;
    obj_coords.y2 += ext_size;

    lv_obj_invalidate_area(obj, &obj_coords);

}
/*=====================
 * Setter functions
 *====================*/

/*--------------------
 * Parent/children set
 *--------------------*/

/**
 * Set a new parent for an object. Its relative position will be the same.
 * @param obj pointer to an object. Can't be a screen.
 * @param parent pointer to the new parent object. (Can't be NULL)
 */
void lv_obj_set_parent(lv_obj_t * obj, lv_obj_t * parent)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_OBJ(parent, LV_OBJX_NAME);

    if(obj->parent == NULL) {
        LV_LOG_WARN("Can't set the parent of a screen");
        return;
    }

    if(parent == NULL) {
        LV_LOG_WARN("Can't set parent == NULL to an object");
        return;
    }

    lv_obj_invalidate(obj);

    lv_obj_t * old_par = obj->parent;
    lv_point_t old_pos;
    old_pos.y = lv_obj_get_y(obj);

    lv_bidi_dir_t new_base_dir = lv_obj_get_base_dir(parent);

    if(new_base_dir != LV_BIDI_DIR_RTL) {
        old_pos.x = lv_obj_get_x(obj);
    } else {
        old_pos.x = old_par->coords.x2 - obj->coords.x2;
    }

    lv_ll_chg_list(&obj->parent->child_ll, &parent->child_ll, obj, true);
    obj->parent = parent;


    if(new_base_dir != LV_BIDI_DIR_RTL) {
        lv_obj_set_pos(obj, old_pos.x, old_pos.y);
    } else {
        /*Align to the right in case of RTL base dir*/
        lv_coord_t new_x = lv_obj_get_width(parent) - old_pos.x - lv_obj_get_width(obj);
        lv_obj_set_pos(obj, new_x , old_pos.y);
    }

    /*Notify the original parent because one of its children is lost*/
    old_par->signal_cb(old_par, LV_SIGNAL_CHILD_CHG, NULL);

    /*Notify the new parent about the child*/
    parent->signal_cb(parent, LV_SIGNAL_CHILD_CHG, obj);

    lv_obj_invalidate(obj);
}

/**
 * Move and object to the foreground
 * @param obj pointer to an object
 */
void lv_obj_move_foreground(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * parent = lv_obj_get_parent(obj);

    /*Do nothing of already in the foreground*/
    if(lv_ll_get_head(&parent->child_ll) == obj) return;

    lv_obj_invalidate(parent);

    lv_ll_chg_list(&parent->child_ll, &parent->child_ll, obj, true);

    /*Notify the new parent about the child*/
    parent->signal_cb(parent, LV_SIGNAL_CHILD_CHG, obj);

    lv_obj_invalidate(parent);
}

/**
 * Move and object to the background
 * @param obj pointer to an object
 */
void lv_obj_move_background(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * parent = lv_obj_get_parent(obj);

    /*Do nothing of already in the background*/
    if(lv_ll_get_tail(&parent->child_ll) == obj) return;

    lv_obj_invalidate(parent);

    lv_ll_chg_list(&parent->child_ll, &parent->child_ll, obj, false);

    /*Notify the new parent about the child*/
    parent->signal_cb(parent, LV_SIGNAL_CHILD_CHG, obj);

    lv_obj_invalidate(parent);
}

/*--------------------
 * Coordinate set
 * ------------------*/

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void lv_obj_set_pos(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    /*Convert x and y to absolute coordinates*/
    lv_obj_t * par = obj->parent;
    
    if(par == NULL) {
        LV_LOG_WARN("lv_obj_set_pos: not changing position of screen object");
        return;
    }

    x = x + par->coords.x1;
    y = y + par->coords.y1;

    /*Calculate and set the movement*/
    lv_point_t diff;
    diff.x = x - obj->coords.x1;
    diff.y = y - obj->coords.y1;

    /* Do nothing if the position is not changed */
    /* It is very important else recursive positioning can
     * occur without position change*/
    if(diff.x == 0 && diff.y == 0) return;

    /*Invalidate the original area*/
    lv_obj_invalidate(obj);

    /*Save the original coordinates*/
    lv_area_t ori;
    lv_obj_get_coords(obj, &ori);

    obj->coords.x1 += diff.x;
    obj->coords.y1 += diff.y;
    obj->coords.x2 += diff.x;
    obj->coords.y2 += diff.y;

    refresh_children_position(obj, diff.x, diff.y);

    /*Inform the object about its new coordinates*/
    obj->signal_cb(obj, LV_SIGNAL_COORD_CHG, &ori);

    /*Send a signal to the parent too*/
    par->signal_cb(par, LV_SIGNAL_CHILD_CHG, obj);

    /*Invalidate the new area*/
    lv_obj_invalidate(obj);
}

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void lv_obj_set_x(lv_obj_t * obj, lv_coord_t x)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_pos(obj, x, lv_obj_get_y(obj));
}

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void lv_obj_set_y(lv_obj_t * obj, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_pos(obj, lv_obj_get_x(obj), y);
}

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width
 * @param h new height
 */
void lv_obj_set_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    /* Do nothing if the size is not changed */
    /* It is very important else recursive resizing can
     * occur without size change*/
    if(lv_obj_get_width(obj) == w && lv_obj_get_height(obj) == h) {
        return;
    }

    /*Invalidate the original area*/
    lv_obj_invalidate(obj);

    /*Save the original coordinates*/
    lv_area_t ori;
    lv_obj_get_coords(obj, &ori);

    /*Set the length and height*/
    obj->coords.y2 = obj->coords.y1 + h - 1;
    if(lv_obj_get_base_dir(obj) == LV_BIDI_DIR_RTL) {
        obj->coords.x1 = obj->coords.x2 - w + 1;
    } else {
        obj->coords.x2 = obj->coords.x1 + w - 1;
    }

    /*Send a signal to the object with its new coordinates*/
    obj->signal_cb(obj, LV_SIGNAL_COORD_CHG, &ori);

    /*Send a signal to the parent too*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par != NULL) par->signal_cb(par, LV_SIGNAL_CHILD_CHG, obj);

    /*Tell the children the parent's size has changed*/
    lv_obj_t * i;
    LV_LL_READ(obj->child_ll, i) {
        i->signal_cb(i, LV_SIGNAL_PARENT_SIZE_CHG,  &ori);
    }

    /*Invalidate the new area*/
    lv_obj_invalidate(obj);

    /*Automatically realign the object if required*/
#if LV_USE_OBJ_REALIGN
    if(obj->realign.auto_realign) lv_obj_realign(obj);
#endif
}

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void lv_obj_set_width(lv_obj_t * obj, lv_coord_t w)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_size(obj, w, lv_obj_get_height(obj));
}

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void lv_obj_set_height(lv_obj_t * obj, lv_coord_t h)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_size(obj, lv_obj_get_width(obj), h);
}

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void lv_obj_align(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_mod, lv_coord_t y_mod)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(base == NULL) base = lv_obj_get_parent(obj);

    LV_ASSERT_OBJ(base, LV_OBJX_NAME);

    lv_point_t new_pos;
    lv_area_align(&base->coords, &obj->coords, align, &new_pos);

    /*Bring together the coordination system of base and obj*/
    lv_obj_t * par        = lv_obj_get_parent(obj);
    lv_coord_t par_abs_x  = par->coords.x1;
    lv_coord_t par_abs_y  = par->coords.y1;
    new_pos.x += x_mod;
    new_pos.y += y_mod;
    new_pos.x -= par_abs_x;
    new_pos.y -= par_abs_y;

    lv_obj_set_pos(obj, new_pos.x, new_pos.y);

#if LV_USE_OBJ_REALIGN
    /*Save the last align parameters to use them in `lv_obj_realign`*/
    obj->realign.align       = align;
    obj->realign.xofs        = x_mod;
    obj->realign.yofs        = y_mod;
    obj->realign.base        = base;
    obj->realign.origo_align = 0;
#endif
}

/**
 * Align an object's middle point to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void lv_obj_align_origo(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_mod, lv_coord_t y_mod)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_coord_t new_x = lv_obj_get_x(obj);
    lv_coord_t new_y = lv_obj_get_y(obj);

    lv_coord_t obj_w_half = lv_obj_get_width(obj) / 2;
    lv_coord_t obj_h_half = lv_obj_get_height(obj) / 2;

    if(base == NULL) {
        base = lv_obj_get_parent(obj);
    }

    LV_ASSERT_OBJ(base, LV_OBJX_NAME);


    switch(align) {
        case LV_ALIGN_CENTER:
            new_x = lv_obj_get_width(base) / 2 - obj_w_half;
            new_y = lv_obj_get_height(base) / 2 - obj_h_half;
            break;

        case LV_ALIGN_IN_TOP_LEFT:
            new_x = -obj_w_half;
            new_y = -obj_h_half;
            break;
        case LV_ALIGN_IN_TOP_MID:
            new_x = lv_obj_get_width(base) / 2 - obj_w_half;
            new_y = -obj_h_half;
            break;

        case LV_ALIGN_IN_TOP_RIGHT:
            new_x = lv_obj_get_width(base) - obj_w_half;
            new_y = -obj_h_half;
            break;

        case LV_ALIGN_IN_BOTTOM_LEFT:
            new_x = -obj_w_half;
            new_y = lv_obj_get_height(base) - obj_h_half;
            break;
        case LV_ALIGN_IN_BOTTOM_MID:
            new_x = lv_obj_get_width(base) / 2 - obj_w_half;
            new_y = lv_obj_get_height(base) - obj_h_half;
            break;

        case LV_ALIGN_IN_BOTTOM_RIGHT:
            new_x = lv_obj_get_width(base) - obj_w_half;
            new_y = lv_obj_get_height(base) - obj_h_half;
            break;

        case LV_ALIGN_IN_LEFT_MID:
            new_x = -obj_w_half;
            new_y = lv_obj_get_height(base) / 2 - obj_h_half;
            break;

        case LV_ALIGN_IN_RIGHT_MID:
            new_x = lv_obj_get_width(base) - obj_w_half;
            new_y = lv_obj_get_height(base) / 2 - obj_h_half;
            break;

        case LV_ALIGN_OUT_TOP_LEFT:
            new_x = -obj_w_half;
            new_y = -obj_h_half;
            break;

        case LV_ALIGN_OUT_TOP_MID:
            new_x = lv_obj_get_width(base) / 2 - obj_w_half;
            new_y = -obj_h_half;
            break;

        case LV_ALIGN_OUT_TOP_RIGHT:
            new_x = lv_obj_get_width(base) - obj_w_half;
            new_y = -obj_h_half;
            break;

        case LV_ALIGN_OUT_BOTTOM_LEFT:
            new_x = -obj_w_half;
            new_y = lv_obj_get_height(base) - obj_h_half;
            break;

        case LV_ALIGN_OUT_BOTTOM_MID:
            new_x = lv_obj_get_width(base) / 2 - obj_w_half;
            new_y = lv_obj_get_height(base) - obj_h_half;
            break;

        case LV_ALIGN_OUT_BOTTOM_RIGHT:
            new_x = lv_obj_get_width(base) - obj_w_half;
            new_y = lv_obj_get_height(base) - obj_h_half;
            break;

        case LV_ALIGN_OUT_LEFT_TOP:
            new_x = -obj_w_half;
            new_y = -obj_h_half;
            break;

        case LV_ALIGN_OUT_LEFT_MID:
            new_x = -obj_w_half;
            new_y = lv_obj_get_height(base) / 2 - obj_h_half;
            break;

        case LV_ALIGN_OUT_LEFT_BOTTOM:
            new_x = -obj_w_half;
            new_y = lv_obj_get_height(base) - obj_h_half;
            break;

        case LV_ALIGN_OUT_RIGHT_TOP:
            new_x = lv_obj_get_width(base) - obj_w_half;
            new_y = -obj_h_half;
            break;

        case LV_ALIGN_OUT_RIGHT_MID:
            new_x = lv_obj_get_width(base) - obj_w_half;
            new_y = lv_obj_get_height(base) / 2 - obj_h_half;
            break;

        case LV_ALIGN_OUT_RIGHT_BOTTOM:
            new_x = lv_obj_get_width(base) - obj_w_half;
            new_y = lv_obj_get_height(base) - obj_h_half;
            break;
    }

    /*Bring together the coordination system of base and obj*/
    lv_obj_t * par        = lv_obj_get_parent(obj);
    lv_coord_t base_abs_x = base->coords.x1;
    lv_coord_t base_abs_y = base->coords.y1;
    lv_coord_t par_abs_x  = par->coords.x1;
    lv_coord_t par_abs_y  = par->coords.y1;
    new_x += x_mod + base_abs_x;
    new_y += y_mod + base_abs_y;
    new_x -= par_abs_x;
    new_y -= par_abs_y;

    lv_obj_set_pos(obj, new_x, new_y);

#if LV_USE_OBJ_REALIGN
    /*Save the last align parameters to use them in `lv_obj_realign`*/
    obj->realign.align       = align;
    obj->realign.xofs        = x_mod;
    obj->realign.yofs        = y_mod;
    obj->realign.base        = base;
    obj->realign.origo_align = 1;
#endif
}

/**
 * Realign the object based on the last `lv_obj_align` parameters.
 * @param obj pointer to an object
 */
void lv_obj_realign(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_OBJ_REALIGN
    if(obj->realign.origo_align)
        lv_obj_align_origo(obj, obj->realign.base, obj->realign.align, obj->realign.xofs, obj->realign.yofs);
    else
        lv_obj_align(obj, obj->realign.base, obj->realign.align, obj->realign.xofs, obj->realign.yofs);
#else
    (void)obj;
    LV_LOG_WARN("lv_obj_realaign: no effect because LV_USE_OBJ_REALIGN = 0");
#endif
}

/**
 * Enable the automatic realign of the object when its size has changed based on the last
 * `lv_obj_align` parameters.
 * @param obj pointer to an object
 * @param en true: enable auto realign; false: disable auto realign
 */
void lv_obj_set_auto_realign(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_OBJ_REALIGN
    obj->realign.auto_realign = en ? 1 : 0;
#else
    (void)obj;
    (void)en;
    LV_LOG_WARN("lv_obj_set_auto_realign: no effect because LV_USE_OBJ_REALIGN = 0");
#endif
}


/**
 * Set the size of an extended clickable area
 * If TINY mode is used, only the largest of the horizontal and vertical padding
 * values are considered.
 * @param obj pointer to an object
 * @param left extended clickable are on the left [px]
 * @param right extended clickable are on the right [px]
 * @param top extended clickable are on the top [px]
 * @param bottom extended clickable are on the bottom [px]
 */
void lv_obj_set_ext_click_area(lv_obj_t * obj, lv_coord_t left, lv_coord_t right, lv_coord_t top, lv_coord_t bottom)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    obj->ext_click_pad.x1 = left;
    obj->ext_click_pad.x2 = right;
    obj->ext_click_pad.y1 = top;
    obj->ext_click_pad.y2 = bottom;
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    obj->ext_click_pad_hor = LV_MATH_MAX(left, right);
    obj->ext_click_pad_ver = LV_MATH_MAX(top, bottom);
#else
    (void)obj;    /*Unused*/
    (void)left;   /*Unused*/
    (void)right;  /*Unused*/
    (void)top;    /*Unused*/
    (void)bottom; /*Unused*/
#endif
}

/*---------------------
 * Appearance set
 *--------------------*/

/**
 * Add a new stye to the style list of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param style pointer to a style to add (Only its pointer will be saved)
 */
void lv_obj_add_style(lv_obj_t * obj, uint8_t part, lv_style_t * style)
{
    if(style == NULL) return;

    lv_style_list_t * style_dsc = lv_obj_get_style_list(obj, part);
    if(style_dsc == NULL) {
        LV_LOG_WARN("lv_obj_add_style: can't find style with `type`");
        return;
    }

    lv_style_list_add_style(style_dsc, style);

    lv_obj_refresh_style(obj);
}

/**
 * Remove all styles from the objects style list. Also reset the local styles
 * @param obj pointer to an object
 * @param part the part of the object which style list should be reseted.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 */
void lv_obj_reset_style_list(lv_obj_t * obj, uint8_t part)
{
    lv_style_list_t * style_dsc = lv_obj_get_style_list(obj, part);
    if(style_dsc == NULL) {
        LV_LOG_WARN("lv_obj_clean_styles: can't find style with `part`");
        return;
    }

    lv_style_list_reset(style_dsc);

    lv_obj_refresh_style(obj);
}

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has integer type
 */
void _lv_obj_set_style_int(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_style_int_t value)
{
    lv_style_list_t * style_dsc = lv_obj_get_style_list(obj, part);
    lv_style_list_set_local_int(style_dsc, prop, value);
    lv_obj_refresh_style(obj);
}

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has color type
 */
void _lv_obj_set_style_color(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_color_t color)
{
    lv_style_list_t * style_dsc = lv_obj_get_style_list(obj, part);
    lv_style_list_set_local_color(style_dsc, prop, color);
    lv_obj_refresh_style(obj);
}

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
void _lv_obj_set_style_opa(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_opa_t opa)
{
    lv_style_list_t * style_dsc = lv_obj_get_style_list(obj, part);
    lv_style_list_set_local_opa(style_dsc, prop, opa);
    lv_obj_refresh_style(obj);
}

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has pointer type
 */
void _lv_obj_set_style_ptr(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, const void * p)
{
    lv_style_list_t * style_dsc = lv_obj_get_style_list(obj, part);
    lv_style_list_set_local_ptr(style_dsc, prop, p);
    lv_obj_refresh_style(obj);
}

/**
 * Notify an object (and its children) about its style is modified
 * @param obj pointer to an object
 */
void lv_obj_refresh_style(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    /*Send style change signals*/
    refresh_children_style(obj);
}

/**
 * Notify all object if a style is modified
 * @param style pointer to a style. Only the objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_obj_report_style_mod(lv_style_t * style)
{
    lv_disp_t * d = lv_disp_get_next(NULL);

    while(d) {
        lv_obj_t * i;
        LV_LL_READ(d->scr_ll, i)
        {
            report_style_mod_core(style, i);
        }
        d = lv_disp_get_next(d);
    }
}

/*-----------------
 * Attribute set
 *----------------*/

/**
 * Hide an object. It won't be visible and clickable.
 * @param obj pointer to an object
 * @param en true: hide the object
 */
void lv_obj_set_hidden(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(!obj->hidden) lv_obj_invalidate(obj); /*Invalidate when not hidden (hidden objects are ignored) */

    obj->hidden = en == false ? 0 : 1;

    if(!obj->hidden) lv_obj_invalidate(obj); /*Invalidate when not hidden (hidden objects are ignored) */

    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par) par->signal_cb(par, LV_SIGNAL_CHILD_CHG, obj);
}

/**
 * Set whether advanced hit-testing is enabled on an object
 * @param obj pointer to an object
 * @param en true: advanced hit-testing is enabled
 */
void lv_obj_set_adv_hittest(lv_obj_t * obj, bool en) {
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->adv_hittest = en == false ? 0 : 1;
}

/**
 * Enable or disable the clicking of an object
 * @param obj pointer to an object
 * @param en true: make the object clickable
 */
void lv_obj_set_click(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->click = (en == true ? 1 : 0);
}

/**
 * Enable to bring this object to the foreground if it
 * or any of its children is clicked
 * @param obj pointer to an object
 * @param en true: enable the auto top feature
 */
void lv_obj_set_top(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->top = (en == true ? 1 : 0);
}

/**
 * Enable the dragging of an object
 * @param obj pointer to an object
 * @param en true: make the object dragable
 */
void lv_obj_set_drag(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(en == true) lv_obj_set_click(obj, true); /*Drag is useless without enabled clicking*/
    obj->drag = (en == true ? 1 : 0);
}

/**
 * Set the directions an object can be dragged in
 * @param obj pointer to an object
 * @param drag_dir bitwise OR of allowed directions an object can be dragged in
 */
void lv_obj_set_drag_dir(lv_obj_t * obj, lv_drag_dir_t drag_dir)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->drag_dir = drag_dir;

    if(obj->drag_dir != 0) lv_obj_set_drag(obj, true); /*Drag direction requires drag*/
}

/**
 * Enable the throwing of an object after is is dragged
 * @param obj pointer to an object
 * @param en true: enable the drag throw
 */
void lv_obj_set_drag_throw(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->drag_throw = (en == true ? 1 : 0);
}

/**
 * Enable to use parent for drag related operations.
 * If trying to drag the object the parent will be moved instead
 * @param obj pointer to an object
 * @param en true: enable the 'drag parent' for the object
 */
void lv_obj_set_drag_parent(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->drag_parent = (en == true ? 1 : 0);
}

/**
* Enable to use parent for gesture related operations.
* If trying to gesture the object the parent will be moved instead
* @param obj pointer to an object
* @param en true: enable the 'gesture parent' for the object
*/
void lv_obj_set_gesture_parent(lv_obj_t * obj, bool en)
{
    obj->gesture_parent = (en == true ? 1 : 0);
}

/**
 * Propagate the events to the parent too
 * @param obj pointer to an object
 * @param en true: enable the event propagation
 */
void lv_obj_set_parent_event(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->parent_event = (en == true ? 1 : 0);
}

/**
 * Set the base direction of the object
 * @param obj pointer to an object
 * @param dir the new base direction. `LV_BIDI_DIR_LTR/RTL/AUTO/INHERIT`
 */
void lv_obj_set_base_dir(lv_obj_t * obj, lv_bidi_dir_t dir)
{
    if(dir != LV_BIDI_DIR_LTR && dir != LV_BIDI_DIR_RTL &&
       dir != LV_BIDI_DIR_AUTO && dir != LV_BIDI_DIR_INHERIT) {

        LV_LOG_WARN("lv_obj_set_base_dir: invalid base dir");
        return;
    }

    obj->base_dir = dir;
    lv_signal_send(obj, LV_SIGNAL_BASE_DIR_CHG, NULL);

    /* Notify the children about the parent base dir has changed.
     * (The children might have `LV_BIDI_DIR_INHERIT`)*/
    base_dir_refr_children(obj);
}

/**
 * Set a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from `lv_protect_t`
 */
void lv_obj_add_protect(lv_obj_t * obj, uint8_t prot)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->protect |= prot;
}

/**
 * Clear a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from `lv_protect_t`
 */
void lv_obj_clear_protect(lv_obj_t * obj, uint8_t prot)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    prot = (~prot) & 0xFF;
    obj->protect &= prot;
}

/**
 * Set the state (fully overwrite) of an object.
 * If specified in the styles a transition animation will be started
 * from the previous state to the current
 * @param obj pointer to an object
 * @param state the new state
 */
void lv_obj_set_state(lv_obj_t * obj, lv_state_t new_state)
{
    if(obj->state_dsc.act == new_state) return;

    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_state_dsc_t dsc_ori = obj->state_dsc;
    obj->state_dsc.act = new_state;
    obj->state_dsc.prev = new_state;
    lv_style_int_t t = lv_obj_get_style_transition_time(obj, LV_OBJ_PART_MAIN);
    obj->state_dsc = dsc_ori;
    if(t == 0) {
        lv_anim_del(obj, obj_state_anim_cb);
        obj->state_dsc.act = new_state;
        obj->state_dsc.prev = new_state;
        obj->state_dsc.anim = 0;
        lv_obj_refresh_style(obj);
    }
    else {

        bool was_anim = lv_anim_del(obj, obj_state_anim_cb);

        if(obj->state_dsc.anim == 0 && was_anim) {
            obj->state_dsc.act = new_state;
        } else {
            obj->state_dsc.prev = obj->state_dsc.act;
            obj->state_dsc.act = new_state;
            obj->state_dsc.anim = 0;
        }

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_exec_cb(&a, obj, obj_state_anim_cb);
        lv_anim_set_values(&a, 0, 255);
        lv_anim_set_time(&a, t, 0);
        lv_anim_create(&a);

    }

}

/**
 * Add a given state or states to the object. The other state bits will remain unchanged.
 * If specified in the styles a transition animation will be started
 * from the previous state to the current
 * @param obj pointer to an object
 * @param state the state bits to add. E.g `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 */
void lv_obj_add_state(lv_obj_t * obj, lv_state_t state)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_state_t new_state = obj->state_dsc.act | state;
    if(obj->state_dsc.act != new_state) {
        lv_obj_set_state(obj, new_state);
    }
}

/**
 * Remove a given state or states to the object. The other state bits will remain unchanged.
 * If specified in the styles a transition animation will be started
 * from the previous state to the current
 * @param obj pointer to an object
 * @param state the state bits to remove. E.g `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 */
void lv_obj_clear_state(lv_obj_t * obj, lv_state_t state)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_state_t new_state = obj->state_dsc.act & (~state);
    if(obj->state_dsc.act != new_state) {
        lv_obj_set_state(obj, new_state);
    }
}
/**
 * Set a an event handler function for an object.
 * Used by the user to react on event which happens with the object.
 * @param obj pointer to an object
 * @param event_cb the new event function
 */
void lv_obj_set_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->event_cb = event_cb;
}

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `lv_event_t`
 * @param data arbitrary data depending on the object type and the event. (Usually `NULL`)
 * @return LV_RES_OK: `obj` was not deleted in the event; LV_RES_INV: `obj` was deleted in the event
 */
lv_res_t lv_event_send(lv_obj_t * obj, lv_event_t event, const void * data)
{
    if(obj == NULL) return LV_RES_OK;

    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_res_t res;
    res = lv_event_send_func(obj->event_cb, obj, event, data);
    return res;
}

/**
 * Call an event function with an object, event, and data.
 * @param event_xcb an event callback function. If `NULL` `LV_RES_OK` will return without any actions.
 *        (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *         the `func_name(object, callback, ...)` convention)
 * @param obj pointer to an object to associate with the event (can be `NULL` to simply call the `event_cb`)
 * @param event an event
 * @param data pointer to a custom data
 * @return LV_RES_OK: `obj` was not deleted in the event; LV_RES_INV: `obj` was deleted in the event
 */
lv_res_t lv_event_send_func(lv_event_cb_t event_xcb, lv_obj_t * obj, lv_event_t event, const void * data)
{
    if(obj != NULL) {
        LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    }

    /* Build a simple linked list from the objects used in the events
     * It's important to know if an this object was deleted by a nested event
     * called from this `even_cb`. */
    lv_event_temp_data_t event_temp_data;
    event_temp_data.obj     = obj;
    event_temp_data.deleted = false;
    event_temp_data.prev    = NULL;

    if(event_temp_data_head) {
        event_temp_data.prev = event_temp_data_head;
    }
    event_temp_data_head = &event_temp_data;

    const void * event_act_data_save = event_act_data;
    event_act_data                   = data;

    /*Call the input device's feedback callback if set*/
    lv_indev_t * indev_act = lv_indev_get_act();
    if(indev_act) {
        if(indev_act->driver.feedback_cb) indev_act->driver.feedback_cb(&indev_act->driver, event);
    }

    /*Call the event callback itself*/
    if(event_xcb) event_xcb(obj, event);

    /*Restore the event data*/
    event_act_data = event_act_data_save;

    /*Remove this element from the list*/
    event_temp_data_head = event_temp_data_head->prev;

    if(event_temp_data.deleted) {
        return LV_RES_INV;
    }

    if(obj) {
        if(obj->parent_event && obj->parent) {
            lv_res_t res = lv_event_send(obj->parent, event, data);
            if(res != LV_RES_OK) {
                return LV_RES_INV;
            }
        }
    }

    return LV_RES_OK;
}

/**
 * Get the `data` parameter of the current event
 * @return the `data` parameter
 */
const void * lv_event_get_data(void)
{
    return event_act_data;
}

/**
 * Set the a signal function of an object. Used internally by the library.
 * Always call the previous signal function in the new.
 * @param obj pointer to an object
 * @param cb the new signal function
 */
void lv_obj_set_signal_cb(lv_obj_t * obj, lv_signal_cb_t signal_cb)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->signal_cb = signal_cb;
}

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `lv_event_t`.
 * @return LV_RES_OK or LV_RES_INV
 */
lv_res_t lv_signal_send(lv_obj_t * obj, lv_signal_t signal, void * param)
{
    if(obj == NULL) return LV_RES_OK;

    lv_res_t res = LV_RES_OK;
    if(obj->signal_cb) res = obj->signal_cb(obj, signal, param);

    return res;
}

/**
 * Set a new design function for an object
 * @param obj pointer to an object
 * @param design_cb the new design function
 */
void lv_obj_set_design_cb(lv_obj_t * obj, lv_design_cb_t design_cb)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->design_cb = design_cb;
}

/*----------------
 * Other set
 *--------------*/

/**
 * Allocate a new ext. data for an object
 * @param obj pointer to an object
 * @param ext_size the size of the new ext. data
 * @return pointer to the allocated ext.
 * If out of memory NULL is returned and the original ext is preserved
 */
void * lv_obj_allocate_ext_attr(lv_obj_t * obj, uint16_t ext_size)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    void * new_ext = lv_mem_realloc(obj->ext_attr, ext_size);
    if(new_ext == NULL) return NULL;

    obj->ext_attr = new_ext;
    return (void *)obj->ext_attr;
}

/**
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object to refresh the extended draw area.
 * he object needs to be invalidated by `lv_obj_invalidate(obj)` manually after this function.
 * @param obj pointer to an object
 */
void lv_obj_refresh_ext_draw_pad(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->ext_draw_pad = 0;
    obj->signal_cb(obj, LV_SIGNAL_REFR_EXT_DRAW_PAD, NULL);

}

/*=======================
 * Getter functions
 *======================*/

/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
lv_obj_t * lv_obj_get_screen(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    const lv_obj_t * par = obj;
    const lv_obj_t * act_p;

    do {
        act_p = par;
        par   = lv_obj_get_parent(act_p);
    } while(par != NULL);

    return (lv_obj_t *)act_p;
}

/**
 * Get the display of an object
 * @param scr pointer to an object
 * @return pointer the object's display
 */
lv_disp_t * lv_obj_get_disp(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    const lv_obj_t * scr;

    if(obj->parent == NULL)
        scr = obj; /*`obj` is a screen*/
    else
        scr = lv_obj_get_screen(obj); /*get the screen of `obj`*/

    lv_disp_t * d;
    LV_LL_READ(LV_GC_ROOT(_lv_disp_ll), d)
    {
        lv_obj_t * s;
        LV_LL_READ(d->scr_ll, s)
        {
            if(s == scr) return d;
        }
    }

    LV_LOG_WARN("lv_scr_get_disp: screen not found")
    return NULL;
}

/*---------------------
 * Parent/children get
 *--------------------*/

/**
 * Returns with the parent of an object
 * @param obj pointer to an object
 * @return pointer to the parent of  'obj'
 */
lv_obj_t * lv_obj_get_parent(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->parent;
}

/**
 * Iterate through the children of an object (start from the "youngest")
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child(const lv_obj_t * obj, const lv_obj_t * child)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * result = NULL;

    if(child == NULL) {
        result = lv_ll_get_head(&obj->child_ll);
    } else {
        result = lv_ll_get_next(&obj->child_ll, child);
    }

    return result;
}

/**
 * Iterate through the children of an object (start from the "oldest")
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child_back(const lv_obj_t * obj, const lv_obj_t * child)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * result = NULL;

    if(child == NULL) {
        result = lv_ll_get_tail(&obj->child_ll);
    } else {
        result = lv_ll_get_prev(&obj->child_ll, child);
    }

    return result;
}

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_count_children(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * i;
    uint16_t cnt = 0;

    LV_LL_READ(obj->child_ll, i) cnt++;

    return cnt;
}

/** Recursively count the children of an object
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_count_children_recursive(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * i;
    uint16_t cnt = 0;

    LV_LL_READ(obj->child_ll, i)
    {
        cnt++;                                     /*Count the child*/
        cnt += lv_obj_count_children_recursive(i); /*recursively count children's children*/
    }

    return cnt;
}

/*---------------------
 * Coordinate get
 *--------------------*/

/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param cords_p pointer to an area to store the coordinates
 */
void lv_obj_get_coords(const lv_obj_t * obj, lv_area_t * cords_p)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_area_copy(cords_p, &obj->coords);
}

/**
 * Reduce area retried by `lv_obj_get_coords()` the get graphically usable area of an object.
 * (Without the size of the border or other extra graphical elements)
 * @param coords_p store the result area here
 */
void lv_obj_get_inner_coords(const lv_obj_t * obj, lv_area_t * coords_p)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_border_side_t part = lv_obj_get_style_border_side(obj, LV_OBJ_PART_MAIN);
    lv_coord_t w = lv_obj_get_style_border_width(obj, LV_OBJ_PART_MAIN);

    if(part & LV_BORDER_SIDE_LEFT) coords_p->x1 += w;

    if(part & LV_BORDER_SIDE_RIGHT) coords_p->x2 -= w;

    if(part & LV_BORDER_SIDE_TOP) coords_p->y1 += w;

    if(part & LV_BORDER_SIDE_BOTTOM) coords_p->y2 -= w;
}

/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent
 */
lv_coord_t lv_obj_get_x(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_coord_t rel_x;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        rel_x             = obj->coords.x1 - parent->coords.x1;
    } else {
        rel_x = obj->coords.x1;
    }
    return rel_x;
}

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent
 */
lv_coord_t lv_obj_get_y(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_coord_t rel_y;
    lv_obj_t * parent = lv_obj_get_parent(obj);
   if(parent) {
       rel_y             = obj->coords.y1 - parent->coords.y1;
   } else {
       rel_y = obj->coords.y1;
   }
    return rel_y;
}

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
lv_coord_t lv_obj_get_width(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return lv_area_get_width(&obj->coords);
}

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
lv_coord_t lv_obj_get_height(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return lv_area_get_height(&obj->coords);
}

/**
 * Get that width reduced by the left and right padding.
 * @param obj pointer to an object
 * @return the width which still fits into the container
 */
lv_coord_t lv_obj_get_width_fit(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_style_int_t left = lv_obj_get_style_pad_left(obj, LV_OBJ_PART_MAIN);
    lv_style_int_t right = lv_obj_get_style_pad_right(obj, LV_OBJ_PART_MAIN);

    return lv_obj_get_width(obj) - left - right;
}

/**
 * Get that height reduced by the top an bottom padding.
 * @param obj pointer to an object
 * @return the height which still fits into the container
 */
lv_coord_t lv_obj_get_height_fit(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_style_int_t top = lv_obj_get_style_pad_top((lv_obj_t *)obj, LV_OBJ_PART_MAIN);
    lv_style_int_t bottom =  lv_obj_get_style_pad_bottom((lv_obj_t *)obj, LV_OBJ_PART_MAIN);

    return lv_obj_get_height(obj) - top - bottom;
}

/**
 * Get the automatic realign property of the object.
 * @param obj pointer to an object
 * @return  true: auto realign is enabled; false: auto realign is disabled
 */
bool lv_obj_get_auto_realign(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_OBJ_REALIGN
    return obj->realign.auto_realign ? true : false;
#else
    (void)obj;
    return false;
#endif
}

/**
 * Get the left padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended left padding
 */
lv_coord_t lv_obj_get_ext_click_pad_left(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    return obj->ext_click_pad_hor;
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    return obj->ext_click_pad.x1;
#else
    (void)obj;    /*Unused*/
    return 0;
#endif
}

/**
 * Get the right padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended right padding
 */
lv_coord_t lv_obj_get_ext_click_pad_right(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    return obj->ext_click_pad_hor;
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    return obj->ext_click_pad.x2;
#else
    (void)obj; /*Unused*/
    return 0;
#endif
}

/**
 * Get the top padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended top padding
 */
lv_coord_t lv_obj_get_ext_click_pad_top(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    return obj->ext_click_pad_ver;
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    return obj->ext_click_pad.y1;
#else
    (void)obj; /*Unused*/
    return 0;
#endif
}

/**
 * Get the bottom padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended bottom padding
 */
lv_coord_t lv_obj_get_ext_click_pad_bottom(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    return obj->ext_click_pad_ver;
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    return obj->ext_click_pad.y2;
#else
    (void)obj; /*Unused*/
    return 0;
#endif
}

/**
 * Get the extended size attribute of an object
 * @param obj pointer to an object
 * @return the extended size attribute
 */
lv_coord_t lv_obj_get_ext_draw_pad(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->ext_draw_pad;
}

/*-----------------
 * Appearance get
 *---------------*/

lv_style_list_t * lv_obj_get_style_list(const lv_obj_t * obj, uint8_t part)
{
	if(part == LV_OBJ_PART_MAIN) return &((lv_obj_t*)obj)->style_list;

	lv_get_style_info_t info;
	info.part = part;
	info.result = NULL;

    lv_res_t res;
    res = lv_signal_send((lv_obj_t*)obj, LV_SIGNAL_GET_STYLE, &info);

    if(res != LV_RES_OK) return NULL;

    return info.result;
}

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_BORDER_WIDTH`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_width()`
 * @note for performance reasons it's not checked if the property really has integer type
 */
lv_style_int_t _lv_obj_get_style_int(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr.full = prop_ori >> 8;

    lv_style_int_t value_act;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_list_t * dsc = lv_obj_get_style_list(parent, part);

        lv_obj_state_dsc_t * state = lv_obj_get_state_dsc(parent, part);
        prop = (uint16_t)prop_ori + ((uint16_t)state->act << LV_STYLE_STATE_POS);

        res = lv_style_list_get_int(dsc, prop, &value_act);
        if(res == LV_RES_OK) {
            if(state->act == state->prev) return value_act;
            else {
                /*Handle transition*/
                lv_style_int_t value_prev;
                prop = (uint16_t)prop_ori + ((uint16_t)state->prev << LV_STYLE_STATE_POS);
                res = lv_style_list_get_int(dsc, prop, &value_prev);
                if(res == LV_RES_INV) value_prev = value_act;

                if(prop == LV_STYLE_RADIUS) {
                    if(value_act == LV_RADIUS_CIRCLE || value_prev == LV_RADIUS_CIRCLE) {
                        lv_coord_t whalf = lv_obj_get_width(obj) / 2;
                        lv_coord_t hhalf = lv_obj_get_width(obj) / 2;

                        if(value_act == LV_RADIUS_CIRCLE) {
                            value_act = LV_MATH_MIN(whalf + 1, hhalf + 1);
                        }

                        if(value_prev == LV_RADIUS_CIRCLE) {
                            value_prev = LV_MATH_MIN(whalf + 1, hhalf + 1);
                        }
                    }
                }

                if(state->anim >= 255) return value_act;
                return value_prev + (((value_act - value_prev) * state->anim) >> 8);

            }
        }

        if(attr.bits.inherit == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    /*Handle unset values*/
    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
    case LV_STYLE_BORDER_SIDE:
        return LV_BORDER_SIDE_FULL;
    case LV_STYLE_SIZE:
        return LV_DPI / 20;
    case LV_STYLE_SCALE_WIDTH:
        return LV_DPI / 8;
    case LV_STYLE_BG_GRAD_STOP:
        return 255;
    }

    return 0;
}

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_BORDER_COLOR`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_color()`
 * @note for performance reasons it's not checked if the property really has color type
 */
lv_color_t _lv_obj_get_style_color(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr.full = prop_ori >> 8;

    lv_color_t value_act;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_list_t * dsc = lv_obj_get_style_list(parent, part);

        lv_obj_state_dsc_t * state = lv_obj_get_state_dsc(parent, part);
        prop = (uint16_t)prop_ori + ((uint16_t)state->act << LV_STYLE_STATE_POS);

        res = lv_style_list_get_color(dsc, prop, &value_act);
        if(res == LV_RES_OK) {
            if(state->act == state->prev) return value_act;
            else {
                /*Handle transition*/
                lv_color_t value_prev;
                prop = (uint16_t)prop_ori + ((uint16_t)state->prev << LV_STYLE_STATE_POS);
                res = lv_style_list_get_color(dsc, prop, &value_prev);
                if(res == LV_RES_INV) value_prev = value_act;
                return lv_color_mix(value_act, value_prev, state->anim);
            }
        }

        if(attr.bits.inherit == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    /*Handle unset values*/
    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
    case LV_STYLE_BG_COLOR:
    case LV_STYLE_BG_GRAD_COLOR:
        return LV_COLOR_WHITE;
    }

    return LV_COLOR_BLACK;
}

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_BORDER_OPA`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
lv_opa_t _lv_obj_get_style_opa(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr.full = prop_ori >> 8;

    lv_opa_t value_act;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_list_t * dsc = lv_obj_get_style_list(parent, part);

        lv_obj_state_dsc_t * state = lv_obj_get_state_dsc(parent, part);
        prop = (uint16_t)prop_ori + ((uint16_t)state->act << LV_STYLE_STATE_POS);

        res = lv_style_list_get_opa(dsc, prop, &value_act);
        if(res == LV_RES_OK) {
            if(state->act == state->prev) return value_act;
            else {
                /*Handle transition*/
                lv_opa_t value_prev;
                prop = (uint16_t)prop_ori + ((uint16_t)state->prev << LV_STYLE_STATE_POS);
                res = lv_style_list_get_opa(dsc, prop, &value_prev);
                if(res == LV_RES_INV) value_prev = value_act;

                if(state->anim >= 255) return value_act;
                return value_prev + (((value_act - value_prev) * state->anim) >> 8);
            }
        }

        if(attr.bits.inherit == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    /*Handle unset values*/
    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
    case LV_STYLE_BG_OPA:
        return LV_OPA_TRANSP;
    }

    return LV_OPA_COVER;
}

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_TEXT_FONT`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has pointer type
 */
const void * _lv_obj_get_style_ptr(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr.full = prop_ori >> 8;

    void * value_act;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_list_t * dsc = lv_obj_get_style_list(parent, part);

        lv_obj_state_dsc_t * state = lv_obj_get_state_dsc(parent, part);
        prop = (uint16_t)prop_ori + ((uint16_t)state->act << LV_STYLE_STATE_POS);

        res = lv_style_list_get_ptr(dsc, prop, &value_act);
        if(res == LV_RES_OK) {
            if(state->act == state->prev) return value_act;
            else {
                /*Handle transition*/
                if(state->anim > 128) return value_act;

                void *  value_prev;
                prop = (uint16_t)prop_ori + ((uint16_t)state->prev << LV_STYLE_STATE_POS);
                res = lv_style_list_get_ptr(dsc, prop, &value_prev);
                if(res == LV_RES_INV) value_prev = value_act;

                return value_prev;
            }
        }

        if(attr.bits.inherit == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    /*Handle unset values*/
    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
    case LV_STYLE_TEXT_FONT:
    case LV_STYLE_VALUE_FONT:
        return LV_FONT_DEFAULT;
    }

    return NULL;
}


/*-----------------
 * Attribute get
 *----------------*/

/**
 * Get the hidden attribute of an object
 * @param obj pointer to an object
 * @return true: the object is hidden
 */
bool lv_obj_get_hidden(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->hidden == 0 ? false : true;
}

/**
 * Get whether advanced hit-testing is enabled on an object
 * @param obj pointer to an object
 * @return true: advanced hit-testing is enabled
 */
bool lv_obj_get_adv_hittest(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->adv_hittest == 0 ? false : true;
}

/**
 * Get the click enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is clickable
 */
bool lv_obj_get_click(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->click == 0 ? false : true;
}

/**
 * Get the top enable attribute of an object
 * @param obj pointer to an object
 * @return true: the auto top feture is enabled
 */
bool lv_obj_get_top(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->top == 0 ? false : true;
}

/**
 * Get the drag enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is dragable
 */
bool lv_obj_get_drag(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->drag == 0 ? false : true;
}

/**
 * Get the directions an object can be dragged
 * @param obj pointer to an object
 * @return bitwise OR of allowed directions an object can be dragged in
 */
lv_drag_dir_t lv_obj_get_drag_dir(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->drag_dir;
}

/**
 * Get the drag throw enable attribute of an object
 * @param obj pointer to an object
 * @return true: drag throw is enabled
 */
bool lv_obj_get_drag_throw(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->drag_throw == 0 ? false : true;
}

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool lv_obj_get_drag_parent(const lv_obj_t * obj)
{
    return obj->drag_parent == 0 ? false : true;
}

/**
* Get the gesture parent attribute of an object
* @param obj pointer to an object
* @return true: gesture parent is enabled
*/
bool lv_obj_get_gesture_parent(const lv_obj_t * obj)
{
    return obj->gesture_parent == 0 ? false : true;
}

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool lv_obj_get_parent_event(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->parent_event == 0 ? false : true;
}


lv_bidi_dir_t lv_obj_get_base_dir(const lv_obj_t * obj)
{
#if LV_USE_BIDI
    const lv_obj_t * parent = obj;

    while(parent) {
        if(parent->base_dir != LV_BIDI_DIR_INHERIT) return parent->base_dir;

        parent = lv_obj_get_parent(parent);
    }

    return LV_BIDI_BASE_DIR_DEF;
#else
    (void) obj;  /*Unused*/
    return LV_BIDI_DIR_LTR;
#endif
}

/**
 * Get the protect field of an object
 * @param obj pointer to an object
 * @return protect field ('OR'ed values of `lv_protect_t`)
 */
uint8_t lv_obj_get_protect(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->protect;
}

/**
 * Check at least one bit of a given protect bitfield is set
 * @param obj pointer to an object
 * @param prot protect bits to test ('OR'ed values of `lv_protect_t`)
 * @return false: none of the given bits are set, true: at least one bit is set
 */
bool lv_obj_is_protected(const lv_obj_t * obj, uint8_t prot)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return (obj->protect & prot) == 0 ? false : true;
}

lv_obj_state_dsc_t * lv_obj_get_state_dsc(const lv_obj_t * obj, uint8_t part)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(part < _LV_OBJ_PART_REAL_LAST) return &((lv_obj_t*)obj)->state_dsc;

    /*If a real part is asked, then use the object's signal to get its state.
     * A real object can be in different state then the main part
     * and only the object itseld knows who to get it's state. */
    lv_get_state_info_t info;
    info.part = part;
    info.result = NULL;
    lv_res_t res;
    res = lv_signal_send((lv_obj_t*)obj, LV_SIGNAL_GET_STATE_DSC, &info);

    if(res != LV_RES_OK) return NULL;

    return info.result;

}

lv_state_t lv_obj_get_state(const lv_obj_t * obj, uint8_t part)
{
    lv_obj_state_dsc_t * state_dsc = lv_obj_get_state_dsc(obj, part);
    return state_dsc->act;
}

/**
 * Get the signal function of an object
 * @param obj pointer to an object
 * @return the signal function
 */
lv_signal_cb_t lv_obj_get_signal_cb(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->signal_cb;
}

/**
 * Get the design function of an object
 * @param obj pointer to an object
 * @return the design function
 */
lv_design_cb_t lv_obj_get_design_cb(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->design_cb;
}

/**
 * Get the event function of an object
 * @param obj pointer to an object
 * @return the event function
 */
lv_event_cb_t lv_obj_get_event_cb(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->event_cb;
}

/*------------------
 * Other get
 *-----------------*/

/**
 * Get the ext pointer
 * @param obj pointer to an object
 * @return the ext pointer but not the dynamic version
 *         Use it as ext->data1, and NOT da(ext)->data1
 */
void * lv_obj_get_ext_attr(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->ext_attr;
}

/**
 * Get object's and its ancestors type. Put their name in `type_buf` starting with the current type.
 * E.g. buf.type[0]="lv_btn", buf.type[1]="lv_cont", buf.type[2]="lv_obj"
 * @param obj pointer to an object which type should be get
 * @param buf pointer to an `lv_obj_type_t` buffer to store the types
 */
void lv_obj_get_type(const lv_obj_t * obj, lv_obj_type_t * buf)
{
    LV_ASSERT_NULL(buf);
    LV_ASSERT_NULL(obj);

    lv_obj_type_t tmp;

    memset(buf, 0, sizeof(lv_obj_type_t));
    memset(&tmp, 0, sizeof(lv_obj_type_t));

    obj->signal_cb((lv_obj_t *)obj, LV_SIGNAL_GET_TYPE, &tmp);

    uint8_t cnt;
    for(cnt = 0; cnt < LV_MAX_ANCESTOR_NUM; cnt++) {
        if(tmp.type[cnt] == NULL) break;
    }

    /*Swap the order. The real type comes first*/
    uint8_t i;
    for(i = 0; i < cnt; i++) {
        buf->type[i] = tmp.type[cnt - 1 - i];
    }
}

#if LV_USE_USER_DATA

/**
 * Get the object's user data
 * @param obj pointer to an object
 * @return user data
 */
lv_obj_user_data_t lv_obj_get_user_data(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->user_data;
}

/**
 * Get a pointer to the object's user data
 * @param obj pointer to an object
 * @return pointer to the user data
 */
lv_obj_user_data_t * lv_obj_get_user_data_ptr(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return (lv_obj_user_data_t *)&obj->user_data;
}

/**
 * Set the object's user data. The data will be copied.
 * @param obj pointer to an object
 * @param data user data
 */
void lv_obj_set_user_data(lv_obj_t * obj, lv_obj_user_data_t data)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    memcpy(&obj->user_data, &data, sizeof(lv_obj_user_data_t));
}
#endif

#if LV_USE_GROUP
/**
 * Get the group of the object
 * @param obj pointer to an object
 * @return the pointer to group of the object
 */
void * lv_obj_get_group(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->group_p;
}

/**
 * Tell whether the object is the focused object of a group or not.
 * @param obj pointer to an object
 * @return true: the object is focused, false: the object is not focused or not in a group
 */
bool lv_obj_is_focused(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(obj->group_p) {
        if(lv_group_get_focused(obj->group_p) == obj) return true;
    }

    return false;
}
#endif


/*-------------------
 * OTHER FUNCTIONS
 *------------------*/

/**
 * Check if a given screen-space point is on an object's coordinates.
 * 
 * This method is intended to be used mainly by advanced hit testing algorithms to check
 * whether the point is even within the object (as an optimization).
 * @param obj object to check
 * @param point screen-space point
 */
bool lv_obj_is_point_on_coords(lv_obj_t * obj, const lv_point_t * point) {
#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    lv_area_t ext_area;
    ext_area.x1 = obj->coords.x1 - obj->ext_click_pad_hor;
    ext_area.x2 = obj->coords.x2 + obj->ext_click_pad_hor;
    ext_area.y1 = obj->coords.y1 - obj->ext_click_pad_ver;
    ext_area.y2 = obj->coords.y2 + obj->ext_click_pad_ver;

    if(!lv_area_is_point_on(&ext_area, point, 0)) {
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    lv_area_t ext_area;
    ext_area.x1 = obj->coords.x1 - obj->ext_click_pad.x1;
    ext_area.x2 = obj->coords.x2 + obj->ext_click_pad.x2;
    ext_area.y1 = obj->coords.y1 - obj->ext_click_pad.y1;
    ext_area.y2 = obj->coords.y2 + obj->ext_click_pad.y2;

    if(!lv_area_is_point_on(&ext_area, point, 0)) {
#else
    if(!lv_area_is_point_on(&obj->coords, point, 0)) {
#endif
        return false;
    }
    return true;
}

/**
 * Hit-test an object given a particular point in screen space.
 * @param obj object to hit-test
 * @param point screen-space point
 * @return true if the object is considered under the point
 */
bool lv_obj_hittest(lv_obj_t * obj, lv_point_t * point) {
    if(obj->adv_hittest) {
        lv_hit_test_info_t hit_info;
        hit_info.point = point;
        hit_info.result = true;
        obj->signal_cb(obj, LV_SIGNAL_HIT_TEST, &hit_info);
        return hit_info.result;
    } else
        return lv_obj_is_point_on_coords(obj, point);
}

/**
 * Used in the signal callback to handle `LV_SIGNAL_GET_TYPE` signal
 * @param obj pointer to an object
 * @param buf pointer to `lv_obj_type_t`. (`param` in the signal callback)
 * @param name name of the object. E.g. "lv_btn". (Only the pointer is saved)
 * @return LV_RES_OK
 */
lv_res_t lv_obj_handle_get_type_signal(lv_obj_type_t * buf, const char * name)
{
    uint8_t i;
    for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
        if(buf->type[i] == NULL) break;
    }
    buf->type[i] = name;

    return LV_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_obj_del_async_cb(void * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_del(obj);
}

/**
 * Initialize a rectangle descriptor from an object's styles
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_OBJ_STYLE_MAIN`, `LV_BTN_STYLE_REL` or `LV_PAGE_STYLE_SCRL`
 * @param draw_dsc the descriptor the initialize
 * @note Only the relevant fields will be set.
 * E.g. if `border width == 0` the other border properties won't be evaluated.
 */
void lv_obj_init_draw_rect_dsc(lv_obj_t * obj, uint8_t part, lv_draw_rect_dsc_t * draw_dsc)
{
    draw_dsc->radius = lv_obj_get_style_radius(obj, part);

    lv_opa_t opa_scale = lv_obj_get_style_opa_scale(obj, part);
    if(opa_scale <= LV_OPA_MIN) {
    	draw_dsc->bg_opa = LV_OPA_TRANSP;
        draw_dsc->border_opa = LV_OPA_TRANSP;
        draw_dsc->shadow_opa = LV_OPA_TRANSP;
        draw_dsc->pattern_opa = LV_OPA_TRANSP;
        draw_dsc->value_opa = LV_OPA_TRANSP;
        return;
    }

    if(draw_dsc->bg_opa != LV_OPA_TRANSP) {
        draw_dsc->bg_opa = lv_obj_get_style_bg_opa(obj, part);
        if(draw_dsc->bg_opa > LV_OPA_MIN) {
            draw_dsc->bg_color = lv_obj_get_style_bg_color(obj, part);
            draw_dsc->bg_grad_dir =  lv_obj_get_style_bg_grad_dir(obj, part);
            if(draw_dsc->bg_grad_dir != LV_GRAD_DIR_NONE) {
                draw_dsc->bg_grad_color = lv_obj_get_style_bg_grad_color(obj, part);
                draw_dsc->bg_main_color_stop =  lv_obj_get_style_bg_main_stop(obj, part);
                draw_dsc->bg_grad_color_stop =  lv_obj_get_style_bg_grad_stop(obj, part);
            }
            draw_dsc->bg_blend_mode = lv_obj_get_style_bg_blend_mode(obj, part);
        }
    }

    if(draw_dsc->border_opa != LV_OPA_TRANSP) {
        draw_dsc->border_width = lv_obj_get_style_border_width(obj, part);
        if(draw_dsc->border_width) {
            draw_dsc->border_opa = lv_obj_get_style_border_opa(obj, part);
            if(draw_dsc->border_opa > LV_OPA_MIN) {
                draw_dsc->border_side = lv_obj_get_style_border_side(obj, part);
                draw_dsc->border_color = lv_obj_get_style_border_color(obj, part);
            }
            draw_dsc->border_blend_mode = lv_obj_get_style_border_blend_mode(obj, part);
        }
    }


    if(draw_dsc->outline_opa != LV_OPA_TRANSP) {
        draw_dsc->outline_width = lv_obj_get_style_outline_width(obj, part);
        if(draw_dsc->outline_width) {
            draw_dsc->outline_opa = lv_obj_get_style_outline_opa(obj, part);
            if(draw_dsc->outline_opa > LV_OPA_MIN) {
                draw_dsc->outline_pad = lv_obj_get_style_outline_pad(obj, part);
                draw_dsc->outline_color = lv_obj_get_style_outline_color(obj, part);
            }
            draw_dsc->outline_blend_mode = lv_obj_get_style_outline_blend_mode(obj, part);
        }
    }

    if(draw_dsc->pattern_opa != LV_OPA_TRANSP) {
        draw_dsc->pattern_image = lv_obj_get_style_pattern_image(obj, part);
        if(draw_dsc->pattern_image) {
            draw_dsc->pattern_opa = lv_obj_get_style_pattern_opa(obj, part);
            if(draw_dsc->pattern_opa > LV_OPA_MIN) {
                draw_dsc->pattern_blend_mode = lv_obj_get_style_pattern_blend_mode(obj, part);
                draw_dsc->pattern_recolor_opa = lv_obj_get_style_pattern_recolor_opa(obj, part);
                draw_dsc->pattern_repeat = lv_obj_get_style_pattern_repeat(obj, part);
                if(lv_img_src_get_type(draw_dsc->pattern_image) == LV_IMG_SRC_SYMBOL) {
                    draw_dsc->pattern_recolor = lv_obj_get_style_pattern_recolor(obj, part);
                    draw_dsc->pattern_font = lv_obj_get_style_text_font(obj, part);
                } else if(draw_dsc->pattern_recolor_opa > LV_OPA_MIN ) {
                    draw_dsc->pattern_recolor = lv_obj_get_style_pattern_recolor(obj, part);
                }
            }
        }
    }

    if(draw_dsc->shadow_opa > LV_OPA_MIN) {
        draw_dsc->shadow_width = lv_obj_get_style_shadow_width(obj, part);
        if(draw_dsc->shadow_width) {
            draw_dsc->shadow_opa = lv_obj_get_style_shadow_opa(obj, part);
            if(draw_dsc->shadow_opa > LV_OPA_MIN) {
                draw_dsc->shadow_ofs_x = lv_obj_get_style_shadow_offset_x(obj, part);
                draw_dsc->shadow_ofs_y = lv_obj_get_style_shadow_offset_y(obj, part);
                draw_dsc->shadow_spread = lv_obj_get_style_shadow_spread(obj, part);
                draw_dsc->shadow_color = lv_obj_get_style_shadow_color(obj, part);
                draw_dsc->shadow_blend_mode = lv_obj_get_style_shadow_blend_mode(obj, part);
            }
        }
    }

    if(draw_dsc->value_opa > LV_OPA_MIN) {
        draw_dsc->value_str = lv_obj_get_style_value_str(obj, part);
        if(draw_dsc->value_str) {
            draw_dsc->value_opa = lv_obj_get_style_value_opa(obj, part);
            if(draw_dsc->value_opa > LV_OPA_MIN) {
                draw_dsc->value_ofs_x = lv_obj_get_style_value_ofs_x(obj, part);
                draw_dsc->value_ofs_y = lv_obj_get_style_value_ofs_y(obj, part);
                draw_dsc->value_color = lv_obj_get_style_value_color(obj, part);
                draw_dsc->value_font = lv_obj_get_style_value_font(obj, part);
                draw_dsc->value_letter_space = lv_obj_get_style_value_letter_space(obj, part);
                draw_dsc->value_line_space = lv_obj_get_style_value_line_space(obj, part);
                draw_dsc->value_align = lv_obj_get_style_value_align(obj, part);
                draw_dsc->value_blend_mode = lv_obj_get_style_value_blend_mode(obj, part);
            }
        }
    }

    if(opa_scale < LV_OPA_MAX) {
        draw_dsc->bg_opa = (uint16_t)((uint16_t)draw_dsc->bg_opa * opa_scale) >> 8;
        draw_dsc->border_opa = (uint16_t)((uint16_t)draw_dsc->border_opa * opa_scale) >> 8;
        draw_dsc->shadow_opa = (uint16_t)((uint16_t)draw_dsc->shadow_opa * opa_scale) >> 8;
        draw_dsc->pattern_opa = (uint16_t)((uint16_t)draw_dsc->pattern_opa * opa_scale) >> 8;
        draw_dsc->value_opa = (uint16_t)((uint16_t)draw_dsc->value_opa * opa_scale) >> 8;
    }
}

void lv_obj_init_draw_label_dsc(lv_obj_t * obj, uint8_t part, lv_draw_label_dsc_t * draw_dsc)
{
    draw_dsc->opa = lv_obj_get_style_text_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN) return;

    lv_opa_t opa_scale = lv_obj_get_style_opa_scale(obj, part);
    if(opa_scale < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa_scale) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN) return;

    draw_dsc->color = lv_obj_get_style_text_color(obj, part);
    draw_dsc->letter_space = lv_obj_get_style_text_letter_space(obj, part);
    draw_dsc->line_space = lv_obj_get_style_text_line_space(obj, part);
    draw_dsc->blend_mode = lv_obj_get_style_text_blend_mode(obj, part);
    draw_dsc->underline = lv_obj_get_style_text_underline(obj, part);
    draw_dsc->strikethrough = lv_obj_get_style_text_strikethrough(obj, part);

    draw_dsc->font = lv_obj_get_style_text_font(obj, part);

    if(draw_dsc->sel_start != LV_DRAW_LABEL_NO_TXT_SEL && draw_dsc->sel_end != LV_DRAW_LABEL_NO_TXT_SEL) {
        draw_dsc->color = lv_obj_get_style_text_sel_color(obj, part);
    }

}

void lv_obj_init_draw_img_dsc(lv_obj_t * obj, uint8_t part, lv_draw_img_dsc_t * draw_dsc)
{
    draw_dsc->opa = lv_obj_get_style_image_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN)  return;
    lv_opa_t opa_scale = lv_obj_get_style_opa_scale(obj, part);
    if(opa_scale < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa_scale) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    draw_dsc->angle = 0;
    draw_dsc->zoom = LV_IMG_ZOOM_NONE;
    draw_dsc->pivot.x = lv_area_get_width(&obj->coords) / 2;
    draw_dsc->pivot.y = lv_area_get_height(&obj->coords) / 2;

    draw_dsc->recolor_opa = lv_obj_get_style_image_recolor_opa(obj, part);
    draw_dsc->recolor = lv_obj_get_style_image_recolor(obj, part);

    draw_dsc->blend_mode = lv_obj_get_style_image_blend_mode(obj, part);
}

void lv_obj_init_draw_line_dsc(lv_obj_t * obj, uint8_t part, lv_draw_line_dsc_t * draw_dsc)
{
    draw_dsc->opa = lv_obj_get_style_line_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN)  return;
    lv_opa_t opa_scale = lv_obj_get_style_opa_scale(obj, part);
    if(opa_scale < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa_scale) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    draw_dsc->width = lv_obj_get_style_line_width(obj, part);
    if(draw_dsc->width == 0) return;

    draw_dsc->color = lv_obj_get_style_line_color(obj, part);
    draw_dsc->blend_mode = lv_obj_get_style_line_blend_mode(obj, part);

    draw_dsc->dash_width = lv_obj_get_style_line_dash_width(obj, part);
    if(draw_dsc->dash_width) {
        draw_dsc->dash_gap = lv_obj_get_style_line_dash_gap(obj, part);
    }

    draw_dsc->round_start = lv_obj_get_style_line_rounded(obj, part);
    draw_dsc->round_end = draw_dsc->round_start;
}

/**
 * Get the required extra size (around the object's part) to draw shadow, outline, value etc.
 * @param obj poinr to an object
 * @param part part of the object
 */
lv_coord_t lv_obj_get_draw_rect_ext_pad_size(lv_obj_t * obj, uint8_t part)
{
    lv_coord_t s = 0;

    lv_opa_t sh_opa = lv_obj_get_style_shadow_opa(obj, part);
    if(sh_opa > LV_OPA_MIN) {
        lv_coord_t sh_width = lv_obj_get_style_shadow_width(obj, part);
        if(sh_width) {
            sh_width = sh_width / 2;    /*THe blur adds only half width*/
            sh_width++;
            sh_width += lv_obj_get_style_shadow_spread(obj, part);
            sh_width += LV_MATH_MAX(LV_MATH_ABS(lv_obj_get_style_shadow_offset_x(obj, part)),
                    LV_MATH_ABS(lv_obj_get_style_shadow_offset_y(obj, part)));
            s = LV_MATH_MAX(s, sh_width);
        }
    }

    lv_opa_t value_opa = lv_obj_get_style_value_opa(obj, part);
    if(value_opa > LV_OPA_MIN) {
        const char * value_str = lv_obj_get_style_value_str(obj, part);
        if(value_str) {
            lv_style_int_t letter_space = lv_obj_get_style_value_letter_space(obj, part);
            lv_style_int_t line_space = lv_obj_get_style_value_letter_space(obj, part);
            const lv_font_t * font = lv_obj_get_style_value_font(obj, part);

            lv_point_t txt_size;
            lv_txt_get_size(&txt_size, value_str, font, letter_space, line_space, LV_COORD_MAX, LV_TXT_FLAG_NONE);

            lv_area_t value_area;
            value_area.x1 = 0;
            value_area.y1 = 0;
            value_area.x2 = txt_size.x - 1;
            value_area.y2 = txt_size.y - 1;

            lv_style_int_t align = lv_obj_get_style_value_align(obj, part);
            lv_style_int_t xofs = lv_obj_get_style_value_ofs_x(obj, part);
            lv_style_int_t yofs = lv_obj_get_style_value_ofs_y(obj, part);
            lv_point_t p_align;
            lv_area_align(&obj->coords, &value_area, align, &p_align);

            value_area.x1 += p_align.x + xofs;
            value_area.y1 += p_align.y + yofs;
            value_area.x2 += p_align.x + xofs;
            value_area.y2 += p_align.y + yofs;

            s = LV_MATH_MAX(s, obj->coords.x1 - value_area.x1);
            s = LV_MATH_MAX(s, obj->coords.y1 - value_area.y1);
            s = LV_MATH_MAX(s, value_area.x2 - obj->coords.x2);
            s = LV_MATH_MAX(s, value_area.y2 - obj->coords.y2);
        }
    }

    lv_opa_t outline_opa = lv_obj_get_style_outline_opa(obj, part);
    if(outline_opa > LV_OPA_MIN) {
        lv_style_int_t outline_width = lv_obj_get_style_outline_width(obj, part);
        if(outline_width) {
            lv_style_int_t outline_pad = lv_obj_get_style_outline_pad(obj, part);
            s = LV_MATH_MAX(s, outline_pad + outline_width);
        }
    }
    return s;
}

/**
 * Handle the drawing related tasks of the base objects.
 * @param obj pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_obj_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        /*Most trivial test. Is the mask fully IN the object? If no it surely doesn't cover it*/
        lv_coord_t r = lv_obj_get_style_radius(obj, LV_OBJ_PART_MAIN);
        if(lv_area_is_in(clip_area, &obj->coords, r) == false) return LV_DESIGN_RES_NOT_COVER;

        if(lv_obj_get_style_clip_corner(obj, LV_OBJ_PART_MAIN)) return LV_DESIGN_RES_MASKED;

        if(lv_obj_get_style_bg_opa(obj, LV_OBJ_PART_MAIN) < LV_OPA_MAX) return LV_DESIGN_RES_NOT_COVER;

        if(lv_obj_get_style_bg_blend_mode(obj, LV_OBJ_PART_MAIN) != LV_BLEND_MODE_NORMAL) return LV_DESIGN_RES_NOT_COVER;
        if(lv_obj_get_style_border_blend_mode(obj, LV_OBJ_PART_MAIN) != LV_BLEND_MODE_NORMAL) return LV_DESIGN_RES_NOT_COVER;
        if(lv_obj_get_style_opa_scale(obj, LV_OBJ_PART_MAIN) < LV_OPA_MAX) return LV_DESIGN_RES_NOT_COVER;

        return  LV_DESIGN_RES_COVER;

    }
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        /*If the border is drawn later disable loading its properties*/
        if(lv_obj_get_style_border_post(obj, LV_OBJ_PART_MAIN)) {
            draw_dsc.border_opa = LV_OPA_TRANSP;
        }

        lv_obj_init_draw_rect_dsc(obj, LV_OBJ_PART_MAIN, &draw_dsc);
        lv_draw_rect(&obj->coords, clip_area, &draw_dsc);

        if(lv_obj_get_style_clip_corner(obj, LV_OBJ_PART_MAIN)) {
            lv_draw_mask_radius_param_t * mp = lv_mem_buf_get(sizeof(lv_draw_mask_radius_param_t));

            lv_coord_t r = lv_obj_get_style_radius(obj, LV_OBJ_PART_MAIN);

            lv_draw_mask_radius_init(mp, &obj->coords, r, false);
            /*Add the mask and use `obj+8` as custom id. Don't use `obj` directly because it might be used by the user*/
            lv_draw_mask_add(mp, obj + 8);
        }
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        if(lv_obj_get_style_clip_corner(obj, LV_OBJ_PART_MAIN)) {
            lv_draw_mask_radius_param_t * param = lv_draw_mask_remove_custom(obj + 8);
            lv_mem_buf_release(param);
        }

        /*If the border is drawn later disable loading other properties*/
        if(lv_obj_get_style_border_post(obj, LV_OBJ_PART_MAIN)) {
            lv_draw_rect_dsc_t draw_dsc;
            lv_draw_rect_dsc_init(&draw_dsc);
            draw_dsc.bg_opa = LV_OPA_TRANSP;
            draw_dsc.pattern_opa = LV_OPA_TRANSP;
            draw_dsc.shadow_opa = LV_OPA_TRANSP;
            lv_obj_init_draw_rect_dsc(obj, LV_OBJ_PART_MAIN, &draw_dsc);
            lv_draw_rect(&obj->coords, clip_area, &draw_dsc);
        }

    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the basic object
 * @param obj pointer to an object
 * @param sign signal type
 * @param param parameter for the signal (depends on signal type)
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_obj_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        if(info->part == LV_OBJ_PART_MAIN) info->result = &obj->style_list;
        else info->result = NULL;
        return LV_RES_OK;
    }
    else if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_res_t res = LV_RES_OK;

    if(sign == LV_SIGNAL_CHILD_CHG) {
        /*Return 'invalid' if the child change signal is not enabled*/
        if(lv_obj_is_protected(obj, LV_PROTECT_CHILD_CHG) != false) res = LV_RES_INV;
    } else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        obj->ext_draw_pad = LV_MATH_MAX(obj->ext_draw_pad, lv_obj_get_draw_rect_ext_pad_size(obj, LV_OBJ_PART_MAIN));
    }
#if LV_USE_OBJ_REALIGN
    else if(sign == LV_SIGNAL_PARENT_SIZE_CHG) {
        if(obj->realign.auto_realign) {
            lv_obj_realign(obj);
        }
    }
#endif
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_obj_refresh_ext_draw_pad(obj);
    }
    else if(sign == LV_SIGNAL_PRESSED) {
        lv_obj_add_state(obj, LV_STATE_PRESSED);
    }
    else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        lv_obj_clear_state(obj, LV_STATE_PRESSED);
    }
#if LV_USE_GROUP
    else if(sign == LV_SIGNAL_FOCUS) {
        if(lv_group_get_editing(lv_obj_get_group(obj))) {
            uint8_t state = LV_STATE_FOCUSED;
            state |= LV_STATE_EDITED;
            lv_obj_add_state(obj, state);
        } else {
            lv_obj_add_state(obj, LV_STATE_FOCUSED);
            lv_obj_clear_state(obj, LV_STATE_EDITED);
        }
    } else if(sign == LV_SIGNAL_DEFOCUS) {
        lv_obj_clear_state(obj, LV_STATE_FOCUSED | LV_STATE_EDITED);
    }
#endif
    else if(sign == LV_SIGNAL_CLEANUP) {

        lv_style_list_reset(&obj->style_list);
    }

    return res;
}

/**
 * Reposition the children of an object. (Called recursively)
 * @param obj pointer to an object which children will be repositioned
 * @param x_diff x coordinate shift
 * @param y_diff y coordinate shift
 */
static void refresh_children_position(lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff)
{
    lv_obj_t * i;
    LV_LL_READ(obj->child_ll, i)
    {
        i->coords.x1 += x_diff;
        i->coords.y1 += y_diff;
        i->coords.x2 += x_diff;
        i->coords.y2 += y_diff;

        refresh_children_position(i, x_diff, y_diff);
    }
}

/**
 * Refresh the style of all children of an object. (Called recursively)
 * @param style refresh objects only with this style_list.
 * @param obj pointer to an object
 */
static void report_style_mod_core(void * style, lv_obj_t * obj)
{
    uint8_t part_sub;
    for(part_sub = 0; part_sub != _LV_OBJ_PART_REAL_LAST; part_sub++) {
        lv_style_list_t * dsc = lv_obj_get_style_list(obj, part_sub);
        if(dsc == NULL) break;

        uint8_t ci;
        for(ci = 0; ci < dsc->style_cnt; ci++) {
            lv_style_t * class = lv_style_list_get_style(dsc, ci);
            if(class == style) {
                lv_obj_refresh_style(obj);
                /*It's enough to handle once (if duplicated)*/
                break;
            }
        }
    }

    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    while(child) {
        report_style_mod_core(style, child);
        child = lv_obj_get_child(obj, child);
    }

}



/**
 * Recursively refresh the style of the children. Go deeper until a not NULL style is found
 * because the NULL styles are inherited from the parent
 * @param obj pointer to an object
 */
static void refresh_children_style(lv_obj_t * obj)
{
    lv_obj_invalidate(obj);
    obj->signal_cb(obj, LV_SIGNAL_STYLE_CHG, NULL);
    lv_obj_invalidate(obj);

    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    while(child != NULL) {
        lv_obj_invalidate(child);
        child->signal_cb(child, LV_SIGNAL_STYLE_CHG, NULL);
        lv_obj_invalidate(child);

        refresh_children_style(child); /*Check children too*/
        child = lv_obj_get_child(obj, child);
    }
}

/**
 * Called by 'lv_obj_del' to delete the children objects
 * @param obj pointer to an object (all of its children will be deleted)
 */
static void delete_children(lv_obj_t * obj)
{
    lv_obj_t * i;
    lv_obj_t * i_next;
    i = lv_ll_get_head(&(obj->child_ll));

    /*Remove from the group; remove before transversing children so that
     * the object still has access to all children during the
     * LV_SIGNAL_DEFOCUS call*/
#if LV_USE_GROUP
    lv_group_t * group = lv_obj_get_group(obj);
    if(group) lv_group_remove_obj(obj);
#endif

    while(i != NULL) {
        /*Get the next object before delete this*/
        i_next = lv_ll_get_next(&(obj->child_ll), i);

        /*Call the recursive del to the child too*/
        delete_children(i);

        /*Set i to the next node*/
        i = i_next;
    }

    /*Let the suer free the resources used in `LV_EVENT_DELETE`*/
    lv_event_send(obj, LV_EVENT_DELETE, NULL);

    lv_event_mark_deleted(obj);

    /*Remove the animations from this object*/
#if LV_USE_ANIMATION
    lv_anim_del(obj, NULL);
#endif

    /* Reset the input devices if
     * the object to delete is used*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(indev->proc.types.pointer.act_obj == obj || indev->proc.types.pointer.last_obj == obj) {
            lv_indev_reset(indev, obj);
        }

        if(indev->proc.types.pointer.last_pressed == obj) {
            indev->proc.types.pointer.last_pressed = NULL;
        }
#if LV_USE_GROUP
        if(indev->group == group && obj == lv_indev_get_obj_act()) {
            lv_indev_reset(indev, obj);
        }
#endif
        indev = lv_indev_get_next(indev);
    }

    /* Clean up the object specific data*/
    obj->signal_cb(obj, LV_SIGNAL_CLEANUP, NULL);

    /*Remove the object from parent's children list*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    lv_ll_remove(&(par->child_ll), obj);

    /*Delete the base objects*/
    if(obj->ext_attr != NULL) lv_mem_free(obj->ext_attr);
    lv_mem_free(obj); /*Free the object itself*/
}

static void base_dir_refr_children(lv_obj_t * obj)
{
    lv_obj_t * child;
    child = lv_obj_get_child(obj, NULL);

    while(child) {
        if(child->base_dir == LV_BIDI_DIR_INHERIT) {
            lv_signal_send(child, LV_SIGNAL_BASE_DIR_CHG, NULL);
            base_dir_refr_children(child);
        }

        child = lv_obj_get_child(obj, child);
    }
}

static void obj_state_anim_cb(void * p, lv_anim_value_t value)
{
    lv_obj_t * obj = p;
    obj->state_dsc.anim = value;
    if(value == 255)  obj->state_dsc.prev = obj->state_dsc.act;

    lv_obj_refresh_style(obj);
}


static void lv_event_mark_deleted(lv_obj_t * obj)
{
    lv_event_temp_data_t * t = event_temp_data_head;

    while(t) {
        if(t->obj == obj) t->deleted = true;
        t = t->prev;
    }
}

