#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

/*The draw descriptor API was renamed in v10*/
#if LVGL_VERSION_MAJOR < 10
    #define LV_DRAW_DSC_TYPE_FILL LV_DRAW_TASK_TYPE_FILL
    #define LV_DRAW_DSC_TYPE_LAYER LV_DRAW_TASK_TYPE_LAYER
#endif

/* Layer effects (transforms, layered opacity, blend modes, bitmap masks, corner clipping, drop
 * shadows, rotated scale labels, masked bar indicators) need a draw unit that renders into child
 * layers. When no draw unit does, lv_draw_layer_supported() is false and the effects are omitted
 * while the content stays visible and the geometry follows what is drawn.
 *
 * The software draw unit supports layers, so these tests wrap its evaluate callback with one that
 * refuses tasks of child layers, like a display-only draw unit does. */

static lv_draw_unit_t * saved_units;
static lv_draw_unit_t mock_unit;
static lv_draw_unit_t * created_unit;
static uint32_t saved_unit_count;
static lv_display_t * other_display;
static lv_display_t * accepted_display;
static unsigned probe_calls;
static uint8_t * reference;
static uint8_t * frame;
static size_t frame_size;

/* The software unit and its real evaluate callback, wrapped while layers are refused */
static lv_draw_unit_t * sw_unit;
static int32_t (*sw_evaluate)(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static bool layers_refused;

/* A supported query probes two tasks: an opaque fill into a child layer, then the blend of
 * that layer onto the display's layer. A refused fill ends the query. */
static int32_t probe_evaluate(lv_draw_unit_t * unit, lv_draw_task_t * task)
{
    LV_UNUSED(unit);
    probe_calls++;
    TEST_ASSERT_TRUE(task->is_probe);
    TEST_ASSERT_EQUAL(LV_DRAW_TASK_STATE_WAITING, task->state);
    TEST_ASSERT_GREATER_THAN(0, lv_area_get_width(&task->area));
    TEST_ASSERT_EQUAL_MEMORY(&task->area, &task->clip_area, sizeof(task->area));
    lv_display_t * disp;
    if(task->type == LV_DRAW_DSC_TYPE_FILL) {
        TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_ARGB8888, task->target_layer->color_format);
        TEST_ASSERT_NULL(task->target_layer->draw_buf);
        TEST_ASSERT_NULL(task->target_layer->draw_task_head);
        TEST_ASSERT_NULL(task->target_layer->next);
        TEST_ASSERT_EQUAL_PTR(task->target_layer->display->layer_head, task->target_layer->parent);
        TEST_ASSERT_EQUAL_PTR(task->target_layer, ((lv_draw_fill_dsc_t *)task->draw_dsc)->base.layer);
        TEST_ASSERT_EQUAL(LV_OPA_COVER, ((lv_draw_fill_dsc_t *)task->draw_dsc)->opa);
        disp = task->target_layer->display;
    }
    else {
        TEST_ASSERT_EQUAL(LV_DRAW_DSC_TYPE_LAYER, task->type);
        lv_draw_image_dsc_t * dsc = task->draw_dsc;
        TEST_ASSERT_NULL(task->target_layer->parent);
        TEST_ASSERT_EQUAL_PTR(task->target_layer, dsc->base.layer);
        TEST_ASSERT_EQUAL_PTR(task->target_layer, ((const lv_layer_t *)dsc->src)->parent);
        TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_ARGB8888, dsc->header.cf);
        disp = ((const lv_layer_t *)dsc->src)->display;
    }
    /* The display under query is the one being refreshed while the probes run */
    TEST_ASSERT_EQUAL_PTR(disp, lv_refr_get_disp_refreshing());
    if(disp == accepted_display) task->preferred_draw_unit_id = 123;
    /* Some draw units claim a task by setting the id and still return zero */
    return 0;
}

/* The software unit's evaluate, refusing every task that targets a child layer */
static int32_t refusing_evaluate(lv_draw_unit_t * unit, lv_draw_task_t * task)
{
    if(task->target_layer->parent != NULL) return 0;
    return sw_evaluate(unit, task);
}

