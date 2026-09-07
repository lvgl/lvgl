#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#define TEST_DISP_HOR_RES   200
#define TEST_DISP_VER_RES   100

/** Every callback appends a character to this log so the call order can be asserted */
static char call_log[64];
static uint32_t call_cnt;
static lv_display_t * last_target;
static lv_display_t * last_current_target;
static void * last_param;
static void * last_user_data;
static lv_event_code_t last_code;
static bool stop_in_preprocess;
static bool stop_in_normal;
static lv_display_t * test_disp;
static lv_draw_buf_t * test_draw_buf;

static void log_append(char c);
static void cb_a(lv_event_t * e);
static void cb_b(lv_event_t * e);
static void cb_pre_a(lv_event_t * e);
static void cb_pre_b(lv_event_t * e);
static void cb_record(lv_event_t * e);
static void cb_count(lv_event_t * e);
static void dummy_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

void setUp(void)
{
    call_log[0] = '\0';
    call_cnt = 0;
    last_target = NULL;
    last_current_target = NULL;
    last_param = NULL;
    last_user_data = NULL;
    last_code = LV_EVENT_ALL;
    stop_in_preprocess = false;
    stop_in_normal = false;

    test_draw_buf = lv_draw_buf_create(TEST_DISP_HOR_RES, TEST_DISP_VER_RES, LV_COLOR_FORMAT_DEFAULT, 0);
    test_disp = lv_display_create(TEST_DISP_HOR_RES, TEST_DISP_VER_RES);
    lv_display_set_flush_cb(test_disp, dummy_flush_cb);
    lv_display_set_draw_buffers(test_disp, test_draw_buf, NULL);
    lv_display_set_render_mode(test_disp, LV_DISPLAY_RENDER_MODE_DIRECT);
}

void tearDown(void)
{
    if(test_disp) lv_display_delete(test_disp);
    test_disp = NULL;
    if(test_draw_buf) lv_draw_buf_destroy(test_draw_buf);
    test_draw_buf = NULL;
}

static void log_append(char c)
{
    uint32_t len = lv_strlen(call_log);
    if(len + 1 >= sizeof(call_log)) return;
    call_log[len] = c;
    call_log[len + 1] = '\0';
}

static void cb_a(lv_event_t * e)
{
    LV_UNUSED(e);
    log_append('a');
}

static void cb_b(lv_event_t * e)
{
    LV_UNUSED(e);
    log_append('b');
}

static void cb_pre_a(lv_event_t * e)
{
    log_append('A');
    if(stop_in_preprocess) lv_event_stop_processing(e);
}

static void cb_pre_b(lv_event_t * e)
{
    LV_UNUSED(e);
    log_append('B');
}

static void cb_record(lv_event_t * e)
{
    call_cnt++;
    last_code = lv_event_get_code(e);
    last_target = lv_event_get_target(e);
    last_current_target = lv_event_get_current_target(e);
    last_param = lv_event_get_param(e);
    last_user_data = lv_event_get_user_data(e);
    if(stop_in_normal) lv_event_stop_processing(e);
}

static void cb_count(lv_event_t * e)
{
    LV_UNUSED(e);
    call_cnt++;
}

static void dummy_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);
    lv_display_flush_ready(disp);
}

/* A freshly created display already has LVGL's own internal callback registered.
 * The API must report it and hand out its descriptor. */
void test_display_event_fresh_display_has_internal_cb(void)
{
    TEST_ASSERT_GREATER_THAN_UINT32(0, lv_display_get_event_count(test_disp));
    TEST_ASSERT_NOT_NULL(lv_display_get_event_dsc(test_disp, 0));
    TEST_ASSERT_NOT_NULL(lv_event_dsc_get_cb(lv_display_get_event_dsc(test_disp, 0)));
}

