/**
 * @file lv_obj.c
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
#include "../lv_misc/lv_debug.h"
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


#if defined(LV_USER_DATA_FREE_INCLUDE)
    #include LV_USER_DATA_FREE_INCLUDE
#endif /* LV_USE_USER_DATA_FREE */

#include LV_THEME_DEFAULT_INCLUDE

#if LV_USE_GPU_STM32_DMA2D
    #include "../lv_gpu/lv_gpu_stm32_dma2d.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_obj"
#define LV_OBJ_DEF_WIDTH    (LV_DPX(100))
#define LV_OBJ_DEF_HEIGHT   (LV_DPX(50))
#define GRID_DEBUG  1 /*Draw rectangles on grid cells*/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_event_temp_data {
    lv_obj_t * obj;
    bool deleted;
    struct _lv_event_temp_data * prev;
} lv_event_temp_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_obj_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_obj_signal(lv_obj_t * obj, lv_signal_t sign, void * param);
static void lv_event_mark_deleted(lv_obj_t * obj);
static bool obj_valid_child(const lv_obj_t * parent, const lv_obj_t * obj_to_find);
static void lv_obj_del_async_cb(void * obj);
static void obj_del_core(lv_obj_t * obj);
static void base_dir_refr_children(lv_obj_t * obj);

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
    _lv_mem_init();
    _lv_task_core_init();

#if LV_USE_FILESYSTEM
    _lv_fs_init();
#endif

#if LV_USE_ANIMATION
    _lv_anim_core_init();
#endif

#if LV_USE_GROUP
    _lv_group_init();
#endif

#if LV_USE_GPU_STM32_DMA2D
    /*Initialize DMA2D GPU*/
    lv_gpu_stm32_dma2d_init();
#endif

    _lv_obj_style_init();

    _lv_ll_init(&LV_GC_ROOT(_lv_disp_ll), sizeof(lv_disp_t));
    _lv_ll_init(&LV_GC_ROOT(_lv_indev_ll), sizeof(lv_indev_t));

    lv_theme_t * th = LV_THEME_DEFAULT_INIT(LV_THEME_DEFAULT_COLOR_PRIMARY, LV_THEME_DEFAULT_COLOR_SECONDARY,
                                            LV_THEME_DEFAULT_FLAG,
                                            LV_THEME_DEFAULT_FONT_SMALL, LV_THEME_DEFAULT_FONT_NORMAL, LV_THEME_DEFAULT_FONT_SUBTITLE, LV_THEME_DEFAULT_FONT_TITLE);
    lv_theme_set_act(th);

    /*Initialize the screen refresh system*/
    _lv_refr_init();

    /*Init the input device handling*/
    _lv_indev_init();

    _lv_img_decoder_init();
    lv_img_cache_set_size(LV_IMG_CACHE_DEF_SIZE);

    /*Test if the IDE has UTF-8 encoding*/
    char * txt = "Á";

    uint8_t * txt_u8 = (uint8_t *) txt;
    if(txt_u8[0] != 0xc3 || txt_u8[1] != 0x81 || txt_u8[2] != 0x00) {
        LV_LOG_WARN("The strings has no UTF-8 encoding. Some characters won't be displayed.")
    }

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
    _lv_gc_clear_roots();

    lv_disp_set_default(NULL);
    _lv_mem_deinit();
    lv_initialized = false;

    LV_LOG_INFO("lv_deinit done");

#if LV_USE_LOG
    lv_log_register_print_cb(NULL);
#endif
}
#endif

/*--------------------
 * Create and delete
 *-------------------*/

