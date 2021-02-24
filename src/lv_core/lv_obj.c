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
#include "lv_theme.h"
#include "../lv_misc/lv_assert.h"
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

#include LV_THEME_INIT_INCLUDE

#if LV_USE_GPU_STM32_DMA2D
    #include "../lv_gpu/lv_gpu_stm32_dma2d.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_obj_class
#define LV_OBJ_DEF_WIDTH    (LV_DPX(100))
#define LV_OBJ_DEF_HEIGHT   (LV_DPX(50))
#define GRID_DEBUG          0   /*Draw rectangles on grid cells*/
#define STYLE_TRANSITION_MAX 32

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_event_temp_data {
    lv_obj_t * obj;
    bool deleted;
    struct _lv_event_temp_data * prev;
} lv_event_temp_data_t;

typedef struct {
    uint16_t time;
    uint16_t delay;
    lv_part_t part;
    lv_state_t state;
    lv_style_prop_t prop;
    const lv_anim_path_t * path;
}trans_set_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_draw_res_t lv_obj_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_obj_signal(lv_obj_t * obj, lv_signal_t sign, void * param);
static void draw_scrollbar(lv_obj_t * obj, const lv_area_t * clip_area);
static lv_res_t scrollbar_init_draw_dsc(lv_obj_t * obj, lv_draw_rect_dsc_t * dsc);
static bool obj_valid_child(const lv_obj_t * parent, const lv_obj_t * obj_to_find);
static void base_dir_refr_children(lv_obj_t * obj);
static void lv_obj_constructor(lv_obj_t * obj, const lv_obj_t * copy);
static void lv_obj_destructor(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool lv_initialized = false;
static lv_event_temp_data_t * event_temp_data_head;
static void * event_act_param;
static void * event_act_user_data_cb;
const lv_obj_class_t lv_obj_class = {
    .constructor_cb = lv_obj_constructor,
    .destructor_cb = lv_obj_destructor,
    .signal_cb = lv_obj_signal,
    .draw_cb = lv_obj_draw,
    .instance_size = (sizeof(lv_obj_t)),
    .base_class = NULL,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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

    _lv_timer_core_init();

    _lv_fs_init();

    _lv_anim_core_init();

    _lv_group_init();

#if LV_USE_GPU_STM32_DMA2D
    /*Initialize DMA2D GPU*/
    lv_gpu_stm32_dma2d_init();
#endif

    _lv_obj_style_init();
    _lv_ll_init(&LV_GC_ROOT(_lv_disp_ll), sizeof(lv_disp_t));
    _lv_ll_init(&LV_GC_ROOT(_lv_indev_ll), sizeof(lv_indev_t));

#ifdef LV_THEME_INIT
    lv_theme_t * th = LV_THEME_INIT(LV_THEME_COLOR_PRIMARY, LV_THEME_COLOR_SECONDARY,
                                    LV_THEME_FONT_SMALL, LV_THEME_FONT_NORMAL,
                                    LV_THEME_FONT_LARGE, LV_THEME_FONT_EXTRA_LARGE);

    lv_theme_set_act(th);
#endif
    /*Initialize the screen refresh system*/
    _lv_refr_init();

    _lv_img_decoder_init();
#if LV_IMG_CACHE_DEF_SIZE
    lv_img_cache_set_size(LV_IMG_CACHE_DEF_SIZE);
#endif
    /*Test if the IDE has UTF-8 encoding*/
    char * txt = "Á";

    uint8_t * txt_u8 = (uint8_t *) txt;
    if(txt_u8[0] != 0xc3 || txt_u8[1] != 0x81 || txt_u8[2] != 0x00) {
        LV_LOG_WARN("The strings has no UTF-8 encoding. Some characters won't be displayed.")
    }

#if LV_USE_ASSERT_MEM_INTEGRITY
    LV_LOG_WARN("Memory integrity checks are enabled via LV_USE_ASSERT_MEM_INTEGRITY which makes LVGL much slower")
#endif

#if LV_USE_ASSERT_OBJ
    LV_LOG_WARN("Object sanity checks are enabled via LV_USE_ASSERT_OBJ which makes LVGL much slower")
#endif


    lv_initialized = true;
    LV_LOG_INFO("lv_init ready");
}

#if LV_ENABLE_GC || !LV_MEM_CUSTOM

void lv_deinit(void)
{
    _lv_gc_clear_roots();

    lv_disp_set_default(NULL);
    lv_mem_deinit();
    lv_initialized = false;

    LV_LOG_INFO("lv_deinit done");

#if LV_USE_LOG
    lv_log_register_print_cb(NULL);
#endif
}
#endif

lv_obj_t * lv_obj_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    return lv_obj_create_from_class(&lv_obj_class, parent, copy);
}