void test_display_event_add_and_query_dsc(void)
{
    const uint32_t base_cnt = lv_display_get_event_count(test_disp);
    int user_data_a = 0;
    int user_data_b = 0;

    lv_event_dsc_t * dsc_a = lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_REFR_START, &user_data_a);
    lv_event_dsc_t * dsc_b = lv_display_add_event_cb(test_disp, cb_b, LV_EVENT_ALL, &user_data_b);

    TEST_ASSERT_NOT_NULL(dsc_a);
    TEST_ASSERT_NOT_NULL(dsc_b);
    TEST_ASSERT_TRUE(dsc_a != dsc_b);
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 2, lv_display_get_event_count(test_disp));

    /* The descriptors are handed back in registration order, after the internal ones */
    TEST_ASSERT_EQUAL_PTR(dsc_a, lv_display_get_event_dsc(test_disp, base_cnt));
    TEST_ASSERT_EQUAL_PTR(dsc_b, lv_display_get_event_dsc(test_disp, base_cnt + 1));

    TEST_ASSERT_EQUAL_PTR(cb_a, lv_event_dsc_get_cb(dsc_a));
    TEST_ASSERT_EQUAL_PTR(cb_b, lv_event_dsc_get_cb(dsc_b));
    TEST_ASSERT_EQUAL_PTR(&user_data_a, lv_event_dsc_get_user_data(dsc_a));
    TEST_ASSERT_EQUAL_PTR(&user_data_b, lv_event_dsc_get_user_data(dsc_b));

    /* Out of range index */
    TEST_ASSERT_NULL(lv_display_get_event_dsc(test_disp, base_cnt + 2));
}

/* An event descriptor is only invoked for its own filter, or for any code when
 * registered with LV_EVENT_ALL */
void test_display_event_filter(void)
{
    lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(test_disp, cb_b, LV_EVENT_ALL, NULL);

    lv_display_send_event(test_disp, LV_EVENT_RESOLUTION_CHANGED, NULL);
    TEST_ASSERT_EQUAL_STRING("ab", call_log);

    lv_display_send_event(test_disp, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);
    TEST_ASSERT_EQUAL_STRING("abb", call_log);
}

/* For a display event both targets are the display itself, and code/param/user_data
 * are handed to the callback unchanged */
void test_display_event_target_param_and_user_data(void)
{
    int user_data = 0;
    int param = 0;

    lv_display_add_event_cb(test_disp, cb_record, LV_EVENT_REFR_REQUEST, &user_data);
    lv_result_t res = lv_display_send_event(test_disp, LV_EVENT_REFR_REQUEST, &param);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL(LV_EVENT_REFR_REQUEST, last_code);
    TEST_ASSERT_EQUAL_PTR(test_disp, last_target);
    TEST_ASSERT_EQUAL_PTR(test_disp, last_current_target);
    TEST_ASSERT_EQUAL_PTR(&param, last_param);
    TEST_ASSERT_EQUAL_PTR(&user_data, last_user_data);
}

/* Descriptors marked with LV_EVENT_PREPROCESS run before the plain ones,
 * and lv_event_get_code() reports the code without the flag */
void test_display_event_preprocess_runs_first(void)
{
    lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_REFR_START, NULL);
    lv_display_add_event_cb(test_disp, cb_pre_a, LV_EVENT_REFR_START | LV_EVENT_PREPROCESS, NULL);
    lv_display_add_event_cb(test_disp, cb_b, LV_EVENT_REFR_START, NULL);
    lv_display_add_event_cb(test_disp, cb_pre_b, LV_EVENT_REFR_START | LV_EVENT_PREPROCESS, NULL);
    lv_display_add_event_cb(test_disp, cb_record, LV_EVENT_REFR_START | LV_EVENT_PREPROCESS, NULL);

    lv_display_send_event(test_disp, LV_EVENT_REFR_START, NULL);

    TEST_ASSERT_EQUAL_STRING("ABab", call_log);
    TEST_ASSERT_EQUAL(LV_EVENT_REFR_START, last_code);
}

/* lv_event_stop_processing() in a preprocess callback skips every remaining
 * callback, including the non-preprocess ones */
