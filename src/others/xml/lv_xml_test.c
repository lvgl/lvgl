/**
 * @file lv_xml_test.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_test.h"
#if LV_USE_XML

#include "../../lvgl.h"
#include "lv_xml.h"
#include "lv_xml_utils.h"
#include "../../misc/lv_fs.h"
#include "../../libs/expat/expat.h"
#include "../../display/lv_display_private.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * create_cursor(void);
static bool execute_step(lv_xml_test_step_t * step, uint32_t step_delay, bool visalize);
static void start_metadata_handler(void * user_data, const char * name, const char ** attrs);
static void end_metadata_handler(void * user_data, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_display_t * normal_display;
static lv_display_t * test_display;
static lv_tick_get_cb_t tick_cb_original;
static lv_obj_t  * canvas;
static lv_obj_t  * cursor;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_xml_test_init(void)
{

}

lv_xml_test_t * lv_xml_test_register_from_data(const char * name, const char * xml_def)
{

    /*Register as a component first to allow creating the view of the test later*/
    lv_result_t res = lv_xml_component_register_from_data(name, xml_def);
    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("Couldn't register test `%s` as a component");
        return NULL;
    }

    lv_xml_test_t * test = lv_zalloc(sizeof(lv_xml_test_t));
    test->name = lv_strdup(name);

    /* Parse the XML to extract metadata */
    XML_Memory_Handling_Suite mem_handlers;
    mem_handlers.malloc_fcn = lv_malloc;
    mem_handlers.realloc_fcn = lv_realloc;
    mem_handlers.free_fcn = lv_free;
    XML_Parser parser = XML_ParserCreate_MM(NULL, &mem_handlers, NULL);
    XML_SetUserData(parser, test);
    XML_SetElementHandler(parser, start_metadata_handler, end_metadata_handler);

    if(XML_Parse(parser, xml_def, lv_strlen(xml_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_ERROR("XML parsing error: %s on line %lu",
                     XML_ErrorString(XML_GetErrorCode(parser)),
                     (unsigned long)XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        return LV_RESULT_INVALID;
    }

    XML_ParserFree(parser);

    return test;
}


lv_xml_test_t * lv_xml_test_register_from_file(const char * path)
{
    /* Extract component name from path */
    /* Create a copy of the filename to modify */
    char * filename = lv_strdup(lv_fs_get_last(path));
    const char * ext = lv_fs_get_ext(filename);
    filename[lv_strlen(filename) - lv_strlen(ext) - 1] = '\0'; /*Trim the extension*/

    lv_fs_res_t fs_res;
    lv_fs_file_t f;
    fs_res = lv_fs_open(&f, path, LV_FS_MODE_RD);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_WARN("Couldn't open %s", path);
        lv_free(filename);
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
        LV_LOG_WARN("Memory allocation failed for file %s (%d bytes)", path, file_size + 1);
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
    lv_xml_test_t * test = lv_xml_test_register_from_data(filename, xml_buf);

    /* Housekeeping */
    lv_free(xml_buf);
    lv_fs_close(&f);

    return test;
}

void lv_xml_test_play_init(lv_xml_test_t * test)
{
    normal_display = lv_display_get_default();
    test_display = lv_test_display_create(test->screen_width, test->screen_height);
    lv_obj_t * test_screen = lv_display_get_screen_active(test_display);
    lv_display_set_default(test_display);

    lv_draw_buf_t * draw_buf_rendered = test_display->buf_act;
    canvas = lv_canvas_create(lv_display_get_screen_active(normal_display));
    lv_canvas_set_draw_buf(canvas, draw_buf_rendered);

    tick_cb_original = lv_tick_get_cb();
    lv_tick_set_cb(NULL);

    lv_sysmon_hide_performance(NULL);
    lv_sysmon_hide_memory(NULL);

    lv_test_indev_create_all();

    cursor = create_cursor();

    lv_xml_create(test_screen, test->name, NULL);
    lv_refr_now(test_display);
    lv_obj_invalidate(canvas);
    lv_refr_now(normal_display);

    test->step_act = 0;
}

uint32_t lv_xml_test_play_all(lv_xml_test_t * test, uint32_t step_delay, bool visalize, bool stop_on_error)
{
    uint32_t failed_cnt = 0;
    lv_xml_test_play_init(test);
    uint32_t i;
    for(i = 0; i < test->step_cnt; i++) {
        test->steps[i].passed = execute_step(&test->steps[i], step_delay, visalize);
        if(!test->steps[i].passed) {
            failed_cnt++;
            if(stop_on_error) break;
        }

        if(visalize) {
            if(test->steps[i].type != LV_XML_TEST_STEP_TYPE_CLICK_AT) {
                lv_delay_ms(step_delay);
            }

            lv_obj_invalidate(canvas);
            lv_refr_now(normal_display);
        }
    }

    lv_xml_test_play_close(test);
    return failed_cnt;
}

void lv_xml_test_play_close(lv_xml_test_t * test)
{

    lv_obj_delete(cursor);
    lv_obj_delete(canvas);
    lv_tick_set_cb(tick_cb_original);
    lv_display_set_default(normal_display);
    lv_display_delete(test_display);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool execute_step(lv_xml_test_step_t * step, uint32_t step_delay, bool visalize)
{
    bool res = true;

    if(step->type == LV_XML_TEST_STEP_TYPE_CLICK_AT) {
        int32_t x = step->param.click.x;
        int32_t y = step->param.click.y;

        lv_test_mouse_click_at(x, y);

        if(visalize) {
            lv_obj_set_pos(cursor, x, y);
            lv_obj_remove_state(cursor, LV_STATE_PRESSED);
            lv_refr_now(normal_display);
            lv_delay_ms(step_delay / 3);

            lv_obj_add_state(cursor, LV_STATE_PRESSED);
            lv_refr_now(normal_display);
            lv_delay_ms(step_delay / 3);

            lv_obj_remove_state(cursor, LV_STATE_PRESSED);
            lv_refr_now(normal_display);
            lv_delay_ms(step_delay / 3);
        }
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE) {
        res = lv_test_screenshot_compare(step->param.screenshot_compare.path);
        if(!res) {
            LV_LOG_WARN("screenshot compare of `%s` failed", step->param.screenshot_compare.path);
            return res;
        }
    }
    else if(step->type == LV_XML_TEST_STEP_TYPE_WAIT) {
        lv_test_wait(step->param.wait.ms);
    }
    return res;
}

static lv_obj_t * create_cursor(void)
{
    lv_obj_t * obj = lv_obj_create(lv_display_get_layer_sys(normal_display));
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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

    lv_xml_test_t * test = (lv_xml_test_t *)user_data;

    if(lv_streq(name, "test")) {
        const char * w = lv_xml_get_value_of(attrs, "width");
        const char * h = lv_xml_get_value_of(attrs, "height");
        if(w == NULL) w = "320";
        if(h == NULL) h = "240";
        test->screen_width = lv_xml_atoi(w);
        test->screen_height = lv_xml_atoi(h);
        return;
    }

    if(lv_streq(name, "steps")) {
        test->processing_steps = 1;
        return;
    }

    /*Process the steps only*/
    if(test->processing_steps == 0) return;

    if(lv_streq(name, "click_at")) {
        test->step_cnt++;
        const char * x = lv_xml_get_value_of(attrs, "x");
        const char * y = lv_xml_get_value_of(attrs, "y");
        test->steps = lv_realloc(test->steps, sizeof(lv_xml_test_step_t) * test->step_cnt);
        uint32_t idx = test->step_cnt - 1;
        test->steps[idx].type = LV_XML_TEST_STEP_TYPE_CLICK_AT;
        test->steps[idx].param.click.x = lv_xml_atoi(x);
        test->steps[idx].param.click.y = lv_xml_atoi(y);

    }
    else if(lv_streq(name, "screenshot_compare")) {
        test->step_cnt++;
        const char * path = lv_xml_get_value_of(attrs, "path");
        test->steps = lv_realloc(test->steps, sizeof(lv_xml_test_step_t) * test->step_cnt);
        uint32_t idx = test->step_cnt - 1;
        test->steps[idx].type = LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE;
        test->steps[idx].param.screenshot_compare.path = lv_strdup(path);
    }
    else if(lv_streq(name, "wait")) {
        test->step_cnt++;
        const char * ms = lv_xml_get_value_of(attrs, "ms");
        test->steps = lv_realloc(test->steps, sizeof(lv_xml_test_step_t) * test->step_cnt);
        uint32_t idx = test->step_cnt - 1;
        test->steps[idx].type = LV_XML_TEST_STEP_TYPE_WAIT;
        test->steps[idx].param.wait.ms = lv_xml_atoi(ms);
    }
}

static void end_metadata_handler(void * user_data, const char * name)
{
    lv_xml_test_t * test = (lv_xml_test_t *)user_data;

    if(lv_streq(name, "steps")) {
        test->processing_steps = 0;
        return;
    }
}


#endif /* LV_USE_XML */
