#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

static void exec_cb(void * obj, int32_t v)
{
    lv_obj_set_style_backdrop_blur_intensity(obj, v, 0);
}

void lv_example_image_1(void)
{

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label,
                      "asdasdasdas a sad sad sasdas\n sadsad jsahd kajhd asd asdsad ss a\n sal kjd a sljdsla kdj\nsad jasd sss tegfd adssa e\nadssa dsad ff  wedsada asdsa\nlasjkuhe  ajkhads askhad\nassldjasjas kjsahds \n asd data khe asww fkha sdas d kcj");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    LV_IMAGE_DECLARE(img_cogwheel_argb);
    lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, &img_cogwheel_argb);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(img1, 70, 0);
    lv_obj_set_style_pad_all(img1, 40, 0);


    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, 0, 200);
    lv_anim_set_duration(&a, 3000);
    lv_anim_set_reverse_duration(&a, 3000);
    lv_anim_set_repeat_count(&a, 1000);
    lv_anim_set_var(&a, img1);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_start(&a);

    lv_obj_t * img2 = lv_image_create(lv_screen_active());
    lv_image_set_src(img2, LV_SYMBOL_OK "Accept");
    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

#endif