void test_display_event_stop_processing_in_preprocess(void)
{
    lv_display_add_event_cb(test_disp, cb_pre_a, LV_EVENT_REFR_START | LV_EVENT_PREPROCESS, NULL);
    lv_display_add_event_cb(test_disp, cb_pre_b, LV_EVENT_REFR_START | LV_EVENT_PREPROCESS, NULL);
    lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_REFR_START, NULL);

    stop_in_preprocess = true;
    lv_display_send_event(test_disp, LV_EVENT_REFR_START, NULL);

    TEST_ASSERT_EQUAL_STRING("A", call_log);
}

/* lv_event_stop_processing() in a plain callback skips the remaining plain ones */
void test_display_event_stop_processing_in_normal(void)
{
    lv_display_add_event_cb(test_disp, cb_record, LV_EVENT_REFR_START, NULL);
    lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_REFR_START, NULL);

    stop_in_normal = true;
    lv_display_send_event(test_disp, LV_EVENT_REFR_START, NULL);

    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL_STRING("", call_log);
}

void test_display_event_remove_by_index(void)
{
    const uint32_t base_cnt = lv_display_get_event_count(test_disp);

    lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_REFR_START, NULL);
    lv_display_add_event_cb(test_disp, cb_b, LV_EVENT_REFR_START, NULL);

    TEST_ASSERT_TRUE(lv_display_remove_event(test_disp, base_cnt));
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 1, lv_display_get_event_count(test_disp));

    lv_display_send_event(test_disp, LV_EVENT_REFR_START, NULL);
    TEST_ASSERT_EQUAL_STRING("b", call_log);

    /* Removing a non-existing index must fail without touching the list */
    TEST_ASSERT_FALSE(lv_display_remove_event(test_disp, base_cnt + 1));
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 1, lv_display_get_event_count(test_disp));
}

/* Only the descriptors matching both the callback and the user_data are removed */
void test_display_event_remove_cb_with_user_data(void)
{
    const uint32_t base_cnt = lv_display_get_event_count(test_disp);
    int user_data_1 = 0;
    int user_data_2 = 0;

    lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_REFR_START, &user_data_1);
    lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_REFR_READY, &user_data_1);
    lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_REFR_START, &user_data_2);
    lv_display_add_event_cb(test_disp, cb_b, LV_EVENT_REFR_START, &user_data_1);

    TEST_ASSERT_EQUAL_UINT32(2, lv_display_remove_event_cb_with_user_data(test_disp, cb_a, &user_data_1));
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 2, lv_display_get_event_count(test_disp));

    lv_display_send_event(test_disp, LV_EVENT_REFR_START, NULL);
    TEST_ASSERT_EQUAL_STRING("ab", call_log);

    /* Nothing matches anymore */
    TEST_ASSERT_EQUAL_UINT32(0, lv_display_remove_event_cb_with_user_data(test_disp, cb_a, &user_data_1));
}

/* A callback may remove itself while the list is being traversed */
static void cb_remove_self(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);
    call_cnt++;
    lv_display_remove_event_cb_with_user_data(disp, cb_remove_self, NULL);
}

void test_display_event_remove_own_cb_while_sending(void)
{
    const uint32_t base_cnt = lv_display_get_event_count(test_disp);

    lv_display_add_event_cb(test_disp, cb_remove_self, LV_EVENT_REFR_START, NULL);
    lv_display_add_event_cb(test_disp, cb_a, LV_EVENT_REFR_START, NULL);

    lv_display_send_event(test_disp, LV_EVENT_REFR_START, NULL);

    /* The self-removing callback ran once, the one after it still ran */
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL_STRING("a", call_log);
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 1, lv_display_get_event_count(test_disp));

    /* The removal really took effect */
    lv_display_send_event(test_disp, LV_EVENT_REFR_START, NULL);
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL_STRING("aa", call_log);
}