static void refuse_layers(bool refuse)
{
    TEST_ASSERT_NOT_NULL(sw_unit);
    sw_unit->evaluate_cb = refuse ? refusing_evaluate : sw_evaluate;
    layers_refused = refuse;
    lv_draw_invalidate_layer_support();
}

void setUp(void)
{
    saved_units = LV_GLOBAL_DEFAULT()->draw_info.unit_head;
    saved_unit_count = LV_GLOBAL_DEFAULT()->draw_info.unit_cnt;
    created_unit = NULL;
    other_display = NULL;
    accepted_display = NULL;
    probe_calls = 0;
    lv_memzero(&mock_unit, sizeof(mock_unit));
    mock_unit.evaluate_cb = probe_evaluate;

    sw_unit = NULL;
    for(lv_draw_unit_t * unit = saved_units; unit; unit = unit->next) {
        if(unit->name && lv_strncmp(unit->name, "SW", 2) == 0) sw_unit = unit;
    }
    TEST_ASSERT_NOT_NULL_MESSAGE(sw_unit, "These tests need the software draw unit");
    sw_evaluate = sw_unit->evaluate_cb;
    layers_refused = false;

    lv_obj_t * screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xffffff), 0);
    frame_size = (size_t)lv_display_get_horizontal_resolution(lv_display_get_default()) *
                 lv_display_get_vertical_resolution(lv_display_get_default()) * 4;
    reference = lv_malloc(frame_size);
    frame = lv_malloc(frame_size);
    TEST_ASSERT_NOT_NULL(reference);
    TEST_ASSERT_NOT_NULL(frame);
}

void tearDown(void)
{
    LV_GLOBAL_DEFAULT()->draw_info.unit_head = saved_units;
    LV_GLOBAL_DEFAULT()->draw_info.unit_cnt = saved_unit_count;
    lv_free(created_unit);
    if(sw_unit) sw_unit->evaluate_cb = sw_evaluate;
    lv_draw_invalidate_layer_support();
    if(other_display) lv_display_delete(other_display);
    lv_test_mouse_release();
    lv_test_wait(40);
    lv_obj_clean(lv_screen_active());
    lv_refr_now(NULL);
    lv_free(reference);
    lv_free(frame);
}

static lv_obj_t * panel_create(void)
{
    lv_obj_t * panel = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(panel);
    lv_obj_set_pos(panel, 100, 100);
    lv_obj_set_size(panel, 100, 60);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0xe06020), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    return panel;
}

/* Render and copy the test display's frame (DIRECT mode, XRGB8888) row by row */
static void capture(uint8_t * pixels)
{
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);
    lv_draw_buf_t * buf = lv_display_get_buf_active(lv_display_get_default());
    TEST_ASSERT_NOT_NULL(buf);
    uint32_t w = lv_display_get_horizontal_resolution(lv_display_get_default());
    uint32_t h = lv_display_get_vertical_resolution(lv_display_get_default());
    for(uint32_t y = 0; y < h; y++) {
        lv_memcpy(pixels + (size_t)y * w * 4, buf->data + (size_t)y * buf->header.stride, w * 4);
    }
}

/* With layers the effect changes the frame and counts no fallback; without layers the frame
 * equals the reference taken before the effect was requested and a fallback is counted */
static void assert_optional_effect(uint32_t before)
{
    capture(frame);
    if(lv_draw_layer_supported(NULL)) {
        TEST_ASSERT_EQUAL_UINT32(before, lv_draw_get_layer_fallback_count(NULL));
        TEST_ASSERT_NOT_EQUAL(0, lv_memcmp(reference, frame, frame_size));
    }
    else {
        TEST_ASSERT_GREATER_THAN_UINT32(before, lv_draw_get_layer_fallback_count(NULL));
        TEST_ASSERT_EQUAL_MEMORY(reference, frame, frame_size);
    }
}

