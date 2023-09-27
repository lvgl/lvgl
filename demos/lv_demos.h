/**
 * @file lv_demos.h
 *
 */

#ifndef LV_DEMOS_H
#define LV_DEMOS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

#if LV_USE_DEMO_BENCHMARK
#include "benchmark/lv_demo_benchmark.h"
#endif

#if LV_USE_DEMO_KEYPAD_AND_ENCODER
#include "keypad_encoder/lv_demo_keypad_encoder.h"
#endif

#if LV_USE_DEMO_MUSIC
#include "music/lv_demo_music.h"
#endif

#if LV_USE_DEMO_STRESS
#include "stress/lv_demo_stress.h"
#endif

#if LV_USE_DEMO_WIDGETS
#include "widgets/lv_demo_widgets.h"
#endif

#if LV_USE_DEMO_FLEX_LAYOUT
#include "flex_layout/lv_demo_flex_layout.h"
#endif

#if LV_USE_DEMO_TRANSFORM
#include "transform/lv_demo_transform.h"
#endif

#if LV_USE_DEMO_SCROLL
#include "scroll/lv_demo_scroll.h"
#endif

#if LV_USE_DEMO_MULTILANG
#include "multilang/lv_demo_multilang.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**
 * The main entry which initializes LVGL instance and runs the specified demo.
 * @param info  the information which contains demo name and parameters needs by lv_demo_xxx.
 * @param size  the size of info.
 */
int lv_demos_main_entry(char * info[], int size);

/**
 * Call lv_demo_xxx.
 * @param   info the information which contains demo name and parameters needs by lv_demo_xxx.
 * @size    size of information.
 */
bool lv_demos_create_demo(char * info[], int size);

#if LV_USE_DEMO_HAL_CUSTOM
lv_disp_t * lv_demos_hal_init_custom(void);
void lv_demos_hal_deinit_custom(void);
void lv_demos_run_custom(void);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_H*/
