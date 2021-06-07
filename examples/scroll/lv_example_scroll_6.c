#include "lvgl/lvgl.h"
#if LV_BUILD_EXAMPLES

static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = 190;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        } else {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }


        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);

        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

static void fade(void * var, int32_t v)
{
    lv_obj_set_style_opa(var, v, 0);
//    lv_obj_invalidate(var);
}



static void ofsx(void * var, int32_t v)
{
    lv_img_set_offset_x(var, v);
}

static void translate_x(void * var, int32_t v)
{
    lv_obj_set_style_translate_x(var, v, 0);
    lv_obj_set_style_img_opa(var, 256 + v, 0);
}
/**
 * Translate the object as they scroll
 */
void sw_test_1(void)
{

    LV_IMG_DECLARE(img_bg);

    lv_obj_t * bg = lv_img_create(lv_scr_act());
    lv_img_set_src(bg, &img_bg);
    lv_obj_set_size(bg, 392, 392);
    lv_obj_center(bg);
    lv_obj_clear_flag(bg, LV_OBJ_FLAG_SCROLLABLE);


    LV_IMG_DECLARE(img_start_1);
    lv_obj_t * start_1 = lv_img_create(bg);
    lv_img_set_src(start_1, &img_start_1);
    lv_obj_align(start_1, LV_ALIGN_CENTER, 22, 5);
    lv_obj_set_style_blend_mode(start_1, LV_BLEND_MODE_ADDITIVE, 0);


    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, start_1);
    lv_anim_set_time(&a1, 1000);
    lv_anim_set_playback_time(&a1, 300);
    lv_anim_set_values(&a1, LV_OPA_50, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a1, fade);
    lv_anim_set_repeat_count(&a1, LV_ANIM_REPEAT_INFINITE);
//    lv_anim_start(&a1);

    LV_IMG_DECLARE(img_flash);
    lv_obj_t * flash_1 = lv_img_create(bg);
    lv_img_set_src(flash_1, &img_flash);
    lv_obj_align_to(flash_1, start_1,  LV_ALIGN_OUT_RIGHT_MID, -100, -10);
    lv_obj_set_style_blend_mode(flash_1, LV_BLEND_MODE_ADDITIVE, 0);
    lv_obj_set_width(flash_1, 110);

    lv_anim_t a2;
    lv_anim_init(&a2);
    lv_anim_set_var(&a2, flash_1);
    lv_anim_set_time(&a2, 1000);
    lv_anim_set_values(&a2, 0, -100);
    lv_anim_set_exec_cb(&a2, ofsx);
    lv_anim_set_repeat_count(&a2, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a2);

    lv_obj_t * flash_2 = lv_img_create(bg);
    lv_img_set_src(flash_2, &img_flash);
    lv_obj_align_to(flash_2, start_1,  LV_ALIGN_OUT_RIGHT_MID, -100, 10);
    lv_obj_set_style_blend_mode(flash_2, LV_BLEND_MODE_ADDITIVE, 0);
    lv_obj_set_width(flash_2, 110);

    lv_anim_set_var(&a2, flash_2);
    lv_anim_set_time(&a2, 1500);
    lv_anim_set_values(&a2, 0, -100);
    lv_anim_set_exec_cb(&a2, ofsx);
    lv_anim_start(&a2);

    LV_IMG_DECLARE(img_planet);
    lv_obj_t * planet = lv_img_create(bg);
    lv_img_set_src(planet, &img_planet);
    lv_obj_align(planet, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_flag(planet, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_add_flag(planet, LV_OBJ_FLAG_FLOATING);
    lv_obj_set_style_blend_mode(planet, LV_BLEND_MODE_ADDITIVE, 0);





    uint32_t i;
    for (i = 0; i < 10; i++) {
    LV_IMG_DECLARE(img_caret_down);
    lv_obj_t * particle = lv_img_create(bg);
    lv_img_set_src(particle, &img_caret_down);
    lv_obj_align(particle, LV_ALIGN_RIGHT_MID, 0, i * 20 - 100);
//    lv_obj_set_style_blend_mode(planet_glow, LV_BLEND_MODE_ADDITIVE, 0);

        lv_anim_t a3;
        lv_anim_init(&a3);
        lv_anim_set_var(&a3, particle);
        lv_anim_set_time(&a3, lv_rand(800, 1200));
        lv_anim_set_values(&a3, 0, -256);
        lv_anim_set_exec_cb(&a3, translate_x);
        lv_anim_set_repeat_count(&a3, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&a3);
    }


    lv_obj_t * cont = lv_obj_create(bg);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_pad_row(cont, 50, 0);
    lv_obj_set_style_text_font(cont, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(cont, lv_color_white(), 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);


    LV_IMG_DECLARE(icn_water);
    for(i = 0; i < 20; i++) {
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_remove_style_all(btn);
        lv_obj_set_width(btn, 250);

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Button %d", i);

        lv_obj_t * img = lv_img_create(btn);
        lv_img_set_src(img, &icn_water);

        lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    }

    /*Update the buttons position manually for first*/
    lv_event_send(cont, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    lv_obj_scroll_to_view(lv_obj_get_child(cont, 0), LV_ANIM_OFF);
}

#endif