/**
 * Create a basic object
 * @param parent pointer to a parent object.
 *                  If NULL then a screen will be created
 *
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other base object to copy.
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

        new_obj = _lv_ll_ins_head(&disp->scr_ll);
        LV_ASSERT_MEM(new_obj);
        if(new_obj == NULL) return NULL;

        _lv_memset_00(new_obj, sizeof(lv_obj_t));

        /*Set the callbacks*/
        new_obj->signal_cb = lv_obj_signal;
        new_obj->design_cb = lv_obj_design;

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
        if(parent->spec_attr == NULL) {
            parent->spec_attr = lv_obj_allocate_rare_attr(parent);
        }
        new_obj = _lv_ll_ins_head(&parent->spec_attr->child_ll);
        LV_ASSERT_MEM(new_obj);
        if(new_obj == NULL) return NULL;

        _lv_memset_00(new_obj, sizeof(lv_obj_t));

        new_obj->parent = parent;

        /*Set the callbacks (signal:cb is required in `lv_obj_get_base_dir` if `LV_USE_ASSERT_OBJ` is enabled)*/
        new_obj->signal_cb = lv_obj_signal;
        new_obj->design_cb = lv_obj_design;

        new_obj->coords.y1    = parent->coords.y1;
        new_obj->coords.y2    = parent->coords.y1 + LV_OBJ_DEF_HEIGHT;
        if(lv_obj_get_base_dir(new_obj) == LV_BIDI_DIR_RTL) {
            new_obj->coords.x2    = parent->coords.x2;
            new_obj->coords.x1    = parent->coords.x2 - LV_OBJ_DEF_WIDTH;
        }
        else {
            new_obj->coords.x1    = parent->coords.x1;
            new_obj->coords.x2    = parent->coords.x1 + LV_OBJ_DEF_WIDTH;
        }
        new_obj->w_set = lv_area_get_width(&new_obj->coords);
        new_obj->h_set = lv_area_get_height(&new_obj->coords);
    }


    new_obj->ext_draw_pad = 0;

    /*Set attributes*/
    new_obj->flags = LV_OBJ_FLAG_CLICKABLE;
    new_obj->flags |= LV_OBJ_FLAG_SNAPABLE;
    new_obj->flags |= LV_OBJ_FLAG_PRESS_LOCK;
    new_obj->flags |= LV_OBJ_FLAG_CLICK_FOCUSABLE;
    new_obj->flags |= LV_OBJ_FLAG_SCROLLABLE;
    new_obj->flags |= LV_OBJ_FLAG_SCROLL_ELASTIC;
    new_obj->flags |= LV_OBJ_FLAG_SCROLL_MOMENTUM;
    if(parent) new_obj->flags |= LV_OBJ_FLAG_GESTURE_BUBBLE;
    new_obj->state = LV_STATE_DEFAULT;

    new_obj->ext_attr = NULL;

    lv_style_list_init(&new_obj->style_list);
    lv_style_list_init(&new_obj->style_list);
     if(copy == NULL) {
         if(parent != NULL) lv_theme_apply(new_obj, LV_THEME_OBJ);
         else  lv_theme_apply(new_obj, LV_THEME_SCR);
     }
     else {
         lv_style_list_copy(&new_obj->style_list, &copy->style_list);
     }
     /*Copy the attributes if required*/
     if(copy != NULL) {
         lv_area_copy(&new_obj->coords, &copy->coords);
         new_obj->ext_draw_pad = copy->ext_draw_pad;

         new_obj->flags  = copy->flags;

 #if LV_USE_GROUP
         /*Add to the same group*/
         if(copy->spec_attr && copy->spec_attr->group_p) {
             lv_group_add_obj(copy->spec_attr->group_p, new_obj);
         }
 #endif

         /*Set the same coordinates for non screen objects*/
         if(lv_obj_get_parent(copy) != NULL && parent != NULL) {
             lv_obj_set_pos(new_obj, lv_obj_get_x(copy), lv_obj_get_y(copy));
         }
     }
    lv_obj_set_pos(new_obj, 0, 0);

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

    lv_disp_t * disp = NULL;
    bool act_scr_del = false;
    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par == NULL) {
        disp = lv_obj_get_disp(obj);
        if(!disp) return LV_RES_INV;   /*Shouldn't happen*/
        if(disp->act_scr == obj) act_scr_del = true;
    }


    obj_del_core(obj);

    /*Send a signal to the parent to notify it about the child delete*/
    if(par) {
        par->signal_cb(par, LV_SIGNAL_CHILD_CHG, NULL);
    }

    /*Handle if the active screen was deleted*/
    if(act_scr_del)  {
        disp->act_scr = NULL;
    }

    return LV_RES_INV;
}

#if LV_USE_ANIMATION
/**
 * A function to be easily used in animation ready callback to delete an object when the animation is ready
 * @param a pointer to the animation
 */
void lv_obj_del_anim_ready_cb(lv_anim_t * a)
{
    lv_obj_del(a->var);
}
#endif

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
    while(child) {
        lv_obj_del(child);
        child = lv_obj_get_child(obj, NULL);    /*Get the new first child*/
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

    lv_area_t area_tmp;
    lv_area_copy(&area_tmp, area);
    bool visible = lv_obj_area_is_visible(obj, &area_tmp);

    if(visible) _lv_inv_area(lv_obj_get_disp(obj), &area_tmp);
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

/**
 * Tell whether an area of an object is visible (even partially) now or not
 * @param obj pointer to an object
 * @param area the are to check. The visible part of the area will be written back here.
 * @return true: visible; false: not visible (hidden, out of parent, on other screen, etc)
 */
bool lv_obj_area_is_visible(const lv_obj_t * obj, lv_area_t * area)
{
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return false;

    /*Invalidate the object only if it belongs to the curent or previous'*/
    lv_obj_t * obj_scr = lv_obj_get_screen(obj);
    lv_disp_t * disp   = lv_obj_get_disp(obj_scr);
    if(obj_scr == lv_disp_get_scr_act(disp) ||
       obj_scr == lv_disp_get_scr_prev(disp) ||
       obj_scr == lv_disp_get_layer_top(disp) ||
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

        is_common = _lv_area_intersect(area, area, &obj_coords);
        if(is_common == false) return false;  /*The area is not on the object*/

        /*Truncate recursively to the parents*/
        lv_obj_t * par = lv_obj_get_parent(obj);
        while(par != NULL) {
            is_common = _lv_area_intersect(area, area, &par->coords);
            if(is_common == false) return false;       /*If no common parts with parent break;*/
            if(lv_obj_has_flag(par, LV_OBJ_FLAG_HIDDEN)) return false; /*If the parent is hidden then the child is hidden and won't be drawn*/

            par = lv_obj_get_parent(par);
        }
    }

    return true;
}

/**
 * Tell whether an object is visible (even partially) now or not
 * @param obj pointer to an object
 * @return true: visible; false: not visible (hidden, out of parent, on other screen, etc)
 */
bool lv_obj_is_visible(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_area_t obj_coords;
    lv_coord_t ext_size = obj->ext_draw_pad;
    lv_area_copy(&obj_coords, &obj->coords);
    obj_coords.x1 -= ext_size;
    obj_coords.y1 -= ext_size;
    obj_coords.x2 += ext_size;
    obj_coords.y2 += ext_size;

    return lv_obj_area_is_visible(obj, &obj_coords);

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

    if(parent->spec_attr == NULL) {
        parent->spec_attr = lv_obj_allocate_rare_attr(parent);
    }

    lv_obj_t * old_par = obj->parent;
    lv_point_t old_pos;
    old_pos.y = lv_obj_get_y(obj);

    lv_bidi_dir_t new_base_dir = lv_obj_get_base_dir(parent);

    if(new_base_dir != LV_BIDI_DIR_RTL) {
        old_pos.x = lv_obj_get_x(obj);
    }
    else {
        old_pos.x = old_par->coords.x2 - obj->coords.x2;
    }

    _lv_ll_chg_list(_lv_obj_get_child_ll(obj->parent), _lv_obj_get_child_ll(parent), obj, true);
    obj->parent = parent;


    if(new_base_dir != LV_BIDI_DIR_RTL) {
        lv_obj_set_pos(obj, old_pos.x, old_pos.y);
    }
    else {
        /*Align to the right in case of RTL base dir*/
        lv_coord_t new_x = lv_obj_get_width(parent) - old_pos.x - lv_obj_get_width(obj);
        lv_obj_set_pos(obj, new_x, old_pos.y);
    }

    /*Notify the original parent because one of its children is lost*/
    old_par->signal_cb(old_par, LV_SIGNAL_CHILD_CHG, obj);

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
    lv_ll_t * ll = _lv_obj_get_child_ll(parent);

    /*Do nothing of already in the foreground*/
    if(_lv_ll_get_head(ll) == obj) return;

    lv_obj_invalidate(parent);

    _lv_ll_chg_list(ll, ll, obj, true);

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
    lv_ll_t * ll = _lv_obj_get_child_ll(parent);

    /*Do nothing of already in the background*/
    if(_lv_ll_get_tail(ll) == obj) return;

    lv_obj_invalidate(parent);

    _lv_ll_chg_list(ll, ll, obj, false);

    /*Notify the new parent about the child*/
    parent->signal_cb(parent, LV_SIGNAL_CHILD_CHG, obj);

    lv_obj_invalidate(parent);
}

/*--------------------
 * Coordinate set
 * ------------------*/



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
    if(obj->spec_attr == NULL) lv_obj_allocate_rare_attr(obj);
    objrare_attr->->ext_click_pad.x1 = left;
    objrare_attr->->ext_click_pad.x2 = right;
    objrare_attr->->ext_click_pad.y1 = top;
    objrare_attr->->ext_click_pad.y2 = bottom;
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    if(obj->spec_attr == NULL) lv_obj_allocate_rare_attr(obj);
    obj->spec_attr->ext_click_pad = LV_MATH_MAX4(left, right, top, bottom);
#else
    LV_UNUSED(obj);
    LV_UNUSED(left);
    LV_UNUSED(right);
    LV_UNUSED(top);
    LV_UNUSED(bottom);
#endif
}

