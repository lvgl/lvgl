#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* A fake draw unit records the calls it gets, so that the tests can check when and with which arguments the draw
 * pipeline calls the main unit. It's created last, so it's the main unit, and it falls back to the software unit.
 * It refuses the fills of `REFUSED_COLOR`, so that the tests can send a fill to the fallback. */

#define LOG_MAX         64
#define CANVAS_SIZE     40

#define DRAWN_COLOR     lv_color_hex(0xff0000)
#define REFUSED_COLOR   lv_color_hex(0x00ff00)

typedef enum {
    CALL_FILL,
    CALL_IMAGE,
    CALL_LAYER,
} call_kind_t;

typedef struct {
    call_kind_t kind;
    lv_layer_t * layer;
    lv_area_t clip_area;
    lv_area_t coords;
    lv_color_t color;
    const void * src;
} call_t;

typedef struct {
    /* The number of draw calls made before the wait */
    uint32_t call_cnt;
    /* The canvas pixel at the center when the wait was made */
    lv_color32_t center_px;
} wait_t;

static call_t calls[LOG_MAX];
static uint32_t call_cnt;
static wait_t waits[LOG_MAX];
static uint32_t wait_cnt;

static lv_draw_unit_t * fake_unit;
static lv_obj_t * canvas;
static lv_draw_buf_t * canvas_buf;

static lv_color32_t center_px(void)
{
    if(canvas == NULL) return (lv_color32_t) {
        0
    };
    return lv_canvas_get_px(canvas, CANVAS_SIZE / 2, CANVAS_SIZE / 2);
}

static call_t * log_call(call_kind_t kind, lv_layer_t * layer, const lv_area_t * clip_area, const lv_area_t * coords)
{
    if(call_cnt >= LOG_MAX) {
        call_cnt++;
        return NULL;
    }
    call_t * c = &calls[call_cnt++];
    lv_memzero(c, sizeof(*c));
    c->kind = kind;
    c->layer = layer;
    if(clip_area) c->clip_area = *clip_area;
    if(coords) c->coords = *coords;
    return c;
}

static void fake_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    call_t * c = log_call(CALL_FILL, layer, clip_area, coords);
    if(c) c->color = dsc->color;
}

static bool fake_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    return !lv_color_eq(dsc->color, REFUSED_COLOR);
}

static void fake_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    call_t * c = log_call(CALL_IMAGE, layer, clip_area, coords);
    if(c) c->src = dsc->src;
}

static void fake_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    call_t * c = log_call(CALL_LAYER, layer, clip_area, coords);
    if(c) c->src = dsc->src;
}

static const lv_draw_unit_draw_cbs_t fake_draw_cbs = {
    .draw_fill_cb = fake_draw_fill,
    .can_draw_fill_cb = fake_can_draw_fill,
    .draw_image_cb = fake_draw_image,
    .draw_layer_cb = fake_draw_layer,
};

static int32_t fake_wait_for_finish(lv_draw_unit_t * draw_unit)
{
    LV_UNUSED(draw_unit);
    if(wait_cnt < LOG_MAX) {
        waits[wait_cnt].call_cnt = call_cnt;
        waits[wait_cnt].center_px = center_px();
    }
    wait_cnt++;
    return 0;
}

static void fake_unit_create(void)
{
    fake_unit = lv_draw_create_unit(sizeof(lv_draw_unit_t));
    fake_unit->name = "FAKE";
    fake_unit->draw_cbs = &fake_draw_cbs;
    fake_unit->wait_for_finish_cb = fake_wait_for_finish;
    fake_unit->fallback = lv_draw_sw_get_draw_unit();
}

static void fake_unit_delete(void)
{
    lv_draw_global_info_t * info = &LV_GLOBAL_DEFAULT()->draw_info;
    lv_draw_unit_t ** u = &info->unit_head;
    while(*u && *u != fake_unit) u = &(*u)->next;
    TEST_ASSERT_NOT_NULL(*u);
    *u = fake_unit->next;
    info->unit_cnt--;
    lv_free(fake_unit);
    fake_unit = NULL;
}