void test_layer_query_is_cached_per_display_without_rendering(void)
{
    lv_display_t * disp = lv_display_get_default();
    other_display = lv_display_create(80, 60);
    LV_GLOBAL_DEFAULT()->draw_info.unit_head = &mock_unit;
    lv_draw_invalidate_layer_support();
    accepted_display = disp;
    lv_layer_t * head = disp->layer_head;
    lv_layer_t * next = head->next;
    lv_draw_task_t * tasks = head->draw_task_head;
    uint32_t fallbacks = lv_draw_get_layer_fallback_count(disp);
    TEST_ASSERT_NULL(lv_refr_get_disp_refreshing());
    TEST_ASSERT_TRUE(lv_draw_layer_supported(disp));
    TEST_ASSERT_TRUE(lv_draw_layer_supported(disp));
    TEST_ASSERT_EQUAL_UINT(2, probe_calls);  /* fill and blend */
    TEST_ASSERT_FALSE(lv_draw_layer_supported(other_display));
    TEST_ASSERT_FALSE(lv_draw_layer_supported(other_display));
    TEST_ASSERT_EQUAL_UINT(3, probe_calls);  /* the refused fill alone */
    accepted_display = other_display;
    TEST_ASSERT_TRUE(lv_draw_layer_supported(disp)); /* Cached until invalidated */
    lv_draw_invalidate_layer_support();
    TEST_ASSERT_FALSE(lv_draw_layer_supported(disp));
    TEST_ASSERT_TRUE(lv_draw_layer_supported(other_display));
    TEST_ASSERT_EQUAL_UINT(6, probe_calls);
    TEST_ASSERT_NULL(lv_refr_get_disp_refreshing());
    TEST_ASSERT_EQUAL_PTR(head, disp->layer_head);
    TEST_ASSERT_EQUAL_PTR(next, head->next);
    TEST_ASSERT_EQUAL_PTR(tasks, head->draw_task_head);
    TEST_ASSERT_EQUAL_UINT32(fallbacks, lv_draw_get_layer_fallback_count(disp));
}

void test_layer_query_follows_the_draw_units(void)
{
    lv_draw_invalidate_layer_support();
    TEST_ASSERT_TRUE(lv_draw_layer_supported(NULL));
    refuse_layers(true);
    TEST_ASSERT_FALSE(lv_draw_layer_supported(NULL));
    refuse_layers(false);
    TEST_ASSERT_TRUE(lv_draw_layer_supported(NULL));
}

void test_layer_query_rechecks_when_a_draw_unit_is_added(void)
{
    LV_GLOBAL_DEFAULT()->draw_info.unit_head = NULL;
    lv_draw_invalidate_layer_support();
    TEST_ASSERT_FALSE(lv_draw_layer_supported(NULL));
    created_unit = lv_draw_create_unit(sizeof(*created_unit));
    TEST_ASSERT_NOT_NULL(created_unit);
    created_unit->evaluate_cb = probe_evaluate;
    accepted_display = lv_display_get_default();
    TEST_ASSERT_TRUE(lv_draw_layer_supported(NULL));
    TEST_ASSERT_EQUAL_UINT(2, probe_calls);
}

void test_layer_fallback_preserves_styles_and_effective_geometry(void)
{
    lv_obj_t * panel = panel_create();
    lv_obj_set_style_transform_rotation(panel, 900, 0);
    lv_obj_set_style_transform_scale_x(panel, 128, 0);
    lv_obj_update_layout(panel);
    refuse_layers(true);
    lv_point_t p = {180, 110};
    lv_point_t original = p;
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_NONE, lv_obj_get_layer_type(panel));
    lv_obj_transform_point(panel, &p, LV_OBJ_POINT_TRANSFORM_FLAG_NONE);
    TEST_ASSERT_EQUAL_MEMORY(&original, &p, sizeof(p));
    TEST_ASSERT_EQUAL(900, lv_obj_get_style_transform_rotation(panel, 0));
    TEST_ASSERT_EQUAL(128, lv_obj_get_style_transform_scale_x(panel, 0));
    TEST_ASSERT_TRUE(lv_obj_hit_test(panel, &p));
    refuse_layers(false);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(panel));
    lv_obj_transform_point(panel, &p, LV_OBJ_POINT_TRANSFORM_FLAG_NONE);
    TEST_ASSERT_NOT_EQUAL(0, lv_memcmp(&original, &p, sizeof(p)));
}

