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
#define LV_DRAW_RECT_CACHE_SIZE     8

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_event_temp_data
{
    lv_obj_t * obj;
    bool deleted;
    struct _lv_event_temp_data * prev;
} lv_event_temp_data_t;

typedef struct {
    lv_draw_rect_dsc_t draw_dsc;
    lv_style_dsc_t * style_dsc;
    uint8_t state;
}lv_draw_rect_cache_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void refresh_children_position(lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff);
static void report_style_mod_core(void * style_p, lv_obj_t * obj);
static void refresh_children_style(lv_obj_t * obj);
static void delete_children(lv_obj_t * obj);
static void base_dir_refr_children(lv_obj_t * obj);
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

static lv_draw_rect_cache_t lv_draw_rect_cache[LV_DRAW_RECT_CACHE_SIZE];
static uint16_t lv_draw_rect_cache_last;
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
    lv_style_built_in_init();

#if LV_USE_FILESYSTEM
    lv_fs_init();
#endif

#if LV_USE_ANIMATION
    lv_anim_core_init();
#endif

#if LV_USE_GROUP
    lv_group_init();
#endif

    /*Init. the sstyles*/
//    lv_style_built_in_init();

    lv_theme_t * th = lv_theme_alien_init(0, NULL);
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
    new_obj->click        = parent ? 1 : 0;
    new_obj->drag         = 0;
    new_obj->drag_throw   = 0;
    new_obj->drag_parent  = 0;
    new_obj->drag_dir     = LV_DRAG_DIR_BOTH;
    new_obj->hidden       = 0;
    new_obj->top          = 0;
    new_obj->protect      = LV_PROTECT_NONE;
    new_obj->parent_event = 0;
    new_obj->state = 0;

#if LV_USE_BIDI
    if(parent == NULL) new_obj->base_dir     = LV_BIDI_BASE_DIR_DEF;
    else new_obj->base_dir     = LV_BIDI_DIR_INHERIT;
#else
    new_obj->base_dir     = LV_BIDI_DIR_LTR;
#endif

    new_obj->ext_attr = NULL;


    lv_style_dsc_init(&new_obj->style_dsc);

    if(parent != NULL) {
        lv_obj_add_style_class(new_obj, LV_OBJ_PART_MAIN, _t(PANEL));
    } else {
        lv_obj_add_style_class(new_obj, LV_OBJ_PART_MAIN, _t(SCR));
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
        new_obj->click        = copy->click;
        new_obj->drag         = copy->drag;
        new_obj->drag_dir     = copy->drag_dir;
        new_obj->drag_throw   = copy->drag_throw;
        new_obj->drag_parent  = copy->drag_parent;
        new_obj->hidden       = copy->hidden;
        new_obj->top          = copy->top;
        new_obj->parent_event = copy->parent_event;

        new_obj->protect      = copy->protect;

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
            lv_indev_reset(indev);
        }
        if(indev->proc.types.pointer.last_pressed == obj) {
            indev->proc.types.pointer.last_pressed = NULL;
        }

