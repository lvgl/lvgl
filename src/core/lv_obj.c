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
#include "../misc/lv_assert.h"
#include "../draw/lv_draw.h"
#include "../misc/lv_anim.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_async.h"
#include "../misc/lv_fs.h"
#include "../misc/lv_gc.h"
#include "../misc/lv_math.h"
#include "../misc/lv_log.h"
#include "../hal/lv_hal.h"
#include "../extra/lv_extra.h"
#include <stdint.h>
#include <string.h>

#if LV_USE_GPU_STM32_DMA2D
    #include "../gpu/lv_gpu_stm32_dma2d.h"
#endif

#if LV_USE_GPU_NXP_PXP && LV_USE_GPU_NXP_PXP_AUTO_INIT
    #include "../gpu/lv_gpu_nxp_pxp.h"
    #include "../gpu/lv_gpu_nxp_pxp_osa.h"
#endif

#if LV_USE_GPU_SDL
    #include "../gpu/lv_gpu_sdl.h"
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

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_obj_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_obj_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_obj_draw(lv_event_t * e);
static void lv_obj_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_scrollbar(lv_obj_t * obj, const lv_area_t * clip_area);
static lv_res_t scrollbar_init_draw_dsc(lv_obj_t * obj, lv_draw_rect_dsc_t * dsc);
static bool obj_valid_child(const lv_obj_t * parent, const lv_obj_t * obj_to_find);
static void lv_obj_set_state(lv_obj_t * obj, lv_state_t new_state);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool lv_initialized = false;
const lv_obj_class_t lv_obj_class = {
    .constructor_cb = lv_obj_constructor,
    .destructor_cb = lv_obj_destructor,
    .event_cb = lv_obj_event,
    .width_def = LV_DPI_DEF,
    .height_def = LV_DPI_DEF,
    .editable = LV_OBJ_CLASS_EDITABLE_FALSE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_FALSE,
    .instance_size = (sizeof(lv_obj_t)),
    .base_class = NULL,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_is_initialized(void)
{
    return lv_initialized;
}

void lv_init(void)
{
    /*Do nothing if already initialized*/
    if(lv_initialized) {
        LV_LOG_WARN("lv_init: already inited");
        return;
    }

    LV_LOG_INFO("begin");

    /*Initialize the misc modules*/
    lv_mem_init();

    _lv_timer_core_init();

    _lv_fs_init();

    _lv_anim_core_init();

    _lv_group_init();

#if LV_USE_GPU_STM32_DMA2D
    /*Initialize DMA2D GPU*/
    lv_gpu_stm32_dma2d_init();
#endif

#if LV_USE_GPU_NXP_PXP && LV_USE_GPU_NXP_PXP_AUTO_INIT
    if(lv_gpu_nxp_pxp_init(&pxp_default_cfg) != LV_RES_OK) {
        LV_LOG_ERROR("PXP init error. STOP.\n");
        for(; ;) ;
    }
#endif
#if LV_USE_GPU_SDL
    lv_gpu_sdl_init();
#endif

    _lv_obj_style_init();
    _lv_ll_init(&LV_GC_ROOT(_lv_disp_ll), sizeof(lv_disp_t));
    _lv_ll_init(&LV_GC_ROOT(_lv_indev_ll), sizeof(lv_indev_t));

    /*Initialize the screen refresh system*/
    _lv_refr_init();

    _lv_img_decoder_init();
#if LV_IMG_CACHE_DEF_SIZE
    lv_img_cache_set_size(LV_IMG_CACHE_DEF_SIZE);
#endif
    /*Test if the IDE has UTF-8 encoding*/
    char * txt = "Á";

    uint8_t * txt_u8 = (uint8_t *)txt;
    if(txt_u8[0] != 0xc3 || txt_u8[1] != 0x81 || txt_u8[2] != 0x00) {
        LV_LOG_WARN("The strings has no UTF-8 encoding. Non-ASCII characters won't be displayed.");
    }

    uint32_t endianess_test = 0x11223344;
    uint8_t * endianess_test_p = (uint8_t *) &endianess_test;
    bool big_endian = endianess_test_p[0] == 0x11 ? true : false;

    if(big_endian) {
        LV_ASSERT_MSG(LV_BIG_ENDIAN_SYSTEM == 1,
                      "It's a big endian system but LV_BIG_ENDIAN_SYSTEM is not enabled in lv_conf.h");
    }
    else {
        LV_ASSERT_MSG(LV_BIG_ENDIAN_SYSTEM == 0,
                      "It's a little endian system but LV_BIG_ENDIAN_SYSTEM is enabled in lv_conf.h");
    }

#if LV_USE_ASSERT_MEM_INTEGRITY
    LV_LOG_WARN("Memory integrity checks are enabled via LV_USE_ASSERT_MEM_INTEGRITY which makes LVGL much slower");
#endif

#if LV_USE_ASSERT_OBJ
    LV_LOG_WARN("Object sanity checks are enabled via LV_USE_ASSERT_OBJ which makes LVGL much slower");
#endif

#if LV_USE_ASSERT_STYLE
    LV_LOG_WARN("Style sanity checks are enabled that uses more RAM");
#endif

#if LV_LOG_LEVEL == LV_LOG_LEVEL_TRACE
    LV_LOG_WARN("Log level is set the Trace which makes LVGL much slower");
#endif

    lv_extra_init();

    lv_initialized = true;

    LV_LOG_TRACE("finished");
}

#if LV_ENABLE_GC || !LV_MEM_CUSTOM || LV_USE_GPU_SDL

void lv_deinit(void)
{
#if LV_USE_GPU_SDL
    lv_gpu_sdl_deinit();
#endif
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

lv_obj_t * lv_obj_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
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

    bool was_on_layout = lv_obj_is_layout_positioned(obj);

    if(f & LV_OBJ_FLAG_HIDDEN) lv_obj_invalidate(obj);

    obj->flags |= f;

    if(f & LV_OBJ_FLAG_HIDDEN) {
        lv_obj_invalidate(obj);
    }

    if((was_on_layout != lv_obj_is_layout_positioned(obj)) || (f & (LV_OBJ_FLAG_LAYOUT_1 |  LV_OBJ_FLAG_LAYOUT_2))) {
        lv_obj_mark_layout_as_dirty(lv_obj_get_parent(obj));
    }

    if(f & LV_OBJ_FLAG_SCROLLABLE) {
        lv_area_t hor_area, ver_area;
        lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);
        lv_obj_invalidate_area(obj, &hor_area);
        lv_obj_invalidate_area(obj, &ver_area);
    }
}

