/**
 * @file lv_imagebutton.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_imagebutton.h"

#if LV_USE_IMAGEBUTTON != 0

#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_imagebutton_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_imagebutton_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void draw_main(lv_event_t * e);
static void lv_imagebutton_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void refr_image(lv_obj_t * imagebutton);
static lv_imagebutton_state_t suggest_state(lv_obj_t * imagebutton, lv_imagebutton_state_t state);
static lv_imagebutton_state_t get_state(const lv_obj_t * imagebutton);
static void update_src_info(lv_imagebutton_src_info_t * info, const void * src);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_imagebutton_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_imagebutton_t),
    .constructor_cb = lv_imagebutton_constructor,
    .event_cb = lv_imagebutton_event,
    .name = "imagebutton",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_imagebutton_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_imagebutton_set_src(lv_obj_t * obj, lv_imagebutton_state_t state, const void * src_left, const void * src_mid,
                            const void * src_right)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_imagebutton_t * imagebutton = (lv_imagebutton_t *)obj;

    update_src_info(&imagebutton->src_left[state], src_left);
    update_src_info(&imagebutton->src_mid[state], src_mid);
    update_src_info(&imagebutton->src_right[state], src_right);

    refr_image(obj);
}

void lv_imagebutton_set_state(lv_obj_t * obj, lv_imagebutton_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_state_t obj_state = LV_STATE_DEFAULT;
    if(state == LV_IMAGEBUTTON_STATE_PRESSED ||
       state == LV_IMAGEBUTTON_STATE_CHECKED_PRESSED) obj_state |= LV_STATE_PRESSED;
    if(state == LV_IMAGEBUTTON_STATE_DISABLED ||
       state == LV_IMAGEBUTTON_STATE_CHECKED_DISABLED) obj_state |= LV_STATE_DISABLED;
    if(state == LV_IMAGEBUTTON_STATE_CHECKED_DISABLED || state == LV_IMAGEBUTTON_STATE_CHECKED_PRESSED ||
       state == LV_IMAGEBUTTON_STATE_CHECKED_RELEASED) {
        obj_state |= LV_STATE_CHECKED;
    }

    lv_obj_remove_state(obj, LV_STATE_CHECKED | LV_STATE_PRESSED | LV_STATE_DISABLED);
    lv_obj_add_state(obj, obj_state);

    refr_image(obj);
}

/*=====================
 * Getter functions
 *====================*/

const void * lv_imagebutton_get_src_left(lv_obj_t * obj, lv_imagebutton_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_imagebutton_t * imagebutton = (lv_imagebutton_t *)obj;

    return imagebutton->src_left[state].img_src;
}

const void * lv_imagebutton_get_src_middle(lv_obj_t * obj, lv_imagebutton_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imagebutton_t * imagebutton = (lv_imagebutton_t *)obj;

    return imagebutton->src_mid[state].img_src;
}

const void * lv_imagebutton_get_src_right(lv_obj_t * obj, lv_imagebutton_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imagebutton_t * imagebutton = (lv_imagebutton_t *)obj;

    return imagebutton->src_right[state].img_src;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_imagebutton_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_imagebutton_t * imagebutton = (lv_imagebutton_t *)obj;
    /*Initialize the allocated 'ext'*/

    lv_memzero(&imagebutton->src_mid, sizeof(imagebutton->src_mid));
    lv_memzero(&imagebutton->src_left, sizeof(imagebutton->src_left));
    lv_memzero(&imagebutton->src_right, sizeof(imagebutton->src_right));
}

static void lv_imagebutton_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res = lv_obj_event_base(&lv_imagebutton_class, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_PRESSED || code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        refr_image(obj);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }
    else if(code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t * info = lv_event_get_param(e);
        if(info->res != LV_COVER_RES_MASKED) info->res = LV_COVER_RES_NOT_COVER;
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_self_size_info(e);
        lv_imagebutton_t * imagebutton = (lv_imagebutton_t *)obj;
        lv_imagebutton_state_t state  = suggest_state(obj, get_state(obj));
        if(imagebutton->src_left[state].img_src == NULL &&
           imagebutton->src_mid[state].img_src != NULL &&
           imagebutton->src_right[state].img_src == NULL) {
            p->x = LV_MAX(p->x, imagebutton->src_mid[state].header.w);
        }
    }
}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_imagebutton_t * imagebutton = (lv_imagebutton_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(e);

    /*Just draw_main an image*/
    lv_imagebutton_state_t state  = suggest_state(obj, get_state(obj));

    /*Simply draw the middle src if no tiled*/
    lv_imagebutton_src_info_t * src_info = &imagebutton->src_left[state];

    int32_t tw = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
    int32_t th = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
    lv_area_t coords;
    lv_area_copy(&coords, &obj->coords);
    lv_area_increase(&coords, tw, th);

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    lv_obj_init_draw_image_dsc(obj, LV_PART_MAIN, &img_dsc);

    lv_area_t coords_part;
    int32_t left_w = 0;
    int32_t right_w = 0;

    if(src_info->img_src) {
        left_w = src_info->header.w;
        coords_part.x1 = coords.x1;
        coords_part.y1 = coords.y1;
        coords_part.x2 = coords.x1 + src_info->header.w - 1;
        coords_part.y2 = coords.y1 + src_info->header.h - 1;
        img_dsc.src = src_info->img_src;
        lv_draw_image(layer, &img_dsc, &coords_part);
    }

    src_info = &imagebutton->src_right[state];
    if(src_info->img_src) {
        right_w = src_info->header.w;
        coords_part.x1 = coords.x2 - src_info->header.w + 1;
        coords_part.y1 = coords.y1;
        coords_part.x2 = coords.x2;
        coords_part.y2 = coords.y1 + src_info->header.h - 1;
        img_dsc.src = src_info->img_src;
        lv_draw_image(layer, &img_dsc, &coords_part);
    }

    src_info = &imagebutton->src_mid[state];
    if(src_info->img_src) {
        lv_area_t clip_area_center;
        clip_area_center.x1 = coords.x1 + left_w;
        clip_area_center.x2 = coords.x2 - right_w;
        clip_area_center.y1 = coords.y1;
        clip_area_center.y2 = coords.y2;

        bool comm_res;
        comm_res = _lv_area_intersect(&clip_area_center, &clip_area_center, &layer->_clip_area);
        if(comm_res) {
            int32_t i;

            const lv_area_t clip_area_ori = layer->_clip_area;
            layer->_clip_area = clip_area_center;

            coords_part.x1 = coords.x1 + left_w;
            coords_part.y1 = coords.y1;
            coords_part.x2 = coords_part.x1 + src_info->header.w - 1;
            coords_part.y2 = coords_part.y1 + src_info->header.h - 1;

            for(i = coords_part.x1; i < (int32_t)(clip_area_center.x2 + src_info->header.w - 1); i += src_info->header.w) {
                img_dsc.src = src_info->img_src;
                lv_draw_image(layer, &img_dsc, &coords_part);
                coords_part.x1 = coords_part.x2 + 1;
                coords_part.x2 += src_info->header.w;
            }
            layer->_clip_area = clip_area_ori;
        }
    }
}

