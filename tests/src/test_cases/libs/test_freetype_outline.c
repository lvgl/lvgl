/**
 * @file test_freetype_outline.c
 *
 * Regression test for FreeType outline event sizes.
 *
 * This test is in a separate file because it overrides the global
 * freetype outline event callback via lv_freetype_outline_add_event(),
 * which uses a single-slot design. Isolating it here prevents any
 * interference with other test cases that rely on the draw unit's
 * real outline callback for rendering.
 */

#if LV_BUILD_TEST
#include "lvgl.h"
#include "src/font/freetype/lv_freetype_private.h"

#include "unity/unity.h"

#if LV_USE_FREETYPE

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/**
 * Test that outline sizes (segments_size and data_size) are correctly
 * populated BEFORE LV_EVENT_CREATE is sent. This is a regression test
 * for a bug introduced in PR #7560 where the sizes calculation was
 * moved after LV_EVENT_CREATE, causing pre-allocation to fail.
 *
 * The test uses NotoSansSC-Regular.ttf glyph 'A' at ref_size=128
 * (LV_FREETYPE_OUTLINE_REF_SIZE_DEF):
 *   glyph_index=34, n_points=18, n_contours=2
 *   Expected: segments_size=18, data_size=44
 */

/**
 * Expected outline point data for NotoSansSC-Regular.ttf 'A' at ref_size=128.
 * Raw F26Dot6 coordinates from FT_Outline_Decompose.
 */
typedef struct {
    lv_freetype_outline_type_t type;
    lv_freetype_outline_vector_t to;
    lv_freetype_outline_vector_t control1;
    lv_freetype_outline_vector_t control2;
} expected_outline_point_t;

typedef struct {
    uint32_t index;
    const expected_outline_point_t * points;
} outline_test_obj_t;

static const expected_outline_point_t expected_outline_points[] = {
    {LV_FREETYPE_OUTLINE_MOVE_TO, {33, 0}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {2064, 6005}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {2908, 6005}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {4948, 0}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {4145, 0}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {3097, 3359}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_CONIC_TO, {2785, 4370}, {2933, 3875}, {0, 0}},
    {LV_FREETYPE_OUTLINE_CONIC_TO, {2490, 5390}, {2638, 4866}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {2458, 5390}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_CONIC_TO, {2166, 4370}, {2318, 4866}, {0, 0}},
    {LV_FREETYPE_OUTLINE_CONIC_TO, {1860, 3359}, {2015, 3875}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {795, 0}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {33, 0}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_MOVE_TO, {1090, 1835}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {1090, 2433}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {3858, 2433}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {3858, 1835}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_LINE_TO, {1090, 1835}, {0, 0}, {0, 0}},
    {LV_FREETYPE_OUTLINE_END, {0, 0}, {0, 0}, {0, 0}},
};

#define EXPECTED_OUTLINE_POINTS_COUNT (sizeof(expected_outline_points) / sizeof(expected_outline_points[0]))

static void test_outline_sizes_event_cb(lv_event_t * e)
{
    lv_freetype_outline_event_param_t * param = lv_event_get_param(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_CREATE: {
                /*
                 * At this point, sizes MUST already be calculated.
                 * Assert directly — if this fires, the regression is back.
                 *
                 * Expected values for NotoSansSC-Regular.ttf 'A' at ref_size=128:
                 *   segments_size = 18 (16 path segments + 2 contour closes)
                 *   data_size = 44 (22 vectors * 2)
                 */
                TEST_ASSERT_EQUAL_INT32(18, param->sizes.segments_size);
                TEST_ASSERT_EQUAL_INT32(44, param->sizes.data_size);

                outline_test_obj_t * obj = lv_malloc_zeroed(sizeof(outline_test_obj_t));
                TEST_ASSERT_NOT_NULL(obj);
                obj->points = expected_outline_points;
                param->outline = obj;
            }
            break;
        case LV_EVENT_DELETE: {
                outline_test_obj_t * obj = param->outline;
                TEST_ASSERT_NOT_NULL(obj);
                TEST_ASSERT_EQUAL_PTR(expected_outline_points, obj->points);
                TEST_ASSERT_EQUAL_UINT32(EXPECTED_OUTLINE_POINTS_COUNT, obj->index);
                lv_free(obj);
                param->outline = NULL;
            }
            break;
        case LV_EVENT_INSERT: {
                outline_test_obj_t * obj = param->outline;
                TEST_ASSERT_NOT_NULL(obj);
                TEST_ASSERT_LESS_THAN_UINT32(EXPECTED_OUTLINE_POINTS_COUNT, obj->index);
                TEST_ASSERT_EQUAL_PTR(expected_outline_points, obj->points);

                const expected_outline_point_t * exp = &obj->points[obj->index];
                TEST_ASSERT_EQUAL_INT32(exp->type, param->type);
                TEST_ASSERT_EQUAL_INT32(exp->to.x, param->to.x);
                TEST_ASSERT_EQUAL_INT32(exp->to.y, param->to.y);
                TEST_ASSERT_EQUAL_INT32(exp->control1.x, param->control1.x);
                TEST_ASSERT_EQUAL_INT32(exp->control1.y, param->control1.y);
                TEST_ASSERT_EQUAL_INT32(exp->control2.x, param->control2.x);
                TEST_ASSERT_EQUAL_INT32(exp->control2.y, param->control2.y);
                obj->index++;
            }
            break;
        default:
            break;
    }
}

void test_freetype_outline_sizes_available_on_create(void)
{
    /* Register our test event callback (overrides the draw unit's callback) */
    lv_freetype_outline_add_event(test_outline_sizes_event_cb, LV_EVENT_ALL, NULL);

    /* Create an outline font - this will trigger the outline event */
    lv_font_t * font = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                               LV_FREETYPE_FONT_RENDER_MODE_OUTLINE,
                                               24,
                                               LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NOT_NULL(font);

    /*
     * Trigger outline creation directly via the font glyph API.
     * Do NOT render (lv_refr_now) because the SW draw unit would try to use
     * our dummy outline object as a real lv_vector_path_t.
     */
    lv_font_glyph_dsc_t g_dsc;
    bool found = lv_font_get_glyph_dsc(font, &g_dsc, 'A', '\0');
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL(LV_FONT_GLYPH_FORMAT_VECTOR, g_dsc.format);

    /* get_glyph_bitmap triggers outline_create -> our event callback */
    const void * bitmap = lv_font_get_glyph_bitmap(&g_dsc, NULL);
    TEST_ASSERT_NOT_NULL(bitmap);

    /* Release the glyph (decrements cache ref count) */
    lv_font_glyph_release_draw_data(&g_dsc);

    /* Deleting the font destroys the cache, triggering LV_EVENT_DELETE */
    lv_freetype_font_delete(font);
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_freetype_outline_sizes_available_on_create(void)
{
}

#endif /*LV_USE_FREETYPE*/

#endif
