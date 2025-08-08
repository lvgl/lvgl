
#if LV_USE_DEMO_GLTF
#include "lv_demo_gltf.h"
#include "../../lvgl_private.h"

#define PI_TO_RAD   0.01745329238f
#define SLIDER_COLOR    lv_color_hex(0x26A69A)
#define PLAY_BTN_COLOR  lv_color_hex(0x00C8535)
#define PAUSE_BTN_COLOR lv_color_hex(0xF9A825)

typedef struct {
    lv_point_t last_pos;
    bool is_dragging;
    float sensitivity;
} mouse_state_t;

static lv_obj_t * viewer;

static lv_subject_t yaw_subject;
static lv_subject_t pitch_subject;
static lv_subject_t distance_subject;

static lv_subject_t camera_subject;
static lv_subject_t animation_subject;

static lv_subject_t antialiasing_subject;
static lv_subject_t background_subject;

static lv_subject_t animation_speed_subject;

typedef void (*lv_gltf_set_float_fn)(lv_obj_t *, float);
typedef void (*lv_gltf_set_int_fn)(lv_obj_t *, uint32_t);

typedef union {
    void * ptr;
    lv_gltf_set_float_fn fn;
} lv_gltf_set_float_fn_union_t;

typedef union {
    void * ptr;
    lv_gltf_set_int_fn fn;
} lv_gltf_set_int_fn_union_t;

lv_gltf_set_float_fn_union_t pitch_fn = { .fn = lv_gltf_set_pitch };
lv_gltf_set_float_fn_union_t yaw_fn = { .fn = lv_gltf_set_yaw };
lv_gltf_set_float_fn_union_t distance_fn = { .fn = lv_gltf_set_distance };

lv_gltf_set_int_fn_union_t camera_fn = { .fn = lv_gltf_set_camera };
lv_gltf_set_int_fn_union_t animation_speed_fn = { .fn = lv_gltf_set_animation_speed };

lv_gltf_set_int_fn_union_t background_mode_fn = { .fn = lv_gltf_set_background_mode };
lv_gltf_set_int_fn_union_t antialiasing_mode_fn = { .fn = lv_gltf_set_antialiasing_mode };

static lv_gltf_model_t * model;

static void on_mouse_event(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * viewer = lv_event_get_target_obj(e);
    lv_indev_t * indev = lv_indev_active();
    mouse_state_t * mouse_state = (mouse_state_t *)lv_event_get_user_data(e);

    if(!mouse_state) {
        return;
    }

    lv_point_t current_pos;
    lv_indev_get_point(indev, &current_pos);

    switch(event_code) {
        case LV_EVENT_PRESSED:
            LV_LOG_USER("Pressed");
            // Start dragging - store initial position
            mouse_state->is_dragging = true;
            mouse_state->last_pos = current_pos;
            break;
        case LV_EVENT_PRESSING:
            if(mouse_state->is_dragging && lv_gltf_get_camera(viewer) == 0) {
                int32_t delta_x = current_pos.x - mouse_state->last_pos.x;
                int32_t delta_y = current_pos.y - mouse_state->last_pos.y;

                float current_yaw = lv_gltf_get_yaw(viewer);
                float current_pitch = lv_gltf_get_pitch(viewer);

                float new_yaw = current_yaw + (delta_x * -mouse_state->sensitivity);
                float new_pitch = current_pitch + (delta_y * -mouse_state->sensitivity);

                if(new_pitch > 89.0f)
                    new_pitch = 89.0f;
                if(new_pitch < -89.0f)
                    new_pitch = -89.0f;

                lv_subject_set_float(&yaw_subject, new_yaw);
                lv_subject_set_float(&pitch_subject, new_pitch);
            }
            mouse_state->last_pos = current_pos;
            break;

        case LV_EVENT_RELEASED:
        case LV_EVENT_PRESS_LOST:
            mouse_state->is_dragging = false;
            break;
        default:
            break;
    }
}