void lv_obj_clear_flag(lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    bool was_on_layout = lv_obj_is_layout_positioned(obj);
    if(f & LV_OBJ_FLAG_SCROLLABLE) {
        lv_area_t hor_area, ver_area;
        lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);
        lv_obj_invalidate_area(obj, &hor_area);
        lv_obj_invalidate_area(obj, &ver_area);
    }

    obj->flags &= (~f);

    if(f & LV_OBJ_FLAG_HIDDEN) {
        lv_obj_invalidate(obj);
        if(lv_obj_is_layout_positioned(obj)) {
            lv_obj_mark_layout_as_dirty(lv_obj_get_parent(obj));
        }
    }

    if((was_on_layout != lv_obj_is_layout_positioned(obj)) || (f & (LV_OBJ_FLAG_LAYOUT_1 |  LV_OBJ_FLAG_LAYOUT_2))) {
        lv_obj_mark_layout_as_dirty(lv_obj_get_parent(obj));
    }
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

/*=======================
 * Getter functions
 *======================*/

bool lv_obj_has_flag(const lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return (obj->flags & f)  == f ? true : false;
}

bool lv_obj_has_flag_any(const lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return (obj->flags & f) ? true : false;
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
        obj->spec_attr = lv_mem_alloc(sizeof(_lv_obj_spec_attr_t));
        LV_ASSERT_MALLOC(obj->spec_attr);
        if(obj->spec_attr == NULL) return;

        lv_memset_00(obj->spec_attr, sizeof(_lv_obj_spec_attr_t));

        obj->spec_attr->scroll_dir = LV_DIR_ALL;
        obj->spec_attr->scrollbar_mode = LV_SCROLLBAR_MODE_AUTO;
    }
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