#if LV_USE_GROUP
        if(indev->group == group && obj == lv_indev_get_obj_act()) {
            lv_indev_reset(indev);
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
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_invalidate(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(lv_obj_get_hidden(obj)) return;

    /*Invalidate the object only if it belongs to the 'LV_GC_ROOT(_lv_act_scr)'*/
    lv_obj_t * obj_scr = lv_obj_get_screen(obj);
    lv_disp_t * disp   = lv_obj_get_disp(obj_scr);
    if(obj_scr == lv_disp_get_scr_act(disp) || obj_scr == lv_disp_get_layer_top(disp) ||
       obj_scr == lv_disp_get_layer_sys(disp)) {
        /*Truncate recursively to the parents*/
        lv_area_t area_trunc;
        lv_obj_t * par = lv_obj_get_parent(obj);
        bool union_ok  = true;
        /*Start with the original coordinates*/
        lv_coord_t ext_size = obj->ext_draw_pad;
        lv_area_copy(&area_trunc, &obj->coords);
        area_trunc.x1 -= ext_size;
        area_trunc.y1 -= ext_size;
        area_trunc.x2 += ext_size;
        area_trunc.y2 += ext_size;

        /*Check through all parents*/
        while(par != NULL) {
            union_ok = lv_area_intersect(&area_trunc, &area_trunc, &par->coords);
            if(union_ok == false) break;       /*If no common parts with parent break;*/
            if(lv_obj_get_hidden(par)) return; /*If the parent is hidden then the child is hidden and won't be drawn*/

            par = lv_obj_get_parent(par);
        }

        if(union_ok) lv_inv_area(disp, &area_trunc);
    }
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
    LV_LL_READ(obj->child_ll, i)
    {
        i->signal_cb(i, LV_SIGNAL_PARENT_SIZE_CHG, NULL);
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

    lv_coord_t new_x = lv_obj_get_x(obj);
    lv_coord_t new_y = lv_obj_get_y(obj);

    if(base == NULL) {
        base = lv_obj_get_parent(obj);
    }

    LV_ASSERT_OBJ(base, LV_OBJX_NAME);


    switch(align) {
        case LV_ALIGN_CENTER:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
            break;

        case LV_ALIGN_IN_TOP_LEFT:
            new_x = 0;
            new_y = 0;
            break;
        case LV_ALIGN_IN_TOP_MID:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = 0;
            break;

        case LV_ALIGN_IN_TOP_RIGHT:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = 0;
            break;

        case LV_ALIGN_IN_BOTTOM_LEFT:
            new_x = 0;
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
            break;
        case LV_ALIGN_IN_BOTTOM_MID:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
            break;

        case LV_ALIGN_IN_BOTTOM_RIGHT:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
            break;

        case LV_ALIGN_IN_LEFT_MID:
            new_x = 0;
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
            break;

        case LV_ALIGN_IN_RIGHT_MID:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
            break;

        case LV_ALIGN_OUT_TOP_LEFT:
            new_x = 0;
            new_y = -lv_obj_get_height(obj);
            break;

        case LV_ALIGN_OUT_TOP_MID:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = -lv_obj_get_height(obj);
            break;

        case LV_ALIGN_OUT_TOP_RIGHT:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = -lv_obj_get_height(obj);
            break;

        case LV_ALIGN_OUT_BOTTOM_LEFT:
            new_x = 0;
            new_y = lv_obj_get_height(base);
            break;

        case LV_ALIGN_OUT_BOTTOM_MID:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = lv_obj_get_height(base);
            break;

        case LV_ALIGN_OUT_BOTTOM_RIGHT:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base);
            break;

        case LV_ALIGN_OUT_LEFT_TOP:
            new_x = -lv_obj_get_width(obj);
            new_y = 0;
            break;

        case LV_ALIGN_OUT_LEFT_MID:
            new_x = -lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
            break;

        case LV_ALIGN_OUT_LEFT_BOTTOM:
            new_x = -lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
            break;

        case LV_ALIGN_OUT_RIGHT_TOP:
            new_x = lv_obj_get_width(base);
            new_y = 0;
            break;

        case LV_ALIGN_OUT_RIGHT_MID:
            new_x = lv_obj_get_width(base);
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
            break;

        case LV_ALIGN_OUT_RIGHT_BOTTOM:
            new_x = lv_obj_get_width(base);
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
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
 * Set a new style for an object
 * @param obj pointer to an object
 * @param style_p pointer to the new style
 */
//void lv_obj_set_style(lv_obj_t * obj, const lv_style_t * style)
//{
//    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
//    if(style) {
//        LV_ASSERT_STYLE(style);
//    }
//
//    obj->style_p = style;
//
//    /*Send a signal about style change to every children with NULL style*/
//    refresh_children_style(obj);
//
//    /*Notify the object about the style change too*/
//    lv_obj_refresh_style(obj);
//}


void lv_obj_set_style_color(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_color_t color)
{
    lv_style_dsc_t * style_dsc = lv_obj_get_style(obj, part);
    lv_style_set_color(&style_dsc->local, prop, color);
    lv_obj_refresh_style(obj);
}

void lv_obj_set_style_value(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_style_int_t value)
{
    lv_style_dsc_t * style_dsc = lv_obj_get_style(obj, part);
    lv_style_set_int(&style_dsc->local, prop, value);
    lv_obj_refresh_style(obj);
}

void lv_obj_set_style_opa(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_opa_t opa)
{
    lv_style_dsc_t * style_dsc = lv_obj_get_style(obj, part);
    lv_style_set_opa(&style_dsc->local, prop, opa);
    lv_obj_refresh_style(obj);
}

void lv_obj_set_style_ptr(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, void * p)
{
    lv_style_dsc_t * style_dsc = lv_obj_get_style(obj, part);
    lv_style_set_ptr(&style_dsc->local, prop, p);
    lv_obj_refresh_style(obj);
}

void lv_obj_add_style_class(lv_obj_t * obj, uint8_t part, lv_style_t * style)
{
    if(style == NULL) return;

    lv_style_dsc_t * style_dsc = lv_obj_get_style(obj, part);
    if(style_dsc == NULL) {
        LV_LOG_WARN("lv_obj_add_style_class: can't find style with `type`");
        return;
    }

    lv_style_dsc_add_class(style_dsc, style);

    lv_obj_refresh_style(obj);
}

void lv_obj_reset_style(lv_obj_t * obj, uint8_t part)
{
    lv_style_dsc_t * style_dsc = lv_obj_get_style(obj, part);
    if(style_dsc == NULL) {
        LV_LOG_WARN("lv_obj_clean_styles: can't find style with `part`");
        return;
    }

    lv_style_dsc_reset(style_dsc);

    lv_obj_refresh_style(obj);
}


/**
 * Notify an object (and its children) about its style is modified
 * @param obj pointer to an object
 */
void lv_obj_refresh_style(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

//    /*Re-cache all children's styles first*/
//    lv_obj_update_style_cache(obj);

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
 * Propagate the events to the parent too
 * @param obj pointer to an object
 * @param en true: enable the event propagation
 */
void lv_obj_set_parent_event(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->parent_event = (en == true ? 1 : 0);
}

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
void lv_obj_set_protect(lv_obj_t * obj, uint8_t prot)
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

void lv_obj_set_state(lv_obj_t * obj, lv_obj_state_t state)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_state_t new_state = obj->state | state;
    if(obj->state != new_state) {
        obj->state = new_state;
        lv_obj_refresh_style(obj);
    }
}

void lv_obj_clear_state(lv_obj_t * obj, lv_obj_state_t state)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    state = (~state) & 0xFF;
    lv_obj_state_t new_state = obj->state & state;
    if(obj->state != new_state) {
        obj->state = new_state;
        lv_obj_refresh_style(obj);
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
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

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
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object
 * @param obj pointer to an object
 */
void lv_obj_refresh_ext_draw_pad(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->ext_draw_pad = 0;
    obj->signal_cb(obj, LV_SIGNAL_REFR_EXT_DRAW_PAD, NULL);

    lv_obj_invalidate(obj);
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

    lv_border_side_t part = lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_BORDER_SIDE);
    lv_coord_t w = lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_BORDER_WIDTH);

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

    lv_style_int_t left = lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_PAD_LEFT);
    lv_style_int_t right = lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_PAD_RIGHT);

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

    lv_style_int_t top = lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_PAD_TOP);
    lv_style_int_t bottom =  lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_PAD_BOTTOM);

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

