#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include "lv_test_indev.h"
#include "lv_test_helpers.h"

static void create_ui(void);
static void chart_type_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void buttonmatrix_event_cb(lv_event_t * e);
static lv_obj_t * list_button_create(lv_obj_t * parent);
static void opa_anim_cb(void * var, int32_t v);
static void draw_to_canvas(lv_obj_t * canvas);

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_binding(void)
{
    create_ui();

    /*Wait for the animation*/
    lv_test_indev_wait(500);

    TEST_ASSERT_EQUAL_SCREENSHOT("binding.png");
}

static void create_ui(void)
{
    /*Create a colors*/
    lv_color_t c1 = lv_color_hex(0xff0000);
    lv_color_t c2 = lv_palette_darken(LV_PALETTE_BLUE, 2);
    lv_color_t c3 = lv_color_mix(c1, c2, LV_OPA_60);

    /*Create a style*/
    static lv_style_t style_big_font;
    lv_style_init(&style_big_font);

    /*Use a built-in font*/
    lv_style_set_text_font(&style_big_font, &lv_font_montserrat_24);

    /*Get the active screen*/
    lv_obj_t * scr = lv_screen_active();

    /*Declare static array of integers, and test grid setting options*/
    static int32_t gird_cols[] = {300, LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static int32_t gird_rows[] = {100, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(scr, gird_cols, gird_rows);

    static lv_subject_t chart_type_subject;
    lv_subject_init_int(&chart_type_subject, 0);

    /*Create a widget*/
    lv_obj_t * dropdown = lv_dropdown_create(scr);

    /*Pass a string as argument*/
    lv_dropdown_set_options(dropdown, "Lines\nBars");

    /*Use grid align options*/
    lv_obj_set_grid_cell(dropdown, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    /*Bind to a subject*/
    lv_dropdown_bind_value(dropdown, &chart_type_subject);

    /*Create a chart with an external array of points*/
    lv_obj_t * chart = lv_chart_create(lv_screen_active());
    lv_obj_set_grid_cell(chart, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_chart_series_t * series = lv_chart_add_series(chart, c3, LV_CHART_AXIS_PRIMARY_X);

    static int32_t chart_y_array[10] = {10, 25, 50, 40, 30, 35, 60, 65, 70, 75};
    lv_chart_set_ext_y_array(chart, series, chart_y_array);

    /*Add custom observer callback*/
    lv_subject_add_observer_obj(&chart_type_subject, chart_type_observer_cb, chart, NULL);

    /*Manually set the subject's value*/
    lv_subject_set_int(&chart_type_subject, 1);

    lv_obj_t * label = lv_label_create(scr);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    /*Apply styles on main part and default state*/
    lv_obj_set_style_bg_opa(label, LV_OPA_70, 0);
    lv_obj_set_style_bg_color(label, c1, 0);
    lv_obj_set_style_text_color(label, c2, 0);
    lv_obj_add_style(label, &style_big_font, 0);

    /*Declare an array of strings*/
    static const char * btnmatrix_options[] = {
        "First", "Second", "\n",
        "Third", ""
    };

    static const lv_buttonmatrix_ctrl_t btnmatrix_ctrl[] = {
        LV_BUTTONMATRIX_CTRL_DISABLED, 2 | LV_BUTTONMATRIX_CTRL_CHECKED,
        1,
    };

    lv_obj_t * btnmatrix = lv_buttonmatrix_create(scr);
    lv_obj_set_grid_cell(btnmatrix, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    /*Pass string and enum arrays*/
    lv_buttonmatrix_set_map(btnmatrix, btnmatrix_options);
    lv_buttonmatrix_set_ctrl_map(btnmatrix, btnmatrix_ctrl);
    /*Add style to non main part and non default state*/
    lv_obj_add_style(btnmatrix, &style_big_font, LV_PART_ITEMS | LV_STATE_CHECKED);

    lv_buttonmatrix_set_selected_button(btnmatrix, 1);
    lv_obj_add_event_cb(btnmatrix, buttonmatrix_event_cb, LV_EVENT_VALUE_CHANGED, label);
    lv_obj_send_event(btnmatrix, LV_EVENT_VALUE_CHANGED, NULL);

    /*Create a base object*/
    lv_obj_t * cont = lv_obj_create(scr);
    /*Span 2 rows*/
    lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 2);

    /*Apply flex layout*/
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    /*For loop*/
    uint32_t i;
    for(i = 0; i < 10; i++) {
        /*Call a function implemented in the binding which returns a widget*/
        lv_obj_t * btn = list_button_create(cont);

        if(i == 0) {
            /*Start an animation*/
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, btn);
            lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_50);
            lv_anim_set_exec_cb(&a, opa_anim_cb);   /*Pass a callback*/
            lv_anim_set_duration(&a, 300);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
            lv_anim_start(&a);
        }
        if(i == 1) {
            /*Use flags*/
            lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
        }
        if(i == 2) {
            lv_obj_t * btn_label = lv_obj_get_child(btn, 0);
            lv_label_set_text(btn_label, "A multi-line text with a ° symbol");
            lv_obj_set_width(btn_label, lv_pct(100));
        }

        if(i == 3) {
            /*Start an infinite animation and delete this button later*/
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, btn);
            lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_50);
            lv_anim_set_exec_cb(&a, opa_anim_cb);   /*Pass a callback*/
            lv_anim_set_duration(&a, 300);
            lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
            lv_anim_start(&a);
        }
    }

    /*Wait and delete the button with the animation*/
    lv_test_wait(300);
    lv_obj_delete(lv_obj_get_child(cont, 3));

    /*Large byte array*/
    static uint8_t canvas_buf[LV_CANVAS_BUF_SIZE(400, 100, 16, LV_DRAW_BUF_STRIDE_ALIGN)];

    lv_obj_t * canvas = lv_canvas_create(scr);
    lv_obj_set_grid_cell(canvas, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 2, 1);
    /*Test RGB565 rendering*/
    lv_canvas_set_buffer(canvas, lv_draw_buf_align(canvas_buf, LV_COLOR_FORMAT_RGB565), 400, 100, LV_COLOR_FORMAT_RGB565);
    lv_canvas_fill_bg(canvas, c2, LV_OPA_COVER);
    draw_to_canvas(canvas);

    LV_IMAGE_DECLARE(test_img_lvgl_logo_jpg);
    lv_obj_t * img;
    img = lv_image_create(scr);
    lv_image_set_src(img, &test_img_lvgl_logo_jpg);
    lv_obj_align(img, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_obj_add_flag(img, LV_OBJ_FLAG_IGNORE_LAYOUT);

    img = lv_image_create(scr);
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo.png");
    lv_obj_set_pos(img, 500, 420);
    lv_obj_add_flag(img, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_image_set_rotation(img, 200);
    lv_image_set_scale_x(img, 400);
}

static void chart_type_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    int32_t v = lv_subject_get_int(subject);
    lv_obj_t * chart = lv_observer_get_target(observer);
    lv_chart_set_type(chart, v == 0 ? LV_CHART_TYPE_LINE : LV_CHART_TYPE_BAR);
}

static void buttonmatrix_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_user_data(e);
    lv_obj_t * buttonmatrix = lv_event_get_target(e);

    uint32_t idx = lv_buttonmatrix_get_selected_button(buttonmatrix);
    const char * text = lv_buttonmatrix_get_button_text(buttonmatrix, idx);
    lv_label_set_text(label, text);
}

