#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../src/misc/cache/lv_cache.h"

#include <string.h>

#include "unity/unity.h"

static lv_layer_t layer;
static lv_obj_t * canvas;
static lv_draw_buf_t * canvas_buf;

void setUp(void)
{
    canvas = lv_canvas_create(lv_screen_active());
    canvas_buf = lv_draw_buf_create(480, 480, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(canvas_buf);
    lv_canvas_set_draw_buf(canvas, canvas_buf);
    lv_canvas_fill_bg(canvas, lv_color_make(0xff, 0xff, 0xff), 255);
    lv_canvas_init_layer(canvas, &layer);
}

void tearDown(void)
{
    lv_image_cache_drop(canvas_buf);
    lv_draw_buf_destroy(canvas_buf);
    lv_obj_delete(canvas);
}

#if LV_USE_VECTOR_GRAPHIC && LV_USE_SVG

#define SNAPSHOT_NAME(n) (#n)

static void draw_snapshot(const char * name)
{
    LV_UNUSED(name);
#ifndef NON_AMD64_BUILD
    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "draw/svg_draw_%s.lp64.png", name);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);
#else
    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "draw/svg_draw_%s.lp32.png", name);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);
#endif
}

static void draw_svg(lv_svg_node_t * svg)
{
    lv_image_cache_drop(canvas_buf);
    lv_canvas_set_draw_buf(canvas, canvas_buf);
    lv_canvas_fill_bg(canvas, lv_color_make(0xff, 0xff, 0xff), 255);
    lv_draw_svg(&layer, svg);
    lv_canvas_finish_layer(canvas, &layer);
}

void test_draw_group(void)
{
    const char * svg_group_1 = \
                               "<svg><g fill=\"#FF0000\">"
                               "<rect x=\"0\" y=\"0\" width=\"100\" height=\"100\"/>"
                               "<circle cx=\"100\" cy=\"100\" r=\"50\"/>"
                               "<ellipse fill=\"#00F\" cx=\"200\" cy=\"200\" rx=\"100\" ry=50/>"
                               "</g></svg>";

    lv_svg_node_t * svg = lv_svg_load_data(svg_group_1, lv_strlen(svg_group_1));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_group_1));
    lv_svg_node_delete(svg);

    const char * svg_group_2 = \
                               "<svg><g fill=\"blue\"><g>"
                               "<rect width=\"100\" height=\"100\" fill=\"inherit\"/>"
                               "<circle cx=\"200\" cy=\"200\" r=\"50\" fill=\"green\"/>"
                               "</g></g></svg>";

    svg = lv_svg_load_data(svg_group_2, lv_strlen(svg_group_2));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_group_2));
    lv_svg_node_delete(svg);

    const char * svg_group_3 = \
                               "<svg><use xlink:href=\"#g1\" x=50 y=50/>"
                               "<g fill=\"blue\" xml:id=\"g1\">"
                               "<rect width=20 height=20/>"
                               "<rect x=30 width=20 height=20 fill=green/>"
                               "</g></svg>";

    svg = lv_svg_load_data(svg_group_3, lv_strlen(svg_group_3));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_group_3));
    lv_svg_node_delete(svg);

    const char * svg_group_4 = \
                               "<svg><defs><linearGradient id=\"s1\">"
                               "<stop offset=\"0.1\" stop-color=\"red\"/>"
                               "<stop offset=\"0.8\" stop-color=\"green\"/>"
                               "</linearGradient></defs>"
                               "<g fill=\"url(#s1)\" id=\"g1\">"
                               "<rect width=20 height=20/><g>"
                               "<rect x=30 width=20 height=20 fill=blue/>"
                               "<rect x=30 x=80 y= 30 width=20 height =20 fill=red/></g></g>"
                               "<use xlink:href=\"#g1\" x=150 y=100/>"
                               "<use xlink:href=\"#g1\" x=250 y=100/></svg>";

    svg = lv_svg_load_data(svg_group_4, lv_strlen(svg_group_4));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_group_4));
    lv_svg_node_delete(svg);

}

static void load_image(const char * image_url, lv_draw_image_dsc_t * img_dsc)
{
    if(strcmp(image_url, "cogwheel.bin") == 0) {
        LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);
        img_dsc->header = test_image_cogwheel_argb8888.header;
        img_dsc->src = &test_image_cogwheel_argb8888;
    }
}