/* Each effect is checked with layers available and with layers refused */
typedef void (*effect_cb_t)(lv_obj_t * panel);

static void check_effect(effect_cb_t prepare, effect_cb_t apply)
{
    for(unsigned refused = 0; refused < 2; refused++) {
        refuse_layers(refused);
        lv_obj_t * panel = panel_create();
        if(prepare) prepare(panel);
        capture(reference);
        uint32_t before = lv_draw_get_layer_fallback_count(NULL);
        apply(panel);
        assert_optional_effect(before);
        lv_obj_delete(panel);
    }
}

static void apply_clip_corner(lv_obj_t * panel)
{
    lv_obj_set_style_clip_corner(panel, true, 0);
}

static void prepare_clip_corner(lv_obj_t * panel)
{
    lv_obj_set_style_radius(panel, 24, 0);
    lv_obj_t * child = lv_obj_create(panel);
    lv_obj_remove_style_all(child);
    lv_obj_set_pos(child, -20, -20);
    lv_obj_set_size(child, 140, 100);
    lv_obj_set_style_bg_color(child, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_bg_opa(child, LV_OPA_COVER, 0);
}

void test_corner_clipping_falls_back_to_rectangular_clipping(void)
{
    check_effect(prepare_clip_corner, apply_clip_corner);
    /* Without layers the corner stays green, and the child is still clipped to the panel's rectangle */
    uint32_t width = lv_display_get_horizontal_resolution(lv_display_get_default());
    TEST_ASSERT_EQUAL_UINT8(0, frame[(101 * width + 101) * 4]);
    TEST_ASSERT_EQUAL_UINT8(255, frame[(101 * width + 101) * 4 + 1]);
    TEST_ASSERT_EQUAL_UINT8(255, frame[(90 * width + 90) * 4]);
}

/* Rotation and scale only: the software unit does not blend skewed layers */
static void apply_transform(lv_obj_t * panel)
{
    lv_obj_set_style_transform_rotation(panel, 300, 0);
    lv_obj_set_style_transform_scale_y(panel, 180, 0);
}

void test_widget_transform_falls_back_to_untransformed_content(void)
{
    check_effect(NULL, apply_transform);
}

static void prepare_opa(lv_obj_t * panel)
{
    lv_obj_set_style_opa(panel, 180, 0);
}

static void apply_opa_layered(lv_obj_t * panel)
{
    lv_obj_set_style_opa_layered(panel, 128, 0);
}

void test_layered_opacity_falls_back_without_losing_ordinary_opacity(void)
{
    check_effect(prepare_opa, apply_opa_layered);
}

static void apply_bitmap_mask(lv_obj_t * panel)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_a8);
    lv_obj_set_style_bitmap_mask_src(panel, &test_image_cogwheel_a8, 0);
}

void test_bitmap_mask_falls_back_to_unmasked_content(void)
{
    check_effect(NULL, apply_bitmap_mask);
}

static void apply_blend_mode(lv_obj_t * panel)
{
    lv_obj_set_style_blend_mode(panel, LV_BLEND_MODE_ADDITIVE, 0);
}

void test_blend_mode_falls_back_to_normal_content(void)
{
    check_effect(NULL, apply_blend_mode);
}

static void prepare_box_shadow(lv_obj_t * panel)
{
    lv_obj_set_style_shadow_width(panel, 8, 0);
    lv_obj_set_style_shadow_opa(panel, 128, 0);
    lv_obj_t * label = lv_label_create(panel);
    lv_label_set_text(label, "Shadow");
}

