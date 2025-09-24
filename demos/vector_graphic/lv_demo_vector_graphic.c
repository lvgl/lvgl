/**
 * @file lv_demo_vector_graphic.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_vector_graphic.h"

#if LV_USE_DEMO_VECTOR_GRAPHIC

/*********************
 *      DEFINES
 *********************/
#define WIDTH 640
#define HEIGHT 480

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_pattern(lv_draw_vector_dsc_t * ctx, lv_vector_path_t * path)
{
    lv_vector_path_clear(path);
    lv_draw_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = {{200, 200}, {300, 200}, {300, 300}, {200, 300}};
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[1]);
    lv_vector_path_quad_to(path, &pts[2], &pts[3]);
    lv_vector_path_close(path);

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);

    LV_IMAGE_DECLARE(img_demo_vector_avatar);
    img_dsc.header = img_demo_vector_avatar.header;
    img_dsc.src = &img_demo_vector_avatar;

    lv_draw_vector_dsc_set_fill_image(ctx, &img_dsc);
    lv_draw_vector_dsc_translate(ctx, 250, 250);
    lv_draw_vector_dsc_rotate(ctx, 25);
    lv_draw_vector_dsc_translate(ctx, -250, -250);
    lv_draw_vector_dsc_add_path(ctx, path); // draw a path
}

static void draw_gradient(lv_draw_vector_dsc_t * ctx, lv_vector_path_t * path)
{
    lv_vector_path_clear(path);
    lv_draw_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = {{400, 200}, {600, 200}, {400, 400}};
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_quad_to(path, &pts[1], &pts[2]);
    lv_vector_path_close(path);

    lv_grad_stop_t stops[2];
    lv_memzero(stops, sizeof(stops));
    stops[0].color = lv_color_hex(0xff0000);
    stops[0].opa = LV_OPA_COVER;
    stops[0].frac = 0;
    stops[1].color = lv_color_hex(0x00ff00);
    stops[1].opa = LV_OPA_COVER;
    stops[1].frac = 255;

    lv_matrix_t mt;
    lv_matrix_identity(&mt);
    lv_matrix_rotate(&mt, 30);
    lv_draw_vector_dsc_set_fill_transform(ctx, &mt);

    lv_draw_vector_dsc_set_fill_linear_gradient(ctx, 200, 200, 400, 400);
    lv_draw_vector_dsc_set_fill_gradient_color_stops(ctx, stops, 2);
    lv_draw_vector_dsc_set_fill_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_PAD);
    lv_draw_vector_dsc_add_path(ctx, path); // draw a path
}

static void draw_radial_gradient(lv_draw_vector_dsc_t * ctx, lv_vector_path_t * path)
{
    lv_vector_path_clear(path);
    lv_draw_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = {{400, 50}, {500, 50}, {500, 200}, {400, 200}};
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[1]);
    lv_vector_path_line_to(path, &pts[2]);
    lv_vector_path_line_to(path, &pts[3]);
    lv_vector_path_close(path);

    lv_grad_stop_t stops[2];
    lv_memzero(stops, sizeof(stops));
    stops[0].color = lv_color_hex(0xff0000);
    stops[0].opa = LV_OPA_COVER;
    stops[0].frac = 0;
    stops[1].color = lv_color_hex(0x0000ff);
    stops[1].opa = LV_OPA_COVER;
    stops[1].frac = 255;

    lv_draw_vector_dsc_set_fill_radial_gradient(ctx, 450, 100, 20);
    lv_draw_vector_dsc_set_fill_gradient_color_stops(ctx, stops, 2);
    lv_draw_vector_dsc_set_fill_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_REFLECT);
    lv_draw_vector_dsc_add_path(ctx, path); // draw a path
}

static void draw_shapes(lv_draw_vector_dsc_t * ctx, lv_vector_path_t * path)
{
    lv_vector_path_clear(path);
    lv_draw_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = {{50, 50}, {200, 200}, {50, 200}};
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[1]);
    lv_vector_path_line_to(path, &pts[2]);
    lv_vector_path_close(path);
    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0xFF, 0x00, 0x00));
    lv_draw_vector_dsc_scale(ctx, 0.5, 0.5);
    lv_draw_vector_dsc_add_path(ctx, path); // draw a path

    lv_vector_path_clear(path);
    lv_draw_vector_dsc_identity(ctx);

    lv_area_t rect = {300, 300, 400, 400};
    lv_vector_path_append_rect(path, &rect, 50, 60);
    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x80, 0xff));
    lv_draw_vector_dsc_skew(ctx, 5, 0);
    lv_draw_vector_dsc_add_path(ctx, path); // draw a path

    lv_vector_path_clear(path);
    lv_draw_vector_dsc_identity(ctx);

    lv_area_t rect2 = {100, 300, 200, 400};
    lv_vector_path_append_rect(path, &rect2, 10, 10);
    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0x80, 0x00, 0x80));

    lv_vector_path_t * path2 = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_fpoint_t p = {50, 420};
    lv_vector_path_append_circle(path2, &p, 50, 30);
    lv_vector_path_append_path(path, path2);

    lv_draw_vector_dsc_add_path(ctx, path); // draw a path

    lv_vector_path_delete(path2);
}

