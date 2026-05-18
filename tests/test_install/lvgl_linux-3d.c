#include <lvgl/lvgl.h>

int main(void)
{
    lv_init();
    lv_display_t * disp1 = lv_sdl_window_create(1024, 768);
    lv_display_t * disp2 = lv_wayland_window_create(1024, 768, "test", NULL);
    lv_display_t * disp3 = lv_linux_drm_create();
    const char * device = lv_linux_drm_find_device_path();
    lv_linux_drm_set_file(disp3, device, -1);
    lv_free(device);

    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_obj_center(gltf);

    while(1) {
        uint32_t ms = lv_timer_handler();
        usleep(ms * 1000);
    }
}