const lv_obj_class_t * lv_obj_get_class(const lv_obj_t * obj)
{
    return obj->class_p;
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

static void lv_obj_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_t * parent = obj->parent;
    if(parent) {
        lv_coord_t sl = lv_obj_get_scroll_left(parent);
        lv_coord_t st = lv_obj_get_scroll_top(parent);

        obj->coords.y1 = parent->coords.y1 + lv_obj_get_style_pad_top(parent, LV_PART_MAIN) - st;
        obj->coords.y2 = obj->coords.y1 - 1;
        obj->coords.x1  = parent->coords.x1 + lv_obj_get_style_pad_left(parent, LV_PART_MAIN) - sl;
        obj->coords.x2  = obj->coords.x1 - 1;
    }

    /*Set attributes*/
    obj->flags = LV_OBJ_FLAG_CLICKABLE;
    obj->flags |= LV_OBJ_FLAG_SNAPPABLE;
    if(parent) obj->flags |= LV_OBJ_FLAG_PRESS_LOCK;
    if(parent) obj->flags |= LV_OBJ_FLAG_SCROLL_CHAIN;
    obj->flags |= LV_OBJ_FLAG_CLICK_FOCUSABLE;
    obj->flags |= LV_OBJ_FLAG_SCROLLABLE;
    obj->flags |= LV_OBJ_FLAG_SCROLL_ELASTIC;
    obj->flags |= LV_OBJ_FLAG_SCROLL_MOMENTUM;
    if(parent) obj->flags |= LV_OBJ_FLAG_GESTURE_BUBBLE;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_obj_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    _lv_event_mark_deleted(obj);

    /*Remove all style*/
    lv_obj_enable_style_refresh(false); /*No need to refresh the style because the object will be deleted*/
    lv_obj_remove_style_all(obj);
    lv_obj_enable_style_refresh(true);

    /*Remove the animations from this object*/
    lv_anim_del(obj, NULL);

    /*Delete from the group*/
    lv_group_t * group = lv_obj_get_group(obj);
    if(group) lv_group_remove_obj(obj);

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

static void lv_obj_draw(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t * info = lv_event_get_param(e);
        if(info->res == LV_COVER_RES_MASKED) return;
        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) {
            info->res = LV_COVER_RES_MASKED;
            return;
        }

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

        if(_lv_area_is_in(info->area, &coords, r) == false) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        if(lv_obj_get_style_bg_opa(obj, LV_PART_MAIN) < LV_OPA_MAX) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

#if LV_DRAW_COMPLEX
        if(lv_obj_get_style_blend_mode(obj, LV_PART_MAIN) != LV_BLEND_MODE_NORMAL) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }
#endif
        if(lv_obj_get_style_opa(obj, LV_PART_MAIN) < LV_OPA_MAX) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        info->res = LV_COVER_RES_COVER;

    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        const lv_area_t * clip_area = lv_event_get_param(e);
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

        lv_obj_draw_part_dsc_t part_dsc;
        lv_obj_draw_dsc_init(&part_dsc, clip_area);
        part_dsc.class_p = MY_CLASS;
        part_dsc.type = LV_OBJ_DRAW_PART_RECTANGLE;
        part_dsc.rect_dsc = &draw_dsc;
        part_dsc.draw_area = &coords;
        part_dsc.part = LV_PART_MAIN;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_dsc);

        lv_draw_rect(&coords, clip_area, &draw_dsc);

        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_dsc);

#if LV_DRAW_COMPLEX
        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) {
            /*If the radius is 0 the parent's coordinates will clip anyway*/
            lv_coord_t r = lv_obj_get_style_radius(obj, LV_PART_MAIN);
            if(r != 0) {
                lv_draw_mask_radius_param_t * mp = lv_mem_buf_get(sizeof(lv_draw_mask_radius_param_t));
                lv_draw_mask_radius_init(mp, &obj->coords, r, false);
                /*Add the mask and use `obj+8` as custom id. Don't use `obj` directly because it might be used by the user*/
                lv_draw_mask_add(mp, obj + 8);
            }
        }
#endif
    }
    else if(code == LV_EVENT_DRAW_POST) {
        const lv_area_t * clip_area = lv_event_get_param(e);
        draw_scrollbar(obj, clip_area);

#if LV_DRAW_COMPLEX
        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) {
            lv_draw_mask_radius_param_t * param = lv_draw_mask_remove_custom(obj + 8);
            if(param) {
                lv_draw_mask_free_param(param);
                lv_mem_buf_release(param);
            }
        }