static void animation_play_pause_event_handler(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target_obj(e);
    lv_obj_t * btn_label = (lv_obj_t *)lv_event_get_user_data(e);

    if(lv_gltf_model_is_animation_paused(model)) {
        lv_gltf_model_play_animation(model, lv_gltf_model_get_animation(model));
        lv_obj_set_style_bg_color(btn, PAUSE_BTN_COLOR, LV_PART_MAIN);
        lv_label_set_text_static(btn_label, LV_SYMBOL_PAUSE);
    }
    else {
        lv_obj_set_style_bg_color(btn, PLAY_BTN_COLOR, LV_PART_MAIN);
        lv_label_set_text_static(btn_label, LV_SYMBOL_PLAY);
        lv_gltf_model_pause_animation(model);
    }
}

static void reset_subject_event_handler(lv_event_t * e)
{
    lv_subject_t * subject = (lv_subject_t *)lv_event_get_user_data(e);
    lv_subject_set_int(subject, 0);
}

static lv_obj_t * row_title(lv_obj_t * parent, const char * title)
{
    lv_obj_t * title_label = lv_label_create(parent);
    lv_label_set_text_static(title_label, title);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_obj_set_style_margin_bottom(title_label, 10, 0);
    return title_label;
}
static lv_obj_t * create_control_row(lv_obj_t * parent)
{
    lv_obj_t * row = lv_obj_create(parent);
    lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(row, 20, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(row, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_right(row, 20, LV_PART_MAIN);
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_COLUMN);
    return row;
}
static void populate_dropdown(lv_obj_t * dropdown, const char * prefix, size_t count, lv_subject_t * subject)
{
    char option[16];
    if(count == 0) {
        lv_snprintf(option, sizeof(option), "No %ss", prefix);
        lv_dropdown_set_options(dropdown, option);
        lv_obj_add_state(dropdown, LV_STATE_DISABLED);
        return;
    }

    for(size_t i = 0; i < count; i++) {
        lv_snprintf(option, sizeof(option), "%s %zu", prefix, i);
        lv_dropdown_add_option(dropdown, option, i);
    }

    lv_dropdown_bind_value(dropdown, subject);
}

static lv_obj_t * add_button_to_row(lv_obj_t * row, lv_color_t color)
{
    lv_obj_t * btn = lv_button_create(row);
    lv_obj_set_size(btn, LV_PCT(100), 30);
    lv_obj_set_style_bg_color(btn, color, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 4, 0);

    return btn;
}

static lv_obj_t * add_dropdown_to_row(lv_obj_t * row)
{
    lv_obj_t * dropdown = lv_dropdown_create(row);

    lv_obj_set_width(dropdown, LV_PCT(100));
    lv_obj_set_style_bg_color(dropdown, lv_color_hex(0x404040), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_color(dropdown, lv_color_hex(0x4A4A4A), LV_PART_MAIN);
    lv_obj_set_style_text_color(dropdown, lv_color_white(), LV_PART_MAIN);
    lv_obj_t * dropdown_list = ((lv_dropdown_t *)dropdown)->list;
    lv_obj_set_style_clip_corner(dropdown_list, false, LV_PART_MAIN);
    return dropdown;
}

static void on_viewer_delete(lv_event_t * e)
{
    void * mouse_state = lv_event_get_user_data(e);
    lv_free(mouse_state);
}

static void viewer_observer_float_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * viewer = lv_observer_get_target_obj(observer);
    float value = lv_subject_get_float(subject);
    lv_gltf_set_float_fn_union_t fn_union = { .ptr = lv_observer_get_user_data(observer) };

    fn_union.fn(viewer, value);
}

static void viewer_observer_int_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    int value = lv_subject_get_int(subject);
    lv_gltf_set_int_fn_union_t fn_union = { .ptr = lv_observer_get_user_data(observer) };

    lv_obj_t * viewer = lv_observer_get_target_obj(observer);
    fn_union.fn(viewer, value);
}

static void model_observer_int_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    int value = lv_subject_get_int(subject);

    lv_gltf_model_play_animation(model, value);
}

