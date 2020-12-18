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
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_timer.h"
#include "../lv_misc/lv_async.h"
#include "../lv_misc/lv_fs.h"
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
#define GRID_DEBUG          0   /*Draw rectangles on grid cells*/

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
static void lv_obj_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_obj_destructor(void * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool lv_initialized = false;
static lv_event_temp_data_t * event_temp_data_head;
static const void * event_act_data;
lv_obj_class_t lv_obj;

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
    _lv_timer_core_init();

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

    _lv_style_system_init();

    LV_CLASS_INIT(lv_obj, lv_base);
    lv_obj.constructor = lv_obj_constructor;
    lv_obj.destructor = lv_obj_destructor;
    lv_obj.signal_cb = lv_obj_signal;
    lv_obj.design_cb = lv_obj_design;

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
    lv_obj_t * obj = lv_class_new(&lv_obj);
    lv_obj.constructor(obj, parent, copy);
    if(copy == NULL) lv_theme_apply(obj);
//    else   lv_style_list_copy(&obj->style_list, &copy->style_list);

    return obj;
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
        lv_signal_send(par, LV_SIGNAL_CHILD_CHG, NULL);
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

    while(1) {
        lv_obj_t * child = lv_obj_get_child(obj, 0);
        obj_del_core(child);
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
    lv_coord_t ext_size = _lv_obj_get_ext_draw_pad(obj);
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

    /*Invalidate the object only if it belongs to the current or previous'*/
    lv_obj_t * obj_scr = lv_obj_get_screen(obj);
    lv_disp_t * disp   = lv_obj_get_disp(obj_scr);
    if(obj_scr == lv_disp_get_scr_act(disp) ||
       obj_scr == lv_disp_get_scr_prev(disp) ||
       obj_scr == lv_disp_get_layer_top(disp) ||
       obj_scr == lv_disp_get_layer_sys(disp)) {

        /*Truncate the area to the object*/
        lv_area_t obj_coords;
        lv_coord_t ext_size = _lv_obj_get_ext_draw_pad(obj);
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
    lv_coord_t ext_size = _lv_obj_get_ext_draw_pad(obj);
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
        parent->spec_attr = lv_obj_allocate_spec_attr(parent);
    }

    lv_obj_t * old_parent = obj->parent;
    lv_point_t old_pos;
    old_pos.y = lv_obj_get_y(obj);

    lv_bidi_dir_t new_base_dir = lv_obj_get_base_dir(parent);

    if(new_base_dir != LV_BIDI_DIR_RTL) old_pos.x = lv_obj_get_x(obj);
    else  old_pos.x = old_parent->coords.x2 - obj->coords.x2;

    /*Remove the object from the old parent's child list*/
    uint32_t i;
    for(i = lv_obj_get_child_id(obj); i < lv_obj_get_child_cnt(old_parent) - 2; i++) {
        old_parent->spec_attr->children[i] = old_parent->spec_attr->children[i+1];
    }
    old_parent->spec_attr->child_cnt--;
    if(old_parent->spec_attr->child_cnt) {
        old_parent->spec_attr->children = lv_mem_realloc(old_parent->spec_attr->children, old_parent->spec_attr->child_cnt * (sizeof(lv_obj_t *)));
    } else {
        lv_mem_free(old_parent->spec_attr->children);
        old_parent->spec_attr->children = NULL;
    }

    /*Add the child to the new parent*/
    parent->spec_attr->child_cnt++;
    parent->spec_attr->children = lv_mem_realloc(parent->spec_attr->children, parent->spec_attr->child_cnt * (sizeof(lv_obj_t *)));
    for(i = lv_obj_get_child_cnt(parent) - 1; i > 0 ; i--) {
        parent->spec_attr->children[i] = parent->spec_attr->children[i - 1];
    }
    parent->spec_attr->children[0] = obj;

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
    lv_signal_send(old_parent, LV_SIGNAL_CHILD_CHG, obj);

    /*Notify the new parent about the child*/
    lv_signal_send(parent, LV_SIGNAL_CHILD_CHG, obj);

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

    lv_obj_invalidate(parent);

    uint32_t i;
    for(i = lv_obj_get_child_id(obj) - 1; i > 0; i--) {
        parent->spec_attr->children[i] = parent->spec_attr->children[i-1];
    }
    parent->spec_attr->children[0] = obj;

    /*Notify the new parent about the child*/
    lv_signal_send(parent, LV_SIGNAL_CHILD_CHG, obj);

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

    lv_obj_invalidate(parent);

    uint32_t i;
    for(i = lv_obj_get_child_id(obj); i < lv_obj_get_child_cnt(parent) - 2; i++) {
        parent->spec_attr->children[i] = parent->spec_attr->children[i + 1];
    }
    parent->spec_attr->children[ lv_obj_get_child_cnt(parent) - 1] = obj;

    /*Notify the new parent about the child*/
    lv_signal_send(parent, LV_SIGNAL_CHILD_CHG, obj);

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
    if(obj->spec_attr == NULL) lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->ext_click_pad.x1 = left;
    obj->spec_attr->ext_click_pad.x2 = right;
    obj->spec_attr->ext_click_pad.y1 = top;
    obj->spec_attr->ext_click_pad.y2 = bottom;
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    if(obj->spec_attr == NULL) lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->ext_click_pad = LV_MATH_MAX4(left, right, top, bottom);
#else
    LV_UNUSED(obj);
    LV_UNUSED(left);
    LV_UNUSED(right);
    LV_UNUSED(top);
    LV_UNUSED(bottom);
#endif
}

/**
 * Get the extended draw area of an object.
 * @param obj pointer to an object
 * @return the size extended draw area around the real coordinates
 */
lv_coord_t _lv_obj_get_ext_draw_pad(const lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->ext_draw_pad;
    else return 0;
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

    lv_obj_allocate_spec_attr(obj);
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
    obj->state = new_state;

    _lv_style_state_cmp_t cmp_res = _lv_obj_style_state_compare(obj, prev_state, new_state);

    /*If there is no difference in styles there is nothing else to do*/
    if(cmp_res == _LV_STYLE_STATE_CMP_SAME) return;

    uint8_t part;
    for(part = 0; part < _LV_OBJ_PART_MAX; part++) {
        uint16_t time = lv_obj_get_style_transition_time(obj, part);
        if(time == 0) continue;

        lv_style_prop_t props[LV_STYLE_TRANS_NUM_MAX];
        uint16_t delay = lv_obj_get_style_transition_delay(obj, part);
        const lv_anim_path_t * path = lv_obj_get_style_transition_path(obj, part);
        props[0] = lv_obj_get_style_transition_prop_1(obj, part);
        props[1] = lv_obj_get_style_transition_prop_2(obj, part);
        props[2] = lv_obj_get_style_transition_prop_3(obj, part);
        props[3] = lv_obj_get_style_transition_prop_4(obj, part);
        props[4] = lv_obj_get_style_transition_prop_5(obj, part);
        props[5] = lv_obj_get_style_transition_prop_6(obj, part);

        uint8_t i;
        for(i = 0; i < LV_STYLE_TRANS_NUM_MAX; i++) {
            if(props[i] != _LV_STYLE_PROP_INV) {
                _lv_obj_create_style_transition(obj, props[i], part, prev_state, new_state, time, delay, path);
            }
        }
    }

    if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_LAYOUT) _lv_obj_refresh_style(obj, LV_STYLE_PROP_ALL);
    else if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD) _lv_obj_refresh_ext_draw_pad(obj);
    else if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_REDRAW) lv_obj_invalidate(obj);

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
    if(obj->spec_attr == NULL) obj->spec_attr = lv_obj_allocate_spec_attr(obj);

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
            uint32_t i;
            for(i = 0; i < d->screen_cnt; i++) {
                lv_event_send_refresh_recursive(d->screens[i]);
            }
            lv_event_send_refresh_recursive(d->top_layer);
            lv_event_send_refresh_recursive(d->sys_layer);

            d = lv_disp_get_next(d);
        }
    }
    else {

        lv_res_t res = lv_event_send_refresh(obj);
        if(res != LV_RES_OK) return; /*If invalid returned do not check the children*/

        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
            lv_event_send_refresh_recursive(lv_obj_get_child(obj, i));
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
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `lv_event_t`.
 * @return LV_RES_OK or LV_RES_INV
 */
lv_res_t lv_signal_send(lv_obj_t * obj, lv_signal_t signal, void * param)
{
    if(obj == NULL) return LV_RES_OK;

    lv_res_t res = LV_RES_OK;
    if(obj->class_p->signal_cb) res = obj->class_p->signal_cb(obj, signal, param);

    return res;
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
lv_obj_spec_attr_t * lv_obj_allocate_spec_attr(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(obj->spec_attr == NULL) {
        static uint32_t x = 0;
        x++;
        obj->spec_attr = lv_mem_alloc(sizeof(lv_obj_spec_attr_t));
        LV_ASSERT_MEM(obj->spec_attr);
        if(obj->spec_attr == NULL) return NULL;

        _lv_memset_00(obj->spec_attr, sizeof(lv_obj_spec_attr_t));

        obj->spec_attr->scroll_dir = LV_DIR_ALL;
        obj->spec_attr->base_dir = LV_BIDI_DIR_INHERIT;
        obj->spec_attr->scrollbar_mode = LV_SCROLLBAR_MODE_AUTO;

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

    if(obj->parent == NULL) scr = obj; /*`obj` is a screen*/
    else scr = lv_obj_get_screen(obj); /*get the screen of `obj`*/

    lv_disp_t * d;
    _LV_LL_READ(&LV_GC_ROOT(_lv_disp_ll), d) {
        uint32_t i;
        for(i = 0; i < d->screen_cnt; i++) {
            if(d->screens[i] == scr) return d;
        }
    }

    LV_LOG_WARN("No screen found")
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
 * Get the Nth child of an object. 0th is the lastly created.
 * @param obj pointer to an object whose children should be get
 * @param id of a child
 * @return the child or `NULL` if `id` was greater then the `number of children - 1`
 */
lv_obj_t * lv_obj_get_child(const lv_obj_t * obj, uint32_t id)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(obj->spec_attr == NULL) return NULL;

    if(id >= obj->spec_attr->child_cnt) return NULL;
    else return obj->spec_attr->children[id];
}

uint32_t lv_obj_get_child_cnt(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    if(obj->spec_attr == NULL) return 0;
    return obj->spec_attr->child_cnt;
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

    uint32_t i = 0;
    for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
        if(lv_obj_get_child(parent, i) == obj) return i;
    }

    return 0xFFFFFFFF;
}

/** Recursively count the children of an object
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint32_t lv_obj_count_children_recursive(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    uint16_t cnt = 0;

    uint32_t i = 0;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        cnt++;
        cnt += lv_obj_count_children_recursive(lv_obj_get_child(obj, i));
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
        return obj->spec_attr->ext_click_pad.x1;
    case LV_DIR_RIGHT:
        return obj->spec_attr->ext_click_pad.x2;
    case LV_DIR_TOP:
        return obj->spec_attr->ext_click_pad.y1;
    case LV_DIR_BOTTOM:
        return obj->spec_attr->ext_click_pad.y2;
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
        lv_signal_send(obj, LV_SIGNAL_HIT_TEST, &hit_info);
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

#if LV_USE_BIDI
    if(obj->spec_attr == NULL) return LV_BIDI_DIR_LTR;
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

lv_obj_t ** lv_obj_get_children(const lv_obj_t * obj)
{
    static lv_obj_t * empty[1] = {NULL};
    if(obj->spec_attr) return obj->spec_attr->children;
    else return empty;
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
 * Get object's and its ancestors type. Put their name in `type_buf` starting with the current type.
 * E.g. buf.type[0]="lv_btn", buf.type[1]="lv_cont", buf.type[2]="lv_obj"
 * @param obj pointer to an object which type should be get
 * @param buf pointer to an `lv_obj_type_t` buffer to store the types
 */
void * lv_obj_check_type(const lv_obj_t * obj, void * class_p)
{
    return obj->class_p == class_p ? true : false;
}
/**
 * Check if any object has a given type
 * @param obj pointer to an object
 * @param obj_type type of the object. (e.g. "lv_btn")
 * @return true: valid
 */
bool _lv_debug_check_obj_type(const lv_obj_t * obj, const char * obj_type)
{
    return true;
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
        uint32_t i;
        for(i = 0; i < disp->screen_cnt; i++) {
            if(disp->screens[i] == obj) return true;
            bool found = obj_valid_child(disp->screens[i], obj);
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
    while(1) {
        lv_obj_t * child = lv_obj_get_child(obj, 0);
        obj_del_core(child);
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
    obj->class_p->destructor(obj);
}

static void lv_obj_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_CLASS_CONSTRUCTOR_BEGIN(obj, lv_obj)
    lv_obj.base_p->constructor(obj);

    /*Create a screen*/
    if(parent == NULL) {
        LV_LOG_TRACE("Screen create started");
        lv_disp_t * disp = lv_disp_get_default();
        if(!disp) {
            LV_LOG_WARN("No display created to so far. No place to assign the new screen");
            return;
        }

        if(disp->screens == NULL) {
            disp->screens = lv_mem_alloc(sizeof(lv_obj_t *));
            disp->screens[0] = obj;
            disp->screen_cnt = 1;
        } else {
            disp->screen_cnt++;
            disp->screens = lv_mem_realloc(disp->screens, sizeof(lv_obj_t *) * disp->screen_cnt);
            disp->screens[disp->screen_cnt - 1] = obj;
        }

        /*Set coordinates to full screen size*/
        obj->coords.x1 = 0;
        obj->coords.y1 = 0;
        obj->coords.x2 = lv_disp_get_hor_res(NULL) - 1;
        obj->coords.y2 = lv_disp_get_ver_res(NULL) - 1;
    }
    /*Create a normal object*/
    else {
        LV_LOG_TRACE("Object create started");
        LV_ASSERT_OBJ(parent, LV_OBJX_NAME);
        if(parent->spec_attr == NULL) {
            parent->spec_attr = lv_obj_allocate_spec_attr(parent);
        }

        if(parent->spec_attr->children == NULL) {
            parent->spec_attr->children = lv_mem_alloc(sizeof(lv_obj_t *) * 2);
            parent->spec_attr->children[0] = obj;
            parent->spec_attr->child_cnt = 1;
        } else {
            parent->spec_attr->child_cnt++;
            parent->spec_attr->children = lv_mem_realloc(parent->spec_attr->children, sizeof(lv_obj_t *) * parent->spec_attr->child_cnt);
            parent->spec_attr->children[parent->spec_attr->child_cnt - 1] = obj;
        }

        obj->parent = parent;

        obj->coords.y1 = parent->coords.y1;
        obj->coords.y2 = parent->coords.y1 + LV_OBJ_DEF_HEIGHT;
        if(lv_obj_get_base_dir(obj) == LV_BIDI_DIR_RTL) {
            obj->coords.x2    = parent->coords.x2;
            obj->coords.x1    = parent->coords.x2 - LV_OBJ_DEF_WIDTH;
        }
        else {
            obj->coords.x1    = parent->coords.x1;
            obj->coords.x2    = parent->coords.x1 + LV_OBJ_DEF_WIDTH;
        }
        obj->w_set = lv_area_get_width(&obj->coords);
        obj->h_set = lv_area_get_height(&obj->coords);
    }

    /*Set attributes*/
    obj->flags = LV_OBJ_FLAG_CLICKABLE;
    obj->flags |= LV_OBJ_FLAG_SNAPABLE;
    if(parent) obj->flags |= LV_OBJ_FLAG_PRESS_LOCK;
    if(parent) obj->flags |= LV_OBJ_FLAG_SCROLL_CHAIN;
    obj->flags |= LV_OBJ_FLAG_CLICK_FOCUSABLE;
    obj->flags |= LV_OBJ_FLAG_SCROLLABLE;
    obj->flags |= LV_OBJ_FLAG_SCROLL_ELASTIC;
    obj->flags |= LV_OBJ_FLAG_SCROLL_MOMENTUM;
    obj->flags |= LV_OBJ_FLAG_FOCUS_SCROLL;
    if(parent) obj->flags |= LV_OBJ_FLAG_GESTURE_BUBBLE;
    obj->state = LV_STATE_DEFAULT;

    /*Copy the attributes if required*/
    if(copy != NULL) {
        lv_area_copy(&obj->coords, &copy->coords);

        obj->flags  = copy->flags;
        if(copy->spec_attr) {
            lv_obj_allocate_spec_attr(obj);
            _lv_memcpy_small(obj->spec_attr, copy->spec_attr, sizeof(lv_obj_spec_attr_t));
            obj->spec_attr->children = NULL;    /*Make the child list empty*/
        }
#if LV_USE_GROUP
        /*Add to the same group*/
        if(copy->spec_attr && copy->spec_attr->group_p) {
            obj->spec_attr->group_p = NULL; /*It was simply copied */
            lv_group_add_obj(copy->spec_attr->group_p, obj);
        }
#endif

        /*Set the same coordinates for non screen objects*/
        if(lv_obj_get_parent(copy) != NULL && parent != NULL) {
            lv_obj_set_pos(obj, lv_obj_get_x(copy), lv_obj_get_y(copy));
            lv_obj_set_size(obj, lv_obj_get_width(copy), lv_obj_get_height(copy));

        }
    } else {
        lv_obj_set_pos(obj, 0, 0);
    }
    /*Send a signal to the parent to notify it about the new child*/
    if(parent != NULL) {
        lv_signal_send(parent, LV_SIGNAL_CHILD_CHG, obj);

        /*Invalidate the area if not screen created*/
        lv_obj_invalidate(obj);
    }

    LV_CLASS_CONSTRUCTOR_END(obj, lv_obj)

    LV_LOG_INFO("Object create ready");
}

static void lv_obj_destructor(void * p)
{
    lv_obj_t * obj = p;
    lv_obj_remove_all_styles(obj);
    if(obj->spec_attr) lv_mem_free(obj->spec_attr);

    lv_class_destroy(obj);
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
        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) return LV_DESIGN_RES_MASKED;

        /*Most trivial test. Is the mask fully IN the object? If no it surely doesn't cover it*/
        lv_coord_t r = lv_obj_get_style_radius(obj, LV_PART_MAIN);
        lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
        lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.x1 -= w;
        coords.x2 += w;
        coords.y1 -= h;
        coords.y2 += h;

        if(_lv_area_is_in(clip_area, &coords, r) == false) return LV_DESIGN_RES_NOT_COVER;

        if(lv_obj_get_style_bg_opa(obj, LV_PART_MAIN) < LV_OPA_MAX) return LV_DESIGN_RES_NOT_COVER;

        if(lv_obj_get_style_bg_blend_mode(obj, LV_PART_MAIN) != LV_BLEND_MODE_NORMAL) return LV_DESIGN_RES_NOT_COVER;
        if(lv_obj_get_style_border_blend_mode(obj, LV_PART_MAIN) != LV_BLEND_MODE_NORMAL) return LV_DESIGN_RES_NOT_COVER;
        if(lv_obj_get_style_opa(obj, LV_PART_MAIN) < LV_OPA_MAX) return LV_DESIGN_RES_NOT_COVER;

        return  LV_DESIGN_RES_COVER;

    }
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        /*If the border is drawn later disable loading its properties*/
        if(lv_obj_get_style_border_post(obj, LV_PART_MAIN)) {
            draw_dsc.border_post = 1;
        }

        lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &draw_dsc);

        lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
        lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.x1 -= w;
        coords.x2 += w;
        coords.y1 -= h;
        coords.y2 += h;

        lv_draw_rect(&coords, clip_area, &draw_dsc);


        /*Draw the content*/
        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_opa = LV_OPA_TRANSP;
        draw_dsc.border_opa = LV_OPA_TRANSP;
        draw_dsc.outline_opa = LV_OPA_TRANSP;
        draw_dsc.shadow_opa = LV_OPA_TRANSP;
        lv_obj_init_draw_rect_dsc(obj, LV_PART_CONTENT, &draw_dsc);
        lv_draw_rect(&coords, clip_area, &draw_dsc);

        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) {
            lv_draw_mask_radius_param_t * mp = _lv_mem_buf_get(sizeof(lv_draw_mask_radius_param_t));
            lv_coord_t r = lv_obj_get_style_radius(obj, LV_PART_MAIN);
            lv_draw_mask_radius_init(mp, &obj->coords, r, false);
            /*Add the mask and use `obj+8` as custom id. Don't use `obj` directly because it might be used by the user*/
            lv_draw_mask_add(mp, obj + 8);
        }
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        _lv_obj_draw_scrollbar(obj, clip_area);

        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) {
            lv_draw_mask_radius_param_t * param = lv_draw_mask_remove_custom(obj + 8);
            _lv_mem_buf_release(param);
        }

        /*If the border is drawn later disable loading other properties*/
        if(lv_obj_get_style_border_post(obj, LV_PART_MAIN)) {
            lv_draw_rect_dsc_t draw_dsc;
            lv_draw_rect_dsc_init(&draw_dsc);
            draw_dsc.bg_opa = LV_OPA_TRANSP;
            draw_dsc.outline_opa = LV_OPA_TRANSP;
            draw_dsc.shadow_opa = LV_OPA_TRANSP;
            draw_dsc.content_opa = LV_OPA_TRANSP;
            lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &draw_dsc);

            lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
            lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
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
            uint32_t cx = ((lv_uintptr_t) obj) & 0xFFFFFF;
            cx = (cx & 0xff) ^ ((cx >> 8) & 0xff) ^ ((cx >> 16) & 0xff) ^ ((cx >> 24) & 0xff);
            lv_color_t c = lv_color_hsv_to_rgb(cx & 0xff, 100, 100);

            lv_draw_rect_dsc_t grid_rect_dsc;
            lv_draw_rect_dsc_init(&grid_rect_dsc);
            grid_rect_dsc.bg_color = c;
            grid_rect_dsc.bg_opa = LV_OPA_10;
            grid_rect_dsc.border_width = 2;
            grid_rect_dsc.border_color = c;
            grid_rect_dsc.border_opa = LV_OPA_30;

            lv_point_t grid_abs;
            lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_OBJ_PART_MAIN);
            lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_OBJ_PART_MAIN);
            grid_abs.x = pad_left + obj->coords.x1 - lv_obj_get_scroll_x(obj);
            grid_abs.y = pad_top + obj->coords.y1 - lv_obj_get_scroll_y(obj);

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
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
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

    if(sign == LV_SIGNAL_PRESSED) {
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
        lv_obj_t * parent = lv_obj_get_parent(obj);
        lv_obj_t * child = obj;
        while(parent && lv_obj_has_flag(child, LV_OBJ_FLAG_FOCUS_SCROLL)) {
            lv_obj_scroll_to_view(child, LV_ANIM_ON);
            child = parent;
            parent = lv_obj_get_parent(parent);
        }

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
    else if(sign == LV_SIGNAL_SCROLL_BEGIN) {
        lv_obj_add_state(obj, LV_STATE_SCROLLED);
    }
    else if(sign == LV_SIGNAL_SCROLL_END) {
        lv_obj_clear_state(obj, LV_STATE_SCROLLED);
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
            uint32_t i = 0;
            for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
                lv_obj_t * child = lv_obj_get_child(obj, i);
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
    }
    else if(sign == LV_SIGNAL_SCROLL) {
        res = lv_event_send(obj, LV_EVENT_SCROLL, NULL);
        if(res != LV_RES_OK) return res;
    }
    else if(sign == LV_SIGNAL_SCROLL_END) {
        if(lv_obj_get_scrollbar_mode(obj) == LV_SCROLLBAR_MODE_ACTIVE) {
            lv_obj_invalidate(obj);
        }
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        lv_coord_t * s = param;
        lv_coord_t d = _lv_obj_get_draw_rect_ext_pad_size(obj, LV_PART_MAIN);
        *s = LV_MATH_MAX(*s, d);
        d = _lv_obj_get_draw_rect_ext_pad_size(obj, LV_PART_CONTENT);
        *s = LV_MATH_MAX(*s, d);
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        if(_lv_obj_is_grid_item(obj)) lv_grid_item_refr_pos(obj);
        if(lv_obj_get_grid(obj)) _lv_grid_full_refresh(obj);

        if(_lv_obj_is_flex_item(obj)) _lv_flex_refresh(lv_obj_get_parent(obj));
        if(lv_obj_get_flex_dir(obj)) _lv_flex_refresh(obj);

        /*Reposition non grid objects on by one*/
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
           lv_obj_t * child = lv_obj_get_child(obj, i);
            if(LV_COORD_IS_PX(child->x_set) || LV_COORD_IS_PX(child->y_set)) {
                lv_obj_set_pos(child, child->x_set, child->y_set);
            }
        }

        if(obj->w_set == LV_SIZE_AUTO || obj->h_set == LV_SIZE_AUTO) {
            lv_obj_set_size(obj, obj->w_set, obj->h_set);
        }
        _lv_obj_refresh_ext_draw_pad(obj);
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
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
        lv_obj_t * child = lv_obj_get_child(parent, i);
        if(child == obj_to_find) return true;

        /*Check the children*/
        bool found = obj_valid_child(child, obj_to_find);
        if(found) return true;
    }

    return false;
}
