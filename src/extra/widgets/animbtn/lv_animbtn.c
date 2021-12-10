/**
 * @file lv_animbtn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_animbtn.h"

#if LV_USE_RLOTTIE != 0 && LV_USE_ANIMBTN != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_animbtn_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_animbtn_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_animbtn_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);
static void apply_state(lv_obj_t * animbtn);
static void loop_state(lv_obj_t * animbtn);
static lv_animbtn_state_t suggest_state(lv_obj_t * animbtn, lv_animbtn_state_t state);
static lv_animbtn_state_t get_state(const lv_obj_t * animbtn);
static int is_state_valid(const lv_animbtn_state_desc_t * state);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_animbtn_class = {
    .base_class = &lv_obj_class,
    .instance_size = sizeof(lv_animbtn_t),
    .constructor_cb = lv_animbtn_constructor,
    .event_cb = lv_animbtn_event,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create an image button object
 * @param parent pointer to an object, it will be the parent of the new image button
 * @return pointer to the created image button
 */
lv_obj_t * lv_animbtn_create(lv_obj_t * parent, lv_obj_t * anim)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    /*Capture the animation picture*/
    ((lv_animbtn_t*)obj)->lottie = anim;
    lv_obj_set_parent(anim, obj);
    lv_obj_add_flag(anim, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_set_size(obj, lv_obj_get_width(anim), lv_obj_get_height(anim));
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set images for a state of the image button
 * @param obj pointer to an image button object
 * @param state for which state set the new image
 * @param src_left pointer to an image source for the left side of the button (a C array or path to
 * a file)
 * @param src_mid pointer to an image source for the middle of the button (ideally 1px wide) (a C
 * array or path to a file)
 * @param src_right pointer to an image source for the right side of the button (a C array or path
 * to a file)
 */
void lv_animbtn_set_state_desc(lv_obj_t * obj, lv_animbtn_state_t state, const lv_animbtn_state_desc_t desc)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_animbtn_t * animbtn = (lv_animbtn_t *)obj;

    animbtn->state_desc[state] = desc;
    animbtn->state_desc[state].control |= 0x80; /*A non existant flag used to mark that the state was used*/
    apply_state(obj);
}