/*---------------------
 * Appearance set
 *--------------------*/

/*-----------------
 * Attribute set
 *----------------*/

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

    lv_obj_allocate_rare_attr(obj);
    obj->spec_attr->base_dir = dir;
    lv_signal_send(obj, LV_SIGNAL_BASE_DIR_CHG, NULL);

    /* Notify the children about the parent base dir has changed.
     * (The children might have `LV_BIDI_DIR_INHERIT`)*/
    base_dir_refr_children(obj);
}


void lv_obj_add_flag(lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->flags |= f;
}



void lv_obj_clear_flag(lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->flags &= (~f);
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
    if(obj->state == new_state) return;

    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    obj->state = new_state;
    _lv_obj_refresh_style(obj, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
#else
    lv_state_t prev_state = obj->state;

    _lv_style_state_cmp_t cmp_res = _lv_obj_style_state_compare(obj, prev_state, new_state);

    obj->state = new_state;

    /*If there is no difference in styles there is nothing else to do*/
    if(cmp_res == _LV_STYLE_STATE_CMP_SAME) return;

    uint8_t part;
    for(part = 0; part < _LV_OBJ_PART_REAL_FIRST; part++) {
        lv_style_list_t * style_list = _lv_obj_get_style_list(obj, part);
        if(style_list == NULL) break;   /*No more style lists*/
        if(style_list->ignore_trans) continue;

        lv_style_int_t time = lv_obj_get_style_transition_time(obj, part);
        lv_style_property_t props[LV_STYLE_TRANS_NUM_MAX];
        lv_style_int_t delay = lv_obj_get_style_transition_delay(obj, part);
        lv_anim_path_t * path = lv_obj_get_style_transition_path(obj, part);
        props[0] = lv_obj_get_style_transition_prop_1(obj, part);
        props[1] = lv_obj_get_style_transition_prop_2(obj, part);
        props[2] = lv_obj_get_style_transition_prop_3(obj, part);
        props[3] = lv_obj_get_style_transition_prop_4(obj, part);
        props[4] = lv_obj_get_style_transition_prop_5(obj, part);
        props[5] = lv_obj_get_style_transition_prop_6(obj, part);

        uint8_t i;
        for(i = 0; i < LV_STYLE_TRANS_NUM_MAX; i++) {
            if(props[i] != 0) {
                _lv_style_list_add_trans_style(style_list);

                _lv_obj_create_style_transition(obj, props[i], part, prev_state, new_state, time, delay, path);

            }
        }
        if(cmp_res == _LV_STYLE_STATE_CMP_VISUAL_DIFF) {
#if LV_STYLE_CACHE_LEVEL > 0
            _lv_obj_invalidate_style_cache(obj, part, LV_STYLE_PROP_ALL);
#endif
        }

    }
    if(cmp_res == _LV_STYLE_STATE_CMP_DIFF) _lv_obj_refresh_style(obj, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    else if(cmp_res == _LV_STYLE_STATE_CMP_VISUAL_DIFF) lv_obj_invalidate(obj);

#endif


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

    lv_state_t new_state = obj->state | state;
    if(obj->state != new_state) {
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

    lv_state_t new_state = obj->state & (~state);
    if(obj->state != new_state) {
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
    if(obj->spec_attr == NULL) obj->spec_attr = lv_obj_allocate_rare_attr(obj);

    obj->spec_attr->event_cb = event_cb;
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
    res = lv_event_send_func(lv_obj_get_event_cb(obj), obj, event, data);
    return res;
}

/**
 * Send LV_EVENT_REFRESH event to an object
 * @param obj point to an obejct. (Can NOT be NULL)
 * @return LV_RES_OK: success, LV_RES_INV: to object become invalid (e.g. deleted) due to this event.
 */
lv_res_t lv_event_send_refresh(lv_obj_t * obj)
{
    return lv_event_send(obj, LV_EVENT_REFRESH, NULL);
}

/**
 * Send LV_EVENT_REFRESH event to an object and all of its children.
 * @param obj pointer to an object or NULL to refresh all objects of all displays
 */
void lv_event_send_refresh_recursive(lv_obj_t * obj)
{
    if(obj == NULL) {
        /*If no obj specified refresh all screen of all displays */
        lv_disp_t * d = lv_disp_get_next(NULL);
        while(d) {
            lv_obj_t * scr = _lv_ll_get_head(&d->scr_ll);
            while(scr) {
                lv_event_send_refresh_recursive(scr);
                scr = _lv_ll_get_next(&d->scr_ll, scr);
            }
            lv_event_send_refresh_recursive(d->top_layer);
            lv_event_send_refresh_recursive(d->sys_layer);

            d = lv_disp_get_next(d);
        }
    }
    else {

        lv_res_t res = lv_event_send_refresh(obj);
        if(res != LV_RES_OK) return; /*If invalid returned do not check the children*/

        lv_obj_t * child;
        lv_ll_t * ll = _lv_obj_get_child_ll(obj);
        _LV_LL_READ(ll, child) {
            lv_event_send_refresh_recursive(child);
        }
    }
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
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE) && obj->parent) {

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
    LV_ASSERT_MEM(new_ext);
    if(new_ext == NULL) return NULL;

    obj->ext_attr = new_ext;
    return (void *)obj->ext_attr;
}

/**
 * Allocate a new ext. data for an object
 * @param obj pointer to an object
 * @param ext_size the size of the new ext. data
 * @return pointer to the allocated ext.
 * If out of memory NULL is returned and the original ext is preserved
 */
lv_obj_spec_attr_t * lv_obj_allocate_rare_attr(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);


    if(obj->spec_attr == NULL) {
        static uint32_t x = 0;
        x++;
        obj->spec_attr = lv_mem_alloc(sizeof(lv_obj_spec_attr_t));
        LV_ASSERT_MEM(obj->spec_attr);
        if(obj->spec_attr == NULL) return NULL;

        _lv_memset_00(obj->spec_attr, sizeof(lv_obj_spec_attr_t));
        _lv_ll_init(&(obj->spec_attr->child_ll), sizeof(lv_obj_t));

        obj->spec_attr->scroll_dir = LV_DIR_ALL;
        obj->spec_attr->base_dir = LV_BIDI_DIR_INHERIT;
        obj->spec_attr->scroll_mode = LV_SCROLL_MODE_AUTO;

    }
    return obj->spec_attr;
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
    _LV_LL_READ(&LV_GC_ROOT(_lv_disp_ll), d) {
        lv_obj_t * s;
        _LV_LL_READ(&d->scr_ll, s) {
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

    lv_ll_t * ll = _lv_obj_get_child_ll(obj);
    if (child) return _lv_ll_get_next(ll, child);
    else return _lv_ll_get_head(ll);
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

    lv_ll_t * ll = _lv_obj_get_child_ll(obj);
    if(child) return _lv_ll_get_prev(ll, child);
    else return _lv_ll_get_tail(ll);
}

/**
 * Get the Nth child of an object. 0th is the lastly created.
 * @param obj pointer to an object whose children should be get
 * @param id of a child
 * @return the child or `NULL` if `id` was greater then the `number of children - 1`
 */
lv_obj_t * lv_obj_get_child_by_id(const lv_obj_t * obj, uint32_t id)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * child = lv_obj_get_child_back(obj, NULL);
    uint32_t i;
    for(i = 0; i < id; i++) {
        child = lv_obj_get_child_back(obj, child);
    }

    return child;
}

/**
 * Get the child index of an object.
 * If this object is the firstly created child of its parent 0 will be return.
 * If its the second child return 1, etc.
 * @param obj pointer to an object whose index should be get
 * @return the child index of the object.
 */
uint32_t lv_obj_get_child_id(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent == NULL) return 0;

    uint32_t id = 0;
    lv_obj_t * child;

    lv_ll_t * ll = _lv_obj_get_child_ll(obj);
    _LV_LL_READ_BACK(ll, child) {
        if(child == obj) return id;
        id++;
    }
    return id;
}

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint32_t lv_obj_count_children(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * i;
    uint16_t cnt = 0;

    lv_ll_t * ll = _lv_obj_get_child_ll(obj);
    _LV_LL_READ(ll, i) cnt++;

    return cnt;
}

/** Recursively count the children of an object
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint32_t lv_obj_count_children_recursive(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_t * i;
    uint16_t cnt = 0;

    lv_ll_t * ll = _lv_obj_get_child_ll(obj);
    _LV_LL_READ(ll, i) {
        cnt++;                                     /*Count the child*/
        cnt += lv_obj_count_children_recursive(i); /*recursively count children's children*/
    }

    return cnt;
}