static const char * get_font_path(const char * font_family)
{
    LV_UNUSED(font_family);
#ifndef TEST_FONT_PATH
    return "./src/test_files/fonts/noto/NotoSansSC-Regular.ttf";
#else
    return TEST_FONT_PATH;
#endif
}

const lv_svg_render_hal_t hal = {
    .load_image = load_image,
    .get_font_path = get_font_path,
};

void test_draw_shapes(void)
{
    lv_svg_render_init(&hal);
    const char * svg_shapes_1 = \
                                "<svg><rect fill=\"red\" x=\"0\" y=\"0\" width=\"100\" height=\"100\"/>"
                                "<circle fill=\"red\" cx=\"100\" cy=\"100\" r=\"50\"/>"
                                "<ellipse stroke=\"red\" fill=\"none\" cx=\"200\" cy=\"200\" rx=\"100\" ry=50/>"
                                "</svg>";

    lv_svg_node_t * svg = lv_svg_load_data(svg_shapes_1, lv_strlen(svg_shapes_1));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_1));
    lv_svg_node_delete(svg);

    const char * svg_shapes_2 = \
                                "<svg>"
                                "<rect fill=\"rgb(0, 0, 200)\" x=\"0\" y=\"0\" width=\"100\" height=\"100\""
                                " transform=\"translate(50, 50) rotate(45) translate(-50, -50)\"/>"
                                "</svg>";

    svg = lv_svg_load_data(svg_shapes_2, lv_strlen(svg_shapes_2));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_2));
    lv_svg_node_delete(svg);

    const char * svg_shapes_3 = \
                                "<svg width=\"4cm\" height=\"2cm\" viewBox=\"0 0 400 200\">"
                                "<defs><linearGradient xml:id=\"MyGradient\">"
                                "<stop offset=\"0.05\" stop-color=\"#F60\"/>"
                                "<stop offset=\"0.95\" stop-color=\"#FF6\"/>"
                                "</linearGradient></defs>"
                                "<rect fill=\"url(#MyGradient)\" stroke=\"black\" stroke-width=\"5\""
                                " x=\"100\" y=\"100\" width=\"600\" height=\"200\"/>"
                                "</svg>";

    svg = lv_svg_load_data(svg_shapes_3, lv_strlen(svg_shapes_3));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_3));
    lv_svg_node_delete(svg);

    const char * svg_shapes_4 = \
                                "<svg width=\"480\" height=\"360\" viewBox=\"0 0 480 360\">"
                                "<defs><solidColor xml:id=\"solidMaroon\" solid-color=\"maroon\" solid-opacity=\"0.7\"/>"
                                "</defs><g>"
                                "<circle transform=\"translate(100, 150)\" fill=\"url(#solidMaroon)\" r=\"30\"/>"
                                "<rect fill=\"url(#solidMaroon)\" transform=\"translate(190, 150)\" x=\"-30\" y=\"-30\" width=\"60\" height=\"60\"/>"
                                "<path fill=\"url(#solidMaroon)\" transform=\"translate(270, 150)\"  d=\"M 0 -30 L 30 30 L -30 30 Z\" />"
                                "<text fill=\"url(#solidMaroon)\" transform=\"translate(340, 150)\" "
                                "y=\"21\" font-weight=\"bold\" font-size=\"60\">A</text>"
                                "</g></svg>";

    svg = lv_svg_load_data(svg_shapes_4, lv_strlen(svg_shapes_4));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_4));
    lv_svg_node_delete(svg);

    const char * svg_shapes_5 = \
                                "<svg width=\"8cm\" height=\"4cm\" viewBox=\"0 0 800 400\">"
                                "<defs><radialGradient id=\"MyGradient\" gradientUnits=\"userSpaceOnUse\""
                                "  cx=\"400\" cy=\"200\" r=\"300\">"
                                "<stop offset=\"0.2\" stop-color=\"black\"/>"
                                "<stop offset=\"0.75\" stop-color=\"white\"/>"
                                "</radialGradient></defs>"
                                "<rect fill=\"url(#MyGradient)\" stroke=\"black\" stroke-width=\"5\""
                                " x=\"100\" y=\"100\" width=\"600\" height=\"200\"/></svg>";

    svg = lv_svg_load_data(svg_shapes_5, lv_strlen(svg_shapes_5));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_5));
    lv_svg_node_delete(svg);

    const char * svg_shapes_6 = \
                                "<svg width=\"8cm\" height=\"4cm\" viewBox=\"0 0 800 400\">"
                                "<defs><radialGradient id=\"MyGradient\">"
                                "<stop offset=\"0.2\" stop-color=\"white\"/>"
                                "<stop offset=\"0.75\" stop-color=\"black\"/>"
                                "</radialGradient></defs>"
                                "<rect fill=\"url(#MyGradient)\" stroke=\"black\" stroke-width=\"5\""
                                "x=\"100\" y=\"100\" width=\"300\" height=\"300\"/></svg>";

    svg = lv_svg_load_data(svg_shapes_6, lv_strlen(svg_shapes_6));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_6));
    lv_svg_node_delete(svg);

    const char * svg_shapes_7 = \
                                "<svg width=\"8cm\" height=\"4cm\" viewBox=\"0 0 800 400\">"
                                "<defs><linearGradient xml:id=\"MyGradient\" x1=0 y1=0 x2=500 y2=350 gradientUnits=\"userSpaceOnUse\">"
                                "<stop offset=\"0.05\" stop-color=\"#F60\"/>"
                                "<stop offset=\"0.95\" stop-color=\"#FF6\"/>"
                                "</linearGradient></defs>"
                                "<rect fill=\"url(#MyGradient)\" stroke=\"black\" stroke-width=\"5\""
                                " x=\"100\" y=\"100\" width=\"600\" height=\"200\"/></svg>";

    svg = lv_svg_load_data(svg_shapes_7, lv_strlen(svg_shapes_7));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_7));
    lv_svg_node_delete(svg);

    const char * svg_shapes_8 = \
                                "<svg width=\"7cm\" height=\"2cm\" viewBox=\"0 0 700 200\">"
                                "<g><defs><linearGradient id=\"MyGradient\" gradientUnits=\"objectBoundingBox\">"
                                "<stop offset=\"0\" stop-color=\"#F60\"/>"
                                "<stop offset=\"1\" stop-color=\"#FF6\"/>"
                                "</linearGradient></defs>"
                                "<rect x=\"1\" y=\"1\" width=\"698\" height=\"198\" fill=\"none\" stroke=\"blue\" stroke-width=\"2\"/>"
                                "<g fill=\"url(#MyGradient)\">"
                                "<rect x=\"100\" y=\"50\" width=\"200\" height=\"100\"/>"
                                "<rect x=\"400\" y=\"50\" width=\"200\" height=\"100\"/>"
                                "</g></g></svg>";

    svg = lv_svg_load_data(svg_shapes_8, lv_strlen(svg_shapes_8));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_8));
    lv_svg_node_delete(svg);

    const char * svg_shapes_9 = \
                                "<svg width=\"8cm\" height=\"4cm\" viewBox=\"0 0 800 400\">"
                                "<g><defs><linearGradient id=\"MyGradient\" gradientUnits=\"userSpaceOnUse\""
                                " x1=0 y1=0 x2=350 y2=350>"
                                "<stop offset=\"0\" stop-color=\"red\"/>"
                                "<stop offset=\"0.5\" stop-color=\"blue\"/>"
                                "</linearGradient></defs>"
                                "<rect fill=\"url(#MyGradient)\" stroke=\"black\" stroke-width=\"5\""
                                " x=\"0\" y=\"0\" width=\"200\" height=\"600\"/></g></svg>";

    svg = lv_svg_load_data(svg_shapes_9, lv_strlen(svg_shapes_9));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_9));
    lv_svg_node_delete(svg);

    const char * svg_shapes_10 = \
                                 "<svg width='144' height='144' viewBox='0 0 144 144'><g>"
                                 "<rect x='4' y='4' width='136' height='136' fill='url(#paint2_linear_13691_50994)' fill-opacity='1.0'/>"
                                 "</g><defs><linearGradient id='paint2_linear_13691_50994' x1='4' y1='4' x2='280' y2='280'"
                                 " gradientUnits='userSpaceOnUse'>"
                                 "<stop stop-color='white'/><stop offset='1' stop-color='#000000'/></linearGradient>"
                                 "</defs></svg>";

    svg = lv_svg_load_data(svg_shapes_10, lv_strlen(svg_shapes_10));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_10));
    lv_svg_node_delete(svg);

    const char * svg_shapes_11 = \
                                 "<svg width=\'800\' height=\'800\' viewBox=\'0 0 800 800\' fill=\'none\' xmlns=\'http://www.w3.org/2000/svg\'>"
                                 "<rect x=\'10\' y=\'10\' width=\'200\' height=\'200\' fill=\'url(#paint0_linear_13691_50971)\'/>"
                                 "<rect x=\'212\' y=\'10\' width=\'200\' height=\'200\' fill=\'url(#paint0_linear_13691_50971)\'/>"
                                 "<rect x=\'10\' y=\'212\' width=\'200\' height=\'200\' fill=\'url(#paint0_linear_13691_50971)\'/>"
                                 "<rect x=\'212\' y=\'212\' width=\'200\' height=\'200\' fill=\'url(#paint0_linear_13691_50971)\'/>"
                                 "<defs><linearGradient id=\'paint0_linear_13691_50971\' x1=\'10\' y1=\'10\' "
                                 "x2=\'400\' y2=\'400\' gradientUnits=\'userSpaceOnUse\' >"
                                 "<stop offset=\'0\' stop-color=\'red\' stop-opacity=\'1\'/>"
                                 "<stop offset=\'1\' stop-color=\'green\' stop-opacity=\'1\'/>"
                                 "</linearGradient></defs></svg>";

    svg = lv_svg_load_data(svg_shapes_11, lv_strlen(svg_shapes_11));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_11));
    lv_svg_node_delete(svg);

    const char * svg_shapes_12 = \
                                 "<svg width=600 height=200 viewBox=\"0 0 1200 400\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"
                                 "<rect x=\"1\" y=\"1\" width=\"1198\" height=\"398\" fill=\"none\" stroke=\"blue\" stroke-width=\"1\"/>"
                                 "<path d=\"M300,200 h-150 a150,150 0 1,0 150,-150 z\" fill=\"red\" stroke=\"blue\" stroke-width=\"5\"/>"
                                 "<path d=\"M275,175 v-150 a150,150 0 0,0 -150,150 z\""
                                 " fill=\"yellow\" stroke=\"blue\" stroke-width=\"5\"/>"
                                 "<path d=\"M600,350 l 50,-25"
                                 " a25,25 -30 0,1 50,-25 l 50,-25"
                                 " a25,50 -30 0,1 50,-25 l 50,-25"
                                 " a25,75 -30 0,1 50,-25 l 50,-25"
                                 " a25,100 -30 0,1 50,-25 l 50,-25\""
                                 " fill=\"none\" stroke=\"red\" stroke-width=\"5\"/></svg>";

    svg = lv_svg_load_data(svg_shapes_12, lv_strlen(svg_shapes_12));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_shapes_12));
    lv_svg_node_delete(svg);
}