/*---------------------
 * Event/Signal sending
 *---------------------*/

lv_res_t lv_event_send(lv_obj_t * obj, lv_event_t event, void * param)
{
    if(obj == NULL) return LV_RES_OK;

    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*Nothing to do if no event function and not bubbled*/
    lv_event_dsc_t * event_dsc = lv_obj_get_event_dsc(obj, 0);
    if(event_dsc == NULL && lv_obj_has_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE) == false) {
        return LV_RES_OK;
    }

    /* Build a simple linked list from the objects used in the events
     * It's important to know if an this object was deleted by a nested event
     * called from this `event_cb`. */
    lv_event_temp_data_t event_temp_data;
    event_temp_data.obj     = obj;
    event_temp_data.deleted = false;
    event_temp_data.prev    = NULL;

    if(event_temp_data_head) {
        event_temp_data.prev = event_temp_data_head;
    }
    event_temp_data_head = &event_temp_data;

    /* There could be nested event sending with different param.
     * It needs to be saved for the current event context because `lv_event_get_data` returns a global param. */
    void * event_act_param_save = event_act_param;
    event_act_param = param;

    /*Call the input device's feedback callback if set*/
    lv_indev_t * indev_act = lv_indev_get_act();
    if(indev_act) {
        if(indev_act->driver.feedback_cb) indev_act->driver.feedback_cb(&indev_act->driver, event);
    }

    uint32_t i = 0;
    lv_res_t res = LV_RES_OK;
    while(event_dsc) {
        if(event_dsc->cb) {
            void * event_act_user_data_cb_save = event_act_user_data_cb;
            event_act_user_data_cb = event_dsc->user_data;

            event_dsc->cb(obj, event);

            event_act_user_data_cb = event_act_user_data_cb_save;

            /*Stop if the object is deleted*/
            if(event_temp_data.deleted) {
                res = LV_RES_INV;
                break;
            }
        }

        i++;
        event_dsc = lv_obj_get_event_dsc(obj, i);
    }

    /*Restore the event param*/
    event_act_param = event_act_param_save;

    /*Remove this element from the list*/
    event_temp_data_head = event_temp_data_head->prev;

    if(res == LV_RES_OK) {
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE) && obj->parent) {
            res = lv_event_send(obj->parent, event, param);
            if(res != LV_RES_OK) return LV_RES_INV;
        }
    }

    return res;
}

void * lv_event_get_param(void)
{
    return event_act_param;
}

void * lv_event_get_user_data(void)
{
    return event_act_user_data_cb;
}

uint32_t lv_event_register_id(void)
{
    static uint32_t last_id = _LV_EVENT_LAST;
    last_id ++;
    return last_id;
}

void _lv_event_mark_deleted(lv_obj_t * obj)
{
    lv_event_temp_data_t * t = event_temp_data_head;

    while(t) {
        if(t->obj == obj) t->deleted = true;
        t = t->prev;
    }
}

lv_res_t lv_signal_send(lv_obj_t * obj, lv_signal_t signal, void * param)
{
    if(obj == NULL) return LV_RES_OK;

    const lv_obj_class_t * class_p = obj->class_p;
    while(class_p && class_p->signal_cb == NULL) class_p = class_p->base_class;

    if(class_p == NULL) return LV_RES_OK;

    lv_res_t res = LV_RES_OK;
    if(class_p->signal_cb) res = class_p->signal_cb(obj, signal, param);

    return res;
}

/*=====================
 * Setter functions
 *====================*/

/*-----------------
 * Attribute set
 *----------------*/