/*---------------------
 * Coordinate get
 *--------------------*/

/**
 * Get the extended extended clickable area in a direction
 * @param obj pointer to an object
 * @param dir in which direction get the extended area (`LV_DIR_LEFT/RIGHT/TOP`)
 * @return the extended left padding
 */
lv_coord_t lv_obj_get_ext_click_area(const lv_obj_t * obj, lv_dir_t dir)
{

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_OFF
    LV_UNUSED(obj);
    LV_UNUSED(dir);
    return 0;
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    LV_UNUSED(dir);
    if(obj->spec_attr) return obj->spec_attr->ext_click_pad;
    else return 0;
#else
    switch(dir) {
    case LV_DIR_LEFT:
        return obj->ext_click_pad.x1;
    case LV_DIR_RIGHT:
        return obj->ext_click_pad.x2;
    case LV_DIR_TOP:
        return obj->ext_click_pad.y1;
    case LV_DIR_BOTTOM:
        return obj->ext_click_pad.y2;
    default:
        return 0;
    }
#endif
}


/**
 * Check if a given screen-space point is on an object's coordinates.
 * This method is intended to be used mainly by advanced hit testing algorithms to check
 * whether the point is even within the object (as an optimization).
 * @param obj object to check
 * @param point screen-space point
 */