void test_draw_image(void)
{
    lv_svg_render_init(&hal);

    const char * svg_image_0 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"240\" preserveAspectRatio=\"none\""
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"240\" fill=\"none\" stroke=\"blue\"/></svg>";

    lv_svg_node_t * svg = lv_svg_load_data(svg_image_0, lv_strlen(svg_image_0));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_0));
    lv_svg_node_delete(svg);

    const char * svg_image_1 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"240\""
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"240\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_1, lv_strlen(svg_image_1));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_1));
    lv_svg_node_delete(svg);

    const char * svg_image_2 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"540\" preserveAspectRatio=\"xMinYMin\" "
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"540\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_2, lv_strlen(svg_image_2));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_2));
    lv_svg_node_delete(svg);

    const char * svg_image_3 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"540\" preserveAspectRatio=\" xMinYMid\" "
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"540\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_3, lv_strlen(svg_image_3));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_3));
    lv_svg_node_delete(svg);

    const char * svg_image_4 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"540\" opacity=\"0.5\" preserveAspectRatio=\" xMinYMax\" "
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"540\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_4, lv_strlen(svg_image_4));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_4));
    lv_svg_node_delete(svg);

    const char * svg_image_5 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"240\" preserveAspectRatio=\"xMidYMin\" "
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"240\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_5, lv_strlen(svg_image_5));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_5));
    lv_svg_node_delete(svg);

    const char * svg_image_6 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"240\" preserveAspectRatio=\"xMidYMax\" "
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"240\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_6, lv_strlen(svg_image_6));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_6));
    lv_svg_node_delete(svg);

    const char * svg_image_7 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"240\" preserveAspectRatio=\"xMaxYMin\" "
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"240\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_7, lv_strlen(svg_image_7));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_7));
    lv_svg_node_delete(svg);

    const char * svg_image_8 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"240\" preserveAspectRatio=\"xMaxYMid\" "
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"240\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_8, lv_strlen(svg_image_8));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_8));
    lv_svg_node_delete(svg);

    const char * svg_image_9 = \
                               "<svg width=\"400\" height=\"400\">"
                               "<image x=\"0\" y=\"0\" width=\"360\" height=\"240\" preserveAspectRatio=\"xMaxYMax\" "
                               "xlink:href=\"cogwheel.bin\" />"
                               "<rect x=\"0\" y=\"0\" width=\"360\" height=\"240\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_9, lv_strlen(svg_image_9));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_9));
    lv_svg_node_delete(svg);

    const char * svg_image_10 = \
                                "<svg width=\"400\" height=\"400\">"
                                "<image x=\"0\" y=\"0\" width=\"50\" height=\"30\" preserveAspectRatio=\"meet\""
                                "xlink:href=\"cogwheel.bin\" />"
                                "<rect x=\"0\" y=\"0\" width=\"50\" height=\"30\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_10, lv_strlen(svg_image_10));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_10));
    lv_svg_node_delete(svg);

    const char * svg_image_11 = \
                                "<svg width=\"400\" height=\"400\">"
                                "<image x=\"0\" y=\"0\" width=\"50\" height=\"30\" preserveAspectRatio=\"xMidYMid slice\""
                                "xlink:href=\"cogwheel.bin\" />"
                                "<rect x=\"0\" y=\"0\" width=\"50\" height=\"30\" fill=\"none\" stroke=\"blue\"/></svg>";

    svg = lv_svg_load_data(svg_image_11, lv_strlen(svg_image_11));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_11));
    lv_svg_node_delete(svg);

    const char * svg_image_12 = \
                                "<svg width=\"200\" height=\"200\">"
                                "<image x=\"90\" y=\"-65\" width=\"80\" height=\"90\" transform=\"rotate(45)\""
                                " xlink:href=\"cogwheel.bin\"/>"
                                "</svg>";

    svg = lv_svg_load_data(svg_image_12, lv_strlen(svg_image_12));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_image_12));
    lv_svg_node_delete(svg);
}

