#include "lv_example_gltf.h"

#if LV_BUILD_EXAMPLES

#if LV_USE_GLTF

static lv_gltf_model_node_t * logo_root;

static void anim_scale_cb(void * var, int32_t v)
{
    lv_gltf_model_node_t * node = (lv_gltf_model_node_t *)var;

    /* Convert integer animation value to float scale (v ranges from 10 to 100, representing 0.1 to 1.0) */
    float scale = v / 100.0f;

    /* Apply uniform scale to all axes */
    lv_gltf_model_node_set_scale_x(node, scale);
    lv_gltf_model_node_set_scale_y(node, scale);
    lv_gltf_model_node_set_scale_z(node, scale);
}

static void logo_scale_cb(lv_event_t * e)
{
    lv_3dpoint_t scale;

    /* Read current scale values */
    lv_gltf_model_node_get_scale(e, &scale);

    LV_LOG_USER("logo scale: %.2f, %.2f, %.2f",
                scale.x, scale.y, scale.z);
}

/**
 * Load a logo model once and share it across four glTF viewers in a 2x2 grid.
 * Each viewer shows the logo from a different angle.
 * Animate the shared model's growth - changes affect all four viewers.
 */
void lv_example_gltf_4(void)
{
    /* Load the logo model once */
    lv_gltf_model_t * logo_model = lv_gltf_data_load_from_file(
                                       "A:lvgl/examples/libs/gltf/lvgl_logo.glb", NULL);

    /* Create first glTF viewer - Top Left */
    lv_obj_t * gltf1 = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf1, LV_PCT(50), LV_PCT(50));
    lv_obj_align(gltf1, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_gltf_set_pitch(gltf1, -30.f);
    lv_gltf_set_yaw(gltf1, -30.f);
    lv_gltf_set_distance(gltf1, 0.5f);
    lv_gltf_add_model(gltf1, logo_model);

    /* Create second glTF viewer - Top Right */
    lv_obj_t * gltf2 = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf2, LV_PCT(50), LV_PCT(50));
    lv_obj_align(gltf2, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_gltf_set_pitch(gltf2, -30.f);
    lv_gltf_set_yaw(gltf2, 30.f);
    lv_gltf_set_distance(gltf2, 0.5f);
    lv_gltf_add_model(gltf2, logo_model);

    /* Create third glTF viewer - Bottom Left */
    lv_obj_t * gltf3 = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf3, LV_PCT(50), LV_PCT(50));
    lv_obj_align(gltf3, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_gltf_set_pitch(gltf3, 30.f);
    lv_gltf_set_yaw(gltf3, -30.f);
    lv_gltf_set_distance(gltf3, 0.5f);
    lv_gltf_add_model(gltf3, logo_model);

    /* Create fourth glTF viewer - Bottom Right */
    lv_obj_t * gltf4 = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf4, LV_PCT(50), LV_PCT(50));
    lv_obj_align(gltf4, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_gltf_set_pitch(gltf4, 30.f);
    lv_gltf_set_yaw(gltf4, 30.f);
    lv_gltf_set_distance(gltf4, 0.5f);
    lv_gltf_add_model(gltf4, logo_model);

    /* Get the root node of the shared logo */
    logo_root = lv_gltf_model_node_get_by_numeric_path(logo_model, ".0");

    /* Register event callback to monitor scale changes */
    lv_gltf_model_node_add_event_cb(logo_root, logo_scale_cb,
                                    LV_EVENT_VALUE_CHANGED, NULL);

    /* Create animation for logo growth */
    /* Since the model is shared, animating the node affects all four viewers */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, logo_root);
    lv_anim_set_values(&a, 10, 100);  /* Scale from 0.1 to 1.0 */
    lv_anim_set_duration(&a, 2000);   /* 2 seconds to grow */
    lv_anim_set_reverse_delay(&a, 1000);  /* Stay big for 1 second */
    lv_anim_set_reverse_duration(&a, 1500);  /* 1.5 seconds to shrink */
    lv_anim_set_repeat_delay(&a, 500);  /* Wait 0.5 seconds before repeating */
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&a, anim_scale_cb);
    lv_anim_start(&a);
}

#else

void lv_example_gltf_4(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "glTF web support is coming soon");
    lv_obj_center(label);
}

#endif
#endif