lv_style_dsc_t * lv_obj_get_style(const lv_obj_t * obj, uint8_t part)
{
	if(part == LV_OBJ_PART_MAIN) return &obj->style_dsc;

	lv_get_style_info_t info;
	info.part = part;
	info.result = NULL;

    lv_res_t res;
    res = lv_signal_send((lv_obj_t*)obj, LV_SIGNAL_GET_STYLE, &info);

    if(res != LV_RES_OK) return NULL;

    return info.result;
}


lv_style_int_t lv_obj_get_style_int(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr.full = prop >> 8;

    lv_style_int_t value;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_dsc_t * dsc = lv_obj_get_style(parent, part);

        uint8_t state = lv_obj_get_state(parent, part);
        prop = (uint16_t)prop_ori + ((uint16_t)state << LV_STYLE_STATE_POS);

        res = lv_style_dsc_get_int(dsc, prop, &value);
        if(res == LV_RES_OK) return value;

        if(attr.bits.inherit == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
    case LV_STYLE_BORDER_SIDE:
        return LV_BORDER_SIDE_FULL;
    case LV_STYLE_SIZE:
        return LV_DPI / 10;
    case LV_STYLE_SCALE_WIDTH:
        return LV_DPI / 8;
    }

    return 0;
}

lv_color_t lv_obj_get_style_color(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr.full = prop >> 8;

    lv_color_t value;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_dsc_t * dsc = lv_obj_get_style(parent, part);

        uint8_t state = lv_obj_get_state(parent, part);
        prop = (uint16_t)prop_ori + ((uint16_t)state << LV_STYLE_STATE_POS);

        res = lv_style_dsc_get_color(dsc, prop, &value);
        if(res == LV_RES_OK) return value;

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
    case LV_STYLE_TEXT_COLOR:
    case LV_STYLE_BORDER_COLOR:
    case LV_STYLE_SHADOW_COLOR:
		return LV_COLOR_BLACK;
	}

    return LV_COLOR_WHITE;
}

