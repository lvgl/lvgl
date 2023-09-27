/**
 * @file lv_demos.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lv_demos.h"

/*********************
 *      DEFINES
 *********************/
#if LV_USE_DEMO_HAL_CUSTOM
    #define LV_DEMO_HAL_INIT() lv_demos_hal_init_custom()
    #define LV_DEMO_HAL_DEINIT() lv_demos_hal_deinit_custom()
    #define LV_DEMO_RUN() lv_demos_run_custom()
#else
    #define LV_DEMO_HAL_INIT() lv_demos_hal_init()
    #define LV_DEMO_HAL_DEINIT() lv_demos_hal_deinit()
    #define LV_DEMO_RUN() lv_demos_run()
#endif

/**********************
 *      TYPEDEFS
 **********************/

enum {
    DEMO_TYPE_VOID,
    DEMO_TYPE_BENCHMARK,
    DEMO_TYPE_BENCHMARK_SCENE,
};

typedef void (*demo_method_cb)(void);
#if LV_USE_DEMO_BENCHMARK
    typedef void (*demo_method_benchmark_cb)(lv_demo_benchmark_mode_t);
    typedef void (*demo_method_benchmark_scene_cb)(lv_demo_benchmark_mode_t, uint16_t);
#endif

typedef struct  {
    const char * name;
    union {
        demo_method_cb entry_cb;
#if LV_USE_DEMO_BENCHMARK
        demo_method_benchmark_cb entry_benchmark_cb;
        demo_method_benchmark_scene_cb entry_benchmark_scene_cb;
#endif
    };
    int type : 8;
    int arg_count : 8;
} demo_entry_info_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static const demo_entry_info_t demos_entry_info[] = {
#if LV_USE_DEMO_WIDGETS
    { "widgets", .entry_cb = lv_demo_widgets, DEMO_TYPE_VOID },
#endif

#if LV_USE_DEMO_MUSIC
    { "music", .entry_cb = lv_demo_music, DEMO_TYPE_VOID },
#endif
#if LV_USE_DEMO_MULTILANG
    { "multilang", .entry_cb = lv_demo_multilang, DEMO_TYPE_VOID },
#endif

#if LV_USE_DEMO_STRESS
    { "stress", .entry_cb = lv_demo_stress, DEMO_TYPE_VOID },
#endif

#if LV_USE_DEMO_KEYPAD_AND_ENCODER
    { "keypad_encoder", .entry_cb = lv_demo_keypad_encoder, DEMO_TYPE_VOID },
#endif

#if LV_USE_DEMO_FLEX_LAYOUT
    { "flex_layout", .entry_cb = lv_demo_flex_layout, DEMO_TYPE_VOID },
#endif

#if LV_USE_DEMO_TRANSFORM
    { "transform", .entry_cb = lv_demo_transform, DEMO_TYPE_VOID },
#endif

#if LV_USE_DEMO_SCROLL
    { "scroll", .entry_cb = lv_demo_scroll, DEMO_TYPE_VOID },
#endif

#if LV_USE_DEMO_BENCHMARK
    { "benchmark", .entry_benchmark_cb = lv_demo_benchmark, DEMO_TYPE_BENCHMARK, 1 },
    { "benchmark_scene", .entry_benchmark_scene_cb = lv_demo_benchmark_run_scene, DEMO_TYPE_BENCHMARK_SCENE, 2 },
#endif

    { "", .entry_cb = NULL, DEMO_TYPE_VOID }
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_demos_create_demo(char * info[], int size)
{
    const demo_entry_info_t * entry_info = NULL;
    if(size <= 0) { /* default: first demo*/
        entry_info = &demos_entry_info[0];
    }

    if(entry_info == NULL && info) {
        const char * name = info[0];
        const int len = sizeof(demos_entry_info) / sizeof(demo_entry_info_t) - 1;

        for(int i = 0; i < len; i++) {
            if(strcmp(name, demos_entry_info[i].name) == 0 && size - 1 >= demos_entry_info[i].arg_count) {
                entry_info = &demos_entry_info[i];
            }
        }
    }

    if(entry_info == NULL) {
        LV_LOG_ERROR("lv_demos create_demo(%s) failure!", size > 0 ? info[0] : "");
        return false;
    }

    if(entry_info->type == DEMO_TYPE_VOID) {
        if(!entry_info->entry_cb)
            return false;
        entry_info->entry_cb();
    }
#if LV_USE_DEMO_BENCHMARK
    else if(entry_info->type == DEMO_TYPE_BENCHMARK)
        entry_info->entry_benchmark_cb((lv_demo_benchmark_mode_t)atoi(info[1]));
    else if(entry_info->type == DEMO_TYPE_BENCHMARK_SCENE)
        entry_info->entry_benchmark_scene_cb((lv_demo_benchmark_mode_t)atoi(info[1]), (uint16_t)atoi(info[2]));
#endif
    return true;
}

#if LV_USE_DEMO_HAL_CUSTOM == 0

#if LV_USE_SDL
static lv_disp_t * lv_demos_hal_init_with_sdl(lv_coord_t w, lv_coord_t h)
{
    lv_disp_t * disp = lv_sdl_window_create(w, h);
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_disp(mouse, disp);
    lv_disp_set_default(disp);

    LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
    lv_obj_t * cursor_obj;
    cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
    lv_indev_set_cursor(mouse, cursor_obj);             /*Connect the image  object to the driver*/

    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_disp(mousewheel, disp);

    lv_indev_t * keyboard = lv_sdl_keyboard_create();
    lv_indev_set_disp(keyboard, disp);
    return disp;
}
#endif

static lv_disp_t * lv_demos_hal_init(void)
{
#if LV_USE_SDL
    return lv_demos_hal_init_with_sdl(800, 480);
#else
    return NULL;
#endif
}

static void lv_demos_hal_deinit(void)
{
}

static void lv_demos_run(void)
{
    while(1) {
        lv_timer_handler();
        usleep(1 * 1000);
    }
}
#endif

static void show_usage(void)
{
    int i;
    const int len = sizeof(demos_entry_info)
                    / sizeof(demo_entry_info_t) - 1;

    if(len == 0) {
        printf("lv_demos: no demo available!\n");
        return;
    }

    printf("\nUsage: lv_demos demo [parameters]\n");
    printf("\ndemo list:\n");

    for(i = 0; i < len; i++) {
        if(demos_entry_info[i].type == DEMO_TYPE_VOID)
            printf("  %s \n", demos_entry_info[i].name);
        else if(demos_entry_info[i].type == DEMO_TYPE_BENCHMARK)
            printf("  %s [0, 1, 2]\n", demos_entry_info[i].name);
        else if(demos_entry_info[i].type == DEMO_TYPE_BENCHMARK_SCENE)
            printf("  %s [0, 1, 2] scene_no\n", demos_entry_info[i].name);

    }
}


int lv_demos_main_entry(char * info[], int size)
{
    lv_init();
    lv_disp_t * disp = LV_DEMO_HAL_INIT();
    if(disp == NULL) {
        LV_LOG_ERROR("lv_demos hal_init failure!");
        return 1;
    }

    if(!lv_demos_create_demo(info, size)) {
        show_usage();
        goto demo_end;
    }

    LV_DEMO_RUN();

demo_end:
    LV_DEMO_HAL_DEINIT();
    lv_deinit();
    return 0;
}

#if LV_USE_DEMO_MAIN
int main(int argc, char ** argv)
{
    return lv_demos_main_entry(&argv[1], argc - 1);
}
#endif

