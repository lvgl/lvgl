#include <lvgl/lvgl.h>

int main(void)
{
    lv_init();
    lv_display_t * disp1 = lv_sdl_window_create(1024, 768);
    lv_display_t * disp2 = lv_wayland_window_create(1024, 768, (char *)"hello", NULL);
    lv_display_t * disp3 = lv_linux_drm_create();
    char * device = lv_linux_drm_find_device_path();
    lv_linux_drm_set_file(disp3, device, -1);
    lv_free(device);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text_static(label, "Hello World!");
    lv_obj_center(label);

    while(1) {
        uint32_t ms = lv_timer_handler();
        lv_sleep_ms(ms);
    }
}
