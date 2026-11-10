/**
* @file lv_test_alpha_scene.c
*
*/
#if LV_BUILD_TEST

/*********************
 *      INCLUDES
 *********************/

#include "lv_test_alpha_scene.h"

/*********************
 *      DEFINES
 *********************/

/* On the 800 x 480 test screen: the background, and the widget inside it */
#define BG_X 100
#define BG_Y 40
#define BG_W 600
#define BG_H 400
#define WIDGET_X 200
#define WIDGET_Y 120
#define WIDGET_W 400
#define WIDGET_H 240

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void vector_draw_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_font_t * label_font = &lv_font_montserrat_48;

/**********************
 *  GLOBAL VARIABLES
 **********************/

const lv_test_alpha_widget_t lv_test_alpha_widgets[LV_TEST_ALPHA_WIDGET_COUNT] = {
    [LV_TEST_ALPHA_WIDGET_OBJ_OPA_COVER]    = {"obj_opa_cover",    LV_TEST_ALPHA_OBJ,    0xff0000, 0xff},
    [LV_TEST_ALPHA_WIDGET_OBJ_OPA_77]       = {"obj_opa_77",       LV_TEST_ALPHA_OBJ,    0xff0000, 0x77},
    [LV_TEST_ALPHA_WIDGET_BUTTON_OPA_COVER] = {"button_opa_cover", LV_TEST_ALPHA_BUTTON, 0xff0000, 0xff},
    [LV_TEST_ALPHA_WIDGET_BUTTON_OPA_DD]    = {"button_opa_dd",    LV_TEST_ALPHA_BUTTON, 0xff0000, 0xdd},
    [LV_TEST_ALPHA_WIDGET_LABEL_OPA_COVER]  = {"label_opa_cover",  LV_TEST_ALPHA_LABEL,  0xff0000, 0xff},
    [LV_TEST_ALPHA_WIDGET_LABEL_OPA_77]     = {"label_opa_77",     LV_TEST_ALPHA_LABEL,  0xff0000, 0x77},
    [LV_TEST_ALPHA_WIDGET_VECTOR_OPA_COVER] = {"vector_opa_cover", LV_TEST_ALPHA_VECTOR, 0xff0000, 0xff},
    [LV_TEST_ALPHA_WIDGET_VECTOR_OPA_77]    = {"vector_opa_77",    LV_TEST_ALPHA_VECTOR, 0xff0000, 0x77},
};

const lv_test_alpha_bg_t lv_test_alpha_bgs[LV_TEST_ALPHA_BG_COUNT] = {
    [LV_TEST_ALPHA_BG_NONE]        = {"none",        0x000000, 0x00},
    [LV_TEST_ALPHA_BG_OPAQUE]      = {"opaque",      0x2060ff, 0xff},
    [LV_TEST_ALPHA_BG_TRANSLUCENT] = {"translucent", 0x2060ff, 0x80},
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_test_alpha_scene_create(lv_obj_t * screen, lv_test_alpha_widget_id_t widget, lv_test_alpha_bg_id_t bg)
{
    const lv_test_alpha_widget_t * c = &lv_test_alpha_widgets[widget];
    const lv_test_alpha_bg_t * b = &lv_test_alpha_bgs[bg];

    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);

    if(b->opa) {
        lv_obj_t * bg_obj = lv_obj_create(screen);
        lv_obj_remove_style_all(bg_obj);
        lv_obj_set_style_bg_color(bg_obj, lv_color_hex(b->color), 0);
        lv_obj_set_style_bg_opa(bg_obj, b->opa, 0);
        lv_obj_set_pos(bg_obj, BG_X, BG_Y);
        lv_obj_set_size(bg_obj, BG_W, BG_H);
    }

    lv_color_t color = lv_color_hex(c->color);
    lv_obj_t * obj = NULL;
    switch(c->kind) {
        case LV_TEST_ALPHA_OBJ:
            obj = lv_obj_create(screen);
            lv_obj_remove_style_all(obj);
            lv_obj_set_style_bg_color(obj, color, 0);
            lv_obj_set_style_bg_opa(obj, c->opa, 0);
            break;
        case LV_TEST_ALPHA_BUTTON:
            obj = lv_button_create(screen);
            lv_obj_set_style_bg_color(obj, color, 0);
            lv_obj_set_style_bg_opa(obj, c->opa, 0);
            /* A shadow under a semi-transparent background would show through */
            lv_obj_set_style_shadow_width(obj, 0, 0);
            /* Renderers antialias rounded corners differently, which isn't what's tested here */
            lv_obj_set_style_radius(obj, 0, 0);
            break;
        case LV_TEST_ALPHA_LABEL:
            obj = lv_label_create(screen);
            lv_label_set_text(obj, "LVGL");
            lv_obj_set_style_text_font(obj, label_font, 0);
            lv_obj_set_style_text_color(obj, color, 0);
            lv_obj_set_style_text_opa(obj, c->opa, 0);
            break;
        case LV_TEST_ALPHA_VECTOR:
            obj = lv_obj_create(screen);
            lv_obj_remove_style_all(obj);
            lv_obj_add_event_cb(obj, vector_draw_event_cb, LV_EVENT_DRAW_MAIN, (void *)c);
            break;
    }

    lv_obj_set_pos(obj, WIDGET_X, WIDGET_Y);
    if(c->kind != LV_TEST_ALPHA_LABEL) lv_obj_set_size(obj, WIDGET_W, WIDGET_H);
}

