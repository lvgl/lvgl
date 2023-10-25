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
#define DEMO_BENCHMARK_NAME "benchmark"
#define DEMO_BENCHMARK_SCENE_NAME "benchmark_scene"
#define LV_DEMOS_COUNT (sizeof(demos_entry_info) / sizeof(demo_entry_info_t) - 1)

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*demo_method_cb)(void);
#if LV_USE_DEMO_BENCHMARK
    //    typedef void (*demo_method_benchmark_cb)(lv_demo_benchmark_mode_t);
    //    typedef void (*demo_method_benchmark_scene_cb)(lv_demo_benchmark_mode_t, uint16_t);
#endif

typedef struct  {
    const char * name;
    union {
        demo_method_cb entry_cb;
#if LV_USE_DEMO_BENCHMARK
        //        demo_method_benchmark_cb entry_benchmark_cb;
        //        demo_method_benchmark_scene_cb entry_benchmark_scene_cb;
#endif
    };
    int arg_count : 8;
} demo_entry_info_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static inline bool demo_is_benchmark(const demo_entry_info_t * entry)
{
    return (entry) && entry->arg_count == 1 && strcmp(entry->name, DEMO_BENCHMARK_NAME) == 0 ? true : false;
}

static inline bool demo_is_benchmark_scene(const demo_entry_info_t * entry)
{
    return (entry) && entry->arg_count == 2 && strcmp(entry->name, DEMO_BENCHMARK_SCENE_NAME) == 0 ? true : false;
}

/**********************
 *  STATIC VARIABLES
 **********************/
static const demo_entry_info_t demos_entry_info[] = {
#if LV_USE_DEMO_WIDGETS
    { "widgets", .entry_cb = lv_demo_widgets },
#endif

#if LV_USE_DEMO_MUSIC
    { "music", .entry_cb = lv_demo_music },
#endif
#if LV_USE_DEMO_MULTILANG
    { "multilang", .entry_cb = lv_demo_multilang },
#endif

#if LV_USE_DEMO_STRESS
    { "stress", .entry_cb = lv_demo_stress },
#endif

#if LV_USE_DEMO_KEYPAD_AND_ENCODER
    { "keypad_encoder", .entry_cb = lv_demo_keypad_encoder },
#endif

#if LV_USE_DEMO_FLEX_LAYOUT
    { "flex_layout", .entry_cb = lv_demo_flex_layout },
#endif

#if LV_USE_DEMO_TRANSFORM
    { "transform", .entry_cb = lv_demo_transform },
#endif

#if LV_USE_DEMO_SCROLL
    { "scroll", .entry_cb = lv_demo_scroll },
#endif

    //#if LV_USE_DEMO_BENCHMARK
    //    { DEMO_BENCHMARK_NAME, .entry_benchmark_cb = lv_demo_benchmark, 1 },
    //    { DEMO_BENCHMARK_SCENE_NAME, .entry_benchmark_scene_cb = lv_demo_benchmark_run_scene, 2 },
    //#endif
    { "", .entry_cb = NULL }
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_demos_create(char * info[], int size)
{
    const int demos_count = LV_DEMOS_COUNT;

    if(demos_count <= 0) {
        LV_LOG_ERROR("Please enable some lv_demos firstly!");
        return false;
    }

    const demo_entry_info_t * entry_info = NULL;
    if(size <= 0) { /* default: first demo*/
        entry_info = &demos_entry_info[0];
    }

    if(entry_info == NULL && info) {
        const char * name = info[0];
        for(int i = 0; i < demos_count; i++) {
            if(strcmp(name, demos_entry_info[i].name) == 0 && size - 1 >= demos_entry_info[i].arg_count) {
                entry_info = &demos_entry_info[i];
            }
        }
    }

    if(entry_info == NULL) {
        LV_LOG_ERROR("lv_demos create(%s) failure!", size > 0 ? info[0] : "");
        return false;
    }

    if(entry_info->arg_count == 0) {
        if(entry_info->entry_cb) {
            entry_info->entry_cb();
            return true;
        }
    }
    //#if LV_USE_DEMO_BENCHMARK
    //    else if(demo_is_benchmark(entry_info) && entry_info->entry_benchmark_cb) {
    //        entry_info->entry_benchmark_cb((lv_demo_benchmark_mode_t)atoi(info[1]));
    //        return true;
    //    }
    //    else if(demo_is_benchmark_scene(entry_info) && entry_info->entry_benchmark_scene_cb) {
    //        entry_info->entry_benchmark_scene_cb((lv_demo_benchmark_mode_t)atoi(info[1]), (uint16_t)atoi(info[2]));
    //        return true;
    //    }
    //#endif

    return false;
}

void lv_demos_show_help(void)
{
    int i;
    const int demos_count = LV_DEMOS_COUNT;

    if(demos_count == 0) {
        LV_LOG("lv_demos: no demo available!\n");
        return;
    }

    LV_LOG("\nUsage: lv_demos demo [parameters]\n");
    LV_LOG("\ndemo list:\n");

    for(i = 0; i < demos_count; i++) {
        if(demos_entry_info[i].arg_count == 0) {
            LV_LOG("     %s \n", demos_entry_info[i].name);
        }
        else if(demo_is_benchmark(&demos_entry_info[i])) {
            LV_LOG("     %s [0, 1, 2] \t\t\t(0 for Render&Driver, 1 for Real Render, 2 for Render Only)\n",
                   demos_entry_info[i].name);
        }
        else if(demo_is_benchmark_scene(&demos_entry_info[i])) {
            LV_LOG("     %s [0, 1, 2] scene_no \t(0 for Render&Driver, 1 for Real Render, 2 for Render Only)\n",
                   demos_entry_info[i].name);
        }
    }
}