void test_draw_text(void)
{
    lv_svg_render_init(&hal);

    const char * svg_text_1 = \
                              "<svg><text x=20 y=60 font-family=\"sans-serif\" font-size=\"24\">"
                              "hello <tspan fill=\"red\" font-size=\"36\">all</tspan> world"
                              "</text></svg>";

    lv_svg_node_t * svg = lv_svg_load_data(svg_text_1, lv_strlen(svg_text_1));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_text_1));
    lv_svg_node_delete(svg);

    const char * svg_text_2 = \
                              "<svg><defs><linearGradient id=\"g1\">"
                              "<stop offset=\"0.1\" stop-color=\"blue\"/>"
                              "<stop offset =\"0.8\" stop-color=\"red\"/>"
                              "</linearGradient></defs>"
                              "<text fill=\"url(#g1)\" x=20 y=60 font-family=\"sans-serif\" font-size=\"48px\" font-weight=\"bold\">"
                              "hello <tspan fill=\"green\" font-size=\"24px\">all</tspan> world"
                              "</text></svg>";

    svg = lv_svg_load_data(svg_text_2, lv_strlen(svg_text_2));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_text_2));
    lv_svg_node_delete(svg);
}

void test_draw_complex(void)
{
    lv_svg_render_init(&hal);

    const char * svg_com_1 = \
                             "<?xml version=\"1.0\"?><svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\""
                             "version=\"1.2\" baseProfile=\"tiny\" width=\"10cm\" height=\"3cm\" viewBox=\"0 0 100 30\">"
                             "<desc>'use' with a 'transform' attribute</desc>"
                             "<defs><rect xml:id=\"MyRect\" x=\"0\" y=\"0\" width=\"60\" height=\"10\"/></defs>"
                             "<rect x=\".1\" y=\".1\" width=\"99.8\" height=\"29.8\" fill=\"none\" stroke=\"blue\" stroke-width=\".2\"/>"
                             "<use xlink:href=\"#MyRect\" fill=\"red\" transform=\"translate(20,2.5) rotate(10)\"/>"
                             "<use xlink:href=\"#MyRect\" fill=\"green\" transform=\"translate(20,2.5) rotate(-10)\"/>"
                             "</svg>";

    lv_svg_node_t * svg = lv_svg_load_data(svg_com_1, lv_strlen(svg_com_1));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_com_1));
    lv_svg_node_delete(svg);

    const char * svg_com_2 = \
                             "<svg width=\"10cm\" height=\"3cm\" viewBox=\"0 0 100 30\">"
                             "<g id=g1><rect id=\"MyRect1\" x=\"0\" y=\"0\" width=\"60\" height=\"10\"/>"
                             "<rect id=\"MyRect2\" x=\"0\" y=\"12\" width=\"60\" height=\"10\"/>"
                             "</g><rect x=\".1\" y=\".1\" width=\"99.8\" height=\"29.8\""
                             " fill=\"none\" stroke=\"blue\" stroke-width=\".2\"/>"
                             "<use xlink:href=\"#g1\" fill=\"green\" transform=\"translate(20,2.5) rotate(-10)\"/>"
                             "<use xlink:href=\"#MyRect1\" fill=\"red\" transform=\"translate(20,2.5) rotate(10)\"/>"
                             "</svg>";

    svg = lv_svg_load_data(svg_com_2, lv_strlen(svg_com_2));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_com_2));
    lv_svg_node_delete(svg);

    const char * svg_com_3 = \
                             "<svg width=5cm height=4cm viewBox=\"0 0 500 400\">"
                             "<rect fill=none stroke=blue stroke-width=1 x=1 y=1 width=498 height=398 />"
                             "<polyline fill=none stroke=#888888 stroke-width=1 points=\"100,200 100,100\" />"
                             "<polyline fill=none stroke=#888888 stroke-width=1 points=\"250,100 250,200\" />"
                             "<polyline fill=none stroke=#888888 stroke-width=1 points=\"250,200 250,300\" />"
                             "<polyline fill=none stroke=#888888 stroke-width=1 points=\"400,300 400,200\" />"
                             "<path fill=none stroke=red stroke-width=5 d=\"M100,200 C100,100 250,100 250,200"
                             "                                       S400,300 400,200\" />"
                             "<circle fill=#888888 stroke=none stroke-width=2 cx=100 cy=200 r=10 />"
                             "<circle fill=#888888 stroke=none stroke-width=2 cx=250 cy=200 r=10 />"
                             "<circle fill=#888888 stroke=none stroke-width=2 cx=400 cy=200 r=10 />"
                             "<circle fill=#888888 stroke=none cx=100 cy=100 r=10 />"
                             "<circle fill=#888888 stroke=none cx=250 cy=100 r=10 />"
                             "<circle fill=#888888 stroke=none cx=400 cy=300 r=10 />"
                             "<circle fill=none stroke=blue stroke-width=4 cx=250 cy=300 r=9 />"
                             "<text font-size=22 font-family=\"Verdana\" x=25 y=70>M100,200 C100,100 250,100 250,200</text>"
                             "<text font-size=22 font-family=\"Verdana\" x=325 y=350>S400,300 400,200</text>"
                             "</svg>";

    svg = lv_svg_load_data(svg_com_3, lv_strlen(svg_com_3));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_com_3));
    lv_svg_node_delete(svg);

    const char * svg_com_4 = \
                             "<?xml version='1.0'?>"
                             "<svg width='12cm' height='4cm' viewBox='0 0 1200 400' "
                             "xmlns='http://www.w3.org/2000/svg' version='1.2' baseProfile='tiny'>"
                             "<desc>Example rect02 - rounded rectangles</desc>"
                             "<!-- Show outline of canvas using 'rect' element -->"
                             "<rect x='1' y='1' width='1198' height='398'"
                             " fill='none' stroke='blue' stroke-width='2'/>"
                             "<rect x='100' y='100' width='400' height='200' rx='50'"
                             " fill='green' />"
                             "<g transform='translate(700 210) rotate(-30)'>"
                             "<rect x='0' y='0' width='400' height='200' rx='50'"
                             " fill='none' stroke='purple' stroke-width='30' />"
                             "</g></svg>";

    svg = lv_svg_load_data(svg_com_4, lv_strlen(svg_com_4));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_com_4));
    lv_svg_node_delete(svg);

    const char * svg_com_5 = \
                             "<svg width='12cm' height='4cm' viewBox='0 0 1200 400'>"
                             "<rect x='1' y='1' width='1198' height='398' "
                             " fill='none' stroke='blue' stroke-width='2' />"
                             "<g transform='translate(300 200)'>"
                             "<ellipse rx='250' ry='100' "
                             "fill='red'  /></g>"
                             "<ellipse transform='translate(900 200) rotate(-30)' "
                             "rx='250' ry='100' fill='none' stroke='blue' stroke-width='20'  />"
                             "</svg>";

    svg = lv_svg_load_data(svg_com_5, lv_strlen(svg_com_5));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_com_5));
    lv_svg_node_delete(svg);

    const char * svg_com_6 = \
                             "<svg width='12cm' height='4cm' viewBox='0 0 1200 400'>"
                             "<rect x='1' y='1' width='1198' height='398'"
                             " fill='none' stroke='blue' stroke-width='2' />"
                             "<g stroke='green' >"
                             "<line x1='100' y1='300' x2='300' y2='100'"
                             " stroke-width='5'  />"
                             "<line x1='300' y1='300' x2='500' y2='100'"
                             "     stroke-width='10'  />"
                             "<line x1='500' y1='300' x2='700' y2='100'"
                             "    stroke-width='15'  />"
                             "<line x1='700' y1='300' x2='900' y2='100'"
                             "      stroke-width='20'  />"
                             "<line x1='900' y1='300' x2='1100' y2='100'"
                             "     stroke-width='25'  />"
                             "</g></svg>";

    svg = lv_svg_load_data(svg_com_6, lv_strlen(svg_com_6));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_com_6));
    lv_svg_node_delete(svg);

    const char * svg_com_7 = \
                             "<svg width='12cm' height='4cm' viewBox='0 0 1200 400'>"
                             "<rect x='1' y='1' width='1198' height='398'"
                             " fill='none' stroke='blue' stroke-width='2' />"
                             "<polyline fill='none' stroke='blue' stroke-width='10' "
                             " points='50,375"
                             "    150,375 150,325 250,325 250,375"
                             "    350,375 350,250 450,250 450,375"
                             "    550,375 550,175 650,175 650,375"
                             "    750,375 750,100 850,100 850,375"
                             "    950,375 950,25 1050,25 1050,375"
                             "    1150,375' />"
                             "</svg>";

    svg = lv_svg_load_data(svg_com_7, lv_strlen(svg_com_7));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_com_7));
    lv_svg_node_delete(svg);

    const char * svg_com_8 = \
                             "<svg xmlns='http://www.w3.org/2000/svg' "
                             "width='100%' height='100%' viewBox='0 0 400 400' "
                             "direction='rtl' xml:lang='fa'>"
                             "<text x='200' y='200' font-size='20'>داستان SVG Tiny 1.2 طولا ني است.</text>"
                             "</svg>";

    svg = lv_svg_load_data(svg_com_8, lv_strlen(svg_com_8));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_com_8));
    lv_svg_node_delete(svg);

}