/* Custom event IDs are unique and can be sent to a display like the built-in ones */
void test_display_event_custom_registered_id(void)
{
    lv_event_code_t custom_1 = (lv_event_code_t)lv_event_register_id();
    lv_event_code_t custom_2 = (lv_event_code_t)lv_event_register_id();

    TEST_ASSERT_NOT_EQUAL(custom_1, custom_2);
    TEST_ASSERT_GREATER_OR_EQUAL(LV_EVENT_LAST, custom_1);

    lv_display_add_event_cb(test_disp, cb_record, custom_1, NULL);
    lv_display_add_event_cb(test_disp, cb_a, custom_2, NULL);

    lv_display_send_event(test_disp, custom_1, NULL);

    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL(custom_1, last_code);
    TEST_ASSERT_EQUAL_STRING("", call_log);
}

void test_display_event_resolution_changed(void)
{
    lv_display_add_event_cb(test_disp, cb_count, LV_EVENT_RESOLUTION_CHANGED, NULL);

    lv_display_set_resolution(test_disp, TEST_DISP_HOR_RES * 2, TEST_DISP_VER_RES * 2);
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);

    lv_display_set_rotation(test_disp, LV_DISPLAY_ROTATION_90);
    TEST_ASSERT_EQUAL_UINT32(2, call_cnt);

    lv_display_set_rotation(test_disp, LV_DISPLAY_ROTATION_0);
    TEST_ASSERT_EQUAL_UINT32(3, call_cnt);
}

void test_display_event_color_format_changed(void)
{
    lv_display_add_event_cb(test_disp, cb_count, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);

    lv_display_set_color_format(test_disp, LV_COLOR_FORMAT_RGB888);
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);

    lv_display_set_color_format(test_disp, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_EQUAL_UINT32(2, call_cnt);
}

/* lv_event_get_invalidated_area() only returns the area for LV_EVENT_INVALIDATE_AREA */
static void cb_check_invalidated_area(lv_event_t * e)
{
    lv_area_t * area = lv_event_get_invalidated_area(e);
    call_cnt++;
    TEST_ASSERT_NOT_NULL(area);
    last_param = area;
}

static void cb_invalidated_area_wrong_code(lv_event_t * e)
{
    call_cnt++;
    TEST_ASSERT_NULL(lv_event_get_invalidated_area(e));
}

void test_display_event_get_invalidated_area(void)
{
    lv_area_t area = { 1, 2, 3, 4 };

    lv_display_add_event_cb(test_disp, cb_check_invalidated_area, LV_EVENT_INVALIDATE_AREA, NULL);
    lv_display_add_event_cb(test_disp, cb_invalidated_area_wrong_code, LV_EVENT_REFR_START, NULL);

    lv_display_send_event(test_disp, LV_EVENT_INVALIDATE_AREA, &area);
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL_PTR(&area, last_param);

    lv_display_send_event(test_disp, LV_EVENT_REFR_START, NULL);
    TEST_ASSERT_EQUAL_UINT32(2, call_cnt);
}

/* The handler of LV_EVENT_INVALIDATE_AREA may enlarge the area, and the
 * modification must be the one LVGL stores as invalid */
static void cb_enlarge_invalidated_area(lv_event_t * e)
{
    lv_area_t * area = lv_event_get_invalidated_area(e);
    call_cnt++;
    area->x1 = 0;
    area->y1 = 0;
    area->x2 = TEST_DISP_HOR_RES - 1;
    area->y2 = TEST_DISP_VER_RES - 1;
}

void test_display_event_invalidate_area_can_be_modified(void)
{
    lv_obj_t * obj = lv_obj_create(lv_display_get_screen_active(test_disp));
    lv_obj_set_size(obj, 10, 10);
    lv_obj_set_pos(obj, 20, 20);

    /* The display was never refreshed, so the coordinates are still unresolved */
    lv_obj_update_layout(obj);

    /* Drop everything invalidated while building the widget */
    test_disp->inv_p = 0;

    lv_display_add_event_cb(test_disp, cb_enlarge_invalidated_area, LV_EVENT_INVALIDATE_AREA, NULL);
    lv_obj_invalidate(obj);

    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, test_disp->inv_p);
    TEST_ASSERT_EQUAL_INT32(0, test_disp->inv_areas[0].x1);
    TEST_ASSERT_EQUAL_INT32(0, test_disp->inv_areas[0].y1);
    TEST_ASSERT_EQUAL_INT32(TEST_DISP_HOR_RES - 1, test_disp->inv_areas[0].x2);
    TEST_ASSERT_EQUAL_INT32(TEST_DISP_VER_RES - 1, test_disp->inv_areas[0].y2);
}

