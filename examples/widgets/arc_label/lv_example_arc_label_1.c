#include "../../lv_examples.h"

#if LV_USE_ARC_LABEL && LV_BUILD_EXAMPLES

const char * ARC_LABEL_TEXT =
    "I'm on an #FA7C45 ARC#! Centered with #12c2E9 C##8B68E8 O##c471ed L##B654E5 O##C84AB2 R##DB417A F##f64659 U##ff8888 L# feature!\n";

void lv_example_arc_label_1(void)
{
    lv_obj_t * arc_label_inner = NULL;
    lv_obj_t * arc_label_outer = NULL;
    lv_obj_t * arc_label_slogan_1 = NULL;
    lv_obj_t * arc_label_slogan_2 = NULL;

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN);

    arc_label_inner = lv_arc_label_create(lv_screen_active());
    lv_obj_set_size(arc_label_inner, 200, 200);
    lv_obj_set_style_text_color(arc_label_inner, lv_color_white(), LV_PART_MAIN);
    lv_arc_label_set_text_static(arc_label_inner, ARC_LABEL_TEXT);
    lv_arc_label_set_angle_start(arc_label_inner, 180);
    lv_arc_label_set_radius(arc_label_inner, LV_PCT(80));
    lv_arc_label_set_recolor(arc_label_inner, true);
    lv_arc_label_set_text_vertical_align(arc_label_inner, LV_ARC_LABEL_TEXT_ALIGN_TRAILING);
    lv_arc_label_set_dir(arc_label_inner, LV_ARC_LABEL_DIR_COUNTER_CLOCKWISE);
    lv_arc_label_set_text_horizontal_align(arc_label_inner, LV_ARC_LABEL_TEXT_ALIGN_CENTER);
    lv_obj_center(arc_label_inner);

    arc_label_outer = lv_arc_label_create(lv_screen_active());
    lv_obj_set_size(arc_label_outer, 200, 200);
    lv_obj_set_style_text_letter_space(arc_label_outer, 2, LV_PART_MAIN);
    lv_obj_set_style_text_color(arc_label_outer, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_arc_label_set_angle_start(arc_label_outer, -180);
    lv_arc_label_set_text_static(arc_label_outer, ARC_LABEL_TEXT);
    lv_arc_label_set_radius(arc_label_outer, LV_PCT(100));
    lv_arc_label_set_recolor(arc_label_outer, true);
    lv_arc_label_set_text_vertical_align(arc_label_outer, LV_ARC_LABEL_TEXT_ALIGN_LEADING);
    lv_arc_label_set_dir(arc_label_outer, LV_ARC_LABEL_DIR_CLOCKWISE);
    lv_arc_label_set_text_horizontal_align(arc_label_outer, LV_ARC_LABEL_TEXT_ALIGN_CENTER);
    lv_obj_center(arc_label_outer);

    arc_label_slogan_1 = lv_arc_label_create(lv_screen_active());
    lv_obj_set_size(arc_label_slogan_1, 300, 200);
    lv_obj_set_style_text_letter_space(arc_label_slogan_1, 2, LV_PART_MAIN);
    lv_obj_set_style_text_color(arc_label_slogan_1, lv_palette_main(LV_PALETTE_AMBER), LV_PART_MAIN);
    lv_arc_label_set_text_static(arc_label_slogan_1, "STAY HUNGRY");
    lv_arc_label_set_offset(arc_label_slogan_1, 30);
    lv_arc_label_set_radius(arc_label_slogan_1, 150);
    lv_arc_label_set_recolor(arc_label_slogan_1, true);
    lv_arc_label_set_text_vertical_align(arc_label_slogan_1, LV_ARC_LABEL_TEXT_ALIGN_TRAILING);
    lv_arc_label_set_text_horizontal_align(arc_label_slogan_1, LV_ARC_LABEL_TEXT_ALIGN_CENTER);
    lv_arc_label_set_dir(arc_label_slogan_1, LV_ARC_LABEL_DIR_COUNTER_CLOCKWISE);
    lv_obj_center(arc_label_slogan_1);

    arc_label_slogan_2 = lv_arc_label_create(lv_screen_active());
    lv_obj_set_size(arc_label_slogan_2, 300, 200);
    lv_obj_set_style_text_letter_space(arc_label_slogan_2, 2, LV_PART_MAIN);
    lv_obj_set_style_text_color(arc_label_slogan_2, lv_palette_main(LV_PALETTE_AMBER), LV_PART_MAIN);
    lv_arc_label_set_text_static(arc_label_slogan_2, "STAY FOOLISH");
    lv_arc_label_set_offset(arc_label_slogan_2, 30);
    lv_arc_label_set_radius(arc_label_slogan_2, 150);
    lv_arc_label_set_angle_start(arc_label_slogan_2, 180);
    lv_arc_label_set_recolor(arc_label_slogan_2, true);
    lv_arc_label_set_text_vertical_align(arc_label_slogan_2, LV_ARC_LABEL_TEXT_ALIGN_TRAILING);
    lv_arc_label_set_text_horizontal_align(arc_label_slogan_2, LV_ARC_LABEL_TEXT_ALIGN_CENTER);
    lv_arc_label_set_dir(arc_label_slogan_2, LV_ARC_LABEL_DIR_COUNTER_CLOCKWISE);
    lv_obj_center(arc_label_slogan_2);


#if LV_FONT_MONTSERRAT_18
    lv_obj_set_style_text_font(arc_label_inner, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_set_style_text_font(arc_label_outer, &lv_font_montserrat_18, LV_PART_MAIN);
#endif
#if LV_FONT_MONTSERRAT_24
    lv_obj_set_style_text_font(arc_label_slogan_1, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_font(arc_label_slogan_2, &lv_font_montserrat_24, LV_PART_MAIN);
#endif
}

#endif
