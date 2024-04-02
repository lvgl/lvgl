#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_LABEL

#define OBJ_HOR_LEN     (LV_HOR_RES/5)
#define OBJ_VER_LEN     (LV_VER_RES/5)
#define MAX_ANIM        (LV_VER_RES - OBJ_VER_LEN)
static lv_obj_t * obj_reference;

/* Animate the reference object to bounce up & down. */
static void lv_anim_callback(void * obj, int value)
{
    (void) obj;

    if(value <= MAX_ANIM) {
        lv_obj_set_y(obj_reference, value);
    }
    else {
        lv_obj_set_y(obj_reference, 2 * MAX_ANIM - value);
    }
}

/* Create the following (i.e. anchored) objects. */
static void create_follower_object(int index)
{
    lv_obj_t * obj;
    lv_obj_t * label;

    obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj, OBJ_HOR_LEN, OBJ_VER_LEN);
    lv_obj_align_to(obj, obj_reference, LV_ALIGN_OUT_RIGHT_MID, OBJ_HOR_LEN * (index - 1), 0);
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_BLUE), 0);

    label = lv_label_create(obj);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text_fmt(label, "Follower #%d", index);
}

/**
 * Align some objects to a non-parent anchor then move the anchor
 * and check that the anchored objects follow it.
 */
void lv_example_style_16(void)
{
    lv_obj_t * label;
    lv_obj_t * obj_child;

    lv_obj_set_style_pad_all(lv_scr_act(), 0, 0);

    /* Reference object. */
    obj_reference = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_reference, OBJ_HOR_LEN, OBJ_VER_LEN);
    lv_obj_align(obj_reference, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(obj_reference, lv_palette_main(LV_PALETTE_RED), 0);

    /* Normal child of the reference object. */
    obj_child = lv_obj_create(obj_reference);
    lv_obj_set_size(obj_child, LV_PCT(100), LV_PCT(100));
    lv_obj_align(obj_child, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(obj_child, lv_palette_main(LV_PALETTE_GREEN), 0);

    label = lv_label_create(obj_child);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Child");

    /* Some anchored followers of the reference object. */
    create_follower_object(1);
    create_follower_object(2);
    create_follower_object(3);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj_reference);
    lv_anim_set_path_cb(&anim, lv_anim_path_linear);
    lv_anim_set_values(&anim, 0, MAX_ANIM * 2);
    lv_anim_set_time(&anim, 3000);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&anim, lv_anim_callback);

    lv_anim_start(&anim);
}

#endif