/* One refresh sends the whole documented sequence of display events, in order */
static void cb_log_refr_code(lv_event_t * e)
{
    switch(lv_event_get_code(e)) {
        case LV_EVENT_REFR_START:
            log_append('S');
            break;
        case LV_EVENT_RENDER_START:
            log_append('r');
            break;
        case LV_EVENT_RENDER_READY:
            log_append('R');
            break;
        case LV_EVENT_FLUSH_START:
            log_append('f');
            break;
        case LV_EVENT_FLUSH_FINISH:
            log_append('F');
            break;
        case LV_EVENT_REFR_READY:
            log_append('E');
            break;
        default:
            break;
    }
}

void test_display_event_refresh_cycle_events(void)
{
    lv_display_add_event_cb(test_disp, cb_log_refr_code, LV_EVENT_REFR_START, NULL);
    lv_display_add_event_cb(test_disp, cb_log_refr_code, LV_EVENT_RENDER_START, NULL);
    lv_display_add_event_cb(test_disp, cb_log_refr_code, LV_EVENT_RENDER_READY, NULL);
    lv_display_add_event_cb(test_disp, cb_log_refr_code, LV_EVENT_FLUSH_START, NULL);
    lv_display_add_event_cb(test_disp, cb_log_refr_code, LV_EVENT_FLUSH_FINISH, NULL);
    lv_display_add_event_cb(test_disp, cb_log_refr_code, LV_EVENT_REFR_READY, NULL);

    lv_obj_invalidate(lv_display_get_screen_active(test_disp));
    lv_refr_now(test_disp);

    /* REFR_START/REFR_READY bracket the cycle, and each phase reports begin before
     * end. How many times the renderer flushes, and whether it does so inside or
     * after the render phase, is renderer specific. */
    const uint32_t len = lv_strlen(call_log);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(6, len);
    TEST_ASSERT_EQUAL_CHAR('S', call_log[0]);
    TEST_ASSERT_EQUAL_CHAR('E', call_log[len - 1]);
    TEST_ASSERT_NOT_NULL(lv_strchr(call_log, 'r'));
    TEST_ASSERT_NOT_NULL(lv_strchr(call_log, 'f'));
    TEST_ASSERT_TRUE(lv_strchr(call_log, 'r') < lv_strchr(call_log, 'R'));
    TEST_ASSERT_TRUE(lv_strchr(call_log, 'f') < lv_strchr(call_log, 'F'));

    /* A refresh with nothing invalidated still brackets the cycle */
    call_log[0] = '\0';
    lv_refr_now(test_disp);
    TEST_ASSERT_EQUAL_STRING("SE", call_log);
}

/* LV_EVENT_DELETE is sent while the display is still usable, and the whole
 * event list is dropped afterwards */
static void cb_on_delete(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);
    call_cnt++;
    last_code = lv_event_get_code(e);
    TEST_ASSERT_EQUAL_INT32(TEST_DISP_HOR_RES, lv_display_get_horizontal_resolution(disp));
}

void test_display_event_delete(void)
{
    lv_display_add_event_cb(test_disp, cb_on_delete, LV_EVENT_DELETE, NULL);

    lv_display_delete(test_disp);
    test_disp = NULL;

    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL(LV_EVENT_DELETE, last_code);
}

static void cb_vsync_request(lv_event_t * e)
{
    log_append(lv_event_get_param(e) == NULL ? '-' : '+');
}