void lv_obj_add_flag(lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(f & LV_OBJ_FLAG_HIDDEN) lv_obj_invalidate(obj);

    obj->flags |= f;

    if(f & LV_OBJ_FLAG_LAYOUTABLE) lv_signal_send(lv_obj_get_parent(obj), LV_SIGNAL_CHILD_CHG, obj);

    if(f & LV_OBJ_FLAG_HIDDEN) {
    	lv_obj_invalidate(obj);
    	if(lv_obj_is_layout_positioned(obj)) {
    		lv_obj_update_layout(lv_obj_get_parent(obj), obj);
    	}
    }
}

void lv_obj_clear_flag(lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    obj->flags &= (~f);

    if(f & LV_OBJ_FLAG_HIDDEN) {
    	lv_obj_invalidate(obj);
    	if(lv_obj_is_layout_positioned(obj)) {
    		lv_obj_update_layout(lv_obj_get_parent(obj), obj);
    	}
    }
    if(f & LV_OBJ_FLAG_LAYOUTABLE) lv_signal_send(lv_obj_get_parent(obj), LV_SIGNAL_CHILD_CHG, obj);
}

void lv_obj_set_state(lv_obj_t * obj, lv_state_t new_state)
{
    if(obj->state == new_state) return;

    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_state_t prev_state = obj->state;
    obj->state = new_state;

    _lv_style_state_cmp_t cmp_res = _lv_obj_style_state_compare(obj, prev_state, new_state);
    /*If there is no difference in styles there is nothing else to do*/
    if(cmp_res == _LV_STYLE_STATE_CMP_SAME) return;

    trans_set_t * ts = lv_mem_buf_get(sizeof(trans_set_t) * STYLE_TRANSITION_MAX);
    lv_memset_00(ts, sizeof(sizeof(trans_set_t) * 64));
    uint32_t tsi = 0;
    uint32_t i;
    for(i = 0; i < obj->style_list.style_cnt && tsi < STYLE_TRANSITION_MAX; i++) {
        lv_obj_style_t * obj_style = &obj->style_list.styles[i];
        if(obj_style->state & (~new_state)) continue; /*Skip unrelated styles*/
        if(obj_style->is_trans) continue;

        lv_style_value_t v;
        if(lv_style_get_prop(obj_style->style, LV_STYLE_TRANSITION, &v) == false) continue;
        const lv_style_transition_dsc_t * tr = v.ptr;

        /*Add the props t the set is not added yet or added but with smaller weight*/
        uint32_t j;
        for(j = 0; tr->props[j] != 0 && tsi < STYLE_TRANSITION_MAX; j++) {
            uint32_t t;
            for(t = 0; t < tsi; t++) {
                if(ts[t].prop == tr->props[j] && ts[t].state >= obj_style->state) break;
            }

            /*If not found  add it*/
            if(t == tsi) {
                ts[tsi].time = tr->time;
                ts[tsi].delay = tr->delay;
                ts[tsi].path = tr->path;
                ts[tsi].prop = tr->props[j];
                ts[tsi].part = obj_style->part;
                ts[tsi].state = obj_style->state;
                tsi++;
            }
        }
    }

    for(i = 0;i < tsi; i++) {
        _lv_obj_style_create_transition(obj, ts[i].prop, ts[i].part, prev_state, new_state, ts[i].time, ts[i].delay, ts[i].path);
    }

    lv_mem_buf_release(ts);

    lv_obj_invalidate(obj);

    if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_LAYOUT) lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ALL);
    else if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD) lv_obj_refresh_ext_draw_size(obj);
}

void lv_obj_add_state(lv_obj_t * obj, lv_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_state_t new_state = obj->state | state;
    if(obj->state != new_state) {
        lv_obj_set_state(obj, new_state);
    }
}

void lv_obj_clear_state(lv_obj_t * obj, lv_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_state_t new_state = obj->state & (~state);
    if(obj->state != new_state) {
        lv_obj_set_state(obj, new_state);
    }
}

