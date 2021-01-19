/**
 * @file lv_test_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
#include "../lv_test_assert.h"
#include "lv_test_style.h"

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
static void empty_style(void);
static void add_remove_read_prop(void);
static void cascade(void);
static void copy(void);
static void states(void);
static void mem_leak(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_test_style(void)
{
    lv_test_print("");
    lv_test_print("====================");
    lv_test_print("Start lv_style tests");
    lv_test_print("====================");

    empty_style();
    add_remove_read_prop();
    cascade();
    copy();
    states();
    mem_leak();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void empty_style(void)
{
    lv_test_print("");
    lv_test_print("Test empty styles:");
    lv_test_print("-----------------");

    lv_style_list_t style_list;
    lv_style_list_init(&style_list);

    lv_res_t found;
    lv_style_int_t value;
    lv_opa_t opa;
    const void * ptr;
    lv_color_t color;

    lv_test_print("Get a properties from an empty style");

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get an 'int' property");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA, &opa);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get an 'opa' property");

    found = _lv_style_list_get_ptr(&style_list, LV_STYLE_TEXT_FONT, &ptr);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get a 'ptr' property");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR, &color);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get a 'color' property");
}

static void add_remove_read_prop(void)
{
    lv_test_print("");
    lv_test_print("Add, remove and read properties:");
    lv_test_print("--------------------------------");

    lv_style_list_t style_list;
    lv_style_list_init(&style_list);

    lv_style_t style;
    lv_style_init(&style);

    _lv_style_list_add_style(&style_list, &style);

    lv_res_t found;
    lv_style_int_t value;
    lv_opa_t opa;
    const void * ptr;
    lv_color_t color;

    lv_test_print("Add an empty style and read properties");

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get a non existing 'int' property");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA, &opa);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get a non existing 'opa' property");

    found = _lv_style_list_get_ptr(&style_list, LV_STYLE_TEXT_FONT, &ptr);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get a non existing 'ptr' property");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR, &color);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get a non existing 'color' property");

    lv_test_print("Set properties and read back the values");
    _lv_style_set_int(&style, LV_STYLE_TEXT_LINE_SPACE, 5);
    _lv_style_set_opa(&style, LV_STYLE_BG_OPA, LV_OPA_50);
    _lv_style_set_ptr(&style, LV_STYLE_TEXT_FONT, LV_THEME_DEFAULT_FONT_NORMAL);
    _lv_style_set_color(&style, LV_STYLE_BG_COLOR, LV_COLOR_RED);

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an existing 'int' property");
    lv_test_assert_int_eq(5, value, "Get the value of an 'int' property");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA, &opa);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an existing 'opa' property");
    lv_test_assert_int_eq(LV_OPA_50, opa, "Get the value of an  'opa' property");

    found = _lv_style_list_get_ptr(&style_list, LV_STYLE_TEXT_FONT, &ptr);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an existing 'ptr' property");
    lv_test_assert_ptr_eq(LV_THEME_DEFAULT_FONT_NORMAL, ptr, "Get the value of a 'ptr' property");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an existing 'color' property");
    lv_test_assert_color_eq(LV_COLOR_RED, color, "Get the value of a 'color' property");

    lv_test_print("Reset the style");
    lv_style_reset(&style);
    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get an 'int' property from a reseted style");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA, &opa);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get an 'opa' property from a reseted style");

    found = _lv_style_list_get_ptr(&style_list, LV_STYLE_TEXT_FONT, &ptr);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get an 'ptr' property from a reseted style");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR, &color);
    lv_test_assert_int_eq(LV_RES_INV, found, "Get an 'color' property from a reseted style");

    /*Clean-up*/
    _lv_style_list_reset(&style_list);
}

