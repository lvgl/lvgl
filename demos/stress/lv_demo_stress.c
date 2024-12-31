/**
 * @file lv_demo_stress.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_stress.h"

#if LV_USE_DEMO_STRESS
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void auto_delete(lv_obj_t * obj, uint32_t delay);
static void msgbox_delete(lv_timer_t * tmr);
static void set_y_anim(void * obj, int32_t v);
static void set_width_anim(void * obj, int32_t v);
static void arc_set_end_angle_anim(void * obj, int32_t v);
static void obj_test_task_cb(lv_timer_t * tmr);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * main_page;
static lv_obj_t * ta;
static size_t mem_free_start = 0;
static int16_t g_state = -1;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_stress(void)
{
    LV_LOG_USER("Starting stress test. (< 100 bytes permanent memory leak is normal due to fragmentation)");
    lv_timer_t * t = lv_timer_create(obj_test_task_cb, LV_DEMO_STRESS_TIME_STEP, NULL);
    lv_timer_ready(t); /*Prepare the test right now in first state change.*/
}

bool lv_demo_stress_finished(void)
{
    return g_state == -1;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void obj_test_task_cb(lv_timer_t * tmr)
{
    (void) tmr;    /*Unused*/

    lv_anim_t a;
    lv_obj_t * obj;

    switch(g_state) {
        case -1: {
                lv_result_t res = lv_mem_test();
                if(res != LV_RESULT_OK) {
                    LV_LOG_ERROR("Memory integrity error");
                }

                lv_mem_monitor_t mon;
                lv_mem_monitor(&mon);

                if(mem_free_start == 0)  mem_free_start = mon.free_size;

                LV_LOG_USER("mem leak since start: %zu, frag: %3d %%", mem_free_start - mon.free_size, mon.frag_pct);
            }
            break;
        case 0:
            /* Holder for all object types */
            main_page = lv_obj_create(lv_screen_active());
            lv_obj_set_size(main_page, LV_HOR_RES / 2, LV_VER_RES);
            lv_obj_set_flex_flow(main_page, LV_FLEX_FLOW_COLUMN);

            obj = lv_button_create(main_page);
            lv_obj_set_size(obj, 100, 70);
            obj = lv_label_create(obj);
            lv_label_set_text(obj, "Multi line\n"LV_SYMBOL_OK LV_SYMBOL_CLOSE LV_SYMBOL_WIFI);
            break;

        case 1: {
                obj = lv_tabview_create(lv_screen_active());
                lv_tabview_set_tab_bar_size(obj, 50);
                lv_obj_set_size(obj, LV_HOR_RES / 2, LV_VER_RES / 2);
                lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
                lv_obj_t * t = lv_tabview_add_tab(obj, "First");

                t = lv_tabview_add_tab(obj, "Second");
                lv_obj_t * label = lv_label_create(t);
                lv_label_set_text(label, "Label on tabview");
                t = lv_tabview_add_tab(obj, LV_SYMBOL_EDIT " Edit");
                t = lv_tabview_add_tab(obj, LV_SYMBOL_CLOSE);

                lv_tabview_set_active(obj, 1, LV_ANIM_ON);
                auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 5 + 30);
            }
            break;

        case 2:
            obj = lv_button_create(main_page);
            lv_obj_set_size(obj, 200, 70);

            /*Move to disabled state very slowly*/
            lv_obj_add_state(obj, LV_STATE_DISABLED);

            /*Add an infinite width change animation*/
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_duration(&a, LV_DEMO_STRESS_TIME_STEP * 2);
            lv_anim_set_exec_cb(&a, set_width_anim);
            lv_anim_set_values(&a, 100, 200);
            lv_anim_set_reverse_duration(&a, LV_DEMO_STRESS_TIME_STEP * 2);
            lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
            lv_anim_start(&a);

            /*Delete the object a few sec later*/
            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 10);

            obj = lv_label_create(obj);
            lv_label_set_text_fmt(obj, "Formatted:\n%d %s", 12, "Volt");
            break;

        case 3:
            ta = lv_textarea_create(lv_screen_active());
            lv_obj_align_to(ta, main_page, LV_ALIGN_OUT_RIGHT_TOP, 10, 10);
            lv_obj_set_size(ta, LV_HOR_RES / 3, LV_VER_RES / 4);
            lv_textarea_set_placeholder_text(ta, "The placeholder");
            break;

        case 4:
            obj = lv_button_create(main_page);
            lv_obj_set_size(obj, 100, 70);
            lv_obj_set_style_bg_image_src(obj, LV_SYMBOL_DUMMY"Text from\nstyle", 0);
            lv_obj_delete_async(obj);  /*Delete on next call of `lv_timer_handler` (so not now)*/
            break;

        case 5:
            lv_textarea_set_one_line(ta, true);
            break;
        case 6:
            lv_obj_set_flex_flow(main_page, LV_FLEX_FLOW_COLUMN_WRAP);
            break;

        case 7:
            obj = lv_bar_create(main_page);
            lv_bar_set_range(obj, -1000, 2000);
            lv_bar_set_value(obj, 1800, LV_ANIM_ON);
            lv_bar_set_start_value(obj, -500, LV_ANIM_ON);

            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 2 + 70);

            obj = lv_slider_create(main_page);
            lv_obj_set_style_anim_duration(obj, LV_DEMO_STRESS_TIME_STEP * 8, 0);
            lv_slider_set_value(obj, 5000, LV_ANIM_ON);    /*Animate to out of range value*/
            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 5 + 22);

            obj = lv_switch_create(main_page);

            obj = lv_switch_create(main_page);
            lv_obj_add_state(obj, LV_STATE_CHECKED);
            auto_delete(obj, 730);

            break;

        case 8:
            obj = lv_win_create(lv_screen_active());
            lv_obj_set_size(obj, LV_HOR_RES / 2, LV_VER_RES / 2);
            lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
            lv_win_add_title(obj, "Window title");
            lv_win_add_button(obj, LV_SYMBOL_CLOSE, 40);
            lv_win_add_button(obj, LV_SYMBOL_DOWN, 40);
            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 3 + 5);

            obj = lv_calendar_create(lv_win_get_content(obj));
            break;
        case 9:
            lv_textarea_set_text(ta, "A very very long text which will should make the text area scrollable"
                                 "Here area some dummy sentences to be sure the text area will be really scrollable.");
            break;
        case 10:
            obj = lv_keyboard_create(lv_screen_active());
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_TEXT_UPPER);
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_values(&a, LV_VER_RES, LV_VER_RES - lv_obj_get_height(obj));
            lv_anim_set_duration(&a, LV_DEMO_STRESS_TIME_STEP + 3);
            lv_anim_set_exec_cb(&a, set_y_anim);
            lv_anim_start(&a);

            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 2 + 18);
            break;

        case 11:
            obj = lv_dropdown_create(main_page);
            lv_dropdown_set_options(obj, "Zero\nOne\nTwo\nThree\nFour\nFive\nSix\nSeven\nEight");
            lv_dropdown_open(obj);
            lv_dropdown_set_selected(obj, 2);
            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 3 + 11);
            break;

        case 12:
            obj = lv_roller_create(main_page);
            lv_roller_set_options(obj, "Zero\nOne\nTwo\nThree\nFour\nFive\nSix\nSeven\nEight", LV_ROLLER_MODE_INFINITE);
            lv_roller_set_selected(obj, 2, LV_ANIM_ON);
            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 20 + 22);
            break;

        case 13:
            obj = lv_arc_create(main_page);
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_values(&a, 180, 400);
            lv_anim_set_duration(&a, LV_DEMO_STRESS_TIME_STEP * 2);
            lv_anim_set_delay(&a, LV_DEMO_STRESS_TIME_STEP + 25);
            lv_anim_set_reverse_duration(&a, LV_DEMO_STRESS_TIME_STEP * 5);
            lv_anim_set_repeat_count(&a, 3);
            lv_anim_set_exec_cb(&a, arc_set_end_angle_anim);
            lv_anim_start(&a);

            obj = lv_scale_create(main_page);
            lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
            lv_obj_scroll_to_view(obj, LV_ANIM_ON);

            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 6 + 30);
            break;

        case 14:
            obj = lv_msgbox_create(NULL);
            lv_msgbox_add_title(obj, "Title");
            lv_msgbox_add_header_button(obj, LV_SYMBOL_AUDIO);
            lv_msgbox_add_text(obj, "Some text");
            lv_msgbox_add_footer_button(obj, "Button 1");
            lv_msgbox_add_footer_button(obj, "Button 2");
            {
                lv_timer_t * msgbox_tmr = lv_timer_create(msgbox_delete, LV_DEMO_STRESS_TIME_STEP * 5 + 30, obj);
                lv_timer_set_repeat_count(msgbox_tmr, 1);
                lv_obj_align(obj, LV_ALIGN_RIGHT_MID, -10, 0);
            }
            break;

        case 15:
            lv_textarea_set_one_line(ta, false);
            break;

        case 16: {
                lv_obj_t * tv = lv_tileview_create(lv_screen_active());
                lv_obj_set_size(tv, 200, 200);
                auto_delete(tv, LV_DEMO_STRESS_TIME_STEP * 4 + 5);

                obj = lv_tileview_add_tile(tv, 0, 0, LV_DIR_ALL);
                obj = lv_label_create(obj);
                lv_label_set_text(obj, "Tile: 0;0");

                obj = lv_tileview_add_tile(tv, 0, 1, LV_DIR_ALL);
                obj = lv_label_create(obj);
                lv_label_set_text(obj, "Tile: 0;1");

                obj = lv_tileview_add_tile(tv, 1, 1, LV_DIR_ALL);
                obj = lv_label_create(obj);
                lv_label_set_text(obj, "Tile: 1;1");

                lv_tileview_set_tile_by_index(tv, 1, 1, LV_ANIM_ON);
            }
            break;

        case 18:
            obj = lv_list_create(main_page);
            {
                lv_obj_t * b;
                b = lv_list_add_button(obj, LV_SYMBOL_OK, "1. Some very long text to scroll");
                auto_delete(b, 10);
                lv_list_add_button(obj, LV_SYMBOL_OK, "2. Some very long text to scroll");
                lv_list_add_button(obj, LV_SYMBOL_OK, "3. Some very long text to scroll");
                b = lv_list_add_button(obj, LV_SYMBOL_OK, "4. Some very long text to scroll");
                auto_delete(b, LV_DEMO_STRESS_TIME_STEP);
                b = lv_list_add_button(obj, LV_SYMBOL_OK, "5. Some very long text to scroll");
                auto_delete(b, LV_DEMO_STRESS_TIME_STEP + 90);
                b = lv_list_add_button(obj, LV_SYMBOL_OK, "6. Some very long text to scroll");
                auto_delete(b, LV_DEMO_STRESS_TIME_STEP + 10);
                lv_obj_scroll_to_view(lv_obj_get_child(obj, -1),  LV_ANIM_ON);
            }
            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 5 + 15);

            obj = lv_table_create(main_page);
            lv_table_set_cell_value(obj, 0, 0, "0,0");
            lv_table_set_cell_value_fmt(obj, 3, 0, "%d,%d", 5, 0);
            lv_table_set_row_count(obj, 5);
            lv_table_set_cell_value_fmt(obj, 1, 0, "%s", "1,0");
            lv_table_set_cell_value(obj, 1, 3, "1,3");
            break;

        case 19:
            lv_textarea_set_cursor_pos(ta, 10);
            lv_textarea_set_text(ta, "__INSERTED TEXT" LV_SYMBOL_EDIT"__");
            break;

        case 20:
            lv_obj_set_flex_flow(main_page, LV_FLEX_FLOW_ROW_WRAP);
            break;

        case 21:
            lv_textarea_set_cursor_pos(ta, 0);
            lv_textarea_add_char(ta, '0');
            lv_textarea_add_char(ta, '1');
            lv_textarea_add_char(ta, '2');
            lv_textarea_add_char(ta, '3');
            lv_textarea_add_char(ta, '4');
            lv_textarea_add_char(ta, '5');
            lv_textarea_add_char(ta, '6');
            lv_textarea_add_char(ta, '7');
            lv_textarea_add_char(ta, '8');
            lv_textarea_add_char(ta, '9');
            lv_textarea_add_char(ta, 'A');
            lv_textarea_add_char(ta, 'B');
            lv_textarea_add_char(ta, 'C');
            lv_textarea_add_char(ta, 'D');
            lv_textarea_add_char(ta, 'E');
            lv_textarea_add_char(ta, 'F');
            lv_textarea_add_text(ta, LV_SYMBOL_OK);
            lv_textarea_add_text(ta, LV_SYMBOL_CLOSE);
            lv_textarea_add_text(ta, LV_SYMBOL_COPY);
            lv_textarea_add_text(ta, LV_SYMBOL_SAVE);
            lv_textarea_add_text(ta, LV_SYMBOL_PASTE);
            break;

        case 22:
            obj = lv_spinbox_create(main_page);
            lv_spinbox_set_digit_format(obj, 6, 3);
            lv_spinbox_set_value(obj, 5678);
            lv_spinbox_set_step(obj, 10);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            lv_spinbox_set_step(obj, 100);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 2 + 15);

            lv_obj_scroll_by(main_page, 0, 20, LV_ANIM_ON);

            break;

        case 23:
            obj = lv_chart_create(main_page);
            {
                lv_chart_series_t * s1 = lv_chart_add_series(obj, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
                lv_chart_set_next_value(obj, s1, 36);
                lv_chart_set_next_value(obj, s1, -29);
                lv_chart_set_next_value(obj, s1, 51);
                lv_chart_set_next_value(obj, s1, 107);
                lv_chart_set_next_value(obj, s1, 70);
                lv_chart_set_next_value(obj, s1, 36);
                lv_chart_set_next_value(obj, s1, -2);
                lv_chart_set_next_value(obj, s1, 63);
                lv_chart_set_next_value(obj, s1, 48);
                lv_chart_set_next_value(obj, s1, 72);

                auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 3);
            }

            lv_obj_scroll_by(main_page, 0, 20, LV_ANIM_ON);
            break;

        case 24:
            obj = lv_checkbox_create(main_page);
            lv_checkbox_set_text(obj, "An option to select");
            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 2 + 20);

            obj = lv_checkbox_create(main_page);
            lv_obj_add_state(obj, LV_STATE_CHECKED);

            obj = lv_checkbox_create(main_page);
            lv_obj_add_state(obj, LV_STATE_CHECKED | LV_STATE_DISABLED);
            auto_delete(obj, LV_DEMO_STRESS_TIME_STEP * 1 + 60);

            lv_obj_scroll_by(main_page, 0, 20, LV_ANIM_ON);

            break;

        case 25:
            lv_textarea_set_cursor_pos(ta, 20);
            {
                uint16_t i;
                for(i = 0; i < 64; i++) {
                    lv_textarea_delete_char_forward(ta);
                }
            }
            break;

        case 26:
            lv_textarea_set_one_line(ta, true);
            break;
        case 29:
            lv_obj_clean(main_page);
            lv_obj_delete(ta);
            ta = NULL;
            break;
        case 31:
            lv_obj_clean(lv_screen_active());
            main_page = NULL;
            g_state = -2;
            break;
        default:
            break;
    }

    g_state++;
}

static void auto_delete(lv_obj_t * obj, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_duration(&a, 0);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);
    lv_anim_start(&a);
}

static void msgbox_delete(lv_timer_t * tmr)
{
    lv_msgbox_close(lv_timer_get_user_data(tmr));
}

static void set_y_anim(void * obj, int32_t v)
{
    lv_obj_set_y(obj, v);
}

static void set_width_anim(void * obj, int32_t v)
{
    lv_obj_set_width(obj, v);
}

static void arc_set_end_angle_anim(void * obj, int32_t v)
{
    lv_arc_set_end_angle(obj, v);
}

#endif /* LV_USE_DEMO_STRESS */