bool _lv_obj_is_click_point_on(lv_obj_t * obj, const lv_point_t * point)
{
#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_OFF
    return _lv_area_is_point_on(&obj->coords, point, 0);
#else
    lv_area_t ext_area;
    ext_area.x1 = obj->coords.x1 - lv_obj_get_ext_click_area(obj, LV_DIR_LEFT);
    ext_area.x2 = obj->coords.x2 + lv_obj_get_ext_click_area(obj, LV_DIR_RIGHT);
    ext_area.y1 = obj->coords.y1 - lv_obj_get_ext_click_area(obj, LV_DIR_TOP);
    ext_area.y2 = obj->coords.y2 + lv_obj_get_ext_click_area(obj, LV_DIR_BOTTOM);

    return _lv_area_is_point_on(&ext_area, point, 0);
#endif
}

/**
 * Hit-test an object given a particular point in screen space.
 * @param obj object to hit-test
 * @param point screen-space point
 * @return true if the object is considered under the point
 */
bool lv_obj_hit_test(lv_obj_t * obj, lv_point_t * point)
{
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_ADV_HITTEST)) {
        lv_hit_test_info_t hit_info;
        hit_info.point = point;
        hit_info.result = true;
        obj->signal_cb(obj, LV_SIGNAL_HIT_TEST, &hit_info);
        return hit_info.result;
    }
    else {
        return _lv_obj_is_click_point_on(obj, point);
    }
}
/*-----------------
 * Appearance get
 *---------------*/


/*-----------------
 * Attribute get
 *----------------*/

bool lv_obj_has_flag(const lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->flags & f ? true : false;
}

lv_bidi_dir_t lv_obj_get_base_dir(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(obj->spec_attr == NULL) return LV_BIDI_DIR_LTR;
#if LV_USE_BIDI
    const lv_obj_t * parent = obj;

    while(parent) {
        /*If the base dir set use it. If not set assume INHERIT so got the next parent*/
        if(parent->spec_attr) {
            if(parent->spec_attr->base_dir != LV_BIDI_DIR_INHERIT) return parent->spec_attr->base_dir;
        }

        parent = lv_obj_get_parent(parent);
    }

    return LV_BIDI_BASE_DIR_DEF;
#else
    (void) obj;  /*Unused*/
    return LV_BIDI_DIR_LTR;
#endif
}

lv_state_t lv_obj_get_state(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return ((lv_obj_t *)obj)->state;
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

    if(obj->spec_attr) return obj->spec_attr->event_cb;
    else return NULL;
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

    _lv_memset_00(buf, sizeof(lv_obj_type_t));
    _lv_memset_00(&tmp, sizeof(lv_obj_type_t));

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
    static lv_obj_user_data_t empty = {0};
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    if(obj->spec_attr) return obj->spec_attr->user_data;
    else return empty;
}

/**
 * Get a pointer to the object's user data
 * @param obj pointer to an object
 * @return pointer to the user data
 */
lv_obj_user_data_t * lv_obj_get_user_data_ptr(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(obj->spec_attr) return obj->spec_attr->user_data;
    return &obj->spec_attr->user_data;
}

/**
 * Set the object's user data. The data will be copied.
 * @param obj pointer to an object
 * @param data user data
 */
void lv_obj_set_user_data(lv_obj_t * obj, lv_obj_user_data_t data)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(obj->spec_attr == NULL) lv_obj_allocate_rare_attr(obj);
    _lv_memcpy(&obj->spec_attr->user_data, &data, sizeof(lv_obj_user_data_t));
}
#endif

/**
 * Get the group of the object
 * @param obj pointer to an object
 * @return the pointer to group of the object
 */
void * lv_obj_get_group(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_GROUP
    if(obj->spec_attr) return obj->spec_attr->group_p;
    else return NULL;
#else
    LV_UNUSED(obj);
    return NULL;
#endif
}

/**
 * Tell whether the object is the focused object of a group or not.
 * @param obj pointer to an object
 * @return true: the object is focused, false: the object is not focused or not in a group
 */
bool lv_obj_is_focused(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_GROUP
    lv_group_t * g = lv_obj_get_group(obj);
    if(g) {
        if(lv_group_get_focused(g) == obj) return true;
    }
    return false;
#else
    LV_UNUSED(obj);
    return false;
#endif
}