lv_opa_t lv_obj_get_style_opa(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr.full = prop >> 8;

    lv_opa_t value;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_dsc_t * dsc = lv_obj_get_style(parent, part);

        uint8_t state = lv_obj_get_state(parent, part);
        prop = (uint16_t)prop_ori + ((uint16_t)state << LV_STYLE_STATE_POS);

        res = lv_style_dsc_get_opa(dsc, prop, &value);
        if(res == LV_RES_OK) return value;

        if(attr.bits.inherit == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
    case LV_STYLE_OPA_SCALE:
    case LV_STYLE_TEXT_OPA:
    case LV_STYLE_IMAGE_OPA:
    case LV_STYLE_LINE_OPA:
    case LV_STYLE_BORDER_OPA:
        return LV_OPA_COVER;
    }

    return LV_OPA_TRANSP;
}


void * lv_obj_get_style_ptr(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr.full = prop >> 8;
    void * value;

    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_dsc_t * dsc = lv_obj_get_style(parent, part);

        uint8_t state = lv_obj_get_state(parent, part);
        prop = (uint16_t)prop_ori + ((uint16_t)state << LV_STYLE_STATE_POS);

        res = lv_style_dsc_get_ptr(dsc, prop, &value);
        if(res == LV_RES_OK) return value;

        if(attr.bits.inherit == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }


    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
    case LV_STYLE_FONT:
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

lv_obj_state_t lv_obj_get_state(const lv_obj_t * obj, uint8_t part)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(part < _LV_OBJ_PART_REAL_LAST) return obj->state;

    /*If a real part is asked, then use the object's signal to get its state.
     * A real object can be in different state then the main part
     * and only the object itseld knows who to get it's state. */
    lv_get_state_info_t info;
    info.part = part;
    info.result = 0;
    lv_res_t res;
    res = lv_signal_send((lv_obj_t*)obj, LV_SIGNAL_GET_STATE, &info);

    if(res != LV_RES_OK) return 0;

    return info.result;

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
#if LV_DRAW_RECT_CACHE_SIZE
    lv_draw_rect_cache_t * cached = NULL;
    lv_style_dsc_t * s = lv_obj_get_style(obj, part);
    lv_obj_state_t state = lv_obj_get_state(obj, part);
    if(lv_refr_get_disp_refreshing() && s) {
        uint32_t i;
        for(i = 0; i < LV_DRAW_RECT_CACHE_SIZE; i++) {
            int32_t c;
            bool err = false;
            if(lv_draw_rect_cache[i].style_dsc == NULL) continue;
            if(lv_draw_rect_cache[i].style_dsc->class_cnt != s->class_cnt) continue;
            if(lv_draw_rect_cache[i].state != state) continue;
            for(c = 0; c < s->class_cnt; c++) {
                    lv_style_t * class1 = lv_style_dsc_get_class(lv_draw_rect_cache[i].style_dsc, c);
                    lv_style_t * class2 = lv_style_dsc_get_class(s, c);
                    if(class1 != class2) {
                        err = true;
                        break;
                    }
            }
            if(err == false) {
                cached = &lv_draw_rect_cache[i];
                break;
            }
        }
    }
    static uint32_t hit = 1;
    static uint32_t miss = 1;
//    printf("hit: %d, miss:%d, pct:%d\n", hit, miss, hit * 100 / (hit+miss));

    if(cached) {
        hit++;
        /*Load the cached data*/
        memcpy(draw_dsc, &cached->draw_dsc, sizeof(lv_draw_rect_dsc_t));

        /*Get inherited properties because they can't be cached*/
        lv_opa_t opa_scale = lv_obj_get_style_opa(obj, part, LV_STYLE_OPA_SCALE);
        if(opa_scale <= LV_OPA_MIN) {
            draw_dsc->bg_opa = LV_OPA_TRANSP;
            draw_dsc->border_opa = LV_OPA_TRANSP;
            draw_dsc->shadow_opa = LV_OPA_TRANSP;
        } else if(opa_scale < LV_OPA_MAX) {
            draw_dsc->bg_opa = (uint16_t)((uint16_t)draw_dsc->bg_opa * opa_scale) >> 8;
            draw_dsc->border_opa = (uint16_t)((uint16_t)draw_dsc->border_opa * opa_scale) >> 8;
            draw_dsc->shadow_opa = (uint16_t)((uint16_t)draw_dsc->shadow_opa * opa_scale) >> 8;
            draw_dsc->pattern_opa = (uint16_t)((uint16_t)draw_dsc->pattern_opa * opa_scale) >> 8;
        }

        draw_dsc->overlay_opa = lv_obj_get_style_opa(obj, part, LV_STYLE_OVERLAY_OPA);
        if(draw_dsc->overlay_opa > LV_OPA_MIN) {
            draw_dsc->overlay_color = lv_obj_get_style_color(obj, part, LV_STYLE_OVERLAY_COLOR);
        }

        return;
    }
    miss++;
#endif

    draw_dsc->radius = lv_obj_get_style_int(obj, part, LV_STYLE_RADIUS);

    lv_opa_t opa_scale = lv_obj_get_style_opa(obj, part, LV_STYLE_OPA_SCALE);
    if(opa_scale <= LV_OPA_MIN) {

#if LV_DRAW_RECT_CACHE_SIZE
    	if(lv_refr_get_disp_refreshing() && s) {
            memcpy(&lv_draw_rect_cache[lv_draw_rect_cache_last].draw_dsc, draw_dsc, sizeof(lv_draw_rect_dsc_t));
            lv_draw_rect_cache[lv_draw_rect_cache_last].style_dsc = s;
            lv_draw_rect_cache[lv_draw_rect_cache_last].state = state;
            lv_draw_rect_cache_last++;
            if(lv_draw_rect_cache_last >= LV_DRAW_RECT_CACHE_SIZE) lv_draw_rect_cache_last = 0;
        }
#endif

    	draw_dsc->bg_opa = LV_OPA_TRANSP;
        draw_dsc->border_opa = LV_OPA_TRANSP;
        draw_dsc->shadow_opa = LV_OPA_TRANSP;
        return;
    }

    draw_dsc->bg_opa = lv_obj_get_style_opa(obj, part, LV_STYLE_BG_OPA);
    if(draw_dsc->bg_opa > LV_OPA_MIN) {
        draw_dsc->bg_color = lv_obj_get_style_color(obj, part, LV_STYLE_BG_COLOR);
        draw_dsc->bg_grad_dir =  lv_obj_get_style_int(obj, part, LV_STYLE_BG_GRAD_DIR);
        if(draw_dsc->bg_grad_dir != LV_GRAD_DIR_NONE) {
            draw_dsc->bg_grad_color = lv_obj_get_style_color(obj, part, LV_STYLE_BG_GRAD_COLOR);
        }
        draw_dsc->bg_blend_mode = lv_obj_get_style_int(obj, part, LV_STYLE_BG_BLEND_MODE);
    }

    draw_dsc->border_opa = lv_obj_get_style_opa(obj, part, LV_STYLE_BORDER_OPA);
    if(draw_dsc->border_opa > LV_OPA_MIN) {
        draw_dsc->border_width = lv_obj_get_style_int(obj, part, LV_STYLE_BORDER_WIDTH);
        if(draw_dsc->border_width) {
            draw_dsc->border_part = lv_obj_get_style_int(obj, part, LV_STYLE_BORDER_SIDE);
            draw_dsc->border_color = lv_obj_get_style_color(obj, part, LV_STYLE_BORDER_COLOR);
        }
        draw_dsc->border_blend_mode = lv_obj_get_style_int(obj, part, LV_STYLE_BORDER_BLEND_MODE);
    }

    draw_dsc->pattern_src = lv_obj_get_style_ptr(obj, part, LV_STYLE_PATTERN_IMAGE);
    if(draw_dsc->pattern_src) {
        draw_dsc->pattern_opa = lv_obj_get_style_opa(obj, part, LV_STYLE_PATTERN_OPA);
        if(draw_dsc->pattern_opa > LV_OPA_MIN) {
            draw_dsc->pattern_repeate = lv_obj_get_style_int(obj, part, LV_STYLE_PATTERN_REPEATE) & 0x1U;
            draw_dsc->pattern_recolor_opa = lv_obj_get_style_opa(obj, part, LV_STYLE_PATTERN_RECOLOR_OPA);
            if(lv_img_src_get_type(draw_dsc->pattern_src) == LV_IMG_SRC_SYMBOL) {
                draw_dsc->pattern_recolor = lv_obj_get_style_color(obj, part, LV_STYLE_PATTERN_RECOLOR);
                draw_dsc->pattern_font = lv_obj_get_style_ptr(obj, part, LV_STYLE_FONT);
            } else if(draw_dsc->pattern_recolor_opa > LV_OPA_MIN ) {
                draw_dsc->pattern_recolor = lv_obj_get_style_color(obj, part, LV_STYLE_PATTERN_RECOLOR);
            }
        }
    }

    draw_dsc->overlay_opa = lv_obj_get_style_opa(obj, part, LV_STYLE_OVERLAY_OPA);
    if(draw_dsc->overlay_opa > LV_OPA_MIN) {
        draw_dsc->overlay_color = lv_obj_get_style_color(obj, part, LV_STYLE_OVERLAY_COLOR);
    }
    draw_dsc->shadow_opa = lv_obj_get_style_opa(obj, part, LV_STYLE_SHADOW_OPA);
    if(draw_dsc->shadow_opa > LV_OPA_MIN) {
        draw_dsc->shadow_width = lv_obj_get_style_int(obj, part, LV_STYLE_SHADOW_WIDTH);
        if(draw_dsc->shadow_width) {
            draw_dsc->shadow_ofs_x = lv_obj_get_style_int(obj, part, LV_STYLE_SHADOW_OFFSET_X);
            draw_dsc->shadow_ofs_y = lv_obj_get_style_int(obj, part, LV_STYLE_SHADOW_OFFSET_Y);
            draw_dsc->shadow_spread = lv_obj_get_style_int(obj, part, LV_STYLE_SHADOW_SPREAD);
            draw_dsc->shadow_color = lv_obj_get_style_color(obj, part, LV_STYLE_SHADOW_COLOR);
        }
    }

#if LV_DRAW_RECT_CACHE_SIZE
    if(lv_refr_get_disp_refreshing() && s) {
        memcpy(&lv_draw_rect_cache[lv_draw_rect_cache_last].draw_dsc, draw_dsc, sizeof(lv_draw_rect_dsc_t));
        lv_draw_rect_cache[lv_draw_rect_cache_last].style_dsc = s;
        lv_draw_rect_cache[lv_draw_rect_cache_last].state = state;
        lv_draw_rect_cache_last++;
        if(lv_draw_rect_cache_last >= LV_DRAW_RECT_CACHE_SIZE) lv_draw_rect_cache_last = 0;
    }
#endif

    if(opa_scale < LV_OPA_MAX) {
        draw_dsc->bg_opa = (uint16_t)((uint16_t)draw_dsc->bg_opa * opa_scale) >> 8;
        draw_dsc->border_opa = (uint16_t)((uint16_t)draw_dsc->border_opa * opa_scale) >> 8;
        draw_dsc->shadow_opa = (uint16_t)((uint16_t)draw_dsc->shadow_opa * opa_scale) >> 8;
        draw_dsc->pattern_opa = (uint16_t)((uint16_t)draw_dsc->pattern_opa * opa_scale) >> 8;
    }

}

