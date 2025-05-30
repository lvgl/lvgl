#include "../../lv_examples.h"

#if LV_USE_ARCLABEL && LV_BUILD_EXAMPLES

static const char * ARCLABEL_TEXT =
    "I'm on an #FA7C45 ARC#! Centered with #12c2E9 C##8B68E8 O##c471ed L##B654E5 O##C84AB2 R##DB417A F##f64659 U##ff8888 L# feature!\n";

void lv_example_arclabel_1(void)
{
    lv_obj_t * arclabel_inner = NULL;
    lv_obj_t * arclabel_outer = NULL;
    lv_obj_t * arclabel_slogan_1 = NULL;
    lv_obj_t * arclabel_slogan_2 = NULL;

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN);

    arclabel_inner = lv_arclabel_create(lv_screen_active());
    lv_obj_set_size(arclabel_inner, 200, 200);
    lv_obj_set_style_text_color(arclabel_inner, lv_color_white(), LV_PART_MAIN);
    lv_arclabel_set_text_static(arclabel_inner, ARCLABEL_TEXT);
    lv_arclabel_set_angle_start(arclabel_inner, 180);
    lv_arclabel_set_radius(arclabel_inner, LV_PCT(80));
    lv_arclabel_set_recolor(arclabel_inner, true);
    lv_arclabel_set_text_vertical_align(arclabel_inner, LV_ARCLABEL_TEXT_ALIGN_TRAILING);
    lv_arclabel_set_dir(arclabel_inner, LV_ARCLABEL_DIR_COUNTER_CLOCKWISE);
    lv_arclabel_set_text_horizontal_align(arclabel_inner, LV_ARCLABEL_TEXT_ALIGN_CENTER);
    lv_obj_center(arclabel_inner);

    arclabel_outer = lv_arclabel_create(lv_screen_active());
    lv_obj_set_size(arclabel_outer, 200, 200);
    lv_obj_set_style_text_letter_space(arclabel_outer, 2, LV_PART_MAIN);
    lv_obj_set_style_text_color(arclabel_outer, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_arclabel_set_angle_start(arclabel_outer, -180);
    lv_arclabel_set_text_static(arclabel_outer, ARCLABEL_TEXT);
    lv_arclabel_set_radius(arclabel_outer, LV_PCT(100));
    lv_arclabel_set_recolor(arclabel_outer, true);
    lv_arclabel_set_text_vertical_align(arclabel_outer, LV_ARCLABEL_TEXT_ALIGN_LEADING);
    lv_arclabel_set_dir(arclabel_outer, LV_ARCLABEL_DIR_CLOCKWISE);
    lv_arclabel_set_text_horizontal_align(arclabel_outer, LV_ARCLABEL_TEXT_ALIGN_CENTER);
    lv_obj_center(arclabel_outer);

    arclabel_slogan_1 = lv_arclabel_create(lv_screen_active());
    lv_obj_set_size(arclabel_slogan_1, 300, 200);
    lv_obj_set_style_text_letter_space(arclabel_slogan_1, 2, LV_PART_MAIN);
    lv_obj_set_style_text_color(arclabel_slogan_1, lv_palette_main(LV_PALETTE_AMBER), LV_PART_MAIN);
    lv_arclabel_set_text_static(arclabel_slogan_1, "STAY HUNGRY");
    lv_arclabel_set_offset(arclabel_slogan_1, 30);
    lv_arclabel_set_radius(arclabel_slogan_1, 150);
    lv_arclabel_set_recolor(arclabel_slogan_1, true);
    lv_arclabel_set_text_vertical_align(arclabel_slogan_1, LV_ARCLABEL_TEXT_ALIGN_TRAILING);
    lv_arclabel_set_text_horizontal_align(arclabel_slogan_1, LV_ARCLABEL_TEXT_ALIGN_CENTER);
    lv_arclabel_set_dir(arclabel_slogan_1, LV_ARCLABEL_DIR_COUNTER_CLOCKWISE);
    lv_obj_center(arclabel_slogan_1);

    arclabel_slogan_2 = lv_arclabel_create(lv_screen_active());
    lv_obj_set_size(arclabel_slogan_2, 300, 200);
    lv_obj_set_style_text_letter_space(arclabel_slogan_2, 2, LV_PART_MAIN);
    lv_obj_set_style_text_color(arclabel_slogan_2, lv_palette_main(LV_PALETTE_AMBER), LV_PART_MAIN);
    lv_arclabel_set_text_static(arclabel_slogan_2, "STAY FOOLISH");
    lv_arclabel_set_offset(arclabel_slogan_2, 30);
    lv_arclabel_set_radius(arclabel_slogan_2, 150);
    lv_arclabel_set_angle_start(arclabel_slogan_2, 180);
    lv_arclabel_set_recolor(arclabel_slogan_2, true);
    lv_arclabel_set_text_vertical_align(arclabel_slogan_2, LV_ARCLABEL_TEXT_ALIGN_TRAILING);
    lv_arclabel_set_text_horizontal_align(arclabel_slogan_2, LV_ARCLABEL_TEXT_ALIGN_CENTER);
    lv_arclabel_set_dir(arclabel_slogan_2, LV_ARCLABEL_DIR_COUNTER_CLOCKWISE);
    lv_obj_center(arclabel_slogan_2);


#if LV_FONT_MONTSERRAT_18
    lv_obj_set_style_text_font(arclabel_inner, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_set_style_text_font(arclabel_outer, &lv_font_montserrat_18, LV_PART_MAIN);
#endif
#if LV_FONT_MONTSERRAT_24
    lv_obj_set_style_text_font(arclabel_slogan_1, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_font(arclabel_slogan_2, &lv_font_montserrat_24, LV_PART_MAIN);
#endif
}

#endif