static void cascade(void)
{
    lv_test_print("");
    lv_test_print("Cascade style styles:");
    lv_test_print("----------------------");

    lv_style_list_t style_list;
    lv_style_list_init(&style_list);

    lv_style_t style_first;
    lv_style_init(&style_first);
    _lv_style_list_add_style(&style_list, &style_first);

    lv_style_t style_second;
    lv_style_init(&style_second);
    _lv_style_list_add_style(&style_list, &style_second);

    lv_res_t found;
    lv_style_int_t value;
    lv_opa_t opa;
    const void * ptr;
    lv_color_t color;

    lv_test_print("Read properties set only in the firstly added style");

    _lv_style_set_int(&style_first, LV_STYLE_TEXT_LINE_SPACE, 5);
    _lv_style_set_opa(&style_first, LV_STYLE_BG_OPA, LV_OPA_50);
    _lv_style_set_ptr(&style_first, LV_STYLE_TEXT_FONT, LV_THEME_DEFAULT_FONT_NORMAL);
    _lv_style_set_color(&style_first, LV_STYLE_BG_COLOR, LV_COLOR_RED);

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'int' property");
    lv_test_assert_int_eq(5, value, "Get the value of an 'int' property");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA, &opa);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'opa' property");
    lv_test_assert_int_eq(LV_OPA_50, opa, "Get the value of an  'opa' property");

    found = _lv_style_list_get_ptr(&style_list, LV_STYLE_TEXT_FONT, &ptr);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'ptr' property");
    lv_test_assert_ptr_eq(LV_THEME_DEFAULT_FONT_NORMAL, ptr, "Get the value of a 'ptr' property");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'color' property");
    lv_test_assert_color_eq(LV_COLOR_RED, color, "Get the value of a 'color' property");

    lv_test_print("Overwrite the properties from the second style");

    _lv_style_set_int(&style_second, LV_STYLE_TEXT_LINE_SPACE, 10);
    _lv_style_set_opa(&style_second, LV_STYLE_BG_OPA, LV_OPA_60);
    _lv_style_set_ptr(&style_second, LV_STYLE_TEXT_FONT, LV_THEME_DEFAULT_FONT_NORMAL + 1);
    _lv_style_set_color(&style_second, LV_STYLE_BG_COLOR, LV_COLOR_BLUE);

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an overwritten 'int' property");
    lv_test_assert_int_eq(10, value, "Get the value of an overwritten 'int' property");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA, &opa);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an overwritten 'opa' property");
    lv_test_assert_int_eq(LV_OPA_60, opa, "Get the value of an overwritten 'opa' property");

    found = _lv_style_list_get_ptr(&style_list, LV_STYLE_TEXT_FONT, &ptr);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an overwritten 'ptr' property");
    lv_test_assert_ptr_eq(LV_THEME_DEFAULT_FONT_NORMAL + 1, ptr, "Get the value of an overwritten 'ptr' property");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an overwritten 'color' property");
    lv_test_assert_color_eq(LV_COLOR_BLUE, color, "Get the value of an overwritten 'color' property");

    lv_test_print("Overwrite the properties with the local style");
    _lv_style_list_set_local_int(&style_list, LV_STYLE_TEXT_LINE_SPACE, 20);
    _lv_style_list_set_local_opa(&style_list, LV_STYLE_BG_OPA, LV_OPA_70);
    _lv_style_list_set_local_ptr(&style_list, LV_STYLE_TEXT_FONT, LV_THEME_DEFAULT_FONT_NORMAL + 2);
    _lv_style_list_set_local_color(&style_list, LV_STYLE_BG_COLOR, LV_COLOR_LIME);

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a local 'int' property");
    lv_test_assert_int_eq(20, value, "Get the value of a local 'int' property");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA, &opa);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a local 'opa' property");
    lv_test_assert_int_eq(LV_OPA_70, opa, "Get the value of a local 'opa' property");

    found = _lv_style_list_get_ptr(&style_list, LV_STYLE_TEXT_FONT, &ptr);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a local 'ptr' property");
    lv_test_assert_ptr_eq(LV_THEME_DEFAULT_FONT_NORMAL + 2, ptr, "Get the value of a local'ptr' property");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a local 'color' property");
    lv_test_assert_color_eq(LV_COLOR_LIME, color, "Get the value of a local'color' property");

    /*Clean-up*/
    _lv_style_list_reset(&style_list);
}

