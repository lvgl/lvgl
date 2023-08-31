#include "../../lv_examples.h"
#if LV_USE_IMG && LV_BUILD_EXAMPLES


void lv_example_img_1(void)
{

    LV_IMG_DECLARE(test_img_cogwheel_rgb565a8);

    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 150, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(cont, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * img = lv_img_create(cont);
    lv_img_set_src(img, &test_img_cogwheel_rgb565a8);
    lv_obj_set_style_img_recolor(img, lv_palette_main(LV_PALETTE_RED), 0);   /*For A8*/
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_70, 0);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, "RGB565");
}

#endif
