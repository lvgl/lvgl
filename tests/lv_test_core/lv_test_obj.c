/**
 * @file lv_test_obj.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
#include "../lv_test_assert.h"
#include "lv_test_obj.h"

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
static void create_delete_change_parent(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_test_obj(void)
{
    lv_test_print("");
    lv_test_print("==================");
    lv_test_print("Start lv_obj tests");
    lv_test_print("==================");

    create_delete_change_parent();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void create_delete_change_parent(void)
{

    lv_test_print("");
    lv_test_print("Create, delete, change parent of a simple object:");
    lv_test_print("-------------------------------------------------");

    lv_test_print("Create an object on the default screen");
    lv_test_assert_int_eq(0, lv_obj_get_child_cnt(lv_scr_act()), "Screen's children count before creation");

    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_test_assert_int_eq(1, lv_obj_get_child_cnt(lv_scr_act()), "Screen's children count after creation");
    lv_test_assert_int_eq(0, lv_obj_get_child_cnt(obj), "New object's children count after creation");

    lv_test_print("Delete the created object");
    lv_obj_del(obj);
    obj = NULL;
    lv_test_assert_int_eq(0, lv_obj_get_child_cnt(lv_scr_act()), "Screen's children count after delete");

    lv_test_print("Create two objects");
    lv_obj_t * obj_parent = lv_obj_create(lv_scr_act());
    lv_obj_t * obj_child = lv_obj_create(lv_scr_act());
    lv_test_assert_int_eq(2, lv_obj_get_child_cnt(lv_scr_act()), "Screen's children count after creation");

    lv_test_print("Change the parent of the second object to the first");
    lv_obj_set_parent(obj_child, obj_parent);
    lv_test_assert_int_eq(1, lv_obj_get_child_cnt(lv_scr_act()), "Screen's children count after parent change");
    lv_test_assert_int_eq(1, lv_obj_get_child_cnt(obj_parent), "Parent object's children count after parent change");
    lv_test_assert_int_eq(0, lv_obj_get_child_cnt(obj_child), "Child object's children count after parent change");

    lv_test_print("Remove the parent object");
    lv_obj_del(obj_parent);
    lv_test_assert_int_eq(0, lv_obj_get_child_cnt(lv_scr_act()), "Screen's children count after delete");
}
#endif
