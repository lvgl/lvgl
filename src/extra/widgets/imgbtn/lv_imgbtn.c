/**
 * @file lv_imgbtn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_imgbtn.h"

#if LV_USE_IMGBTN != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_imgbtn_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_imgbtn_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void draw_main(lv_event_t * e);
static void lv_imgbtn_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void refr_img(lv_obj_t * imgbtn);
static lv_imgbtn_state_t suggest_state(lv_obj_t * imgbtn, lv_imgbtn_state_t state);
lv_imgbtn_state_t get_state(const lv_obj_t * imgbtn);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_imgbtn_class = {
    .base_class = &lv_obj_class,
    .instance_size = sizeof(lv_imgbtn_t),
    .constructor_cb = lv_imgbtn_constructor,
    .event_cb = lv_imgbtn_event,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a image button object
 * @param par pointer to an object, it will be the parent of the new image button
 * @return pointer to the created image button
 */
lv_obj_t * lv_imgbtn_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image
 * @param src_left pointer to an image source for the left side of the button (a C array or path to
 * a file)
 * @param src_mid pointer to an image source for the middle of the button (ideally 1px wide) (a C
 * array or path to a file)
 * @param src_right pointer to an image source for the right side of the button (a C array or path
 * to a file)
 */
void lv_imgbtn_set_src(lv_obj_t * obj, lv_imgbtn_state_t state, const void * src_left, const void * src_mid,
                       const void * src_right)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *)obj;

    imgbtn->img_src_left[state] = src_left;
    imgbtn->img_src_mid[state] = src_mid;
    imgbtn->img_src_right[state] = src_right;

    refr_img(obj);
}

void lv_imgbtn_set_state(lv_obj_t * obj, lv_imgbtn_state_t state)
{
	LV_ASSERT_OBJ(obj, MY_CLASS);

	lv_state_t obj_state = LV_STATE_DEFAULT;
	if(state == LV_IMGBTN_STATE_PRESSED || state == LV_IMGBTN_STATE_CHECKED_PRESSED) obj_state |= LV_STATE_PRESSED;
	if(state == LV_IMGBTN_STATE_DISABLED || state == LV_IMGBTN_STATE_CHECKED_DISABLED) obj_state |= LV_STATE_DISABLED;
	if(state == LV_IMGBTN_STATE_CHECKED_DISABLED || state == LV_IMGBTN_STATE_CHECKED_PRESSED || state == LV_IMGBTN_STATE_CHECKED_RELEASED) {
		obj_state |= LV_STATE_CHECKED;
	}

	lv_obj_clear_state(obj, LV_STATE_CHECKED | LV_STATE_PRESSED | LV_STATE_DISABLED);
	lv_obj_add_state(obj, obj_state);

	refr_img(obj);
}

/*=====================
 * Getter functions
 *====================*/


/**
 * Get the left image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_left(lv_obj_t * obj, lv_imgbtn_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *)obj;

    return imgbtn->img_src_left[state];
}

/**
 * Get the middle image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the middle image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_middle(lv_obj_t * obj, lv_imgbtn_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *)obj;

    return imgbtn->img_src_mid[state];
}

/**
 * Get the right image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_right(lv_obj_t * obj, lv_imgbtn_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *)obj;

    return imgbtn->img_src_right[state];
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_imgbtn_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *)obj;
    /*Initialize the allocated 'ext'*/
    lv_memset_00((void *)imgbtn->img_src_mid, sizeof(imgbtn->img_src_mid));
    lv_memset_00(imgbtn->img_src_left, sizeof(imgbtn->img_src_left));
    lv_memset_00(imgbtn->img_src_right, sizeof(imgbtn->img_src_right));

    imgbtn->act_cf = LV_IMG_CF_UNKNOWN;
}


