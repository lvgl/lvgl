#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"
#include "refr/lv_test_refr.h"

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG && LV_USE_DRAW_SW

/* Vector drawing in LV_DISPLAY_RENDER_MODE_PARTIAL, with draw buffers of a few rows, must give the
 * same pixels as rendering the whole frame at once. The strips put the layer's buffer origin away
 * from the screen origin and translate the shapes below zero, which exercises the vector leaf's
 * viewport and ThorVG's clipping and texture mapping of partially visible shapes. */

#define HOR_RES     200
#define VER_RES     200
#define STRIP_ROWS  16

typedef enum {
    SCENE_IMAGE_FILL,   /* image-filled rectangle rotated by 25 degrees */
    SCENE_IMAGE_CLIP,   /* a tall image pattern clipped by a small circle */
    SCENE_CLEAR_AREA,   /* only lv_draw_vector_dsc_clear_area */
} scene_t;

static scene_t scene;
static uint32_t pattern[96 * 96];
static uint32_t tall_pattern[32 * 128];
static lv_image_dsc_t pattern_img;
static lv_image_dsc_t tall_img;
static uint32_t * direct_px;
static uint32_t * partial_px;

/* The flat colors of the scene, as 0xRRGGBB */
#define SCREEN_COLOR    0x31577b
#define PANEL_COLOR     0x235fa1
#define LEFT_COLOR      0x7b395d
#define TAIL_COLOR      0x29af83

void setUp(void)
{
    refr_ctx_reset();
    uint32_t x, y;
    for(y = 0; y < 96; y++) {
        for(x = 0; x < 96; x++) {
            pattern[y * 96 + x] = 0xff000000u | ((x * 13 % 256) << 16) | ((y * 7 % 256) << 8) | ((x + y) * 3 % 256);
        }
    }
    lv_memset(tall_pattern, 255, sizeof(tall_pattern));

    lv_memzero(&pattern_img, sizeof(pattern_img));
    pattern_img.header.magic = LV_IMAGE_HEADER_MAGIC;
    pattern_img.header.cf = LV_COLOR_FORMAT_ARGB8888;
    pattern_img.header.w = 96;
    pattern_img.header.h = 96;
    pattern_img.header.stride = 96 * 4;
    pattern_img.header.flags = LV_IMAGE_FLAGS_PREMULTIPLIED;
    pattern_img.data_size = sizeof(pattern);
    pattern_img.data = (const uint8_t *)pattern;

    lv_memzero(&tall_img, sizeof(tall_img));
    tall_img.header.magic = LV_IMAGE_HEADER_MAGIC;
    tall_img.header.cf = LV_COLOR_FORMAT_ARGB8888;
    tall_img.header.w = 32;
    tall_img.header.h = 128;
    tall_img.header.stride = 32 * 4;
    tall_img.header.flags = LV_IMAGE_FLAGS_PREMULTIPLIED;
    tall_img.data_size = sizeof(tall_pattern);
    tall_img.data = (const uint8_t *)tall_pattern;

    direct_px = lv_malloc(HOR_RES * VER_RES * sizeof(uint32_t));
    partial_px = lv_malloc(HOR_RES * VER_RES * sizeof(uint32_t));
    TEST_ASSERT_NOT_NULL(direct_px);
    TEST_ASSERT_NOT_NULL(partial_px);
}

void tearDown(void)
{
    refr_disp_delete();
    lv_image_cache_drop(&pattern_img);
    lv_image_cache_drop(&tall_img);
    lv_free(direct_px);
    lv_free(partial_px);
}