#endif

        /*If the border is drawn later disable loading other properties*/
        if(lv_obj_get_style_border_post(obj, LV_PART_MAIN)) {
            lv_draw_rect_dsc_t draw_dsc;
            lv_draw_rect_dsc_init(&draw_dsc);
            draw_dsc.bg_opa = LV_OPA_TRANSP;
            draw_dsc.outline_opa = LV_OPA_TRANSP;
            draw_dsc.shadow_opa = LV_OPA_TRANSP;
            draw_dsc.bg_img_opa = LV_OPA_TRANSP;
            lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &draw_dsc);

            lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
            lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
            lv_area_t coords;
            lv_area_copy(&coords, &obj->coords);
            coords.x1 -= w;
            coords.x2 += w;
            coords.y1 -= h;
            coords.y2 += h;

            lv_obj_draw_part_dsc_t part_dsc;
            lv_obj_draw_dsc_init(&part_dsc, clip_area);
            part_dsc.class_p = MY_CLASS;
            part_dsc.type = LV_OBJ_DRAW_PART_BORDER_POST;
            part_dsc.rect_dsc = &draw_dsc;
            part_dsc.draw_area = &coords;
            part_dsc.part = LV_PART_MAIN;
            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_dsc);

            lv_draw_rect(&coords, clip_area, &draw_dsc);
            lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_dsc);
        }
    }
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

    lv_obj_draw_part_dsc_t part_dsc;
    lv_obj_draw_dsc_init(&part_dsc, clip_area);
    part_dsc.class_p = MY_CLASS;
    part_dsc.type = LV_OBJ_DRAW_PART_SCROLLBAR;
    part_dsc.rect_dsc = &draw_dsc;
    part_dsc.part = LV_PART_SCROLLBAR;

    if(lv_area_get_size(&hor_area) > 0) {
        part_dsc.draw_area = &hor_area;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_dsc);
        lv_draw_rect(&hor_area, clip_area, &draw_dsc);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_dsc);
    }
    if(lv_area_get_size(&ver_area) > 0) {
        part_dsc.draw_area = &ver_area;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_dsc);
        lv_draw_rect(&ver_area, clip_area, &draw_dsc);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_dsc);
    }
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
        }
        else {
            dsc->border_opa = LV_OPA_TRANSP;
        }
    }

#if LV_DRAW_COMPLEX
    dsc->shadow_opa = lv_obj_get_style_shadow_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->shadow_opa > LV_OPA_MIN) {
        dsc->shadow_width = lv_obj_get_style_shadow_width(obj, LV_PART_SCROLLBAR);
        if(dsc->shadow_width > 0) {
            dsc->shadow_spread = lv_obj_get_style_shadow_spread(obj, LV_PART_SCROLLBAR);
            dsc->shadow_color = lv_obj_get_style_shadow_color(obj, LV_PART_SCROLLBAR);
        }
        else {
            dsc->shadow_opa = LV_OPA_TRANSP;
        }
    }

    lv_opa_t opa = lv_obj_get_style_opa(obj, LV_PART_SCROLLBAR);
    if(opa < LV_OPA_MAX) {
        dsc->bg_opa = (dsc->bg_opa * opa) >> 8;
        dsc->border_opa = (dsc->bg_opa * opa) >> 8;
        dsc->shadow_opa = (dsc->bg_opa * opa) >> 8;
    }

    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->border_opa != LV_OPA_TRANSP || dsc->shadow_opa != LV_OPA_TRANSP) {
        dsc->radius = lv_obj_get_style_radius(obj, LV_PART_SCROLLBAR);
        return LV_RES_OK;
    }
    else {
        return LV_RES_INV;
    }
#else
    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->border_opa != LV_OPA_TRANSP) return LV_RES_OK;
    else return LV_RES_INV;
#endif
}

