#include "lv_example_gltf.h"

#if LV_USE_GLTF && LV_BUILD_EXAMPLES

/**
 * Timer callback to continuously rotate the model
 */
static void spin_timer_cb(lv_timer_t * timer)
{
    lv_obj_t * gltf = lv_timer_get_user_data(timer);

    float yaw = lv_gltf_get_yaw(gltf);
    yaw += 1.0f;
    if(yaw >= 360.0f) {
        yaw = 0.0f;
    }
    lv_gltf_set_yaw(gltf, yaw);
}

/**
 * Open a GLTF from a file and make it spin forever like a platter
 */
void lv_example_gltf_1(void)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf,
                                                           "A:lvgl/examples/libs/gltf/webp_diffuse_transmission_plant.glb");
    lv_obj_set_size(gltf, LV_PCT(100), LV_PCT(100));

    lv_gltf_model_play_animation(model, 0);
    lv_gltf_set_pitch(gltf, -45.f);
    lv_timer_create(spin_timer_cb, LV_DEF_REFR_PERIOD, gltf);
}

#endif