static lv_obj_t * list_button_create(lv_obj_t * parent)
{
    lv_obj_t * btn = lv_button_create(parent);
    lv_obj_set_size(btn, lv_pct(100), LV_SIZE_CONTENT);

    /*Get an integer*/
    uint32_t idx = lv_obj_get_index(btn);

    /*Formatted string for label*/
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text_fmt(label, LV_SYMBOL_FILE " Item %d", idx);

    return btn;
}

static void opa_anim_cb(void * var, int32_t v)
{
    lv_obj_set_style_opa(var, v, 0);
}

static void draw_to_canvas(lv_obj_t * canvas)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    /*Use draw descriptors*/
    LV_IMAGE_DECLARE(test_img_lvgl_logo_png);
    lv_draw_image_dsc_t image_draw_dsc;
    lv_draw_image_dsc_init(&image_draw_dsc);
    image_draw_dsc.src = &test_img_lvgl_logo_png;

    lv_area_t coords = {10, 10, 10 + test_img_lvgl_logo_png.header.w - 1, 10 + test_img_lvgl_logo_png.header.h - 1};
    lv_draw_image(&layer, &image_draw_dsc, &coords);

    /*Reuse the draw descriptor*/
    lv_area_move(&coords, 40, 40);
    image_draw_dsc.opa = LV_OPA_50;
    lv_draw_image(&layer, &image_draw_dsc, &coords);

    lv_draw_line_dsc_t line_draw_dsc;
    lv_draw_line_dsc_init(&line_draw_dsc);
    line_draw_dsc.color = lv_color_hex3(0xCA8);
    line_draw_dsc.width = 8;
    line_draw_dsc.round_end = 1;
    line_draw_dsc.round_start = 1;
    lv_point_precise_set(&line_draw_dsc.p1, 150, 30);
    lv_point_precise_set(&line_draw_dsc.p2, 350, 55);
    lv_draw_line(&layer, &line_draw_dsc);

    lv_canvas_finish_layer(canvas, &layer);

    lv_color_t c = lv_color_hex(0xff0000);
    uint32_t i;
    for(i = 0; i < 50; i++) {
        lv_canvas_set_px(canvas, 100 + i * 2, 10, c, LV_OPA_COVER);
    }
}

#endif