static void lv_obj_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_PRESSED) {
        lv_obj_add_state(obj, LV_STATE_PRESSED);
    }
    else if(code == LV_EVENT_RELEASED) {
        lv_obj_clear_state(obj, LV_STATE_PRESSED);
        void * param = lv_event_get_param(e);
        /*Go the checked state if enabled*/
        if(lv_indev_get_scroll_obj(param) == NULL && lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE)) {
            if(!(lv_obj_get_state(obj) & LV_STATE_CHECKED)) lv_obj_add_state(obj, LV_STATE_CHECKED);
            else lv_obj_clear_state(obj, LV_STATE_CHECKED);

            lv_res_t res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
    }
    else if(code == LV_EVENT_PRESS_LOST) {
        lv_obj_clear_state(obj, LV_STATE_PRESSED);
    }
    else if(code == LV_EVENT_STYLE_CHANGED) {
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);
        for(uint32_t i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            lv_obj_mark_layout_as_dirty(child);
        }
    }
    else if(code == LV_EVENT_KEY) {
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE)) {
            char c = *((char *)lv_event_get_param(e));
            if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
                lv_obj_add_state(obj, LV_STATE_CHECKED);
            }
            else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
                lv_obj_clear_state(obj, LV_STATE_CHECKED);
            }

            /*With Enter LV_EVENT_RELEASED will send VALUE_CHANGE event*/
            if(c != LV_KEY_ENTER) {
                lv_res_t res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return;
            }
        }
        else if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE) && !lv_obj_is_editable(obj)) {
            /*scroll by keypad or encoder*/
            lv_anim_enable_t anim_enable = LV_ANIM_OFF;
            lv_coord_t sl = lv_obj_get_scroll_left(obj);
            lv_coord_t sr = lv_obj_get_scroll_right(obj);
            char c = *((char *)lv_event_get_param(e));
            if(c == LV_KEY_DOWN) {
                /*use scroll_to_x/y functions to enforce scroll limits*/
                lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) + lv_obj_get_height(obj) / 4, anim_enable);
            }
            else if(c == LV_KEY_UP) {
                lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) - lv_obj_get_height(obj) / 4, anim_enable);
            }
            else if(c == LV_KEY_RIGHT) {
                /*If the object can't be scrolled horizontally then scroll it vertically*/
                if(!((lv_obj_get_scroll_dir(obj) & LV_DIR_HOR) && (sl > 0 || sr > 0)))
                    lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) + lv_obj_get_height(obj) / 4, anim_enable);
                else
                    lv_obj_scroll_to_x(obj, lv_obj_get_scroll_x(obj) + lv_obj_get_width(obj) / 4, anim_enable);
            }
            else if(c == LV_KEY_LEFT) {
                /*If the object can't be scrolled horizontally then scroll it vertically*/
                if(!((lv_obj_get_scroll_dir(obj) & LV_DIR_HOR) && (sl > 0 || sr > 0)))
                    lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) - lv_obj_get_height(obj) / 4, anim_enable);
                else
                    lv_obj_scroll_to_x(obj, lv_obj_get_scroll_x(obj) - lv_obj_get_width(obj) / 4, anim_enable);
            }
        }
    }
    else if(code == LV_EVENT_FOCUSED) {
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS)) {
            lv_obj_scroll_to_view_recursive(obj, LV_ANIM_ON);
        }

        bool editing = false;
        editing = lv_group_get_editing(lv_obj_get_group(obj));
        lv_state_t state = LV_STATE_FOCUSED;

        /* Use the indev for then indev handler.
         * But if the obj was focused manually it returns NULL so try to
         * use the indev from the event*/
        lv_indev_t * indev = lv_indev_get_act();
        if(indev == NULL) indev = lv_event_get_indev(e);

        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type == LV_INDEV_TYPE_KEYPAD || indev_type == LV_INDEV_TYPE_ENCODER) state |= LV_STATE_FOCUS_KEY;
        if(editing) {
            state |= LV_STATE_EDITED;
            lv_obj_add_state(obj, state);
        }
        else {
            lv_obj_add_state(obj, state);
            lv_obj_clear_state(obj, LV_STATE_EDITED);
        }
    }
    else if(code == LV_EVENT_SCROLL_BEGIN) {
        lv_obj_add_state(obj, LV_STATE_SCROLLED);
    }
    else if(code == LV_EVENT_SCROLL_END) {
        lv_obj_clear_state(obj, LV_STATE_SCROLLED);
        if(lv_obj_get_scrollbar_mode(obj) == LV_SCROLLBAR_MODE_ACTIVE) {
            lv_area_t hor_area, ver_area;
            lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);
            lv_obj_invalidate_area(obj, &hor_area);
            lv_obj_invalidate_area(obj, &ver_area);
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_obj_clear_state(obj, LV_STATE_FOCUSED | LV_STATE_EDITED | LV_STATE_FOCUS_KEY);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        lv_coord_t align = lv_obj_get_style_align(obj, LV_PART_MAIN);
        uint16_t layout = lv_obj_get_style_layout(obj, LV_PART_MAIN);
        if(layout || align) {
            lv_obj_mark_layout_as_dirty(obj);
        }

        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            lv_obj_mark_layout_as_dirty(child);
        }
    }
    else if(code == LV_EVENT_CHILD_CHANGED) {
        lv_coord_t w = lv_obj_get_style_width(obj, LV_PART_MAIN);
        lv_coord_t h = lv_obj_get_style_height(obj, LV_PART_MAIN);
        lv_coord_t align = lv_obj_get_style_align(obj, LV_PART_MAIN);
        uint16_t layout = lv_obj_get_style_layout(obj, LV_PART_MAIN);
        if(layout || align || w == LV_SIZE_CONTENT || h == LV_SIZE_CONTENT) {
            lv_obj_mark_layout_as_dirty(obj);
        }
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t * s = lv_event_get_param(e);
        lv_coord_t d = lv_obj_calculate_ext_draw_size(obj, LV_PART_MAIN);
        *s = LV_MAX(*s, d);
    }
    else if(code == LV_EVENT_DRAW_MAIN || code == LV_EVENT_DRAW_POST || code == LV_EVENT_COVER_CHECK) {
        lv_obj_draw(e);
    }
}

