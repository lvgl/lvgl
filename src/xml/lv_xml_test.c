/**
 * @file lv_xml_test.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_test.h"
#if LV_USE_XML && LV_USE_TEST

#include "../../lvgl.h"
#include "lv_xml.h"
#include "lv_xml_utils.h"
#include "lv_xml_component_private.h"
#include "../misc/lv_fs.h"
#include "../libs/expat/expat.h"
#include "../display/lv_display_private.h"
#include "../core/lv_obj_private.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TEST_NAME    "__test__"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_xml_test_step_type_t type;
    union {
        struct {
            int32_t x;
            int32_t y;
        } mouse_pos;

        const char * str;

        struct {
            int32_t ms;
        } wait;

        struct {
            int32_t ms;
        } freeze;

        struct {
            const char * path;
        } screenshot_compare;

        struct {
            lv_subject_t * subject;
            const char * value;
        } subject_set;
        struct {
            lv_subject_t * subject;
            const char * value;
        } subject_compare;
    } param;
    uint32_t passed : 1;
} lv_xml_test_step_t;

typedef struct {
    const char * ref_image_path_prefix;
    uint32_t step_cnt;
    uint32_t step_act;
    lv_xml_test_step_t * steps;
    uint32_t processing_steps : 1;
} lv_xml_test_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * create_cursor(lv_obj_t * parent);
static bool execute_step(lv_xml_test_step_t * step, uint32_t slowdown);
static void start_metadata_handler(void * user_data, const char * name, const char ** attrs);
static void end_metadata_handler(void * user_data, const char * name);
static void click_at(int32_t x, int32_t y, int32_t slowdown);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_xml_test_t test;
static lv_display_t * test_display;
static lv_obj_t * cursor;
static lv_tick_get_cb_t tick_cb_original;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


lv_result_t lv_xml_test_register_from_data(const char * xml_def, const char * ref_image_path_prefix)
{
    /*Cleanup the previous test*/
    lv_xml_unregister_test();

    test.ref_image_path_prefix = ref_image_path_prefix;

    /*Register as a component first to allow creating the view of the test later*/
    lv_result_t res = lv_xml_register_component_from_data(LV_TEST_NAME, xml_def);
    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("Couldn't register the test as a component");
        return LV_RESULT_INVALID;
    }

    /* Parse the XML to extract metadata */
    XML_Memory_Handling_Suite mem_handlers;
    mem_handlers.malloc_fcn = lv_malloc;
    mem_handlers.realloc_fcn = lv_realloc;
    mem_handlers.free_fcn = lv_free;
    XML_Parser parser = XML_ParserCreate_MM(NULL, &mem_handlers, NULL);
    XML_SetElementHandler(parser, start_metadata_handler, end_metadata_handler);


    if(XML_Parse(parser, xml_def, lv_strlen(xml_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_ERROR("XML parsing error: %s on line %lu",
                     XML_ErrorString(XML_GetErrorCode(parser)),
                     (unsigned long)XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        test.ref_image_path_prefix = NULL;
        return LV_RESULT_INVALID;
    }

    XML_ParserFree(parser);
    test.ref_image_path_prefix = NULL;

    return LV_RESULT_OK;
}


lv_result_t lv_xml_test_register_from_file(const char * path, const char * ref_image_path_prefix)
{
    lv_fs_res_t fs_res;
    lv_fs_file_t f;
    fs_res = lv_fs_open(&f, path, LV_FS_MODE_RD);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_WARN("Failed to open %s", path);
        return LV_RESULT_INVALID;
    }

    /* Determine file size */
    lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    uint32_t file_size = 0;
    lv_fs_tell(&f, &file_size);
    lv_fs_seek(&f, 0, LV_FS_SEEK_SET);

    /* Create the buffer */
    char * xml_buf = lv_zalloc(file_size + 1);
    if(xml_buf == NULL) {
        LV_LOG_WARN("Memory allocation failed for file %s (%" LV_PRIu32 "bytes)", path, file_size + 1);
        lv_fs_close(&f);
        return LV_RESULT_INVALID;
    }

    /* Read the file content  */
    uint32_t rn;
    lv_fs_read(&f, xml_buf, file_size, &rn);
    if(rn != file_size) {
        LV_LOG_WARN("Couldn't read %s fully", path);
        lv_free(xml_buf);
        lv_fs_close(&f);
        return LV_RESULT_INVALID;
    }

    /* Null-terminate the buffer */
    xml_buf[rn] = '\0';

    /* Register the test */
    lv_result_t res = lv_xml_test_register_from_data(xml_buf, ref_image_path_prefix);

    /* Housekeeping */
    lv_free(xml_buf);
    lv_fs_close(&f);

    return res;
}

void lv_xml_unregister_test(void)
{
    uint32_t i;
    for(i = 0; i < test.step_cnt; i++) {
        lv_xml_test_step_type_t type = test.steps[i].type;
        if(type == LV_XML_TEST_STEP_TYPE_CLICK_ON) {
            lv_free((void *)test.steps[i].param.str);
        }
        if(type == LV_XML_TEST_STEP_TYPE_SET_LANGUAGE) {
            lv_free((void *)test.steps[i].param.str);
        }
        if(type == LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE) {
            lv_free((void *)test.steps[i].param.screenshot_compare.path);
        }
        if(type == LV_XML_TEST_STEP_TYPE_SUBJECT_SET) {
            lv_free((void *)test.steps[i].param.subject_set.value);
        }
        if(type == LV_XML_TEST_STEP_TYPE_SUBJECT_COMPARE) {
            lv_free((void *)test.steps[i].param.subject_compare.value);
        }
    }
    lv_free(test.steps);
    test.steps = NULL;
    test.step_cnt = 0;

    lv_xml_unregister_component(LV_TEST_NAME);
}

void lv_xml_test_run_init(void)
{
    lv_display_t * normal_display = lv_display_get_default();
    test_display = lv_test_display_create(normal_display->hor_res, normal_display->ver_res);

    /*The test will control the ticks*/
    tick_cb_original = lv_tick_get_cb();
    lv_tick_set_cb(NULL);

    lv_test_indev_create_all();
    cursor = create_cursor(lv_display_get_layer_sys(normal_display));

    lv_xml_component_scope_t * scope = lv_xml_component_get_scope(LV_TEST_NAME);
    lv_xml_component_scope_t * extends_scope = lv_xml_component_get_scope(scope->extends);
    lv_obj_t * act_screen = lv_screen_active();
    if(extends_scope && extends_scope->is_screen) {
        lv_obj_t * test_screen = lv_xml_create(NULL, LV_TEST_NAME, NULL);
        lv_screen_load(test_screen);
        lv_obj_delete(act_screen);
    }
    else {
        lv_obj_clean(act_screen);
        lv_xml_create(act_screen, LV_TEST_NAME, NULL);
    }
    lv_refr_now(normal_display);
    test.step_act = 0;
}

bool lv_xml_test_run_next(uint32_t slowdown)
{
    bool passed = execute_step(&test.steps[test.step_act], slowdown);
    test.steps[test.step_act].passed = passed;
    if(!test.steps[test.step_act].passed) {
        LV_LOG_WARN("Step %" LV_PRIu32 "failed", test.step_act);
    }

    test.step_act++;
    return passed;
}

bool lv_xml_test_run_to(uint32_t slowdown, uint32_t step)
{
    if(test.step_cnt == 0) {
        LV_LOG_WARN("There are no test steps");
        return false;
    }

    if(step >= test.step_cnt) {

        LV_LOG_WARN("%" LV_PRIu32 " step index was greater than the total step count."
                    "Limited to %" LV_PRIu32,
                    step, test.step_cnt);
        step = test.step_cnt - 1;
    }

    bool passed = true;
    while(passed && test.step_act <= step) {
        passed = lv_xml_test_run_next(slowdown);
    }

    return passed;
}

void lv_xml_test_run_stop(void)
{
    if(cursor) {
        lv_obj_delete(cursor);
        cursor = NULL;
    }
    if(test_display) {
        lv_tick_set_cb(tick_cb_original);
        lv_display_delete(test_display);
        test_display = NULL;
    }

    lv_test_indev_delete_all();
}

uint32_t lv_xml_test_run_all(uint32_t slowdown)
{
    lv_xml_test_run_init();

    uint32_t failed_cnt = 0;
    uint32_t i;
    for(i = 0; i < test.step_cnt; i++) {
        bool passed = lv_xml_test_run_next(slowdown);
        if(!passed) failed_cnt++;
    }

    lv_xml_test_run_stop();

    return failed_cnt;
}


uint32_t lv_xml_test_get_step_count(void)
{
    return test.step_cnt;
}

uint32_t lv_xml_test_get_step_next(void)
{
    return test.step_act;
}

lv_xml_test_step_type_t lv_xml_test_get_step_type(uint32_t idx)
{
    if(idx >= test.step_cnt) return LV_XML_TEST_STEP_TYPE_NONE;

    return test.steps[idx].type;

}

bool lv_xml_test_get_status(uint32_t idx)
{
    if(idx >= test.step_cnt) return LV_XML_TEST_STEP_TYPE_NONE;

    return test.steps[idx].passed;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_xml_test_wait(uint32_t ms, uint32_t slowdown)
{
    lv_test_wait(ms);
    lv_delay_ms(ms * slowdown);
}

static bool execute_step(lv_xml_test_step_t * step, uint32_t slowdown)
{
    bool res = true;

    if(step->type == LV_XML_TEST_STEP_TYPE_CLICK_AT) {
        int32_t x = step->param.mouse_pos.x;
        int32_t y = step->param.mouse_pos.y;
        click_at(x, y, slowdown);
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_CLICK_ON) {
        const char * name = step->param.str;
        lv_obj_t * obj = lv_obj_find_by_name(lv_screen_active(), name);
        if(obj == NULL) {
            LV_LOG_WARN("No widget found by `%s` name", name);
            return LV_RESULT_INVALID;
        }

        if(lv_obj_is_visible(obj) == false) {
            LV_LOG_WARN("`%s` is not visible, so can't click on it", name);
            return LV_RESULT_INVALID;
        }

        int32_t x = obj->coords.x1 + lv_area_get_width(&obj->coords) / 2;
        int32_t y = obj->coords.y1 + lv_area_get_height(&obj->coords) / 2;

        click_at(x, y, slowdown);
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_PRESS) {
        lv_obj_add_state(cursor, LV_STATE_PRESSED);
        lv_test_mouse_press();
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_RELEASE) {
        lv_obj_remove_state(cursor, LV_STATE_PRESSED);
        lv_test_mouse_release();
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_MOVE_TO) {
        int32_t x = step->param.mouse_pos.x;
        int32_t y = step->param.mouse_pos.y;
        lv_test_mouse_move_to(x, y);
        lv_obj_set_pos(cursor, x, y);
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE) {

        /*Set the act_screen's pointer to for the test display so that it will render it
         *for screenshot compare*/
        lv_obj_t * act_screen_original = test_display->act_scr;
        test_display->act_scr = lv_screen_active();

        /*lv_test_screenshot_compare assumes that the default display is test_display*/
        lv_display_t * default_display = lv_display_get_default();
        lv_display_set_default(test_display);

        /*Make sure that both displays will be updated. Don't invalidate by
         *using the act_screen as now it belongs to both displays.*/
        lv_obj_invalidate(lv_display_get_layer_sys(default_display));
        lv_obj_invalidate(lv_display_get_layer_sys(test_display));

        /*Do the actual screenshot compare*/
        lv_test_screenshot_result_t screen_cmp_res;
        screen_cmp_res = lv_test_screenshot_compare(step->param.screenshot_compare.path);
        if(screen_cmp_res != LV_TEST_SCREENSHOT_RESULT_PASSED) {
            LV_LOG_WARN("screenshot compare of `%s` failed", step->param.screenshot_compare.path);
            res = false;
        }
        else {
            res = true;
        }

        /*Restore*/
        lv_display_set_default(default_display);
        test_display->act_scr = act_screen_original;

    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_WAIT) {
        lv_xml_test_wait(step->param.wait.ms, slowdown);
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_FREEZE) {
        lv_delay_ms(step->param.freeze.ms * slowdown);
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_SUBJECT_SET) {
        lv_subject_type_t type = step->param.subject_set.subject->type;
        if(type == LV_SUBJECT_TYPE_INT) {
            lv_subject_set_int(step->param.subject_set.subject, lv_xml_atoi(step->param.subject_set.value));
        }
        else if(type == LV_SUBJECT_TYPE_STRING) {
            lv_subject_copy_string(step->param.subject_set.subject, step->param.subject_set.value);
        }
        else {
            LV_LOG_WARN("Not supported subject type %d", type);
        }
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_SUBJECT_COMPARE) {
        lv_subject_type_t type = step->param.subject_compare.subject->type;
        if(type == LV_SUBJECT_TYPE_INT) {
            int32_t v =  lv_subject_get_int(step->param.subject_compare.subject);
            if(v != lv_xml_atoi(step->param.subject_compare.value)) {
                LV_LOG_WARN("Subject compare failed. Expected: %s, Actual: %" LV_PRId32, step->param.subject_compare.value, v);
                res = false;
            }
        }
        else if(type == LV_SUBJECT_TYPE_STRING) {
            const char * v =  lv_subject_get_string(step->param.subject_compare.subject);
            if(lv_streq(v, step->param.subject_compare.value) == 0) {
                LV_LOG_WARN("Subject compare failed. Expected: %s, Actual: %s", step->param.subject_compare.value, v);
                res = false;
            }
        }
        else {
            LV_LOG_WARN("Not supported subject type %d", type);
        }
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_SET_LANGUAGE) {
        const char * lang = step->param.str;
        lv_translation_set_language(lang);
    }

    return res;
}

static lv_obj_t * create_cursor(lv_obj_t * parent)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(obj, 30, 30);
    lv_obj_set_style_opa(obj, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_DEEP_ORANGE), LV_STATE_PRESSED);
    lv_obj_set_style_border_color(obj, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_translate_x(obj, lv_pct(-50), 0);
    lv_obj_set_style_translate_y(obj, lv_pct(-50), 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_transform_width(obj, -5, LV_STATE_PRESSED);
    lv_obj_set_style_transform_height(obj, -5, LV_STATE_PRESSED);

    return obj;
}

static void start_metadata_handler(void * user_data, const char * name, const char ** attrs)
{
    LV_UNUSED(user_data);

    if(lv_streq(name, "test")) {
        return;
    }

    if(lv_streq(name, "steps")) {
        test.processing_steps = 1;
        return;
    }

    /*Process the steps only*/
    if(test.processing_steps == 0) return;

    if(lv_streq(name, "click_at")) {
        test.step_cnt++;
        const char * x = lv_xml_get_value_of(attrs, "x");
        const char * y = lv_xml_get_value_of(attrs, "y");
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_CLICK_AT;
        test.steps[idx].param.mouse_pos.x = lv_xml_atoi(x);
        test.steps[idx].param.mouse_pos.y = lv_xml_atoi(y);
    }
    else if(lv_streq(name, "click_on")) {
        const char * obj_name = lv_xml_get_value_of(attrs, "name");
        if(obj_name == NULL) {
            LV_LOG_WARN("No name is set in test step");
            return;
        }

        test.step_cnt++;
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_CLICK_ON;
        test.steps[idx].param.str = lv_strdup(obj_name);
        LV_ASSERT_MALLOC(test.steps[idx].param.str);
    }
    else if(lv_streq(name, "move_to")) {
        test.step_cnt++;
        const char * x = lv_xml_get_value_of(attrs, "x");
        const char * y = lv_xml_get_value_of(attrs, "y");
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_MOVE_TO;
        test.steps[idx].param.mouse_pos.x = lv_xml_atoi(x);
        test.steps[idx].param.mouse_pos.y = lv_xml_atoi(y);
    }
    else if(lv_streq(name, "press")) {
        test.step_cnt++;
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_PRESS;
    }
    else if(lv_streq(name, "release")) {
        test.step_cnt++;
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_RELEASE;
    }
    else if(lv_streq(name, "screenshot_compare")) {
        test.step_cnt++;
        const char * path = lv_xml_get_value_of(attrs, "path");
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE;

        char buf[256];
        lv_snprintf(buf, sizeof(buf), "%s%s", test.ref_image_path_prefix, path);

        test.steps[idx].param.screenshot_compare.path = lv_strdup(buf);
    }
    else if(lv_streq(name, "wait")) {
        test.step_cnt++;
        const char * ms = lv_xml_get_value_of(attrs, "ms");
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_WAIT;
        test.steps[idx].param.wait.ms = lv_xml_atoi(ms);
    }
    else if(lv_streq(name, "freeze")) {
        test.step_cnt++;
        const char * ms = lv_xml_get_value_of(attrs, "ms");
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_FREEZE;
        test.steps[idx].param.freeze.ms = lv_xml_atoi(ms);
    }
    else if(lv_streq(name, "subject_set")) {
        const char * subject_str = lv_xml_get_value_of(attrs, "subject");
        const char * value_str = lv_xml_get_value_of(attrs, "value");
        if(subject_str == NULL) {
            LV_LOG_WARN("subject is not set in `%s`", name);
            return;
        }
        lv_subject_t * subject = lv_xml_get_subject(NULL, subject_str);
        if(subject == NULL) {
            LV_LOG_WARN("`%s` subject is not found in `%s`", subject_str, name);
            return;
        }

        test.step_cnt++;
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_SUBJECT_SET;
        test.steps[idx].param.subject_set.subject = subject;
        test.steps[idx].param.subject_set.value = lv_strdup(value_str);
    }
    else if(lv_streq(name, "subject_compare")) {
        const char * subject_str = lv_xml_get_value_of(attrs, "subject");
        const char * value_str = lv_xml_get_value_of(attrs, "value");
        if(subject_str == NULL) {
            LV_LOG_WARN("subject is not set in `%s`", name);
            return;
        }
        lv_subject_t * subject = lv_xml_get_subject(NULL, subject_str);
        if(subject == NULL) {
            LV_LOG_WARN("`%s` subject is not found in `%s`", subject_str, name);
            return;
        }

        test.step_cnt++;
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_SUBJECT_COMPARE;
        test.steps[idx].param.subject_compare.subject = subject;
        test.steps[idx].param.subject_compare.value = lv_strdup(value_str);
    }
    else if(lv_streq(name, "set_language")) {
        const char * obj_name = lv_xml_get_value_of(attrs, "name");
        if(obj_name == NULL) {
            LV_LOG_WARN("No name is set in test step");
            return;
        }

        test.step_cnt++;
        test.steps = lv_realloc(test.steps, sizeof(lv_xml_test_step_t) * test.step_cnt);
        uint32_t idx = test.step_cnt - 1;
        test.steps[idx].type = LV_XML_TEST_STEP_TYPE_SET_LANGUAGE;
        test.steps[idx].param.str = lv_strdup(obj_name);
        LV_ASSERT_MALLOC(test.steps[idx].param.str);
    }
}

static void end_metadata_handler(void * user_data, const char * name)
{
    LV_UNUSED(user_data);
    if(lv_streq(name, "steps")) {
        test.processing_steps = 0;
        return;
    }
}


static void click_at(int32_t x, int32_t y, int32_t slowdown)
{
    lv_obj_remove_state(cursor, LV_STATE_PRESSED);
    lv_test_mouse_release();
    lv_xml_test_wait(50, slowdown);
    lv_test_mouse_move_to(x, y);
    lv_test_mouse_press();
    lv_obj_set_pos(cursor, x, y);
    lv_obj_add_state(cursor, LV_STATE_PRESSED);
    lv_xml_test_wait(100, slowdown);
    lv_test_mouse_release();
    lv_xml_test_wait(50, slowdown);
    lv_obj_remove_state(cursor, LV_STATE_PRESSED);
    lv_refr_now(NULL);
}

#endif /* LV_USE_XML */
