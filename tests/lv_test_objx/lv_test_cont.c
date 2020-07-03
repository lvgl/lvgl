/**
 * @file lv_test_cont.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
#include "../lv_test_assert.h"

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

void lv_test_cont(void)
{
    lv_test_print("");
    lv_test_print("===================");
    lv_test_print("Start lv_cont tests");
    lv_test_print("===================");

    create_copy();
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void create_copy(void)
{
    lv_test_print("");
    lv_test_print("Create and copy a container");
    lv_test_print("---------------------------");

    lv_test_print("Create a container");
    lv_test_assert_int_eq(0, lv_obj_count_children(lv_scr_act()), "Screen's children count before creation");

    lv_obj_t * obj = lv_cont_create(lv_scr_act(), NULL);
    lv_test_assert_int_eq(1, lv_obj_count_children(lv_scr_act()), "Screen's children count after creation");

    lv_test_print("Test the default values");
    lv_test_assert_int_eq(LV_FIT_NONE, lv_cont_get_fit_left(obj), "Default left fit");
    lv_test_assert_int_eq(LV_FIT_NONE, lv_cont_get_fit_right(obj), "Default right fit");
    lv_test_assert_int_eq(LV_FIT_NONE, lv_cont_get_fit_top(obj), "Default top fit");
    lv_test_assert_int_eq(LV_FIT_NONE, lv_cont_get_fit_bottom(obj), "Default bottom fit");
    lv_test_assert_int_eq(LV_LAYOUT_OFF, lv_cont_get_layout(obj), "Default layout");

    lv_test_print("Delete the container");
    lv_obj_del(obj);
    obj = NULL;
    lv_test_assert_int_eq(0, lv_obj_count_children(lv_scr_act()), "Screen's children count after delete");

}
#endif