void setUp(void)
{
    call_cnt = 0;
    wait_cnt = 0;

    canvas_buf = lv_draw_buf_create(CANVAS_SIZE, CANVAS_SIZE, LV_COLOR_FORMAT_ARGB8888, 0);
    canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, canvas_buf);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);

    fake_unit_create();
}

void tearDown(void)
{
    fake_unit_delete();
    lv_obj_clean(lv_screen_active());
    canvas = NULL;
    lv_draw_buf_destroy(canvas_buf);
    canvas_buf = NULL;
}

static void fill(lv_layer_t * layer, lv_color_t color, const lv_area_t * area)
{
    lv_draw_fill_dsc_t dsc;
    lv_draw_fill_dsc_init(&dsc);
    dsc.color = color;
    lv_draw_fill(layer, &dsc, area);
}

static void assert_area(const lv_area_t * expected, const lv_area_t * actual)
{
    TEST_ASSERT_EQUAL_INT32(expected->x1, actual->x1);
    TEST_ASSERT_EQUAL_INT32(expected->y1, actual->y1);
    TEST_ASSERT_EQUAL_INT32(expected->x2, actual->x2);
    TEST_ASSERT_EQUAL_INT32(expected->y2, actual->y2);
}

static void assert_center_px(lv_color_t expected)
{
    lv_color32_t px = lv_canvas_get_px(canvas, CANVAS_SIZE / 2, CANVAS_SIZE / 2);
    TEST_ASSERT_EQUAL_HEX8(expected.red, px.red);
    TEST_ASSERT_EQUAL_HEX8(expected.green, px.green);
    TEST_ASSERT_EQUAL_HEX8(expected.blue, px.blue);
}

static const lv_area_t full_area = {0, 0, CANVAS_SIZE - 1, CANVAS_SIZE - 1};

void test_draw_sync_dispatch_draws_before_return(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    fill(&layer, DRAWN_COLOR, &full_area);

    /* The main unit has drawn the fill when lv_draw_fill() returns */
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL(CALL_FILL, calls[0].kind);
    TEST_ASSERT_EQUAL_PTR(&layer, calls[0].layer);
    TEST_ASSERT_TRUE(lv_color_eq(DRAWN_COLOR, calls[0].color));
    assert_area(&full_area, &calls[0].coords);
    assert_area(&full_area, &calls[0].clip_area);

    lv_canvas_finish_layer(canvas, &layer);

    /* and only once */
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
}

void test_draw_sync_dispatch_keeps_order_and_clip_area(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    const lv_area_t clips[3] = {
        {0, 0, 19, 19},
        {10, 10, 29, 29},
        {5, 20, 39, 39},
    };
    const lv_color_t colors[3] = {lv_color_hex(0x110000), lv_color_hex(0x220000), lv_color_hex(0x330000)};

    uint32_t i;
    for(i = 0; i < 3; i++) {
        layer._clip_area = clips[i];
        fill(&layer, colors[i], &full_area);
    }
    layer._clip_area = full_area;

    lv_canvas_finish_layer(canvas, &layer);

    /* Drawn in the order of the calls, each with the clip area set when it was called */
    TEST_ASSERT_EQUAL_UINT32(3, call_cnt);
    for(i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL(CALL_FILL, calls[i].kind);
        TEST_ASSERT_TRUE(lv_color_eq(colors[i], calls[i].color));
        assert_area(&clips[i], &calls[i].clip_area);
        assert_area(&full_area, &calls[i].coords);
    }
}

static uint32_t event_call_cnt_before;
static uint32_t event_call_cnt_after;
static lv_area_t event_clip_area;
static bool event_draw_main_sent;

static void draw_main_cb(lv_event_t * e)
{
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);

    event_draw_main_sent = true;
    event_clip_area = layer->_clip_area;
    event_call_cnt_before = call_cnt;
    fill(layer, DRAWN_COLOR, &obj->coords);
    event_call_cnt_after = call_cnt;
}