void lv_obj_init_draw_label_dsc(lv_obj_t * obj, uint8_t part, lv_draw_label_dsc_t * draw_dsc)
{
    draw_dsc->opa = lv_obj_get_style_opa(obj, part, LV_STYLE_TEXT_OPA);
    if(draw_dsc->opa <= LV_OPA_MIN) return;

    lv_opa_t opa_scale = lv_obj_get_style_opa(obj, part, LV_STYLE_OPA_SCALE);
    if(opa_scale < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa_scale) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN) return;

    draw_dsc->color = lv_obj_get_style_color(obj, part, LV_STYLE_TEXT_COLOR);
    draw_dsc->letter_space = lv_obj_get_style_int(obj, part, LV_STYLE_LETTER_SPACE);
    draw_dsc->line_space = lv_obj_get_style_int(obj, part, LV_STYLE_LETTER_SPACE);

    draw_dsc->font = lv_obj_get_style_ptr(obj, part, LV_STYLE_FONT);

    if(draw_dsc->sel_start != LV_DRAW_LABEL_NO_TXT_SEL && draw_dsc->sel_end != LV_DRAW_LABEL_NO_TXT_SEL) {
        draw_dsc->color = lv_obj_get_style_color(obj, part, LV_STYLE_TEXT_SEL_COLOR);
    }

}