void test_display_event_vsync_register_and_send(void)
{
    int user_data = 0;
    int param = 0;

    lv_display_add_event_cb(test_disp, cb_vsync_request, LV_EVENT_VSYNC_REQUEST, NULL);

    /* Without a registered handler there is nothing to notify */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_display_send_vsync_event(test_disp, &param));

    /* The first registration asks the driver to start generating vsync */
    TEST_ASSERT_TRUE(lv_display_register_vsync_event(test_disp, cb_record, &user_data));
    TEST_ASSERT_EQUAL_STRING("+", call_log);

    /* A second registration must not ask again */
    TEST_ASSERT_TRUE(lv_display_register_vsync_event(test_disp, cb_count, &user_data));
    TEST_ASSERT_EQUAL_STRING("+", call_log);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_display_send_vsync_event(test_disp, &param));
    TEST_ASSERT_EQUAL_UINT32(2, call_cnt);
    TEST_ASSERT_EQUAL(LV_EVENT_VSYNC, last_code);
    TEST_ASSERT_EQUAL_PTR(&param, last_param);
    TEST_ASSERT_EQUAL_PTR(&user_data, last_user_data);
}

void test_display_event_vsync_unregister(void)
{
    int user_data = 0;
    int param = 0;

    lv_display_add_event_cb(test_disp, cb_vsync_request, LV_EVENT_VSYNC_REQUEST, NULL);

    lv_display_register_vsync_event(test_disp, cb_record, &user_data);
    lv_display_register_vsync_event(test_disp, cb_count, &user_data);
    TEST_ASSERT_EQUAL_STRING("+", call_log);

    /* Unregistering something that was never registered fails */
    TEST_ASSERT_FALSE(lv_display_unregister_vsync_event(test_disp, cb_a, &user_data));
    TEST_ASSERT_FALSE(lv_display_unregister_vsync_event(test_disp, cb_record, &param));

    /* While one handler is left the driver keeps generating vsync */
    TEST_ASSERT_TRUE(lv_display_unregister_vsync_event(test_disp, cb_count, &user_data));
    TEST_ASSERT_EQUAL_STRING("+", call_log);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_display_send_vsync_event(test_disp, &param));
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);

    /* Dropping the last one asks the driver to stop */
    TEST_ASSERT_TRUE(lv_display_unregister_vsync_event(test_disp, cb_record, &user_data));
    TEST_ASSERT_EQUAL_STRING("+-", call_log);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_display_send_vsync_event(test_disp, &param));
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
}

/* The vsync helpers fall back to the default display when disp is NULL */
void test_display_event_vsync_default_display(void)
{
    lv_display_t * def = lv_display_get_default();
    TEST_ASSERT_NOT_NULL(def);
    TEST_ASSERT_TRUE(def != test_disp);

    const uint32_t base_cnt = lv_display_get_event_count(def);

    TEST_ASSERT_TRUE(lv_display_register_vsync_event(NULL, cb_count, NULL));
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 1, lv_display_get_event_count(def));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_display_send_vsync_event(NULL, NULL));
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);

    TEST_ASSERT_TRUE(lv_display_unregister_vsync_event(NULL, cb_count, NULL));
    TEST_ASSERT_EQUAL_UINT32(base_cnt, lv_display_get_event_count(def));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_display_send_vsync_event(NULL, NULL));
}

/* A vsync handler must not be notified after being removed with the plain
 * event API, even though vsync_count was not decremented */
void test_display_event_vsync_removed_with_plain_api(void)
{
    lv_display_register_vsync_event(test_disp, cb_count, NULL);
    lv_display_remove_event_cb_with_user_data(test_disp, cb_count, NULL);

    lv_display_send_vsync_event(test_disp, NULL);
    TEST_ASSERT_EQUAL_UINT32(0, call_cnt);
}