void lv_test_alpha_scene_set_label_font(const lv_font_t * font)
{
    label_font = font;
}

lv_point_t lv_test_alpha_scene_sample_point(lv_test_alpha_widget_id_t widget)
{
    lv_point_t p = {WIDGET_X + WIDGET_W / 2, WIDGET_Y + WIDGET_H / 2};
    if(lv_test_alpha_widgets[widget].kind != LV_TEST_ALPHA_LABEL) return p;

    /* Over a background, alpha doesn't show where the glyph is: find it over
     * nothing, where the glyph is all there is */
    lv_obj_t * screen = lv_screen_active();
    lv_test_alpha_scene_create(screen, widget, LV_TEST_ALPHA_BG_NONE);
    lv_obj_invalidate(screen);
    lv_refr_now(NULL);

    const lv_draw_buf_t * buf = lv_display_get_buf_active(lv_display_get_default());
    uint8_t max_a = 0;
    for(int32_t y = 0; y < (int32_t)buf->header.h; y++) {
        const uint8_t * row = buf->data + y * buf->header.stride;
        for(int32_t x = 0; x < (int32_t)buf->header.w; x++) {
            if(row[x * 4 + 3] > max_a) {
                max_a = row[x * 4 + 3];
                p.x = x;
                p.y = y;
            }
        }
    }

    lv_obj_clean(screen);
    return p;
}

lv_color32_t lv_test_alpha_expected(lv_test_alpha_widget_id_t widget, lv_test_alpha_bg_id_t bg)
{
    const lv_test_alpha_widget_t * s = &lv_test_alpha_widgets[widget];
    const lv_test_alpha_bg_t * d = &lv_test_alpha_bgs[bg];

    /* Source-over in straight alpha, scaled by 255:
     * out_a * 255 = Sa + Da * (1 - Sa)
     * out_c = (S * Sa + D * Da * (1 - Sa)) / out_a */
    uint32_t sa = s->opa;
    uint32_t da_part = d->opa * (255 - sa);
    uint32_t out_a_255 = sa * 255 + da_part;

    lv_color32_t res = {0};
    if(out_a_255 == 0) return res;
    res.alpha = (uint8_t)((out_a_255 + 127) / 255);

    uint32_t shift[3] = {16, 8, 0};
    uint8_t * ch[3] = {&res.red, &res.green, &res.blue};
    for(int32_t i = 0; i < 3; i++) {
        uint32_t sc = (s->color >> shift[i]) & 0xff;
        uint32_t dc = (d->color >> shift[i]) & 0xff;
        *ch[i] = (uint8_t)((sc * sa * 255 + dc * da_part + out_a_255 / 2) / out_a_255);
    }
    return res;
}

lv_test_alpha_pixel_t lv_test_alpha_scene_read(const lv_draw_buf_t * buf, lv_point_t point)
{
    const uint8_t * px = buf->data + point.y * buf->header.stride + point.x * 4;

    lv_test_alpha_pixel_t res;
    lv_memcpy(res.raw, px, 4);

    res.straight.blue = px[0];
    res.straight.green = px[1];
    res.straight.red = px[2];
    res.straight.alpha = px[3];
    if(buf->header.cf == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED && px[3] != 0) {
        uint32_t a = px[3];
        res.straight.blue = (uint8_t)LV_MIN(255, (px[0] * 255 + a / 2) / a);
        res.straight.green = (uint8_t)LV_MIN(255, (px[1] * 255 + a / 2) / a);
        res.straight.red = (uint8_t)LV_MIN(255, (px[2] * 255 + a / 2) / a);
    }
    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void vector_draw_event_cb(lv_event_t * e)
{
#if LV_USE_VECTOR_GRAPHIC
    const lv_test_alpha_widget_t * c = lv_event_get_user_data(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);

    lv_draw_vector_dsc_t * dsc = lv_draw_vector_dsc_create(lv_event_get_layer(e));
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_vector_path_append_rect(path, &coords, 0, 0);
    lv_draw_vector_dsc_set_fill_color(dsc, lv_color_hex(c->color));
    lv_draw_vector_dsc_set_fill_opa(dsc, c->opa);
    lv_draw_vector_dsc_add_path(dsc, path);
    lv_draw_vector(dsc);
    lv_vector_path_delete(path);
    lv_draw_vector_dsc_delete(dsc);
#else
    LV_UNUSED(e);
#endif
}

#endif /*LV_BUILD_TEST*/