void test_draw_sync_dispatch_draws_before_return_while_refreshing(void)
{
    event_draw_main_sent = false;
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 50, 50);
    lv_obj_center(obj);
    lv_obj_add_event_cb(obj, draw_main_cb, LV_EVENT_DRAW_MAIN, NULL);

    lv_refr_now(NULL);

    TEST_ASSERT_TRUE(event_draw_main_sent);
    /* The fill made in the widget's draw event is drawn when lv_draw_fill() returns */
    TEST_ASSERT_EQUAL_UINT32(event_call_cnt_before + 1, event_call_cnt_after);
    call_t * c = &calls[event_call_cnt_before];
    TEST_ASSERT_EQUAL(CALL_FILL, c->kind);
    TEST_ASSERT_TRUE(lv_color_eq(DRAWN_COLOR, c->color));
    assert_area(&obj->coords, &c->coords);
    assert_area(&event_clip_area, &c->clip_area);
}

void test_draw_sync_dispatch_fallback_draws_refused(void)
{
#if LV_USE_DRAW_NANOVG
    TEST_IGNORE_MESSAGE("NanoVG overwrites the canvas on finish");
#endif

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    fill(&layer, REFUSED_COLOR, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    /* The main unit refused the fill and the software fallback drew it */
    TEST_ASSERT_EQUAL_UINT32(0, call_cnt);
    assert_center_px(REFUSED_COLOR);
}

void test_draw_sync_dispatch_waits_for_main_unit_before_fallback(void)
{
#if LV_USE_DRAW_NANOVG
    TEST_IGNORE_MESSAGE("NanoVG overwrites the canvas on finish");
#endif

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    const lv_area_t corner = {0, 0, 9, 9};
    fill(&layer, DRAWN_COLOR, &corner);
    fill(&layer, REFUSED_COLOR, &full_area);

    /* When lv_draw_fill() returns, the main unit has been waited for after its fill and before the fallback
     * touched the buffer */
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    bool waited = false;
    uint32_t i;
    for(i = 0; i < wait_cnt && i < LOG_MAX; i++) {
        if(waits[i].call_cnt == 1 && waits[i].center_px.green == 0x00) waited = true;
    }
    TEST_ASSERT_TRUE(waited);

    lv_canvas_finish_layer(canvas, &layer);
    assert_center_px(REFUSED_COLOR);
}

void test_draw_sync_dispatch_drops_what_no_unit_draws(void)
{
    fake_unit->fallback = NULL;

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    fill(&layer, REFUSED_COLOR, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    TEST_ASSERT_EQUAL_UINT32(0, call_cnt);
    assert_center_px(lv_color_black());
}

void test_draw_sync_dispatch_layer_drawn_before_return(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    const lv_area_t area = {5, 5, 34, 34};
    lv_layer_t * child = lv_draw_layer_create(&layer, LV_COLOR_FORMAT_ARGB8888, &area);
    TEST_ASSERT_NOT_NULL(child);

    fill(child, DRAWN_COLOR, &area);

    lv_draw_image_dsc_t dsc;
    lv_draw_image_dsc_init(&dsc);
    dsc.src = child;
    lv_draw_layer(&layer, &dsc, &area);

    /* The child is drawn, then blended into the parent, both when lv_draw_layer() returns */
    TEST_ASSERT_EQUAL_UINT32(2, call_cnt);
    TEST_ASSERT_EQUAL(CALL_FILL, calls[0].kind);
    TEST_ASSERT_EQUAL_PTR(child, calls[0].layer);
    TEST_ASSERT_EQUAL(CALL_LAYER, calls[1].kind);
    TEST_ASSERT_EQUAL_PTR(&layer, calls[1].layer);
    TEST_ASSERT_EQUAL_PTR(child, calls[1].src);
    assert_area(&area, &calls[1].coords);

    lv_canvas_finish_layer(canvas, &layer);
    TEST_ASSERT_EQUAL_UINT32(2, call_cnt);
}

/* Draws below the primitive in LV_EVENT_DRAW_DSC_BEGIN and on top of it in LV_EVENT_DRAW_DSC_END */
static void begin_fill_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_color_hex(0x0000b0);
    fill_dsc.base.obj = lv_event_get_target_obj(e);
    lv_draw_fill(dsc->layer, &fill_dsc, &full_area);
}

static void end_fill_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_color_hex(0x0000e0);
    fill_dsc.base.obj = lv_event_get_target_obj(e);
    lv_draw_fill(dsc->layer, &fill_dsc, &full_area);
}