static void draw_lines(lv_draw_vector_dsc_t * ctx, lv_vector_path_t * path)
{
    lv_vector_path_clear(path);
    lv_draw_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = {{50, 50}, {200, 200}, {250, 300}, {350, 150}};

    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_cubic_to(path, &pts[1], &pts[2], &pts[3]);

    lv_draw_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xff, 0x00));
    lv_draw_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_0);
    lv_draw_vector_dsc_set_stroke_width(ctx, 8.0f);

    float dashes[] = {10, 15, 20, 12};
    lv_draw_vector_dsc_set_stroke_dash(ctx, dashes, 4);

    lv_draw_vector_dsc_add_path(ctx, path); // draw a path

    lv_draw_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
}

static void draw_blend(lv_draw_vector_dsc_t * ctx, lv_vector_path_t * path)
{
    lv_vector_path_clear(path);
    lv_draw_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = {{200, 200}, {400, 200}, {450, 350}, {350, 150}};

    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_cubic_to(path, &pts[1], &pts[2], &pts[3]);
    lv_vector_path_close(path);
    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0xFF, 0x00, 0xFF));
    lv_draw_vector_dsc_set_blend_mode(ctx, LV_VECTOR_BLEND_SCREEN);

    lv_draw_vector_dsc_add_path(ctx, path); // draw a path
}

static void draw_arc(lv_draw_vector_dsc_t * ctx, lv_vector_path_t * path)
{
    lv_vector_path_clear(path);
    lv_draw_vector_dsc_identity(ctx);

    lv_area_t rect = {100, 0, 150, 50};
    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_lighten(lv_color_black(), 50));
    lv_draw_vector_dsc_clear_area(ctx, &rect); // clear screen

    lv_fpoint_t p = {100, 50}; /* Center */
    lv_draw_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xff, 0xff));
    lv_draw_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_draw_vector_dsc_set_stroke_width(ctx, 2.0f);
    lv_draw_vector_dsc_set_stroke_dash(ctx, NULL, 0);
    lv_draw_vector_dsc_set_fill_color32(ctx, lv_color32_make(0xFF, 0x00, 0x00, 0x80));

    lv_vector_path_move_to(path, &p);
    lv_fpoint_t temp = {p.x + 50, p.y};
    lv_vector_path_line_to(path, &temp);
    lv_vector_path_append_arc(path, &p, 50, 0, -90, false);
    lv_vector_path_line_to(path, &p);
    lv_vector_path_close(path);

    lv_draw_vector_dsc_add_path(ctx, path);

    /* Below code has same effect as above one. */
    lv_vector_path_clear(path);
    lv_vector_path_append_arc(path, &p, 50, 45, 45, true);
    lv_draw_vector_dsc_add_path(ctx, path); // draw a path
}

static void draw_vector(lv_layer_t * layer)
{
    lv_draw_vector_dsc_t * ctx = lv_draw_vector_dsc_create(layer);

    lv_area_t rect = {0, 100, 300, 300};
    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_lighten(lv_color_black(), 50));
    lv_draw_vector_dsc_clear_area(ctx, &rect); // clear screen

    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    draw_shapes(ctx, path);
    draw_lines(ctx, path);
    draw_pattern(ctx, path);
    draw_radial_gradient(ctx, path);
    draw_gradient(ctx, path);
    draw_blend(ctx, path);
    draw_arc(ctx, path);
    lv_draw_vector(ctx); // submit draw
    lv_vector_path_delete(path);
    lv_draw_vector_dsc_delete(ctx);
}

static void delete_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    lv_draw_buf_destroy(draw_buf);
}

static void event_cb(lv_event_t * e)
{
    lv_layer_t * layer = lv_event_get_layer(e);

    draw_vector(layer);
}

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_vector_graphic_not_buffered(void)
{
    lv_obj_add_event_cb(lv_screen_active(), event_cb, LV_EVENT_DRAW_MAIN, NULL);
}

void lv_demo_vector_graphic_buffered(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(WIDTH, HEIGHT, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    lv_draw_buf_clear(draw_buf, NULL);

    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, draw_buf);
    lv_obj_add_event_cb(canvas, delete_event_cb, LV_EVENT_DELETE, NULL);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    draw_vector(&layer);

    lv_canvas_finish_layer(canvas, &layer);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#else

void lv_demo_vector_graphic_not_buffered(void)
{
    /*fallback for online examples*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Vector graphics is not enabled");
    lv_obj_center(label);
}

void lv_demo_vector_graphic_buffered(void)
{
    /*fallback for online examples*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Vector graphics is not enabled");
    lv_obj_center(label);
}

#endif
