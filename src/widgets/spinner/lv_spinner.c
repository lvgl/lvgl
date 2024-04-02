/**
 * @file lv_spinner.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
#if LV_USE_SPINNER

/*********************
 *      DEFINES
 *********************/
#define DEF_ARC_ANGLE   200
#define DEF_TIME        1000

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_spinner_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void arc_anim_start_angle(void * obj, int32_t v);
static void arc_anim_end_angle(void * obj, int32_t v);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_spinner_class = {
    .base_class = &lv_arc_class,
    .constructor_cb = lv_spinner_constructor,
    .name = "spinner",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_spinner_create(lv_obj_t * parent)
{

    lv_obj_t * obj = lv_obj_class_create_obj(&lv_spinner_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_spinner_set_anim_params(lv_obj_t * obj, uint32_t t, uint32_t angle)
{
    /*Delete the current animation*/
    lv_anim_delete(obj, NULL);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, arc_anim_end_angle);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_duration(&a, t);
    lv_anim_set_values(&a, angle, 360 + angle);
    lv_anim_start(&a);

    lv_anim_set_path_cb(&a, lv_anim_path_custom_bezier3);
    lv_anim_set_bezier3_param(&a, LV_BEZIER_VAL_FLOAT(0.42), LV_BEZIER_VAL_FLOAT(0.58),
                              LV_BEZIER_VAL_FLOAT(0), LV_BEZIER_VAL_FLOAT(1));
    lv_anim_set_values(&a, 0, 360);
    lv_anim_set_exec_cb(&a, arc_anim_start_angle);
    lv_anim_start(&a);

    lv_arc_set_bg_angles(obj, 0, 360);
    lv_arc_set_rotation(obj, 270);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_spinner_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_TRACE_OBJ_CREATE("begin");

    LV_UNUSED(class_p);

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    lv_spinner_set_anim_params(obj, DEF_TIME, DEF_ARC_ANGLE);
}

static void arc_anim_start_angle(void * obj, int32_t v)
{
    lv_arc_set_start_angle(obj, (uint32_t) v);
}

static void arc_anim_end_angle(void * obj, int32_t v)
{
    lv_arc_set_end_angle(obj, (uint32_t) v);
}

#endif /*LV_USE_SPINNER*/