/**
 * Tell if an object is an instance of a certain widget type or not
 * @param obj pointer to an object
 * @param type_str the type to check. The name of the widget's type, g.g. "lv_label", "lv_btn", etc
 * @return true: `obj` has the given type
 * @note Not only the "final" type matters. Therefore every widget has "lv_obj" type and "lv_slider" is an "lv_bar" too.
 */
bool lv_obj_is_instance_of(lv_obj_t * obj, const char * type_str)
{
    lv_obj_type_t type;
    lv_obj_get_type(obj, &type);
    uint8_t cnt;
    for(cnt = 0; cnt < LV_MAX_ANCESTOR_NUM; cnt++) {
        if(type.type[cnt] == NULL) break;
        if(!strcmp(type.type[cnt], type_str)) return true;
    }
    return false;
}

lv_ll_t * _lv_obj_get_child_ll(const lv_obj_t * obj)
{
    if(obj->spec_attr) return &obj->spec_attr->child_ll;
    else return NULL;
}

/*-------------------
 * OTHER FUNCTIONS
 *------------------*/


/**
 * Get the really focused object by taking `focus_parent` into account.
 * @param obj the start object
 * @return the object to really focus
 */
lv_obj_t * _lv_obj_get_focused_obj(const lv_obj_t * obj)
{
    if(obj == NULL) return NULL;
    const lv_obj_t * focus_obj = obj;
    while(lv_obj_has_flag(focus_obj, LV_OBJ_FLAG_FOCUS_BUBBLE) != false && focus_obj != NULL) {
        focus_obj = lv_obj_get_parent(focus_obj);
    }

    return (lv_obj_t *)focus_obj;
}

/**
 * Used in the signal callback to handle `LV_SIGNAL_GET_TYPE` signal
 * @param obj pointer to an object
 * @param buf pointer to `lv_obj_type_t`. (`param` in the signal callback)
 * @param name name of the object. E.g. "lv_btn". (Only the pointer is saved)
 * @return LV_RES_OK
 */
lv_res_t _lv_obj_handle_get_type_signal(lv_obj_type_t * buf, const char * name)
{
    uint8_t i;
    for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
        if(buf->type[i] == NULL) break;
    }
    buf->type[i] = name;

    return LV_RES_OK;
}


/**
 * Check if any object has a given type
 * @param obj pointer to an object
 * @param obj_type type of the object. (e.g. "lv_btn")
 * @return true: valid
 */
bool _lv_debug_check_obj_type(const lv_obj_t * obj, const char * obj_type)
{
    if(obj_type[0] == '\0') return true;

    lv_obj_type_t types;
    lv_obj_get_type((lv_obj_t *)obj, &types);

    uint8_t i;
    for(i = 0; i < LV_MAX_ANCESTOR_NUM; i++) {
        if(types.type[i] == NULL) break;
        if(strcmp(types.type[i], obj_type) == 0) return true;
    }

    return false;
}

/**
 * Check if any object is still "alive", and part of the hierarchy
 * @param obj pointer to an object
 * @param obj_type type of the object. (e.g. "lv_btn")
 * @return true: valid
 */