static void draw_scene(lv_event_t * e)
{
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_area_t area;
    lv_obj_get_coords(lv_event_get_target_obj(e), &area);

    /* Two different backdrops under the vector drawing */
    lv_draw_rect_dsc_t rect;
    lv_draw_rect_dsc_init(&rect);
    rect.bg_color = lv_color_hex(LEFT_COLOR);
    lv_area_t left = area;
    left.x2 = left.x1 + 57;
    lv_draw_rect(layer, &rect, &left);

    lv_draw_vector_dsc_t * dsc = lv_draw_vector_dsc_create(layer);
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_draw_vector_dsc_set_stroke_opa(dsc, LV_OPA_TRANSP);
    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);

    if(scene == SCENE_IMAGE_FILL) {
        img_dsc.src = &pattern_img;
        img_dsc.header = pattern_img.header;
        lv_draw_vector_dsc_set_fill_image(dsc, &img_dsc);
        lv_draw_vector_dsc_translate(dsc, (float)(area.x1 + 64), (float)(area.y1 + 64));
        lv_draw_vector_dsc_rotate(dsc, 25);
        lv_draw_vector_dsc_translate(dsc, -64, -64);
        lv_area_t v = { 16, 16, 111, 111 };
        lv_vector_path_append_rect(path, &v, 0, 0);
        lv_draw_vector_dsc_add_path(dsc, path);
    }
    else if(scene == SCENE_IMAGE_CLIP) {
        img_dsc.src = &tall_img;
        img_dsc.header = tall_img.header;
        lv_draw_vector_dsc_set_fill_image(dsc, &img_dsc);
        lv_fpoint_t center = { (float)(area.x1 + 36), (float)(area.y1 + 24) };
        lv_vector_path_append_circle(path, &center, 12, 12);
        lv_draw_vector_dsc_add_path(dsc, path);
    }
    else {
        lv_area_t v = { area.x1 + 9, area.y1 + 7, area.x2 - 8, area.y2 - 6 };
        lv_draw_vector_dsc_clear_area(dsc, &v);
    }
    lv_draw_vector(dsc);
    lv_vector_path_delete(path);
    lv_draw_vector_dsc_delete(dsc);

    /* An ordinary task after the vector drawing */
    lv_area_t tail = { area.x1 + 28, area.y1 + 81, area.x2 - 23, area.y1 + 89 };
    rect.bg_color = lv_color_hex(TAIL_COLOR);
    lv_draw_rect(layer, &rect, &tail);
}

static lv_obj_t * build_ui(void)
{
    lv_obj_t * screen = refr_screen();
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(screen, lv_color_hex(SCREEN_COLOR), 0);
    lv_obj_t * panel = lv_obj_create(screen);
    lv_obj_remove_style_all(panel);
    lv_obj_set_pos(panel, 37, 43);
    lv_obj_set_size(panel, 119, 141);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(PANEL_COLOR), 0);
    lv_obj_add_event_cb(panel, draw_scene, LV_EVENT_DRAW_MAIN, NULL);
    return panel;
}

/* Render the scene. With `panel_only` the panel is invalidated and rendered once more, so the
 * invalidated area no longer starts at the screen origin. */
static void render(lv_display_render_mode_t mode, int32_t buf_h, bool panel_only, uint32_t * out)
{
    refr_disp_create(HOR_RES, VER_RES, LV_COLOR_FORMAT_XRGB8888, mode, 1, buf_h);
    lv_obj_t * panel = build_ui();
    refr_frame();
    if(panel_only) {
        lv_obj_invalidate(panel);
        refr_frame();
    }
    int32_t x, y;
    for(y = 0; y < VER_RES; y++) {
        for(x = 0; x < HOR_RES; x++) {
            out[y * HOR_RES + x] = refr_screen_px(x, y) & 0xffffff;
        }
    }
    refr_disp_delete();
}

static bool is_flat(uint32_t px)
{
    return px == SCREEN_COLOR || px == PANEL_COLOR || px == LEFT_COLOR || px == TAIL_COLOR;
}