void test_display_event_code_names(void)
{
    /* Display related codes */
    TEST_ASSERT_EQUAL_STRING("EVENT_INVALIDATE_AREA", lv_event_code_get_name(LV_EVENT_INVALIDATE_AREA));
    TEST_ASSERT_EQUAL_STRING("EVENT_RESOLUTION_CHANGED", lv_event_code_get_name(LV_EVENT_RESOLUTION_CHANGED));
    TEST_ASSERT_EQUAL_STRING("EVENT_COLOR_FORMAT_CHANGED", lv_event_code_get_name(LV_EVENT_COLOR_FORMAT_CHANGED));
    TEST_ASSERT_EQUAL_STRING("EVENT_REFR_REQUEST", lv_event_code_get_name(LV_EVENT_REFR_REQUEST));
    TEST_ASSERT_EQUAL_STRING("EVENT_REFR_START", lv_event_code_get_name(LV_EVENT_REFR_START));
    TEST_ASSERT_EQUAL_STRING("EVENT_REFR_READY", lv_event_code_get_name(LV_EVENT_REFR_READY));
    TEST_ASSERT_EQUAL_STRING("EVENT_RENDER_START", lv_event_code_get_name(LV_EVENT_RENDER_START));
    TEST_ASSERT_EQUAL_STRING("EVENT_RENDER_READY", lv_event_code_get_name(LV_EVENT_RENDER_READY));
    TEST_ASSERT_EQUAL_STRING("EVENT_FLUSH_START", lv_event_code_get_name(LV_EVENT_FLUSH_START));
    TEST_ASSERT_EQUAL_STRING("EVENT_FLUSH_FINISH", lv_event_code_get_name(LV_EVENT_FLUSH_FINISH));
    TEST_ASSERT_EQUAL_STRING("EVENT_FLUSH_WAIT_START", lv_event_code_get_name(LV_EVENT_FLUSH_WAIT_START));
    TEST_ASSERT_EQUAL_STRING("EVENT_FLUSH_WAIT_FINISH", lv_event_code_get_name(LV_EVENT_FLUSH_WAIT_FINISH));
    TEST_ASSERT_EQUAL_STRING("EVENT_SYNC_START", lv_event_code_get_name(LV_EVENT_SYNC_START));
    TEST_ASSERT_EQUAL_STRING("EVENT_SYNC_FINISH", lv_event_code_get_name(LV_EVENT_SYNC_FINISH));
    TEST_ASSERT_EQUAL_STRING("EVENT_SYNC_WAIT_START", lv_event_code_get_name(LV_EVENT_SYNC_WAIT_START));
    TEST_ASSERT_EQUAL_STRING("EVENT_SYNC_WAIT_FINISH", lv_event_code_get_name(LV_EVENT_SYNC_WAIT_FINISH));
    TEST_ASSERT_EQUAL_STRING("EVENT_VSYNC", lv_event_code_get_name(LV_EVENT_VSYNC));
    TEST_ASSERT_EQUAL_STRING("EVENT_VSYNC_REQUEST", lv_event_code_get_name(LV_EVENT_VSYNC_REQUEST));

    /* A few non-display codes to cover the remaining groups */
    TEST_ASSERT_EQUAL_STRING("EVENT_ALL", lv_event_code_get_name(LV_EVENT_ALL));
    TEST_ASSERT_EQUAL_STRING("EVENT_LEAVE", lv_event_code_get_name(LV_EVENT_LEAVE));
    TEST_ASSERT_EQUAL_STRING("EVENT_INSERT", lv_event_code_get_name(LV_EVENT_INSERT));
    TEST_ASSERT_EQUAL_STRING("EVENT_CREATE", lv_event_code_get_name(LV_EVENT_CREATE));
    TEST_ASSERT_EQUAL_STRING("EVENT_REFRESH", lv_event_code_get_name(LV_EVENT_REFRESH));
    TEST_ASSERT_EQUAL_STRING("EVENT_UPDATE_LAYOUT_COMPLETED",
                             lv_event_code_get_name(LV_EVENT_UPDATE_LAYOUT_COMPLETED));

    /* The preprocess flag is not part of the name */
    TEST_ASSERT_EQUAL_STRING("EVENT_REFR_START",
                             lv_event_code_get_name(LV_EVENT_REFR_START | LV_EVENT_PREPROCESS));

    /* Codes that are flags or out of range have no name */
    TEST_ASSERT_EQUAL_STRING("EVENT_UNKNOWN", lv_event_code_get_name(LV_EVENT_LAST));
    TEST_ASSERT_EQUAL_STRING("EVENT_UNKNOWN", lv_event_code_get_name((lv_event_code_t)lv_event_register_id()));
}

#endif