void lv_obj_add_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb, void * user_data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_allocate_spec_attr(obj);

    obj->spec_attr->event_dsc_cnt++;
    obj->spec_attr->event_dsc = lv_mem_realloc(obj->spec_attr->event_dsc, obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
    LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);

    obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1].cb = event_cb;
    obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1].user_data = user_data;

}

void lv_obj_set_base_dir(lv_obj_t * obj, lv_bidi_dir_t dir)
{
    if(dir != LV_BIDI_DIR_LTR && dir != LV_BIDI_DIR_RTL &&
       dir != LV_BIDI_DIR_AUTO && dir != LV_BIDI_DIR_INHERIT) {

        LV_LOG_WARN("lv_obj_set_base_dir: invalid base direction: %d", dir);
        return;
    }

    lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->base_dir = dir;
    lv_signal_send(obj, LV_SIGNAL_BASE_DIR_CHG, NULL);

    /* Notify the children about the parent base dir has changed.
     * (The children might have `LV_BIDI_DIR_INHERIT`)*/
    base_dir_refr_children(obj);
}


/*=======================
 * Getter functions
 *======================*/

bool lv_obj_has_flag(const lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return (obj->flags & f)  == f ? true : false;
}

lv_bidi_dir_t lv_obj_get_base_dir(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

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
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return obj->state;
}


bool lv_obj_has_state(const lv_obj_t * obj, lv_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return obj->state & state ? true : false;
}

lv_event_dsc_t * lv_obj_get_event_dsc(const lv_obj_t * obj, uint32_t id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(!obj->spec_attr) return NULL;
    if(id >= obj->spec_attr->event_dsc_cnt) return NULL;

    return &obj->spec_attr->event_dsc[id];
}

void * lv_obj_get_group(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(obj->spec_attr) return obj->spec_attr->group_p;
    else return NULL;
}

/*-------------------
 * OTHER FUNCTIONS
 *------------------*/

void lv_obj_allocate_spec_attr(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(obj->spec_attr == NULL) {
        static uint32_t x = 0;
        x++;
        obj->spec_attr = lv_mem_alloc(sizeof(lv_obj_spec_attr_t));
        LV_ASSERT_MALLOC(obj->spec_attr);
        if(obj->spec_attr == NULL) return;

        lv_memset_00(obj->spec_attr, sizeof(lv_obj_spec_attr_t));

        obj->spec_attr->scroll_dir = LV_DIR_ALL;
        obj->spec_attr->base_dir = LV_BIDI_DIR_INHERIT;
        obj->spec_attr->scrollbar_mode = LV_SCROLLBAR_MODE_AUTO;

    }
}

lv_obj_t * lv_obj_get_focused_obj(const lv_obj_t * obj)
{
    if(obj == NULL) return NULL;
    const lv_obj_t * focus_obj = obj;
    while(lv_obj_has_flag(focus_obj, LV_OBJ_FLAG_FOCUS_BUBBLE) != false && focus_obj != NULL) {
        focus_obj = lv_obj_get_parent(focus_obj);
    }

    return (lv_obj_t *)focus_obj;
}

bool lv_obj_check_type(const lv_obj_t * obj, const lv_obj_class_t * class_p)
{
    if(obj == NULL) return false;
    return obj->class_p == class_p ? true : false;
}

bool lv_obj_has_class(const lv_obj_t * obj, const lv_obj_class_t * class_p)
{
    const lv_obj_class_t * obj_class = obj->class_p;
    while(obj_class) {
        if(obj_class == class_p) return true;
        obj_class = obj_class->base_class;
    }

    return false;
}

bool lv_obj_is_valid(const lv_obj_t * obj)
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