static void copy(void)
{
    lv_test_print("");
    lv_test_print("Copy styles and style lists");
    lv_test_print("---------------------------");

    lv_test_print("Copy a style");
    lv_style_t style_src;
    lv_style_init(&style_src);
    _lv_style_set_int(&style_src, LV_STYLE_TEXT_LINE_SPACE, 5);

    lv_style_t style_dest;
    lv_style_init(&style_dest);
    lv_style_copy(&style_dest, &style_src);

    int16_t weight;
    lv_style_int_t value;

    weight = _lv_style_get_int(&style_dest, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(0, weight, "Get a copied property from a style");
    lv_test_assert_int_eq(5, value, "Get the value of a copied from a property");

    lv_test_print("Copy a style list");
    lv_style_list_t list_src;
    lv_style_list_init(&list_src);
    _lv_style_list_add_style(&list_src, &style_src);
    _lv_style_list_set_local_int(&list_src, LV_STYLE_LINE_DASH_WIDTH, 20);

    lv_style_list_t list_dest;
    lv_style_list_init(&list_dest);
    lv_style_list_copy(&list_dest, &list_src);

    lv_res_t found;
    found = _lv_style_list_get_int(&list_dest, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a copied property from a list");
    lv_test_assert_int_eq(5, value, "Get the value of a copied property from a list");
    found = _lv_style_list_get_int(&list_dest, LV_STYLE_LINE_DASH_WIDTH, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a copied local property from a list");
    lv_test_assert_int_eq(20, value, "Get the value of a copied local property from a list");

    /*Clean up*/
    _lv_style_list_reset(&list_dest);
    _lv_style_list_reset(&list_src);

    lv_style_reset(&style_dest);
    lv_style_reset(&style_src);
}

static void states(void)
{
    lv_test_print("");
    lv_test_print("Test style states:");
    lv_test_print("------------------");

    lv_style_list_t style_list;
    lv_style_list_init(&style_list);

    lv_style_t style_first;
    lv_style_init(&style_first);
    _lv_style_list_add_style(&style_list, &style_first);

    lv_style_t style_second;
    lv_style_init(&style_second);
    _lv_style_list_add_style(&style_list, &style_second);

    lv_test_print("Test state precedence in 1 style");
    _lv_style_set_int(&style_first, LV_STYLE_TEXT_LINE_SPACE, 5);
    _lv_style_set_int(&style_first, LV_STYLE_TEXT_LINE_SPACE | (LV_STATE_CHECKED) << LV_STYLE_STATE_POS, 6);
    _lv_style_set_int(&style_first, LV_STYLE_TEXT_LINE_SPACE | (LV_STATE_PRESSED) << LV_STYLE_STATE_POS, 7);

    lv_res_t found;
    lv_style_int_t value;

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'int' property in normal state");
    lv_test_assert_int_eq(5, value, "Get the value of an 'int' property in normal state");

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE | (LV_STATE_CHECKED) << LV_STYLE_STATE_POS, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'int' property in checked state");
    lv_test_assert_int_eq(6, value, "Get the value of an 'int' in checked state");

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE | (LV_STATE_PRESSED) << LV_STYLE_STATE_POS, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'int' property in pressed state");
    lv_test_assert_int_eq(7, value, "Get the value of an 'int' in pressed state");

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE | (LV_STATE_HOVERED) << LV_STYLE_STATE_POS, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'int' property in hover (unspecified) state");
    lv_test_assert_int_eq(5, value, "Get the value of an 'int' in hover (unspecified) state");

    found = _lv_style_list_get_int(&style_list, LV_STYLE_TEXT_LINE_SPACE | (LV_STATE_CHECKED | LV_STATE_PRESSED | LV_STATE_HOVERED) << LV_STYLE_STATE_POS, &value);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'int' property in checked pressed hovered state");
    lv_test_assert_int_eq(7, value, "Get the value of an 'int' in checked pressed hovered state");

    lv_test_print("Test state precedence in 1 style with combined states");
    _lv_style_set_opa(&style_first, LV_STYLE_BG_OPA, LV_OPA_50);
    _lv_style_set_opa(&style_first, LV_STYLE_BG_OPA | (LV_STATE_CHECKED | LV_STATE_PRESSED) << LV_STYLE_STATE_POS, LV_OPA_60);

    lv_opa_t opa;
    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA , &opa);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'opa' property in normal state");
    lv_test_assert_int_eq(LV_OPA_50, opa, "Get the value of an 'int' in normal state");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA | (LV_STATE_CHECKED) << LV_STYLE_STATE_POS, &opa);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'opa' property in checked (unspecified) state");
    lv_test_assert_int_eq(LV_OPA_50, opa, "Get the value of an 'int' in checked (unspecified) state");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA | (LV_STATE_CHECKED | LV_STATE_PRESSED) << LV_STYLE_STATE_POS, &opa);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'opa' property in checked pressed state");
    lv_test_assert_int_eq(LV_OPA_60, opa, "Get the value of an 'int' in checked pressed state");

    found = _lv_style_list_get_opa(&style_list, LV_STYLE_BG_OPA | (LV_STATE_CHECKED | LV_STATE_PRESSED | LV_STATE_HOVERED) << LV_STYLE_STATE_POS, &opa);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get an 'opa' property in checked pressed hovered state");
    lv_test_assert_int_eq(LV_OPA_60, opa, "Get the value of an 'int' in checked pressed hovered state");

    lv_test_print("Test state precedence in 2 styles");
    _lv_style_set_color(&style_first, LV_STYLE_BG_COLOR, LV_COLOR_YELLOW);
    _lv_style_set_color(&style_first, LV_STYLE_BG_COLOR | (LV_STATE_HOVERED) << LV_STYLE_STATE_POS, LV_COLOR_RED);
    _lv_style_set_color(&style_second, LV_STYLE_BG_COLOR | (LV_STATE_CHECKED) << LV_STYLE_STATE_POS, LV_COLOR_LIME);
    _lv_style_set_color(&style_second, LV_STYLE_BG_COLOR | (LV_STATE_HOVERED | LV_STATE_PRESSED) << LV_STYLE_STATE_POS, LV_COLOR_BLUE);

    lv_color_t color;

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a 'color' property in normal state");
    lv_test_assert_color_eq(LV_COLOR_YELLOW, color, "Get the value of a 'color' property in normal state");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR | (LV_STATE_HOVERED) << LV_STYLE_STATE_POS, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a 'color' property in hovered state");
    lv_test_assert_color_eq(LV_COLOR_RED, color, "Get the value of a 'color' in hovered state");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR | (LV_STATE_CHECKED) << LV_STYLE_STATE_POS, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a 'color' property in checked state");
    lv_test_assert_color_eq(LV_COLOR_LIME, color, "Get the value of a 'color' in checked state");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR | (LV_STATE_HOVERED | LV_STATE_PRESSED) << LV_STYLE_STATE_POS, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a 'color' property in hover pressed state");
    lv_test_assert_color_eq(LV_COLOR_BLUE, color, "Get the value of a 'color' in hover pressed state");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR | (LV_STATE_EDITED) << LV_STYLE_STATE_POS, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a 'color' property in edit (unspecified) state");
    lv_test_assert_color_eq(LV_COLOR_YELLOW, color, "Get the value of a 'color' in edit (unspecified) state");

    found = _lv_style_list_get_color(&style_list, LV_STYLE_BG_COLOR | (LV_STATE_CHECKED | LV_STATE_EDITED) << LV_STYLE_STATE_POS, &color);
    lv_test_assert_int_eq(LV_RES_OK, found, "Get a 'color' property in checked edit state");
    lv_test_assert_color_eq(LV_COLOR_LIME, color, "Get the value of a 'color' in checked edit state");

    /*Clean-up*/
    _lv_style_list_reset(&style_list);
}

