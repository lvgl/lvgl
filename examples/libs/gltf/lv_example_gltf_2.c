#include "lv_example_gltf.h"

#if LV_USE_GLTF && LV_BUILD_EXAMPLES

static size_t camera_count;
static size_t current_camera;
static size_t current_speed = LV_GLTF_ANIM_SPEED_HALF;

/**
 * Timer callback that updates the current camera and the current animation speed
 */
static void timer_cb(lv_timer_t * timer)
{
    lv_obj_t * gltf = (lv_obj_t *) lv_timer_get_user_data(timer);
    current_camera = (current_camera + 1) % (camera_count + 1);
    current_speed *= 2;

    if(current_speed > LV_GLTF_ANIM_SPEED_4X) {
        current_speed = LV_GLTF_ANIM_SPEED_HALF;
    }

    lv_gltf_set_camera(gltf, current_camera);
    lv_gltf_set_animation_speed(gltf, current_speed);
}

/**
 * Open a GLTF from a file and loop through the model cameras and multiple animation speeds
 */
void lv_example_gltf_2(void)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf,
                                                           "A:lvgl/examples/libs/gltf/webp_diffuse_transmission_plant.glb");


    lv_gltf_set_animation_speed(gltf, current_speed);
    lv_gltf_model_play_animation(model, 0);
    lv_obj_set_size(gltf, LV_PCT(100), LV_PCT(100));
    camera_count = lv_gltf_get_camera_count(gltf);
    lv_gltf_set_pitch(gltf, -45.f);
    lv_timer_create(timer_cb, 5000, gltf);
}

#endif