static void lv_obj_constructor(lv_obj_t * obj, const lv_obj_t * copy)
{
    lv_obj_t * parent = obj->parent;
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
        LV_ASSERT_OBJ(parent, MY_CLASS);
        if(parent->spec_attr == NULL) {
            lv_obj_allocate_spec_attr(parent);
        }

        if(parent->spec_attr->children == NULL) {
            parent->spec_attr->children = lv_mem_alloc(sizeof(lv_obj_t *));
            parent->spec_attr->children[0] = obj;
            parent->spec_attr->child_cnt = 1;
        } else {
            parent->spec_attr->child_cnt++;
            parent->spec_attr->children = lv_mem_realloc(parent->spec_attr->children, sizeof(lv_obj_t *) * parent->spec_attr->child_cnt);
            parent->spec_attr->children[parent->spec_attr->child_cnt - 1] = obj;
        }

        obj->parent = parent;

        lv_coord_t sl = lv_obj_get_scroll_left(parent);
        lv_coord_t st = lv_obj_get_scroll_top(parent);

        obj->coords.y1 = parent->coords.y1 + lv_obj_get_style_pad_top(parent, LV_PART_MAIN) - st;
        obj->coords.y2 = obj->coords.y1 + LV_OBJ_DEF_HEIGHT;
        if(lv_obj_get_base_dir(obj) == LV_BIDI_DIR_RTL) {
            obj->coords.x2  = parent->coords.x2 - lv_obj_get_style_pad_right(parent, LV_PART_MAIN) - sl;
            obj->coords.x1  = obj->coords.x2 - LV_OBJ_DEF_WIDTH;
        }
        else {
            obj->coords.x1  = parent->coords.x1 + lv_obj_get_style_pad_left(parent, LV_PART_MAIN) - sl;
            obj->coords.x2  = obj->coords.x1 + LV_OBJ_DEF_WIDTH;
        }
        obj->w_set = lv_area_get_width(&obj->coords);
        obj->h_set = lv_area_get_height(&obj->coords);
    }

    /*Set attributes*/
    obj->flags = LV_OBJ_FLAG_CLICKABLE;
    obj->flags |= LV_OBJ_FLAG_SNAPABLE;
    if(parent) obj->flags |= LV_OBJ_FLAG_PRESS_LOCK;
    if(parent) obj->flags |= LV_OBJ_FLAG_SCROLL_CHAIN;
    if(parent) obj->flags |= LV_OBJ_FLAG_LAYOUTABLE;
    obj->flags |= LV_OBJ_FLAG_CLICK_FOCUSABLE;
    obj->flags |= LV_OBJ_FLAG_SCROLLABLE;
    obj->flags |= LV_OBJ_FLAG_SCROLL_ELASTIC;
    obj->flags |= LV_OBJ_FLAG_SCROLL_MOMENTUM;
    obj->flags |= LV_OBJ_FLAG_SCROLL_ON_FOCUS;
    if(parent) obj->flags |= LV_OBJ_FLAG_GESTURE_BUBBLE;

    obj->style_list.cache_state = LV_OBJ_STYLE_CACHE_STATE_INVALID;

    /*Copy the attributes if required*/
    if(copy != NULL) {
        lv_area_copy(&obj->coords, &copy->coords);

        obj->flags  = copy->flags;
        if(copy->spec_attr) {
            lv_obj_allocate_spec_attr(obj);
            lv_memcpy_small(obj->spec_attr, copy->spec_attr, sizeof(lv_obj_spec_attr_t));
            obj->spec_attr->children = NULL;    /*Make the child list empty*/
        }
        /*Add to the same group*/
        if(copy->spec_attr && copy->spec_attr->group_p) {
            obj->spec_attr->group_p = NULL; /*It was simply copied */
            lv_group_add_obj(copy->spec_attr->group_p, obj);
        }

        /*Set the same coordinates for non screen objects*/
        if(lv_obj_get_parent(copy) != NULL && parent != NULL) {
            lv_obj_set_pos(obj, lv_obj_get_x(copy), lv_obj_get_y(copy));
            lv_obj_set_size(obj, lv_obj_get_width(copy), lv_obj_get_height(copy));

        }
    }

    LV_LOG_INFO("Object create ready");
}

static void lv_obj_destructor(lv_obj_t * p)
{
    lv_obj_t * obj = p;
    if(obj->spec_attr) {
        if(obj->spec_attr->children) {
            lv_mem_free(obj->spec_attr->children);
            obj->spec_attr->children = NULL;
        }
        if(obj->spec_attr->event_dsc) {
            lv_mem_free(obj->spec_attr->event_dsc);
            obj->spec_attr->event_dsc = NULL;
        }

        lv_mem_free(obj->spec_attr);
        obj->spec_attr = NULL;
    }

}