static void style_slider(lv_obj_t * slider, lv_color_t accent_color)
{
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x1A1A1A), LV_PART_MAIN);
    lv_obj_set_style_radius(slider, 20, LV_PART_MAIN);
    lv_obj_set_style_border_width(slider, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(slider, lv_color_hex(0x444444), LV_PART_MAIN);

    lv_obj_set_style_bg_color(slider, accent_color, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, 20, LV_PART_INDICATOR);

    lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_shadow_width(slider, 6, LV_PART_KNOB);
    lv_obj_set_style_shadow_color(slider, accent_color, LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(slider, 150, LV_PART_KNOB);
    lv_obj_set_style_border_width(slider, 2, LV_PART_KNOB);
    lv_obj_set_style_border_color(slider, accent_color, LV_PART_KNOB);

    lv_obj_set_style_bg_color(slider, lv_color_hex(0x0F0F0F), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_border_color(slider, lv_color_hex(0x222222), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_opa(slider, 128, LV_PART_MAIN | LV_STATE_DISABLED);

    lv_color_t dimmed_accent = lv_color_mix(accent_color, lv_color_black(), 128);
    lv_obj_set_style_bg_color(slider, dimmed_accent, LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_set_style_opa(slider, 102, LV_PART_INDICATOR | LV_STATE_DISABLED);

    lv_obj_set_style_bg_color(slider, lv_color_hex(0xCCCCCC), LV_PART_KNOB | LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(slider, 2, LV_PART_KNOB | LV_STATE_DISABLED);
    lv_obj_set_style_shadow_opa(slider, 51, LV_PART_KNOB | LV_STATE_DISABLED);
}

static void control_panel(lv_obj_t * parent, lv_gltf_model_t * model)
{
    lv_subject_init_int(&camera_subject, 1);
    lv_subject_init_int(&animation_speed_subject, LV_GLTF_ANIM_SPEED_NORMAL);
    lv_subject_init_int(&animation_subject, lv_gltf_model_get_animation(model));
    lv_subject_init_int(&antialiasing_subject, lv_gltf_get_antialiasing_mode(viewer));
    lv_subject_init_int(&background_subject, lv_gltf_get_background_mode(viewer));

    lv_subject_add_observer_obj(&camera_subject, viewer_observer_int_cb, viewer, camera_fn.ptr);
    lv_subject_add_observer_obj(&animation_speed_subject, viewer_observer_int_cb, viewer, animation_speed_fn.ptr);

    lv_subject_add_observer_obj(&background_subject, viewer_observer_int_cb, viewer, background_mode_fn.ptr);
    lv_subject_add_observer_obj(&antialiasing_subject, viewer_observer_int_cb, viewer, antialiasing_mode_fn.ptr);

    lv_subject_add_observer(&animation_subject, model_observer_int_cb, NULL);

    lv_subject_init_float(&yaw_subject, lv_gltf_get_yaw(viewer));
    lv_subject_init_float(&pitch_subject, lv_gltf_get_pitch(viewer));
    lv_subject_init_float(&distance_subject, lv_gltf_get_distance(viewer));

    lv_obj_t * control_panel = lv_obj_create(parent);
    lv_obj_set_size(control_panel, LV_PCT(100), lv_obj_get_height(parent));
    lv_obj_align_to(control_panel, parent, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(control_panel, lv_color_hex(0x2C2C2C), 0);
    lv_obj_set_style_border_width(control_panel, 1, 0);
    lv_obj_set_style_border_color(control_panel, lv_color_hex(0x555555), 0);
    lv_obj_set_style_radius(control_panel, 8, 0);
    lv_obj_set_style_pad_all(control_panel, 5, 0);
    lv_obj_set_layout(control_panel, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(control_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(control_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(control_panel, 128, LV_PART_MAIN);

    lv_obj_t * camera_row = create_control_row(control_panel);
    row_title(camera_row, "Cameras");

    lv_obj_t * camera_dropdown = add_dropdown_to_row(camera_row);
    populate_dropdown(camera_dropdown, "Camera", lv_gltf_model_get_camera_count(model), &camera_subject);
    lv_dropdown_add_option(camera_dropdown, "No Camera", 0);

    lv_obj_t * camera_reset_btn = add_button_to_row(camera_row, lv_color_hex(0xFF6B35));
    lv_obj_add_event_cb(camera_reset_btn, reset_subject_event_handler, LV_EVENT_CLICKED, &camera_subject);

    lv_obj_t * camera_reset_btn_label = lv_label_create(camera_reset_btn);
    lv_label_set_text_static(camera_reset_btn_label, LV_SYMBOL_REFRESH);
    lv_obj_set_style_text_color(camera_reset_btn_label, lv_color_white(), 0);
    lv_obj_center(camera_reset_btn_label);

    lv_obj_t * animation_row = create_control_row(control_panel);
    row_title(animation_row, "Animations");

    lv_obj_t * animation_dropdown = add_dropdown_to_row(animation_row);
    populate_dropdown(animation_dropdown, "Animation", lv_gltf_model_get_animation_count(model), &animation_subject);

    const bool has_animations = lv_gltf_model_get_animation_count(model) > 0;
    const lv_color_t btn_color = has_animations ? PAUSE_BTN_COLOR : PLAY_BTN_COLOR;

    lv_obj_t * animation_pp_btn = add_button_to_row(animation_row, btn_color);
    lv_obj_t * animation_pp_btn_label = lv_label_create(animation_pp_btn);
    lv_obj_center(animation_pp_btn_label);
    lv_obj_set_style_text_color(animation_pp_btn_label, lv_color_white(), 0);

    if(has_animations) {
        lv_label_set_text_static(animation_pp_btn_label, LV_SYMBOL_PAUSE);
    }
    else {
        lv_obj_add_state(animation_pp_btn, LV_STATE_DISABLED);
        lv_label_set_text_static(animation_pp_btn_label, LV_SYMBOL_PLAY);
    }

    lv_obj_add_event_cb(animation_pp_btn, animation_play_pause_event_handler, LV_EVENT_CLICKED, animation_pp_btn_label);

    lv_obj_t * animation_reset_btn = add_button_to_row(animation_row, lv_color_hex(0xFF6B35));

    lv_obj_t * animation_reset_btn_label = lv_label_create(animation_reset_btn);
    lv_label_set_text_static(animation_reset_btn_label, LV_SYMBOL_REFRESH);
    lv_obj_set_style_text_color(animation_reset_btn_label, lv_color_white(), 0);
    lv_obj_center(animation_reset_btn_label);

    lv_obj_add_event_cb(animation_reset_btn, reset_subject_event_handler, LV_EVENT_CLICKED, &animation_subject);

    lv_subject_add_observer_obj(&pitch_subject, viewer_observer_float_cb, viewer, pitch_fn.ptr);
    lv_subject_add_observer_obj(&yaw_subject, viewer_observer_float_cb, viewer, yaw_fn.ptr);
    lv_subject_add_observer_obj(&distance_subject, viewer_observer_float_cb, viewer, distance_fn.ptr);

    lv_obj_t * sliders_row = create_control_row(control_panel);
    lv_obj_t * yaw_title = row_title(sliders_row, "");
    lv_label_bind_text(yaw_title, &yaw_subject, "Yaw %.2f");

    lv_obj_t * yaw_slider = lv_slider_create(sliders_row);
    lv_slider_bind_value(yaw_slider, &yaw_subject);
    lv_obj_set_width(yaw_slider, LV_PCT(100));
    lv_slider_set_max_value(yaw_slider, 360);
    lv_slider_set_min_value(yaw_slider, -360);
    style_slider(yaw_slider, SLIDER_COLOR);

    lv_obj_t * pitch_title = row_title(sliders_row, "");
    lv_label_bind_text(pitch_title, &pitch_subject, "Pitch %.2f");

    lv_obj_t * pitch_slider = lv_slider_create(sliders_row);
    lv_slider_bind_value(pitch_slider, &pitch_subject);
    lv_obj_set_width(pitch_slider, LV_PCT(100));
    lv_slider_set_min_value(pitch_slider, -90);
    lv_slider_set_max_value(pitch_slider, 90);

    style_slider(pitch_slider, SLIDER_COLOR);

    lv_obj_t * distance_title = row_title(sliders_row, "");
    lv_label_bind_text(distance_title, &distance_subject, "Distance %.2f");

    lv_obj_t * distance_slider = lv_slider_create(sliders_row);
    lv_obj_set_width(distance_slider, LV_PCT(100));
    lv_slider_bind_value(distance_slider, &distance_subject);
    lv_slider_set_min_value(distance_slider, 1);
    lv_slider_set_max_value(distance_slider, 25);
    style_slider(distance_slider, SLIDER_COLOR);

    row_title(sliders_row, "Animation Speed");
    lv_obj_t * animation_ratio_value = row_title(sliders_row, "");
    lv_label_bind_text(animation_ratio_value, &animation_speed_subject, "%d (x1000)");

    lv_obj_t * animation_ratio_slider = lv_slider_create(sliders_row);
    lv_obj_set_width(animation_ratio_slider, LV_PCT(100));
    lv_slider_set_min_value(animation_ratio_slider, 100);
    lv_slider_set_max_value(animation_ratio_slider, LV_GLTF_ANIM_SPEED_4X);
    lv_slider_bind_value(animation_ratio_slider, &animation_speed_subject);

    style_slider(animation_ratio_slider, SLIDER_COLOR);

    lv_obj_bind_state_if_gt(yaw_slider, &camera_subject, LV_STATE_DISABLED, 0);
    lv_obj_bind_state_if_gt(pitch_slider, &camera_subject, LV_STATE_DISABLED, 0);
    lv_obj_bind_state_if_gt(distance_slider, &camera_subject, LV_STATE_DISABLED, 0);

    lv_obj_t * bg_row = create_control_row(control_panel);
    row_title(bg_row, "Background");
    lv_obj_t * background_dropdown = add_dropdown_to_row(bg_row);
    lv_dropdown_set_options(background_dropdown, "Solid Color\nEnvironnement");
    lv_dropdown_bind_value(background_dropdown, &background_subject);

    lv_obj_t * antialiasing_row = create_control_row(control_panel);
    row_title(antialiasing_row, "Anti Aliasing");
    lv_obj_t * antialiasing_dropdown = add_dropdown_to_row(antialiasing_row);
    lv_dropdown_set_options(antialiasing_dropdown, "Off\nOn\nDynamic");
    lv_dropdown_bind_value(antialiasing_dropdown, &antialiasing_subject);
}

void lv_demo_gltf(const char * path)
{
    viewer = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(viewer, LV_PCT(100), LV_PCT(100));
    lv_obj_remove_flag(viewer, LV_OBJ_FLAG_SCROLLABLE);
    lv_gltf_set_background_mode(viewer, LV_GLTF_BG_ENVIRONMENT);
    lv_obj_set_style_bg_color(viewer, lv_color_hex(0x78F5a5), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(viewer, 128, LV_PART_MAIN),

                            model = lv_gltf_load_model_from_file(viewer, path);
    if(lv_gltf_model_get_animation_count(model) > 0) {
        lv_gltf_model_play_animation(model, 0);
    }

    control_panel(viewer, model);

    mouse_state_t * mouse_state = lv_zalloc(sizeof(*mouse_state));
    LV_ASSERT_MALLOC(mouse_state);
    mouse_state->sensitivity = 0.3;

    lv_obj_add_event_cb(viewer, on_mouse_event, LV_EVENT_PRESSED, mouse_state);
    lv_obj_add_event_cb(viewer, on_mouse_event, LV_EVENT_PRESSING, mouse_state);
    lv_obj_add_event_cb(viewer, on_mouse_event, LV_EVENT_RELEASED, mouse_state);
    lv_obj_add_event_cb(viewer, on_mouse_event, LV_EVENT_PRESS_LOST, mouse_state);
    lv_obj_add_event_cb(viewer, on_viewer_delete, LV_EVENT_DELETE, mouse_state);
}
#endif /*LV_USE_DEMO_GLTF*/
