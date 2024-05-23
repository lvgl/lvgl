/**
 * @file lv_demos.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demos.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DEMOS_COUNT (sizeof(demos_entry_info) / sizeof(demo_entry_info_t) - 1)

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*demo_method_cb)(void);

typedef struct  {
    const char * name;
    demo_method_cb entry_cb;
} demo_entry_info_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

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

#if LV_USE_DEMO_VECTOR_GRAPHIC && LV_USE_VECTOR_GRAPHIC
    { "vector_graphic", .entry_cb = lv_demo_vector_graphic },
#endif

#if LV_USE_DEMO_BENCHMARK
    { "benchmark", .entry_cb = lv_demo_benchmark },
#endif

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
    else if(entry_info == NULL && info) {
        const char * name = info[0];
        for(int i = 0; i < demos_count; i++) {
            if(lv_strcmp(name, demos_entry_info[i].name) == 0) {
                entry_info = &demos_entry_info[i];
            }
        }
    }

    if(entry_info == NULL) {
        LV_LOG_ERROR("lv_demos create(%s) failure!", size > 0 ? info[0] : "");
        return false;
    }

    if(entry_info->entry_cb) {
        entry_info->entry_cb();
        return true;
    }

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
        LV_LOG("     %s \n", demos_entry_info[i].name);
    }
}