void lv_obj_init_draw_img_dsc(lv_obj_t * obj, uint8_t part, lv_draw_img_dsc_t * draw_dsc)
{
    draw_dsc->opa = lv_obj_get_style_opa(obj, part, LV_STYLE_IMAGE_OPA);
    if(draw_dsc->opa <= LV_OPA_MIN)  return;
    lv_opa_t opa_scale = lv_obj_get_style_opa(obj, part, LV_STYLE_OPA_SCALE);
    if(opa_scale < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa_scale) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    draw_dsc->angle = 0;
    draw_dsc->zoom = LV_IMG_ZOOM_NONE;
    draw_dsc->pivot.x = lv_area_get_width(&obj->coords) / 2;
    draw_dsc->pivot.y = lv_area_get_height(&obj->coords) / 2;

    draw_dsc->recolor_opa = lv_obj_get_style_opa(obj, part, LV_STYLE_IMAGE_RECOLOR_OPA);
    draw_dsc->recolor = lv_obj_get_style_color(obj, part, LV_STYLE_IMAGE_RECOLOR);

    draw_dsc->overlay_opa = lv_obj_get_style_opa(obj, part, LV_STYLE_OVERLAY_OPA);
    if(draw_dsc->overlay_opa > LV_OPA_MIN) {
        draw_dsc->overlay_color = lv_obj_get_style_color(obj, part, LV_STYLE_OVERLAY_COLOR);
    }

    draw_dsc->blend_mode = lv_obj_get_style_int(obj, part, LV_STYLE_IMAGE_BLEND_MODE);
}