void lv_animbtn_set_state(lv_obj_t * obj, lv_animbtn_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_state_t obj_state = LV_STATE_DEFAULT;
    if(state == LV_ANIMBTN_STATE_PRESSED || state == LV_ANIMBTN_STATE_CHECKED_PRESSED) obj_state |= LV_STATE_PRESSED;
    if(state == LV_ANIMBTN_STATE_DISABLED || state == LV_ANIMBTN_STATE_CHECKED_DISABLED) obj_state |= LV_STATE_DISABLED;
    if(state == LV_ANIMBTN_STATE_CHECKED_DISABLED || state == LV_ANIMBTN_STATE_CHECKED_PRESSED ||
       state == LV_ANIMBTN_STATE_CHECKED_RELEASED) {
        obj_state |= LV_STATE_CHECKED;
    }

    lv_obj_clear_state(obj, LV_STATE_CHECKED | LV_STATE_PRESSED | LV_STATE_DISABLED);
    lv_obj_add_state(obj, obj_state);

    apply_state(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the right image in a given state
 * @param obj pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const lv_animbtn_state_desc_t * lv_animbtn_get_state_desc(lv_obj_t * obj, lv_animbtn_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animbtn_t * animbtn = (lv_animbtn_t *)obj;

    return &animbtn->state_desc[state];
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_animbtn_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_animbtn_t * animbtn = (lv_animbtn_t *)obj;
    /*Initialize the allocated 'ext'*/
    lv_memset_00(animbtn->state_desc, sizeof(animbtn->state_desc));
    animbtn->lottie = NULL;
    animbtn->prev_state = _LV_ANIMBTN_STATE_NUM;

    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
}


static void lv_animbtn_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(&lv_animbtn_class, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_animbtn_t * animbtn = (lv_animbtn_t *)obj;
    int is_same = obj == lv_event_get_target(e);
    switch(code) {
        case LV_EVENT_READY:
            /*Should we loop?*/
            loop_state(obj);
            break;
        case LV_EVENT_PRESSED:
        case LV_EVENT_RELEASED:
        case LV_EVENT_PRESS_LOST:
            apply_state(obj);
            break;
        case LV_EVENT_DRAW_MAIN:
            //draw_main(e);
            lv_obj_invalidate(animbtn->lottie);
            break;
        case LV_EVENT_COVER_CHECK: {
            lv_cover_check_info_t * info = lv_event_get_param(e);
            if(info->res != LV_COVER_RES_MASKED) info->res = LV_COVER_RES_NOT_COVER;
            break;
        }
        case LV_EVENT_GET_SELF_SIZE: {
            lv_point_t * p = lv_event_get_self_size_info(e);
            lv_img_header_t * header = &((lv_rlottie_t*)animbtn->lottie)->imgdsc.header;
            p->x = LV_MAX(p->x, header->w);
            break;
        }
        default: break;
    }
}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_animbtn_t * animbtn = (lv_animbtn_t *)obj;
    const lv_area_t * clip_area = lv_event_get_param(e);

    lv_coord_t tw = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
    lv_coord_t th = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
    lv_area_t coords;
    lv_area_copy(&coords, &obj->coords);
    lv_rlottie_t * lottie = (lv_rlottie_t *)animbtn->lottie;
    coords.x1 -= tw;
    coords.y1 -= th;
    coords.x2 = coords.x1 + lottie->imgdsc.header.w - 1;
    coords.y2 = coords.y1 + lottie->imgdsc.header.h - 1;

    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    lv_obj_init_draw_img_dsc(obj, LV_PART_MAIN, &img_dsc);

    lv_draw_img(&coords, clip_area, ((lv_rlottie_t*)animbtn->lottie)->allocated_buf, &img_dsc);
}


static void loop_state(lv_obj_t * obj)
{
    lv_animbtn_t * animbtn = (lv_animbtn_t *)obj;
    lv_animbtn_state_t state  = suggest_state(obj, get_state(obj));

    lv_rlottie_t * lottie = (lv_rlottie_t *)animbtn->lottie;
    if(animbtn->prev_state != state || lottie == NULL || !is_state_valid(&animbtn->state_desc[state])) return;

    /*Set the logic for the current state*/
    if ((animbtn->state_desc[state].control & LV_RLOTTIE_CTRL_LOOP) == LV_RLOTTIE_CTRL_LOOP) {
        lv_rlottie_set_current_frame(lottie, animbtn->state_desc[state].first_frame);
        lv_rlottie_stopat_frame(lottie, animbtn->state_desc[state].last_frame, (animbtn->state_desc[state].control & LV_RLOTTIE_CTRL_FORWARD) == LV_RLOTTIE_CTRL_FORWARD);
        lv_obj_invalidate(lottie);
    }
}


static void apply_state(lv_obj_t * obj)
{
    lv_animbtn_t * animbtn = (lv_animbtn_t *)obj;
    lv_animbtn_state_t state  = suggest_state(obj, get_state(obj));

    lv_rlottie_t * lottie = (lv_rlottie_t *)animbtn->lottie;
    if(state == animbtn->prev_state || lottie == NULL || !is_state_valid(&animbtn->state_desc[state])) return;

    /*Set the logic for the current state*/
    lv_rlottie_set_current_frame(lottie, animbtn->state_desc[state].first_frame);
    lv_rlottie_stopat_frame(lottie, animbtn->state_desc[state].last_frame, (animbtn->state_desc[state].control & LV_RLOTTIE_CTRL_FORWARD) == LV_RLOTTIE_CTRL_FORWARD);

    lv_obj_refresh_self_size(obj);
    lv_obj_set_height(obj, lottie->imgdsc.header.h); /*Keep the user defined width*/

    lv_obj_invalidate(obj);
    animbtn->prev_state = state;
}

/**
 * Check if a state is valid (initialized).
 */
static int is_state_valid(const lv_animbtn_state_desc_t * state) {
    return (state->control & 0x80) == 0x80;
}


/**
 * If `src` is not defined for the current state try to get a state which is related to the current but has a valid descriptor.
 * E.g. if the PRESSED src is not set but the RELEASED does, use the RELEASED.
 * @param animbtn pointer to an image button
 * @param state the state to convert
 * @return the suggested state
 */
static lv_animbtn_state_t suggest_state(lv_obj_t * obj, lv_animbtn_state_t state)
{
    lv_animbtn_t * animbtn = (lv_animbtn_t *)obj;
    if(!is_state_valid(&animbtn->state_desc[state])) {
        switch(state) {
            case LV_ANIMBTN_STATE_PRESSED:
                if(is_state_valid(&animbtn->state_desc[LV_ANIMBTN_STATE_RELEASED])) return LV_ANIMBTN_STATE_RELEASED;
                break;
            case LV_ANIMBTN_STATE_CHECKED_RELEASED:
                if(is_state_valid(&animbtn->state_desc[LV_ANIMBTN_STATE_RELEASED])) return LV_ANIMBTN_STATE_RELEASED;
                break;
            case LV_ANIMBTN_STATE_CHECKED_PRESSED:
                if(is_state_valid(&animbtn->state_desc[LV_ANIMBTN_STATE_CHECKED_RELEASED])) return LV_ANIMBTN_STATE_CHECKED_RELEASED;
                if(is_state_valid(&animbtn->state_desc[LV_ANIMBTN_STATE_PRESSED])) return LV_ANIMBTN_STATE_PRESSED;
                if(is_state_valid(&animbtn->state_desc[LV_ANIMBTN_STATE_RELEASED])) return LV_ANIMBTN_STATE_RELEASED;
                break;
            case LV_ANIMBTN_STATE_DISABLED:
                if(is_state_valid(&animbtn->state_desc[LV_ANIMBTN_STATE_RELEASED])) return LV_ANIMBTN_STATE_RELEASED;
                break;
            case LV_ANIMBTN_STATE_CHECKED_DISABLED:
                if(is_state_valid(&animbtn->state_desc[LV_ANIMBTN_STATE_CHECKED_RELEASED])) return LV_ANIMBTN_STATE_CHECKED_RELEASED;
                if(is_state_valid(&animbtn->state_desc[LV_ANIMBTN_STATE_RELEASED])) return LV_ANIMBTN_STATE_RELEASED;
                break;
            default:
                break;
        }
    }

    return state;
}

lv_animbtn_state_t get_state(const lv_obj_t * animbtn)
{
    LV_ASSERT_OBJ(animbtn, MY_CLASS);

    lv_state_t obj_state = lv_obj_get_state(animbtn);

    if(obj_state & LV_STATE_DISABLED) {
        if(obj_state & LV_STATE_CHECKED) return LV_ANIMBTN_STATE_CHECKED_DISABLED;
        else return LV_ANIMBTN_STATE_DISABLED;
    }

    if(obj_state & LV_STATE_CHECKED) {
        if(obj_state & LV_STATE_PRESSED) return LV_ANIMBTN_STATE_CHECKED_PRESSED;
        else return LV_ANIMBTN_STATE_CHECKED_RELEASED;
    }
    else {
        if(obj_state & LV_STATE_PRESSED) return LV_ANIMBTN_STATE_PRESSED;
        else return LV_ANIMBTN_STATE_RELEASED;
    }
}

#endif