static void apply_drop_shadow(lv_obj_t * panel)
{
    lv_obj_t * label = lv_obj_get_child(panel, 0);
    lv_obj_set_style_drop_shadow_radius(panel, 4, 0);
    lv_obj_set_style_drop_shadow_offset_x(panel, 10, 0);
    lv_obj_set_style_drop_shadow_opa(panel, 200, 0);
    lv_obj_set_style_drop_shadow_radius(label, 3, 0);
    lv_obj_set_style_drop_shadow_offset_y(label, 8, 0);
    lv_obj_set_style_drop_shadow_opa(label, 200, 0);
}

void test_drop_shadow_falls_back_without_removing_native_box_shadow(void)
{
    check_effect(prepare_box_shadow, apply_drop_shadow);
}

void test_scale_label_rotation_falls_back_to_readable_labels(void)
{
    for(unsigned refused = 0; refused < 2; refused++) {
        refuse_layers(refused);
        lv_obj_t * scale = lv_scale_create(lv_screen_active());
        lv_obj_set_pos(scale, 100, 100);
        lv_obj_set_size(scale, 300, 100);
        lv_scale_set_mode(scale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
        lv_scale_set_total_tick_count(scale, 5);
        lv_scale_set_major_tick_every(scale, 1);
        capture(reference);
        uint32_t before = lv_draw_get_layer_fallback_count(NULL);
        lv_obj_set_style_transform_rotation(scale, 450, LV_PART_INDICATOR);
        assert_optional_effect(before);
        lv_obj_delete(scale);
    }
}

void test_bar_mask_falls_back_to_visible_indicator(void)
{
    uint32_t width = lv_display_get_horizontal_resolution(lv_display_get_default());
    for(unsigned refused = 0; refused < 2; refused++) {
        refuse_layers(refused);
        lv_obj_t * bar = lv_bar_create(lv_screen_active());
        lv_obj_remove_style_all(bar);
        lv_obj_set_pos(bar, 100, 100);
        lv_obj_set_size(bar, 200, 60);
        lv_obj_set_style_bg_color(bar, lv_color_hex(0xff0000), 0);
        lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(bar, lv_color_hex(0x00ff00), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_color(bar, lv_color_hex(0x00ff00), LV_PART_INDICATOR);
        lv_bar_set_value(bar, 50, LV_ANIM_OFF);
        capture(reference);
        uint32_t before = lv_draw_get_layer_fallback_count(NULL);
        /* A uniform gradient still takes the bar's layer-mask path */
        lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
        capture(frame);
        /* Visible green content either way, independent of gradient quantization */
        TEST_ASSERT_LESS_THAN_UINT8(16, frame[(130 * width + 150) * 4]);
        TEST_ASSERT_GREATER_THAN_UINT8(240, frame[(130 * width + 150) * 4 + 1]);
        TEST_ASSERT_LESS_THAN_UINT8(16, frame[(130 * width + 150) * 4 + 2]);
        if(lv_draw_layer_supported(NULL)) {
            TEST_ASSERT_EQUAL_UINT32(before, lv_draw_get_layer_fallback_count(NULL));
        }
        else {
            TEST_ASSERT_GREATER_THAN_UINT32(before, lv_draw_get_layer_fallback_count(NULL));
        }
        before = lv_draw_get_layer_fallback_count(NULL);
        lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_NONE, LV_PART_INDICATOR);
        lv_obj_set_style_radius(bar, 30, 0);
        capture(frame);
        TEST_ASSERT_EQUAL_UINT8(0, frame[(130 * width + 150) * 4]);
        TEST_ASSERT_EQUAL_UINT8(255, frame[(130 * width + 150) * 4 + 1]);
        if(!lv_draw_layer_supported(NULL)) {
            TEST_ASSERT_GREATER_THAN_UINT32(before, lv_draw_get_layer_fallback_count(NULL));
        }
        lv_obj_delete(bar);
    }
}

static void drag_up(lv_obj_t * obj)
{
    lv_obj_update_layout(obj);
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);
    lv_point_t start = {coords.x1 + lv_area_get_width(&coords) / 2,
                        coords.y1 + lv_area_get_height(&coords) * 3 / 4
                       };
    lv_test_mouse_release();
    lv_indev_t * mouse = lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER);
    lv_indev_read(mouse);
    for(int i = 0; i < 3; i++) {
        lv_point_t p = {start.x, start.y - i * 16};
        lv_obj_transform_point(obj, &p, LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE);
        lv_test_mouse_move_to(p.x, p.y);
        lv_test_mouse_press();
        lv_tick_inc(40);
        lv_indev_read(mouse);
        lv_obj_update_layout(obj);
    }
}