void lv_obj_init_draw_line_dsc(lv_obj_t * obj, uint8_t part, lv_draw_line_dsc_t * draw_dsc)
{
    draw_dsc->opa = lv_obj_get_style_opa(obj, part, LV_STYLE_IMAGE_OPA);
    if(draw_dsc->opa <= LV_OPA_MIN)  return;
    lv_opa_t opa_scale = lv_obj_get_style_opa(obj, part, LV_STYLE_OPA_SCALE);
    if(opa_scale < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa_scale) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    draw_dsc->width = lv_obj_get_style_int(obj, part, LV_STYLE_LINE_WIDTH);
    if(draw_dsc->width == 0) return;

    draw_dsc->color = lv_obj_get_style_color(obj, part, LV_STYLE_LINE_COLOR);
    draw_dsc->blend_mode = lv_obj_get_style_int(obj, part, LV_STYLE_LINE_BLEND_MODE);
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
        lv_coord_t r = lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_RADIUS);
        if(lv_area_is_in(clip_area, &obj->coords, r) == false) return LV_DESIGN_RES_NOT_COVER;

        if(lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_CLIP_CORNER)) return LV_DESIGN_RES_MASKED;

        if(lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_BG_BLEND_MODE) != LV_BLEND_MODE_NORMAL) return LV_DESIGN_RES_NOT_COVER;
        if(lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_BORDER_BLEND_MODE) != LV_BLEND_MODE_NORMAL) return LV_DESIGN_RES_NOT_COVER;

        /*Can cover the area only if fully solid (no opacity)*/
        if(lv_obj_get_style_opa(obj, LV_OBJ_PART_MAIN, LV_STYLE_BG_OPA) < LV_OPA_MAX) return LV_DESIGN_RES_NOT_COVER;
        if(lv_obj_get_style_opa(obj, LV_OBJ_PART_MAIN, LV_STYLE_OPA_SCALE) < LV_OPA_MAX) return LV_DESIGN_RES_NOT_COVER;

        return  LV_DESIGN_RES_COVER;

    }
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        lv_obj_init_draw_rect_dsc(obj, LV_OBJ_PART_MAIN, &draw_dsc);
        lv_draw_rect(&obj->coords, clip_area, &draw_dsc);

        if(lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_CLIP_CORNER)) {
            lv_draw_mask_radius_param_t * mp = lv_mem_buf_get(sizeof(lv_draw_mask_radius_param_t));

            lv_coord_t r = lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_RADIUS);

            lv_draw_mask_radius_init(mp, &obj->coords, r, false);
            /*Add the mask and use `obj+8` as custom id. Don't use `obj` directly because it might be used by the user*/
            lv_draw_mask_add(mp, obj + 8);
        }
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        if(lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_CLIP_CORNER)) {
            lv_draw_mask_radius_param_t * param = lv_draw_mask_remove_custom(obj + 8);
            lv_mem_buf_release(param);
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
        if(info->part == LV_OBJ_PART_MAIN) info->result = &obj->style_dsc;
        else info->result = NULL;
        return LV_RES_OK;
    }
    else if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_res_t res = LV_RES_OK;

    if(sign == LV_SIGNAL_CHILD_CHG) {
        /*Return 'invalid' if the child change signal is not enabled*/
        if(lv_obj_is_protected(obj, LV_PROTECT_CHILD_CHG) != false) res = LV_RES_INV;
    } else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        lv_coord_t shadow = (lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_SHADOW_WIDTH) >> 1) + 1;
        shadow += lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_SHADOW_SPREAD);
        shadow += LV_MATH_MAX(LV_MATH_ABS(lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_SHADOW_OFFSET_X)),
                              LV_MATH_ABS(lv_obj_get_style_int(obj, LV_OBJ_PART_MAIN, LV_STYLE_SHADOW_OFFSET_Y)));

        if(shadow > obj->ext_draw_pad) obj->ext_draw_pad = shadow;
    } else if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_obj_refresh_ext_draw_pad(obj);
    }