bool _lv_debug_check_obj_valid(const lv_obj_t * obj)
{
    lv_disp_t * disp = lv_disp_get_next(NULL);
    while(disp) {
        lv_obj_t * scr;
        _LV_LL_READ(&disp->scr_ll, scr) {

            if(scr == obj) return true;
            bool found = obj_valid_child(scr, obj);
            if(found) return true;
        }

        disp = lv_disp_get_next(disp);
    }

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_obj_del_async_cb(void * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_del(obj);
}

static void obj_del_core(lv_obj_t * obj)
{
    /*Let the user free the resources used in `LV_EVENT_DELETE`*/
    lv_res_t res = lv_event_send(obj, LV_EVENT_DELETE, NULL);
    if(res == LV_RES_INV) return;

    /*Delete from the group*/
#if LV_USE_GROUP
    lv_group_t * group = lv_obj_get_group(obj);
    if(group) lv_group_remove_obj(obj);
#endif

    /*Remove the animations from this object*/
#if LV_USE_ANIMATION
    lv_anim_del(obj, NULL);
    _lv_obj_remove_style_trans(obj);
#endif

    /*Delete the user data*/
#if LV_USE_USER_DATA
#if LV_USE_USER_DATA_FREE
    LV_USER_DATA_FREE(obj);
#endif
#endif

    /*Recursively delete the children*/
    lv_ll_t * ll = _lv_obj_get_child_ll(obj);
    lv_obj_t * i;
    i = _lv_ll_get_head(ll);
    while(i != NULL) {
        /*Call the recursive delete to the child too*/
        obj_del_core(i);

        /*Set i to the new head node*/
        i = _lv_ll_get_head(ll);
    }

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
        _lv_ll_remove(&d->scr_ll, obj);
    }
    else {
        _lv_ll_remove(_lv_obj_get_child_ll(par), obj);
    }

    /*Delete the base objects*/
    if(obj->ext_attr != NULL) lv_mem_free(obj->ext_attr);
    lv_mem_free(obj); /*Free the object itself*/
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
        if(lv_obj_get_style_clip_corner(obj, LV_OBJ_PART_MAIN)) return LV_DESIGN_RES_MASKED;

        /*Most trivial test. Is the mask fully IN the object? If no it surely doesn't cover it*/
        lv_coord_t r = lv_obj_get_style_radius(obj, LV_OBJ_PART_MAIN);
        lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_OBJ_PART_MAIN);
        lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_OBJ_PART_MAIN);
        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.x1 -= w;
        coords.x2 += w;
        coords.y1 -= h;
        coords.y2 += h;

        if(_lv_area_is_in(clip_area, &coords, r) == false) return LV_DESIGN_RES_NOT_COVER;

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
            draw_dsc.border_post = 1;
        }

        lv_obj_init_draw_rect_dsc(obj, LV_OBJ_PART_MAIN, &draw_dsc);

        lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_OBJ_PART_MAIN);
        lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_OBJ_PART_MAIN);
        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.x1 -= w;
        coords.x2 += w;
        coords.y1 -= h;
        coords.y2 += h;

        lv_draw_rect(&coords, clip_area, &draw_dsc);

        if(lv_obj_get_style_clip_corner(obj, LV_OBJ_PART_MAIN)) {
            lv_draw_mask_radius_param_t * mp = _lv_mem_buf_get(sizeof(lv_draw_mask_radius_param_t));

            lv_coord_t r = lv_obj_get_style_radius(obj, LV_OBJ_PART_MAIN);

            lv_draw_mask_radius_init(mp, &obj->coords, r, false);
            /*Add the mask and use `obj+8` as custom id. Don't use `obj` directly because it might be used by the user*/
            lv_draw_mask_add(mp, obj + 8);
        }
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        _lv_obj_draw_scrollbar(obj, clip_area);

        if(lv_obj_get_style_clip_corner(obj, LV_OBJ_PART_MAIN)) {
            lv_draw_mask_radius_param_t * param = lv_draw_mask_remove_custom(obj + 8);
            _lv_mem_buf_release(param);
        }

        /*If the border is drawn later disable loading other properties*/
        if(lv_obj_get_style_border_post(obj, LV_OBJ_PART_MAIN)) {
            lv_draw_rect_dsc_t draw_dsc;
            lv_draw_rect_dsc_init(&draw_dsc);
            draw_dsc.bg_opa = LV_OPA_TRANSP;
            draw_dsc.pattern_opa = LV_OPA_TRANSP;
            draw_dsc.shadow_opa = LV_OPA_TRANSP;
            draw_dsc.value_opa = LV_OPA_TRANSP;
            lv_obj_init_draw_rect_dsc(obj, LV_OBJ_PART_MAIN, &draw_dsc);

            lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_OBJ_PART_MAIN);
            lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_OBJ_PART_MAIN);
            lv_area_t coords;
            lv_area_copy(&coords, &obj->coords);
            coords.x1 -= w;
            coords.x2 += w;
            coords.y1 -= h;
            coords.y2 += h;
            lv_draw_rect(&coords, clip_area, &draw_dsc);
        }

#if GRID_DEBUG
        /*Draw the grid cells*/
        if(lv_obj_get_grid(obj)) {
            _lv_grid_calc_t calc;
            _lv_grid_calc(obj, &calc);

            /*Create a color unique to this object. */
            lv_color_t c = lv_color_hex(((lv_uintptr_t) obj) & 0xFFFFFF);

            lv_draw_rect_dsc_t grid_rect_dsc;
            lv_draw_rect_dsc_init(&grid_rect_dsc);
            grid_rect_dsc.bg_color = c;
            grid_rect_dsc.bg_opa = LV_OPA_20;
            grid_rect_dsc.border_width = 2;
            grid_rect_dsc.border_color = c;
            grid_rect_dsc.border_opa = LV_OPA_70;

            lv_point_t grid_abs;
            lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_OBJ_PART_MAIN);
            lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_OBJ_PART_MAIN);
            grid_abs.x = pad_left + obj->coords.x1 - lv_obj_get_scroll_left(obj);
            grid_abs.y = pad_top + obj->coords.y1 - lv_obj_get_scroll_top(obj);

            uint32_t row;
            uint32_t col;
            for(row = 0; row < calc.row_num; row ++) {
                for(col = 0; col < calc.col_num; col ++) {
                    lv_area_t a;
                    a.x1 = grid_abs.x + calc.x[col];
                    a.x2 = a.x1 + calc.w[col];
                    a.y1 = grid_abs.y + calc.y[row];
                    a.y2 = a.y1 + calc.h[row];
                    lv_draw_rect(&a, clip_area, &grid_rect_dsc);
                }
            }


            _lv_grid_calc_free(&calc);
        }
#endif
    }

    return LV_DESIGN_RES_OK;
}

static void base_dir_refr_children(lv_obj_t * obj)
{
    lv_obj_t * child;
    _LV_LL_READ(_lv_obj_get_child_ll(obj), child) {
        if(lv_obj_get_base_dir(child) == LV_BIDI_DIR_INHERIT) {
            lv_signal_send(child, LV_SIGNAL_BASE_DIR_CHG, NULL);
            base_dir_refr_children(child);
        }
    }
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
    lv_res_t res = LV_RES_OK;

    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        if(info->part == LV_OBJ_PART_MAIN) info->result = &obj->style_list;
        else info->result = NULL;
        return LV_RES_OK;
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        return _lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);
    }
    else if(sign == LV_SIGNAL_PRESSED) {
        lv_obj_add_state(obj, LV_STATE_PRESSED);
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        lv_obj_clear_state(obj, LV_STATE_PRESSED);

        /*Go the checked state if enabled*/
        if(lv_indev_get_scroll_obj(param) == NULL && lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE)) {
            uint32_t toggled = 0;
            if(!(lv_obj_get_state(obj) & LV_STATE_CHECKED)) {
                lv_obj_add_state(obj, LV_STATE_CHECKED);
                toggled = 0;
            }
            else {
                lv_obj_clear_state(obj, LV_STATE_CHECKED);
                toggled = 1;
            }

            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &toggled);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_PRESS_LOST) {
        lv_obj_clear_state(obj, LV_STATE_PRESSED);
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE)) {
            uint32_t state = 0;
            char c = *((char *)param);
            if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
                lv_obj_set_state(obj, LV_STATE_CHECKED);
                state = 1;
            }
            else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
                lv_obj_clear_state(obj, LV_STATE_CHECKED);
                state = 0;
            }
            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &state);
            if(res != LV_RES_OK) return res;
        }