static void refr_image(lv_obj_t * obj)
{
    lv_imagebutton_t * imagebutton = (lv_imagebutton_t *)obj;
    lv_imagebutton_state_t state  = suggest_state(obj, get_state(obj));

    const void * src = imagebutton->src_mid[state].img_src;
    if(src == NULL) return;

    lv_obj_refresh_self_size(obj);
    lv_obj_set_height(obj, imagebutton->src_mid[state].header.h); /*Keep the user defined width*/

    lv_obj_invalidate(obj);
}

/**
 * If `src` is not defined for the current state try to get a state which is related to the current but has `src`.
 * E.g. if the PRESSED src is not set but the RELEASED does, use the RELEASED.
 * @param imagebutton pointer to an image button
 * @param state the state to convert
 * @return the suggested state
 */
static lv_imagebutton_state_t suggest_state(lv_obj_t * obj, lv_imagebutton_state_t state)
{
    lv_imagebutton_t * imagebutton = (lv_imagebutton_t *)obj;
    if(imagebutton->src_mid[state].img_src == NULL) {
        switch(state) {
            case LV_IMAGEBUTTON_STATE_PRESSED:
                if(imagebutton->src_mid[LV_IMAGEBUTTON_STATE_RELEASED].img_src) return LV_IMAGEBUTTON_STATE_RELEASED;
                break;
            case LV_IMAGEBUTTON_STATE_CHECKED_RELEASED:
                if(imagebutton->src_mid[LV_IMAGEBUTTON_STATE_RELEASED].img_src) return LV_IMAGEBUTTON_STATE_RELEASED;
                break;
            case LV_IMAGEBUTTON_STATE_CHECKED_PRESSED:
                if(imagebutton->src_mid[LV_IMAGEBUTTON_STATE_CHECKED_RELEASED].img_src) return LV_IMAGEBUTTON_STATE_CHECKED_RELEASED;
                if(imagebutton->src_mid[LV_IMAGEBUTTON_STATE_PRESSED].img_src) return LV_IMAGEBUTTON_STATE_PRESSED;
                if(imagebutton->src_mid[LV_IMAGEBUTTON_STATE_RELEASED].img_src) return LV_IMAGEBUTTON_STATE_RELEASED;
                break;
            case LV_IMAGEBUTTON_STATE_DISABLED:
                if(imagebutton->src_mid[LV_IMAGEBUTTON_STATE_RELEASED].img_src) return LV_IMAGEBUTTON_STATE_RELEASED;
                break;
            case LV_IMAGEBUTTON_STATE_CHECKED_DISABLED:
                if(imagebutton->src_mid[LV_IMAGEBUTTON_STATE_CHECKED_RELEASED].img_src) return LV_IMAGEBUTTON_STATE_CHECKED_RELEASED;
                if(imagebutton->src_mid[LV_IMAGEBUTTON_STATE_RELEASED].img_src) return LV_IMAGEBUTTON_STATE_RELEASED;
                break;
            default:
                break;
        }
    }

    return state;
}

static lv_imagebutton_state_t get_state(const lv_obj_t * imagebutton)
{
    LV_ASSERT_OBJ(imagebutton, MY_CLASS);

    lv_state_t obj_state = lv_obj_get_state(imagebutton);

    if(obj_state & LV_STATE_DISABLED) {
        if(obj_state & LV_STATE_CHECKED) return LV_IMAGEBUTTON_STATE_CHECKED_DISABLED;
        else return LV_IMAGEBUTTON_STATE_DISABLED;
    }

    if(obj_state & LV_STATE_CHECKED) {
        if(obj_state & LV_STATE_PRESSED) return LV_IMAGEBUTTON_STATE_CHECKED_PRESSED;
        else return LV_IMAGEBUTTON_STATE_CHECKED_RELEASED;
    }
    else {
        if(obj_state & LV_STATE_PRESSED) return LV_IMAGEBUTTON_STATE_PRESSED;
        else return LV_IMAGEBUTTON_STATE_RELEASED;
    }
}

static void update_src_info(lv_imagebutton_src_info_t * info, const void * src)
{
    if(!src) {
        lv_memzero(info, sizeof(lv_imagebutton_src_info_t));
        return;
    }

    lv_result_t res = lv_image_decoder_get_info(src, &info->header);
    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("can't get info");
        return;
    }

    info->img_src = src;
}

#endif