#if LV_USE_GROUP
    else if(sign == LV_SIGNAL_FOCUS) {
        if(lv_group_get_editing(lv_obj_get_group(obj))) {
            uint8_t state = LV_OBJ_STATE_FOCUS;
            state |= LV_OBJ_STATE_EDIT;
        } else {
            lv_obj_set_state(obj, LV_OBJ_STATE_FOCUS);
            lv_obj_clear_state(obj, LV_OBJ_STATE_EDIT);
        }
    } else if(sign == LV_SIGNAL_DEFOCUS) {
        lv_obj_clear_state(obj, LV_OBJ_STATE_FOCUS | LV_OBJ_STATE_EDIT);
    }
#endif

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
 * @param style refresh objects only with this style_dsc.
 * @param obj pointer to an object
 */
static void report_style_mod_core(void * style, lv_obj_t * obj)
{
    uint8_t part_sub;
    for(part_sub = 0; part_sub != _LV_OBJ_PART_ALL; part_sub++) {
        lv_style_dsc_t * dsc = lv_obj_get_style(obj, part_sub);
        if(dsc == NULL) break;

        if(&dsc->local == style) {
            lv_obj_refresh_style(obj);
            /* Two local style can't have the same pointer
             * so there won't be an other match*/
            return;
        }

        uint8_t ci;
        for(ci = 0; ci < dsc->class_cnt; ci++) {
            lv_style_t * class = lv_style_dsc_get_class(dsc, ci);
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
            lv_indev_reset(indev);
        }

        if(indev->proc.types.pointer.last_pressed == obj) {
            indev->proc.types.pointer.last_pressed = NULL;
        }
#if LV_USE_GROUP
        if(indev->group == group && obj == lv_indev_get_obj_act()) {
            lv_indev_reset(indev);
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


static void lv_event_mark_deleted(lv_obj_t * obj)
{
    lv_event_temp_data_t * t = event_temp_data_head;

    while(t) {
        if(t->obj == obj) t->deleted = true;
        t = t->prev;
    }
}
