/**
 * @file lv_test_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
#include "../lv_test_assert.h"
#include "lv_test_label.h"

#if LV_BUILD_TEST

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_copy(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_test_label(void)
{
    lv_test_print("");
    lv_test_print("===================");
    lv_test_print("Start lv_label tests");
    lv_test_print("===================");

#if LV_USE_LABEL
    create_copy();
#else
    lv_test_print("Skip label test: LV_USE_LABEL == 0");
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void create_copy(void)
{
    lv_test_print("");
    lv_test_print("Create a label");
    lv_test_print("---------------------------");

    lv_label_create(lv_scr_act(), NULL);
#if LV_COLOR_DEPTH == 32
    lv_test_assert_img_eq("lv_test_img32_label_1.png", "Create a label and leave the default settings");
#endif
}
#endif