static void lv_imgbtn_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(&lv_imgbtn_class, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_PRESSED || code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        refr_img(obj);
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
        lv_imgbtn_t * imgbtn = (lv_imgbtn_t *)obj;
        lv_imgbtn_state_t state  = suggest_state(obj, get_state(obj));
        if(imgbtn->img_src_left[state] == NULL &&
           imgbtn->img_src_mid[state] != NULL &&
           imgbtn->img_src_right[state] == NULL) {
            lv_img_header_t header;
            lv_img_decoder_get_info(imgbtn->img_src_mid[state], &header);
            p->x = LV_MAX(p->x, header.w);
        }
    }
}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *)obj;
    const lv_area_t * clip_area = lv_event_get_param(e);

    /*Just draw_main an image*/
    lv_imgbtn_state_t state  = suggest_state(obj, get_state(obj));

    /*Simply draw the middle src if no tiled*/
    const void * src = imgbtn->img_src_left[state];

    lv_coord_t tw = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
    lv_coord_t th = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
    lv_area_t coords;
    lv_area_copy(&coords, &obj->coords);
    coords.x1 -= tw;
    coords.x2 += tw;
    coords.y1 -= th;
    coords.y2 += th;

    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    lv_obj_init_draw_img_dsc(obj, LV_PART_MAIN, &img_dsc);

    lv_img_header_t header;
    lv_area_t coords_part;
    lv_coord_t left_w = 0;
    lv_coord_t right_w = 0;

    if(src) {
        lv_img_decoder_get_info(src, &header);
        left_w = header.w;
        coords_part.x1 = coords.x1;
        coords_part.y1 = coords.y1;
        coords_part.x2 = coords.x1 + header.w - 1;
        coords_part.y2 = coords.y1 + header.h - 1;
        lv_draw_img(&coords_part, clip_area, src, &img_dsc);
    }

    src = imgbtn->img_src_right[state];
    if(src) {
        lv_img_decoder_get_info(src, &header);
        right_w = header.w;
        coords_part.x1 = coords.x2 - header.w + 1;
        coords_part.y1 = coords.y1;
        coords_part.x2 = coords.x2;
        coords_part.y2 = coords.y1 + header.h - 1;
        lv_draw_img(&coords_part, clip_area, src, &img_dsc);
    }

    src = imgbtn->img_src_mid[state];
    if(src) {
        lv_area_t clip_center_area;
        clip_center_area.x1 = coords.x1 + left_w;
        clip_center_area.x2 = coords.x2 - right_w;
        clip_center_area.y1 = coords.y1;
        clip_center_area.y2 = coords.y2;

        bool comm_res;
        comm_res = _lv_area_intersect(&clip_center_area, &clip_center_area, clip_area);
        if(comm_res) {
            lv_coord_t i;
            lv_img_decoder_get_info(src, &header);

            coords_part.x1 = coords.x1 + left_w;
            coords_part.y1 = coords.y1;
            coords_part.x2 = coords_part.x1 + header.w - 1;
            coords_part.y2 = coords_part.y1 + header.h - 1;

            for(i = coords_part.x1; i < (lv_coord_t)(clip_center_area.x2 + header.w - 1); i += header.w) {
                lv_draw_img(&coords_part, &clip_center_area, src, &img_dsc);
                coords_part.x1 = coords_part.x2 + 1;
                coords_part.x2 += header.w;
            }
        }
    }
}

static void refr_img(lv_obj_t * obj)
{
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *)obj;
    lv_imgbtn_state_t state  = suggest_state(obj, get_state(obj));
    lv_img_header_t header;

    const void * src = imgbtn->img_src_mid[state];
    if(src == NULL) return;

    lv_res_t info_res = LV_RES_OK;
    info_res = lv_img_decoder_get_info(src, &header);

    if(info_res == LV_RES_OK) {
        imgbtn->act_cf = header.cf;
        lv_obj_refresh_self_size(obj);
        lv_obj_set_height(obj, header.h); /*Keep the user defined width*/
    }
    else {
        imgbtn->act_cf = LV_IMG_CF_UNKNOWN;
    }

    lv_obj_invalidate(obj);
}

/**
 * If `src` is not defined for the current state try to get a state which is related to the current but has `src`.
 * E.g. if the PRESSED src is not set but the RELEASED does, use the RELEASED.
 * @param imgbtn pointer to an image button
 * @param state the state to convert
 * @return the suggested state
 */
static lv_imgbtn_state_t suggest_state(lv_obj_t * obj, lv_imgbtn_state_t state)
{
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *)obj;
    if(imgbtn->img_src_mid[state] == NULL) {
        switch(state) {
            case LV_IMGBTN_STATE_PRESSED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            case LV_IMGBTN_STATE_CHECKED_RELEASED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            case LV_IMGBTN_STATE_CHECKED_PRESSED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_CHECKED_RELEASED]) return LV_IMGBTN_STATE_CHECKED_RELEASED;
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_PRESSED]) return LV_IMGBTN_STATE_PRESSED;
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            case LV_IMGBTN_STATE_DISABLED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            case LV_IMGBTN_STATE_CHECKED_DISABLED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_CHECKED_RELEASED]) return LV_IMGBTN_STATE_CHECKED_RELEASED;
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            default:
                break;
        }
    }

    return state;
}

lv_imgbtn_state_t get_state(const lv_obj_t * imgbtn)
{
    LV_ASSERT_OBJ(imgbtn, MY_CLASS);

    lv_state_t obj_state = lv_obj_get_state(imgbtn);

    if(obj_state & LV_STATE_DISABLED) {
        if(obj_state & LV_STATE_CHECKED) return LV_IMGBTN_STATE_CHECKED_DISABLED;
        else return LV_IMGBTN_STATE_DISABLED;
    }

    if(obj_state & LV_STATE_CHECKED) {
        if(obj_state & LV_STATE_PRESSED) return LV_IMGBTN_STATE_CHECKED_PRESSED;
        else return LV_IMGBTN_STATE_CHECKED_RELEASED;
    }
    else {
        if(obj_state & LV_STATE_PRESSED) return LV_IMGBTN_STATE_PRESSED;
        else return LV_IMGBTN_STATE_RELEASED;
    }
}

#endif