void test_draw_svg(void)
{
    const char * svg_viewport_1 = \
                                  "<svg width=\"300px\" height=\"300px\" viewport-fill-opacity=\"0.5\" viewport-fill=\"blue\"></svg>";

    lv_svg_node_t * svg = lv_svg_load_data(svg_viewport_1, lv_strlen(svg_viewport_1));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_viewport_1));
    lv_svg_node_delete(svg);

    const char * svg_viewport_2 = \
                                  "<svg width=\"100px\" height=\"100px\" viewport-fill=\"green\"></svg>";

    svg = lv_svg_load_data(svg_viewport_2, lv_strlen(svg_viewport_2));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_viewport_2));
    lv_svg_node_delete(svg);

    const char * svg_viewport_3 = \
                                  "<svg width=\"300\" height=\"200\" viewBox=\"100 0 1500 1000\""
                                  "preserveAspectRatio=\"none\">"
                                  "<rect x=\"0\" y=\"0\" width=\"1500\" height=\"1000\""
                                  "fill=\"yellow\" stroke=\"blue\" stroke-width=\"12\"/>"
                                  "<path fill=\"red\"  d=\"M 750,100 L 250,900 L 1250,900 z\"/>"
                                  "<text x=\"100\" y=\"600\" font-size=\"200\" font-family=\"Verdana\">"
                                  "Stretch to fit</text>"
                                  "</svg>";

    svg = lv_svg_load_data(svg_viewport_3, lv_strlen(svg_viewport_3));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    draw_svg(svg);
    draw_snapshot(SNAPSHOT_NAME(svg_viewport_3));
    lv_svg_node_delete(svg);
}
#else

void test_draw_svg(void)
{
    ;
}
#endif
#endif