#endif
    }
    else if(sign == LV_SIGNAL_FOCUS) {
        bool editing = false;
#if LV_USE_GROUP
        editing = lv_group_get_editing(lv_obj_get_group(obj));
#endif
        if(editing) {
            uint8_t state = LV_STATE_FOCUSED;
            state |= LV_STATE_EDITED;

            /*if using focus mode, change target to parent*/
            obj = _lv_obj_get_focused_obj(obj);

            lv_obj_add_state(obj, state);
        }
        else {

            /*if using focus mode, change target to parent*/
            obj = _lv_obj_get_focused_obj(obj);

            lv_obj_add_state(obj, LV_STATE_FOCUSED);
            lv_obj_clear_state(obj, LV_STATE_EDITED);
        }
    }
    else if(sign == LV_SIGNAL_DEFOCUS) {

        /*if using focus mode, change target to parent*/
        obj = _lv_obj_get_focused_obj(obj);

        lv_obj_clear_state(obj, LV_STATE_FOCUSED | LV_STATE_EDITED);
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {

        bool w_new = true;
        bool h_new = true;
        if(param) {
            if(lv_area_get_width(param) == lv_obj_get_width(obj)) w_new = false;
            if(lv_area_get_height(param) == lv_obj_get_height(obj)) h_new = false;
        }

        if(w_new || h_new) {
            lv_ll_t * ll = _lv_obj_get_child_ll(obj);
            lv_obj_t * child;
            _LV_LL_READ(ll, child) {
                if((LV_COORD_IS_PCT(child->w_set) && w_new) ||
                        (LV_COORD_IS_PCT(child->h_set) && h_new))
                {
                    lv_obj_set_size(child, child->w_set, child->h_set);
                }
            }
            if(lv_obj_get_grid(obj)) _lv_grid_full_refresh(obj);
            if(lv_obj_get_flex_dir(obj) != LV_FLEX_DIR_NONE) _lv_flex_refresh(obj);
        }
    }
    else if(sign == LV_SIGNAL_CHILD_CHG) {
        if(param == NULL || _lv_obj_is_flex_item(param)) {
            _lv_flex_refresh(obj);
        }

        if(obj->w_set == LV_SIZE_AUTO || obj->h_set == LV_SIZE_AUTO) {
            lv_obj_set_size(obj, obj->w_set, obj->h_set);
        }

        /*If the changed children was a grid item refresh this objects grid*/
        if(lv_obj_get_grid(obj)) {
            lv_obj_t * child = param;
            if(child) {
                if(_lv_obj_is_grid_item(child)) _lv_grid_full_refresh(obj);
            } else {
                _lv_grid_full_refresh(obj);
            }
        }

    }
    else if(sign == LV_SIGNAL_SCROLL) {
        res = lv_event_send(obj, LV_EVENT_SCROLL, NULL);
        if(res != LV_RES_OK) return res;
    }
    else if(sign == LV_SIGNAL_SCROLL_END) {
        if(lv_obj_get_scroll_mode(obj) == LV_SCROLL_MODE_ACTIVE) {
            lv_obj_invalidate(obj);
        }
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        lv_coord_t d = _lv_obj_get_draw_rect_ext_pad_size(obj, LV_OBJ_PART_MAIN);
        obj->ext_draw_pad = LV_MATH_MAX(obj->ext_draw_pad, d);
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        if(_lv_obj_is_grid_item(obj)) _lv_grid_full_refresh(obj);

        if(lv_obj_get_grid(obj)) _lv_grid_full_refresh(obj);

        /*Reposition non grid objects on by one*/
        lv_obj_t * child;

        lv_ll_t * ll = _lv_obj_get_child_ll(obj);
        _LV_LL_READ(ll, child) {
            if(LV_COORD_IS_PX(child->x_set) || LV_COORD_IS_PX(child->y_set)) {
                lv_obj_set_pos(child, child->x_set, child->y_set);
            }
        }

        if(obj->w_set == LV_SIZE_AUTO || obj->h_set == LV_SIZE_AUTO) {
            lv_obj_set_size(obj, obj->w_set, obj->h_set);
        }
        _lv_obj_refresh_ext_draw_pad(obj);
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
        _lv_obj_reset_style_list_no_refr(obj, LV_OBJ_PART_MAIN);
    }

    return res;
}


static void lv_event_mark_deleted(lv_obj_t * obj)
{
    lv_event_temp_data_t * t = event_temp_data_head;

    while(t) {
        if(t->obj == obj) t->deleted = true;
        t = t->prev;
    }
}

static bool obj_valid_child(const lv_obj_t * parent, const lv_obj_t * obj_to_find)
{
    /*Check all children of `parent`*/
    lv_obj_t * child;
    lv_ll_t * ll = _lv_obj_get_child_ll(parent);
    _LV_LL_READ(ll, child) {
        if(child == obj_to_find) return true;

        /*Check the children*/
        bool found = obj_valid_child(child, obj_to_find);
        if(found) return true;
    }

    return false;
}
