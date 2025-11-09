#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

static void exec_cb(void * obj, int32_t v)
{
    lv_obj_set_style_backdrop_blur_intensity(obj, v, 0);
}

void lv_example_image_1(void)
{

    lv_obj_set_style_bg_color(lv_screen_active(), lv_palette_main(LV_PALETTE_ORANGE), 0);
    //    lv_obj_t * label = lv_obj_create(lv_screen_active());
    //    lv_obj_set_size(label, 20, 20);
    //    lv_obj_align(label, LV_ALIGN_CENTER, 0, -100);
    //    lv_obj_set_style_bg_color(label, lv_color_hex(0x000000), 0);
    //    lv_obj_set_style_border_opa(label, 0, 0);

    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_bg_opa(parent, 40, 0);
    lv_obj_set_style_transform_rotation(parent, 450, 0);
    lv_obj_set_size(parent, 300, 300);
    lv_obj_center(parent);


    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label,
                      "asdasdasdas a sad sad sasdas\n sadsad jsahd kajhd asd asdsad ss a\n sal kjd a sljdsla kdj\nsad jasd sss tegfd adssa e\nadssa dsad ff  wedsada asdsa\nlasjkuhe  ajkhads askhad\nassldjasjas kjsahds \n asd data khe asww fkha sdas d kcj");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t * obj = lv_calendar_create(parent);
    lv_obj_center(obj);
    lv_obj_set_style_backdrop_blur_intensity(obj, 20, 0);
    lv_obj_set_style_bg_opa(obj, 100, 0);
    lv_obj_set_style_border_opa(obj, 100, 0);


    //    lv_obj_t * obj = lv_slider_create(lv_screen_active());
    //    lv_obj_set_style_backdrop_blur_intensity(obj, 40, LV_PART_INDICATOR);
    ////    lv_obj_set_style_bg_opa(obj, 0, LV_PART_INDICATOR);
    ////    lv_obj_set_style_opa_layered(obj, 100, LV_PART_MAIN);
    //    lv_obj_set_height(obj, 320);
    //    lv_obj_set_width(obj, 700);
    //    lv_obj_center(obj);
    //    lv_slider_set_value(obj, 100, 0);

    //
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, 0, 64);
    lv_anim_set_duration(&a, 3000);
    lv_anim_set_reverse_duration(&a, 1000);
    lv_anim_set_repeat_count(&a, 1000);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, exec_cb);
    //        lv_anim_start(&a);
    //
    //    lv_obj_t * img2 = lv_image_create(lv_screen_active());
    //    lv_image_set_src(img2, LV_SYMBOL_OK "Accept");
    //    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

#endif