static lv_draw_res_t lv_obj_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) return LV_DRAW_RES_MASKED;

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

        if(_lv_area_is_in(clip_area, &coords, r) == false) return LV_DRAW_RES_NOT_COVER;

        if(lv_obj_get_style_bg_opa(obj, LV_PART_MAIN) < LV_OPA_MAX) return LV_DRAW_RES_NOT_COVER;

#if LV_DRAW_COMPLEX
        if(lv_obj_get_style_blend_mode(obj, LV_PART_MAIN) != LV_BLEND_MODE_NORMAL) return LV_DRAW_RES_NOT_COVER;
#endif
        if(lv_obj_get_style_opa(obj, LV_PART_MAIN) < LV_OPA_MAX) return LV_DRAW_RES_NOT_COVER;

        return  LV_DRAW_RES_COVER;

    }
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
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

#if LV_DRAW_COMPLEX
        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) {
            lv_draw_mask_radius_param_t * mp = lv_mem_buf_get(sizeof(lv_draw_mask_radius_param_t));
            lv_coord_t r = lv_obj_get_style_radius(obj, LV_PART_MAIN);
            lv_draw_mask_radius_init(mp, &obj->coords, r, false);
            /*Add the mask and use `obj+8` as custom id. Don't use `obj` directly because it might be used by the user*/
            lv_draw_mask_add(mp, obj + 8);
        }
#endif
    }
    else if(mode == LV_DRAW_MODE_POST_DRAW) {
        draw_scrollbar(obj, clip_area);

#if LV_DRAW_COMPLEX
        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) {
            lv_draw_mask_radius_param_t * param = lv_draw_mask_remove_custom(obj + 8);
            lv_mem_buf_release(param);
        }
#endif

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
    }

    return LV_DRAW_RES_OK;
}

static void draw_scrollbar(lv_obj_t * obj, const lv_area_t * clip_area)
{

    lv_area_t hor_area;
    lv_area_t ver_area;
    lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);

    if(lv_area_get_size(&hor_area) <= 0 && lv_area_get_size(&ver_area) <= 0) return;

    lv_draw_rect_dsc_t draw_dsc;
    lv_res_t sb_res = scrollbar_init_draw_dsc(obj, &draw_dsc);
    if(sb_res != LV_RES_OK) return;

    if(lv_area_get_size(&hor_area) > 0) lv_draw_rect(&hor_area, clip_area, &draw_dsc);
    if(lv_area_get_size(&ver_area) > 0) lv_draw_rect(&ver_area, clip_area, &draw_dsc);
}

/**
 * Initialize the draw descriptor for the scrollbar
 * @param obj pointer to an object
 * @param dsc the draw descriptor to initialize
 * @return LV_RES_OK: the scrollbar is visible; LV_RES_INV: the scrollbar is not visible
 */
static lv_res_t scrollbar_init_draw_dsc(lv_obj_t * obj, lv_draw_rect_dsc_t * dsc)
{
    lv_draw_rect_dsc_init(dsc);
    dsc->bg_opa = lv_obj_get_style_bg_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->bg_opa > LV_OPA_MIN) {
        dsc->bg_color = lv_obj_get_style_bg_color(obj, LV_PART_SCROLLBAR);
    }

    dsc->border_opa = lv_obj_get_style_border_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->border_opa > LV_OPA_MIN) {
        dsc->border_width = lv_obj_get_style_border_width(obj, LV_PART_SCROLLBAR);
        if(dsc->border_width > 0) {
            dsc->border_color = lv_obj_get_style_border_color(obj, LV_PART_SCROLLBAR);
        } else {
            dsc->border_opa = LV_OPA_TRANSP;
        }
    }

#if LV_DRAW_COMPLEX
    lv_opa_t opa = lv_obj_get_style_opa(obj, LV_PART_SCROLLBAR);
    if(opa < LV_OPA_MAX) {
        dsc->bg_opa = (dsc->bg_opa * opa) >> 8;
        dsc->border_opa = (dsc->bg_opa * opa) >> 8;
    }

    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->border_opa != LV_OPA_TRANSP) {
        dsc->radius = lv_obj_get_style_radius(obj, LV_PART_SCROLLBAR);
        return LV_RES_OK;
    } else {
        return LV_RES_INV;
    }