/**
 * Set the state (fully overwrite) of an object.
 * If specified in the styles, transition animations will be started from the previous state to the current.
 * @param obj       pointer to an object
 * @param state     the new state
 */
static void lv_obj_set_state(lv_obj_t * obj, lv_state_t new_state)
{
    if(obj->state == new_state) return;

    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_state_t prev_state = obj->state;
    obj->state = new_state;

    _lv_style_state_cmp_t cmp_res = _lv_obj_style_state_compare(obj, prev_state, new_state);
    /*If there is no difference in styles there is nothing else to do*/
    if(cmp_res == _LV_STYLE_STATE_CMP_SAME) return;

    _lv_obj_style_transition_dsc_t * ts = lv_mem_buf_get(sizeof(_lv_obj_style_transition_dsc_t) * STYLE_TRANSITION_MAX);
    lv_memset_00(ts, sizeof(_lv_obj_style_transition_dsc_t) * STYLE_TRANSITION_MAX);
    uint32_t tsi = 0;
    uint32_t i;
    for(i = 0; i < obj->style_cnt && tsi < STYLE_TRANSITION_MAX; i++) {
        _lv_obj_style_t * obj_style = &obj->styles[i];
        lv_state_t state_act = lv_obj_style_get_selector_state(obj->styles[i].selector);
        lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);
        if(state_act & (~new_state)) continue; /*Skip unrelated styles*/
        if(obj_style->is_trans) continue;

        lv_style_value_t v;
        if(lv_style_get_prop_inlined(obj_style->style, LV_STYLE_TRANSITION, &v) == false) continue;
        const lv_style_transition_dsc_t * tr = v.ptr;

        /*Add the props to the set if not added yet or added but with smaller weight*/
        uint32_t j;
        for(j = 0; tr->props[j] != 0 && tsi < STYLE_TRANSITION_MAX; j++) {
            uint32_t t;
            for(t = 0; t < tsi; t++) {
                lv_style_selector_t selector = ts[t].selector;
                lv_state_t state_ts = lv_obj_style_get_selector_state(selector);
                lv_part_t part_ts = lv_obj_style_get_selector_part(selector);
                if(ts[t].prop == tr->props[j] && part_ts == part_act && state_ts >= state_act) break;
            }

            /*If not found  add it*/
            if(t == tsi) {
                ts[tsi].time = tr->time;
                ts[tsi].delay = tr->delay;
                ts[tsi].path_cb = tr->path_xcb;
                ts[tsi].prop = tr->props[j];
#if LV_USE_USER_DATA
                ts[tsi].user_data = tr->user_data;
#endif
                ts[tsi].selector = obj_style->selector;
                tsi++;
            }
        }
    }

    for(i = 0; i < tsi; i++) {
        lv_part_t part_act = lv_obj_style_get_selector_part(ts[i].selector);
        _lv_obj_style_create_transition(obj, part_act, prev_state, new_state, &ts[i]);
    }

    lv_mem_buf_release(ts);

    if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_REDRAW) {
        lv_obj_invalidate(obj);
    }
    else if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_LAYOUT) {
        lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY);
    }
    else if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD) {
        lv_obj_invalidate(obj);
        lv_obj_refresh_ext_draw_size(obj);
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