static void mem_leak(void)
{

    lv_test_print("");
    lv_test_print("Test style set, add, remove memory leak");
    lv_test_print("----------------------------------------");

    lv_mem_monitor_t mon_start;
    lv_mem_monitor_t mon_end;

    lv_style_list_t style_list;
    lv_style_list_init(&style_list);

    lv_style_t style1;
    lv_style_init(&style1);

    lv_style_t style2;
    lv_style_init(&style2);

    lv_style_t style3;
    lv_style_init(&style3);

    uint32_t i;

    lv_test_print("Set style properties");
    lv_mem_monitor(&mon_start);
    for(i = 0; i < 100; i++) {
        _lv_style_set_color(&style2, LV_STYLE_LINE_COLOR, LV_COLOR_RED);

        _lv_style_set_color(&style3, LV_STYLE_LINE_COLOR, LV_COLOR_RED);
        _lv_style_set_color(&style3, LV_STYLE_LINE_COLOR | (LV_STATE_EDITED) << LV_STYLE_STATE_POS, LV_COLOR_BLUE);
        _lv_style_set_color(&style3, LV_STYLE_LINE_COLOR, LV_COLOR_BLUE);
        _lv_style_set_color(&style3, LV_STYLE_LINE_COLOR | (LV_STATE_EDITED | LV_STATE_FOCUSED) << LV_STYLE_STATE_POS, LV_COLOR_GREEN);

        _lv_style_set_color(&style3, LV_STYLE_BG_COLOR, LV_COLOR_RED);

        _lv_style_set_color(&style3, LV_STYLE_IMAGE_RECOLOR, LV_COLOR_RED);

        lv_style_reset(&style1);
        lv_style_reset(&style2);
        lv_style_reset(&style3);

    }

    lv_test_assert_int_eq(LV_RES_OK, lv_mem_test(), "Memory integrity check");
    lv_mem_defrag();
    lv_mem_monitor(&mon_end);
    lv_test_assert_int_lt(sizeof(void*) * 8, mon_start.free_size - mon_end.free_size, "Style memory leak");

    lv_test_print("Use local style");
    lv_mem_monitor(&mon_start);
    for(i = 0; i < 100; i++) {
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED) << LV_STYLE_STATE_POS, LV_THEME_DEFAULT_FONT_NORMAL);

        _lv_style_list_reset(&style_list);

        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED) << LV_STYLE_STATE_POS, NULL);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);

        _lv_style_list_reset(&style_list);
    }

    _lv_style_list_reset(&style_list);
    lv_test_assert_int_eq(LV_RES_OK, lv_mem_test(), "Memory integrity check");
    lv_mem_defrag();
    lv_mem_monitor(&mon_end);
    lv_test_assert_int_lt(sizeof(void*) * 8, mon_start.free_size - mon_end.free_size, "Style memory leak");

    lv_test_print("Add styles");
    lv_mem_monitor(&mon_start);
    for(i = 0; i < 100; i++) {
        _lv_style_set_color(&style1, LV_STYLE_LINE_COLOR, LV_COLOR_RED);
        _lv_style_set_color(&style2, LV_STYLE_LINE_COLOR, LV_COLOR_RED);
        _lv_style_set_color(&style3, LV_STYLE_LINE_COLOR, LV_COLOR_RED);

        _lv_style_list_add_style(&style_list, &style1);
        _lv_style_list_remove_style(&style_list, &style1);

        _lv_style_list_add_style(&style_list, &style2);
        _lv_style_list_add_style(&style_list, &style3);

        _lv_style_list_remove_style(&style_list, &style2);
        _lv_style_list_add_style(&style_list, &style1);

        _lv_style_list_reset(&style_list);
        lv_style_reset(&style1);
        lv_style_reset(&style2);
        lv_style_reset(&style3);
    }

    lv_test_assert_int_eq(LV_RES_OK, lv_mem_test(), "Memory integrity check");
    lv_mem_defrag();
    lv_mem_monitor(&mon_end);
    lv_test_assert_int_lt(sizeof(void*) * 8, mon_start.free_size - mon_end.free_size, "Style memory leak");

    lv_test_print("Add styles and use local style");
     lv_mem_monitor(&mon_start);
     for(i = 0; i < 100; i++) {
         _lv_style_set_color(&style1, LV_STYLE_LINE_COLOR, LV_COLOR_RED);
         _lv_style_set_color(&style2, LV_STYLE_LINE_COLOR, LV_COLOR_RED);
         _lv_style_set_color(&style3, LV_STYLE_LINE_COLOR, LV_COLOR_RED);

         if(i % 2 == 0) _lv_style_list_set_local_color(&style_list, LV_STYLE_LINE_COLOR, LV_COLOR_RED);

         _lv_style_list_add_style(&style_list, &style1);
         _lv_style_list_remove_style(&style_list, &style1);

         _lv_style_list_add_style(&style_list, &style2);
         _lv_style_list_add_style(&style_list, &style3);

         _lv_style_list_remove_style(&style_list, &style2);
         _lv_style_list_add_style(&style_list, &style1);

         if(i % 2 != 0) _lv_style_list_set_local_color(&style_list, LV_STYLE_LINE_COLOR, LV_COLOR_RED);

         _lv_style_list_reset(&style_list);
         lv_style_reset(&style1);
         lv_style_reset(&style2);
         lv_style_reset(&style3);
     }

     lv_test_assert_int_eq(LV_RES_OK, lv_mem_test(), "Memory integrity check");
     lv_mem_defrag();
     lv_mem_monitor(&mon_end);
     lv_test_assert_int_lt(sizeof(void*) * 8, mon_start.free_size - mon_end.free_size, "Style memory leak");

    lv_test_print("Complex test");

    lv_mem_monitor(&mon_start);

    for(i = 0; i < 100; i++) {
        if(i % 2 == 0) {
            _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_CLOSE);
        }

        _lv_style_set_color(&style1, LV_STYLE_LINE_COLOR, LV_COLOR_RED);
        _lv_style_set_color(&style1, LV_STYLE_LINE_COLOR | (LV_STATE_EDITED) << LV_STYLE_STATE_POS, LV_COLOR_BLUE);
        _lv_style_set_color(&style1, LV_STYLE_LINE_COLOR, LV_COLOR_BLUE);
        _lv_style_set_color(&style1, LV_STYLE_LINE_COLOR | (LV_STATE_EDITED | LV_STATE_FOCUSED) << LV_STYLE_STATE_POS, LV_COLOR_GREEN);

        _lv_style_list_add_style(&style_list, &style1);

        if(i % 4 == 0) {
            _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_CLOSE);
        }

        _lv_style_list_remove_style(&style_list, &style1);

        _lv_style_set_opa(&style2, LV_STYLE_TEXT_OPA, LV_OPA_10);
        _lv_style_set_opa(&style2, LV_STYLE_TEXT_OPA, LV_OPA_20);
        _lv_style_set_opa(&style2, LV_STYLE_TEXT_OPA, LV_OPA_30);
        _lv_style_set_opa(&style2, LV_STYLE_TEXT_OPA, LV_OPA_40);
        _lv_style_set_opa(&style2, LV_STYLE_TEXT_OPA, LV_OPA_50);
        _lv_style_set_opa(&style2, LV_STYLE_TEXT_OPA, LV_OPA_60);

        _lv_style_list_add_style(&style_list, &style2);

        if(i % 8 == 0) {
            _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_CLOSE);
        }

        _lv_style_list_add_style(&style_list, &style2);
        _lv_style_list_add_style(&style_list, &style2);
        _lv_style_list_add_style(&style_list, &style2);
        _lv_style_list_add_style(&style_list, &style2);
        _lv_style_list_remove_style(&style_list, &style2);

        _lv_style_set_int(&style3, LV_STYLE_PAD_LEFT, 10);
        _lv_style_set_int(&style3, LV_STYLE_PAD_RIGHT, 20);
        _lv_style_set_int(&style3, LV_STYLE_PAD_LEFT, 11);
        _lv_style_set_int(&style3, LV_STYLE_PAD_RIGHT, 21);
        _lv_style_set_int(&style3, LV_STYLE_PAD_LEFT, 12);
        _lv_style_set_int(&style3, LV_STYLE_PAD_RIGHT, 22);
        _lv_style_set_int(&style3, LV_STYLE_PAD_LEFT, 12);
        _lv_style_set_int(&style3, LV_STYLE_PAD_RIGHT, 23);

        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED) << LV_STYLE_STATE_POS, LV_THEME_DEFAULT_FONT_NORMAL);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED) << LV_STYLE_STATE_POS, NULL);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);
        _lv_style_list_set_local_ptr(&style_list, LV_STYLE_PATTERN_IMAGE, LV_SYMBOL_OK);

        _lv_style_list_add_style(&style_list, &style3);
        _lv_style_list_add_style(&style_list, &style2);
        _lv_style_list_add_style(&style_list, &style1);

        _lv_style_list_reset(&style_list);
        lv_style_reset(&style1);
        lv_style_reset(&style2);
        lv_style_reset(&style3);

    }

    _lv_style_list_reset(&style_list);
    lv_style_reset(&style1);
    lv_style_reset(&style2);
    lv_style_reset(&style3);

    lv_test_assert_int_eq(LV_RES_OK, lv_mem_test(), "Memory integrity check");
    lv_mem_defrag();
    lv_mem_monitor(&mon_end);
    lv_test_assert_int_lt(sizeof(void*) * 8, mon_start.free_size - mon_end.free_size, "Style memory leak");
}
#endif