#else
    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->border_opa != LV_OPA_TRANSP) return LV_RES_OK;
    else return LV_RES_INV;
#endif
}


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
            if(!(lv_obj_get_state(obj) & LV_STATE_CHECKED)) lv_obj_add_state(obj, LV_STATE_CHECKED);
            else lv_obj_clear_state(obj, LV_STATE_CHECKED);
        }
    }
    else if(sign == LV_SIGNAL_PRESS_LOST) {
        lv_obj_clear_state(obj, LV_STATE_PRESSED);
    }
    else if(sign == LV_SIGNAL_CONTROL) {
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
    }
    else if(sign == LV_SIGNAL_FOCUS) {
        lv_obj_t * parent = lv_obj_get_parent(obj);
        lv_obj_t * child = obj;
        while(parent && lv_obj_has_flag(child, LV_OBJ_FLAG_SCROLL_ON_FOCUS)) {
            lv_obj_scroll_to_view(child, LV_ANIM_ON);
            child = parent;
            parent = lv_obj_get_parent(parent);
        }

        bool editing = false;
        editing = lv_group_get_editing(lv_obj_get_group(obj));
        lv_state_t state = LV_STATE_FOCUSED;
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if(indev_type == LV_INDEV_TYPE_KEYPAD || indev_type == LV_INDEV_TYPE_ENCODER) state |= LV_STATE_FOCUS_KEY;
        if(editing) {
            state |= LV_STATE_EDITED;

            /*if using focus mode, change target to parent*/
            obj = lv_obj_get_focused_obj(obj);

            lv_obj_add_state(obj, state);
        }
        else {
            /*if using focus mode, change target to parent*/
            obj = lv_obj_get_focused_obj(obj);

            lv_obj_add_state(obj, state);
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
        obj = lv_obj_get_focused_obj(obj);

        lv_obj_clear_state(obj, LV_STATE_FOCUSED | LV_STATE_EDITED | LV_STATE_FOCUS_KEY);
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
            lv_obj_update_layout(obj, NULL);
        }
    }
    else if(sign == LV_SIGNAL_CHILD_CHG) {
        lv_obj_update_layout(obj, param);

        if(obj->w_set == LV_SIZE_CONTENT || obj->h_set == LV_SIZE_CONTENT) {
            lv_obj_set_size(obj, obj->w_set, obj->h_set);
        }
    }
    else if(sign == LV_SIGNAL_BASE_DIR_CHG) {
        /* The layout might depend on the base dir.
         * E.g. the first is element is on the left or right*/
        lv_obj_update_layout(obj, NULL);
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
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_SIZE) {
        lv_coord_t * s = param;
        lv_coord_t d = lv_obj_calculate_ext_draw_size(obj, LV_PART_MAIN);
        *s = LV_MAX(*s, d);
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        /* Padding might have changed so the layout should be recalculated*/
        lv_obj_update_layout(obj, NULL);

        /*Reposition non grid objects on by one*/
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
           lv_obj_t * child = lv_obj_get_child(obj, i);
            if(LV_COORD_IS_PX(child->x_set) || LV_COORD_IS_PX(child->y_set)) {
                lv_obj_set_pos(child, child->x_set, child->y_set);
            }
        }

        if(obj->w_set == LV_SIZE_CONTENT || obj->h_set == LV_SIZE_CONTENT) {
            lv_obj_set_size(obj, obj->w_set, obj->h_set);
        }
        lv_obj_refresh_ext_draw_size(obj);
    }
    return res;
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


static bool obj_valid_child(const lv_obj_t * parent, const lv_obj_t * obj_to_find)
{

    /*Check all children of `parent`*/
    uint32_t child_cnt = 0;
    if(parent->spec_attr) child_cnt = parent->spec_attr->child_cnt;
    uint32_t i;
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = parent->spec_attr->children[i];
        if(child == obj_to_find) {
            return true;
        }

        /*Check the children*/
        bool found = obj_valid_child(child, obj_to_find);
        if(found) {
            return true;
        }
    }
    return false;
}