static lv_obj_t * create_event_obj(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_send_draw_dsc_events(obj, true);
    return obj;
}

void test_draw_sync_dispatch_draw_dsc_events_order(void)
{
    lv_obj_t * obj = create_event_obj();
    lv_obj_add_event_cb(obj, begin_fill_cb, LV_EVENT_DRAW_DSC_BEGIN, NULL);
    lv_obj_add_event_cb(obj, end_fill_cb, LV_EVENT_DRAW_DSC_END, NULL);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_fill_dsc_t dsc;
    lv_draw_fill_dsc_init(&dsc);
    dsc.color = DRAWN_COLOR;
    dsc.base.obj = obj;
    lv_draw_fill(&layer, &dsc, &full_area);

    lv_canvas_finish_layer(canvas, &layer);

    /* The BEGIN fill goes below the primitive, the END fill on top */
    TEST_ASSERT_EQUAL_UINT32(3, call_cnt);
    TEST_ASSERT_TRUE(lv_color_eq(lv_color_hex(0x0000b0), calls[0].color));
    TEST_ASSERT_TRUE(lv_color_eq(DRAWN_COLOR, calls[1].color));
    TEST_ASSERT_TRUE(lv_color_eq(lv_color_hex(0x0000e0), calls[2].color));
}

static void shrink_begin_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    if(lv_draw_dsc_get_type(dsc) == LV_DRAW_DSC_TYPE_FILL) lv_draw_dsc_get_fill_dsc(dsc)->coords.x2 -= 10;
    if(lv_draw_dsc_get_type(dsc) == LV_DRAW_DSC_TYPE_IMAGE) lv_draw_dsc_get_image_dsc(dsc)->coords.x2 -= 10;
}

void test_draw_sync_dispatch_begin_coords_reach_unit(void)
{
    lv_obj_t * obj = create_event_obj();
    lv_obj_add_event_cb(obj, shrink_begin_cb, LV_EVENT_DRAW_DSC_BEGIN, NULL);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_fill_dsc_t dsc;
    lv_draw_fill_dsc_init(&dsc);
    dsc.color = DRAWN_COLOR;
    dsc.base.obj = obj;
    lv_draw_fill(&layer, &dsc, &full_area);

    lv_canvas_finish_layer(canvas, &layer);

    /* The unit gets the area changed in LV_EVENT_DRAW_DSC_BEGIN */
    lv_area_t expected = full_area;
    expected.x2 -= 10;
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    assert_area(&expected, &calls[0].coords);
}

void test_draw_sync_dispatch_begin_coords_reach_unit_rotated_image(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565a8);

    lv_obj_t * obj = create_event_obj();
    lv_obj_add_event_cb(obj, shrink_begin_cb, LV_EVENT_DRAW_DSC_BEGIN, NULL);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_image_dsc_t dsc;
    lv_draw_image_dsc_init(&dsc);
    dsc.src = &test_image_cogwheel_rgb565a8;
    dsc.rotation = 300;
    dsc.base.obj = obj;
    lv_draw_image(&layer, &dsc, &full_area);

    lv_canvas_finish_layer(canvas, &layer);

    lv_area_t expected = full_area;
    expected.x2 -= 10;
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL(CALL_IMAGE, calls[0].kind);
    TEST_ASSERT_EQUAL_PTR(&test_image_cogwheel_rgb565a8, calls[0].src);
    assert_area(&expected, &calls[0].coords);
}

#endif