/* Dragging an untransformed and a rotated scrollable panel scrolls them the same amount,
 * whether the rotation is drawn (and the drag follows it) or omitted (and the drag is plain) */
static void check_scroll_gestures(void)
{
    int32_t dragged[2];
    lv_indev_t * mouse = lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER);
    for(unsigned transformed = 0; transformed < 2; transformed++) {
        lv_obj_t * panel = panel_create();
        lv_obj_set_size(panel, 120, 120);
        lv_obj_set_scroll_dir(panel, LV_DIR_VER);
        lv_obj_t * content = lv_obj_create(panel);
        lv_obj_set_size(content, 80, 1000);
        if(transformed) {
            lv_obj_set_style_transform_pivot_x(panel, 60, 0);
            lv_obj_set_style_transform_pivot_y(panel, 60, 0);
            lv_obj_set_style_transform_rotation(panel, 900, 0);
        }
        drag_up(panel);
        dragged[transformed] = lv_obj_get_scroll_y(panel);
        lv_test_mouse_release();
        lv_indev_read(mouse);
        /* The throw velocity uses the same vertical axis as the content */
        TEST_ASSERT_EQUAL_INT32(0, mouse->pointer.scroll_throw_vect.x);
        TEST_ASSERT_LESS_THAN(0, mouse->pointer.scroll_throw_vect.y);
        lv_obj_delete(panel);
    }
    TEST_ASSERT_GREATER_THAN(0, dragged[0]);
    TEST_ASSERT_EQUAL_INT32(dragged[0], dragged[1]);
}

static void check_roller_drag(void)
{
    int32_t dragged[2];
    lv_indev_t * mouse = lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER);
    for(unsigned transformed = 0; transformed < 2; transformed++) {
        lv_obj_t * roller = lv_roller_create(lv_screen_active());
        lv_obj_set_pos(roller, 100, 100);
        lv_obj_set_size(roller, 120, 120);
        lv_roller_set_options(roller, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15", LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(roller, 4, LV_ANIM_OFF);
        if(transformed) {
            lv_obj_set_style_transform_pivot_x(roller, 60, 0);
            lv_obj_set_style_transform_pivot_y(roller, 60, 0);
            lv_obj_set_style_transform_rotation(roller, 900, 0);
        }
        lv_obj_update_layout(roller);
        lv_obj_t * label = lv_obj_get_child(roller, 0);
        int32_t before = lv_obj_get_y(label);
        drag_up(roller);
        dragged[transformed] = before - lv_obj_get_y(label);
        lv_test_mouse_release();
        lv_indev_read(mouse);
        TEST_ASSERT_GREATER_THAN_UINT32(4, lv_roller_get_selected(roller));
        lv_obj_delete(roller);
    }
    TEST_ASSERT_EQUAL_INT32(32, dragged[0]);
    TEST_ASSERT_EQUAL_INT32(dragged[0], dragged[1]);
}

void test_scroll_gestures_follow_the_effective_transform(void)
{
    refuse_layers(false);
    check_scroll_gestures();
    refuse_layers(true);
    check_scroll_gestures();
}

void test_roller_drag_follows_the_effective_transform(void)
{
    refuse_layers(false);
    check_roller_drag();
    refuse_layers(true);
    check_roller_drag();
}

#endif