/* Away from any antialiased contour: no flat color within 2 px in the reference */
static bool is_interior(const uint32_t * ref, int32_t x, int32_t y)
{
    int32_t dx, dy;
    for(dy = -2; dy <= 2; dy++) {
        for(dx = -2; dx <= 2; dx++) {
            int32_t xx = x + dx, yy = y + dy;
            if(xx < 0 || yy < 0 || xx >= HOR_RES || yy >= VER_RES) return false;
            if(is_flat(ref[yy * HOR_RES + xx])) return false;
        }
    }
    return true;
}

static uint32_t channel_diff(uint32_t a, uint32_t b)
{
    uint32_t d = 0, c;
    for(c = 0; c < 3; c++) {
        uint32_t v = LV_ABS((int32_t)((a >> (c * 8)) & 0xff) - (int32_t)((b >> (c * 8)) & 0xff));
        if(v > d) d = v;
    }
    return d;
}

/* Every pixel (or every interior pixel) of `act` must be within `tolerance` of `ref`.
 * Returns the number of pixels compared. */
static uint32_t assert_same_pixels(const uint32_t * ref, const uint32_t * act, bool interior_only, uint32_t tolerance)
{
    uint32_t compared = 0;
    int32_t x, y;
    for(y = 0; y < VER_RES; y++) {
        for(x = 0; x < HOR_RES; x++) {
            if(interior_only && !is_interior(ref, x, y)) continue;
            compared++;
            uint32_t i = y * HOR_RES + x;
            uint32_t d = channel_diff(ref[i], act[i]);
            if(d > tolerance) {
                char msg[128];
                lv_snprintf(msg, sizeof(msg), "pixel (%d,%d): direct %06x, partial %06x", (int)x, (int)y,
                            (unsigned)ref[i], (unsigned)act[i]);
                TEST_FAIL_MESSAGE(msg);
            }
        }
    }
    return compared;
}

void test_draw_vector_image_fill_in_partial_mode(void)
{
    scene = SCENE_IMAGE_FILL;
    render(LV_DISPLAY_RENDER_MODE_DIRECT, VER_RES, false, direct_px);

    /* The contour's antialiasing may differ where a strip cuts the shape; the texture must not. */
    render(LV_DISPLAY_RENDER_MODE_PARTIAL, STRIP_ROWS, false, partial_px);
    uint32_t compared = assert_same_pixels(direct_px, partial_px, true, 2);
    TEST_ASSERT_GREATER_THAN_UINT32(4000, compared);

    render(LV_DISPLAY_RENDER_MODE_PARTIAL, STRIP_ROWS, true, partial_px);
    assert_same_pixels(direct_px, partial_px, true, 2);
}

void test_draw_vector_image_clip_in_partial_mode(void)
{
    scene = SCENE_IMAGE_CLIP;
    render(LV_DISPLAY_RENDER_MODE_DIRECT, VER_RES, false, direct_px);

    /* Strips that meet the image but not the circle must stay empty */
    render(LV_DISPLAY_RENDER_MODE_PARTIAL, STRIP_ROWS, false, partial_px);
    assert_same_pixels(direct_px, partial_px, false, 2);

    render(LV_DISPLAY_RENDER_MODE_PARTIAL, STRIP_ROWS, true, partial_px);
    assert_same_pixels(direct_px, partial_px, false, 2);
}

void test_draw_vector_clear_area_in_partial_mode(void)
{
    scene = SCENE_CLEAR_AREA;
    render(LV_DISPLAY_RENDER_MODE_DIRECT, VER_RES, false, direct_px);

    render(LV_DISPLAY_RENDER_MODE_PARTIAL, STRIP_ROWS, false, partial_px);
    assert_same_pixels(direct_px, partial_px, false, 0);

    render(LV_DISPLAY_RENDER_MODE_PARTIAL, STRIP_ROWS, true, partial_px);
    assert_same_pixels(direct_px, partial_px, false, 0);
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_draw_vector_partial_mode_disabled(void)
{
    TEST_IGNORE_MESSAGE("Vector graphics with ThorVG are disabled");
}

#endif /*LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG && LV_USE_DRAW_SW*/
#endif /*LV_BUILD_TEST*/
