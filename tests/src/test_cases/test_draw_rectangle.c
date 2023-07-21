#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);
}

void tearDown(void)
{
    /* Function run after every test */
}


static void create_rects(lv_style_t * style, const char * name, lv_opa_t opa)
{
    static lv_coord_t w[] = {1,  30,   1,   5,  30,  5,   30,  100,  30,  100};
    static lv_coord_t h[] = {1,  1,    30,  5,  5,   30,  30,  100,  100, 30};
    static lv_coord_t r[] = {0,  8,   200};

    char buf[64];

    lv_obj_clean(lv_scr_act());
    uint32_t ri;
    for(ri = 0; ri < sizeof(r) / sizeof(r[0]); ri++) {

        uint32_t si;
        for(si = 0; si < sizeof(w) / sizeof(w[0]); si++) {
            lv_obj_t * obj = lv_obj_create(lv_scr_act());
            lv_obj_remove_style_all(obj);
            lv_obj_add_style(obj, style, 0);
            lv_obj_set_style_radius(obj, r[ri], 0);
            lv_obj_set_size(obj, w[si], h[si]);
            if(si == 0 && ri != 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        }

    }
    lv_snprintf(buf, sizeof(buf), "draw/rectangle_%s_opa_%d.png", name, opa);
    TEST_ASSERT_EQUAL_SCREENSHOT(buf);
}

static const lv_opa_t opas[] = {255, 200, 50};

void test_rect_solid(void)
{

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_RED));

    uint32_t i;
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        create_rects(&style, "solid", opas[i]);
    }

}

void test_rect_grad_hor(void)
{
    uint32_t i;

    static lv_style_t style;
    lv_style_init(&style);


    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_HOR;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_hex(0xff0000);
    grad.stops[0].opa = LV_OPA_COVER;
    grad.stops[0].frac = 0;
    grad.stops[1].color = lv_color_hex(0x00ff00);
    grad.stops[1].opa = LV_OPA_COVER;
    grad.stops[1].frac = 255;

    lv_style_set_bg_grad(&style, &grad);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        create_rects(&style, "grad_hor_simple", opas[i]);
    }
    grad.stops[0].frac = 64;
    grad.stops[1].frac = 150;

    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        create_rects(&style, "grad_hor_fract", opas[i]);
    }

    grad.stops[0].opa = LV_OPA_TRANSP;
    grad.stops[1].opa = LV_OPA_50;
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        create_rects(&style, "grad_hor_fade", opas[i]);
    }
}

void test_rect_grad_ver(void)
{
    uint32_t i;

    static lv_style_t style;
    lv_style_init(&style);


    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_hex(0xff0000);
    grad.stops[0].opa = LV_OPA_COVER;
    grad.stops[0].frac = 0;
    grad.stops[1].color = lv_color_hex(0x00ff00);
    grad.stops[1].opa = LV_OPA_COVER;
    grad.stops[1].frac = 255;

    lv_style_set_bg_grad(&style, &grad);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        create_rects(&style, "grad_ver_simple", opas[i]);
    }
    grad.stops[0].frac = 64;
    grad.stops[1].frac = 150;

    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        create_rects(&style, "grad_ver_fract", opas[i]);
    }

    grad.stops[0].opa = LV_OPA_TRANSP;
    grad.stops[1].opa = LV_OPA_50;
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        create_rects(&style, "grad_ver_fade", opas[i]);
    }
}

void test_border_full(void)
{
    uint32_t i;

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_INDIGO));

    lv_style_set_border_width(&style, 1);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_border_opa(&style, opas[i]);
        create_rects(&style, "w_1", opas[i]);
    }

    lv_style_set_border_width(&style, 10);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_border_opa(&style, opas[i]);
        create_rects(&style, "w_10", opas[i]);
    }

    lv_style_set_border_width(&style, 40);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_border_opa(&style, opas[i]);
        create_rects(&style, "w_40", opas[i]);
    }
}

void test_border_part(void)
{
    uint32_t i;

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_PINK));
    lv_style_set_border_width(&style, 8);

    lv_style_set_border_side(&style, LV_BORDER_SIDE_TOP);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_border_opa(&style, opas[i]);
        create_rects(&style, "top", opas[i]);
    }

    lv_style_set_border_side(&style, LV_BORDER_SIDE_BOTTOM);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_border_opa(&style, opas[i]);
        create_rects(&style, "bottom", opas[i]);
    }

    lv_style_set_border_side(&style, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_border_opa(&style, opas[i]);
        create_rects(&style, "top_bottom", opas[i]);
    }

    lv_style_set_border_side(&style, LV_BORDER_SIDE_LEFT);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_border_opa(&style, opas[i]);
        create_rects(&style, "left", opas[i]);
    }

    lv_style_set_border_side(&style, LV_BORDER_SIDE_RIGHT);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_border_opa(&style, opas[i]);
        create_rects(&style, "right", opas[i]);
    }

    lv_style_set_border_side(&style, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_border_opa(&style, opas[i]);
        create_rects(&style, "left_right", opas[i]);
    }
}

void test_rect_shadow(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_LIME));
    lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_BLUE));

    lv_style_set_shadow_width(&style, 32);
    uint32_t i;
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        lv_style_set_shadow_opa(&style, opas[i]);
        create_rects(&style, "shadow_simple", opas[i]);
    }

    lv_style_set_shadow_spread(&style, 16);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        lv_style_set_shadow_opa(&style, opas[i]);
        create_rects(&style, "shadow_spread", opas[i]);
    }

    lv_style_set_shadow_ofs_x(&style, 8);
    lv_style_set_shadow_ofs_y(&style, 16);
    for(i = 0; i < sizeof(opas) / sizeof(opas[0]); i++) {
        lv_style_set_bg_opa(&style, opas[i]);
        lv_style_set_shadow_opa(&style, opas[i]);
        create_rects(&style, "shadow_ofs", opas[i]);
    }

}

#endif
