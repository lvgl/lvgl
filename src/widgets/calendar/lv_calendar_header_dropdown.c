/**
 * @file lv_calendar_header_dropdown.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_class_private.h"
#include "../../core/lv_obj_private.h"
#include "lv_calendar_header_dropdown.h"
#if LV_USE_CALENDAR && LV_USE_CALENDAR_HEADER_DROPDOWN

#include "lv_calendar.h"
#include "../dropdown/lv_dropdown.h"
#include "../dropdown/lv_dropdown_private.h"
#include "../label/lv_label.h"
#include "../../core/lv_obj_scroll.h"
#include "../../layouts/flex/lv_flex.h"

/*********************
 *      DEFINES
 *********************/
#define LV_CALENDAR_HEADER_DROPDOWN_YEAR_WINDOW_SIZE 101
#define LV_CALENDAR_HEADER_DROPDOWN_YEAR_SHIFT_STEP 10
#define LV_CALENDAR_HEADER_DROPDOWN_YEAR_TEXT_LEN 8
#define LV_CALENDAR_HEADER_DROPDOWN_YEAR_BUF_SIZE \
    (LV_CALENDAR_HEADER_DROPDOWN_YEAR_WINDOW_SIZE * LV_CALENDAR_HEADER_DROPDOWN_YEAR_TEXT_LEN)
#define LV_CALENDAR_HEADER_DROPDOWN_SCROLL_THRESHOLD_ITEMS 2

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;
    int32_t auto_top_year;
    uint8_t custom_year_list;
    uint8_t rebuilding_list;
    char year_list_buf[LV_CALENDAR_HEADER_DROPDOWN_YEAR_BUF_SIZE];
} lv_calendar_header_dropdown_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void my_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void year_event_cb(lv_event_t * e);
static void year_dropdown_ready_event_cb(lv_event_t * e);
static void year_list_scroll_event_cb(lv_event_t * e);
static void month_event_cb(lv_event_t * e);
static void value_changed_event_cb(lv_event_t * e);
static int32_t rebuild_auto_year_window(lv_obj_t * header, int32_t top_year);
static void refresh_open_year_list(lv_obj_t * year_dropdown);
static void sync_auto_year_dropdown_state(lv_obj_t * header, bool preserve_scroll);
static void set_year_dropdown_selected(lv_obj_t * year_dropdown, uint32_t sel, bool preserve_scroll);
static bool parse_year_text(const char * text, int32_t * year);
static int32_t get_year_option_height(lv_obj_t * year_dropdown);
static int32_t clamp_auto_top_year(int32_t top_year);
static lv_obj_t * get_year_dropdown(lv_obj_t * header);
static lv_obj_t * get_month_dropdown(lv_obj_t * header);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_calendar_header_dropdown_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .constructor_cb = my_constructor,
    .instance_size = sizeof(lv_calendar_header_dropdown_t),
    .name = "lv_calendar_header_dropdown",
};

static const char * month_list = "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_calendar_add_header_dropdown(lv_obj_t * parent)
{
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_calendar_header_dropdown_class, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

void lv_calendar_header_dropdown_set_year_list(lv_obj_t * parent, const char * years_list)
{
    /* Search for the header dropdown */
    lv_obj_t * header = lv_obj_get_child_by_type(parent, 0, &lv_calendar_header_dropdown_class);
    if(NULL == header) {
        /* Header not found */
        return;
    }

    /* Search for the year dropdown
     * Index is 0 because in the header dropdown constructor the year dropdown (year_dd)
     * is the first created child of the header */
    const int32_t year_dropdown_index = 0;
    lv_obj_t * year_dropdown = lv_obj_get_child_by_type(header, year_dropdown_index, &lv_dropdown_class);
    if(NULL == year_dropdown) {
        /* year dropdown not found */
        return;
    }

    lv_calendar_header_dropdown_t * header_dropdown = (lv_calendar_header_dropdown_t *)header;
    header_dropdown->custom_year_list = 1;
    header_dropdown->rebuilding_list = 1;

    lv_dropdown_set_selected_highlight(year_dropdown, true);
    lv_dropdown_set_text(year_dropdown, NULL);
    lv_dropdown_set_options(year_dropdown, years_list);
    refresh_open_year_list(year_dropdown);
    header_dropdown->rebuilding_list = 0;

    lv_obj_send_event(header, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_invalidate(parent);
}

/**********************
 *  STATIC FUNCTIONS
 **********************/

static void my_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_TRACE_OBJ_CREATE("begin");

    LV_UNUSED(class_p);

    lv_calendar_header_dropdown_t * header = (lv_calendar_header_dropdown_t *)obj;
    lv_obj_t * calendar = lv_obj_get_parent(obj);
    const lv_calendar_date_t * showed_date = lv_calendar_get_showed_date(calendar);

    header->auto_top_year = 0;
    header->custom_year_list = 0;
    header->rebuilding_list = 0;
    header->year_list_buf[0] = '\0';

    lv_obj_move_to_index(obj, 0);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

    lv_obj_t * year_dd = lv_dropdown_create(obj);
    rebuild_auto_year_window(obj, showed_date->year);
    sync_auto_year_dropdown_state(obj, false);
    lv_obj_add_event_cb(year_dd, year_event_cb, LV_EVENT_VALUE_CHANGED, calendar);
    lv_obj_add_event_cb(year_dd, year_dropdown_ready_event_cb, LV_EVENT_READY, obj);
    lv_obj_add_event_cb(lv_dropdown_get_list(year_dd), year_list_scroll_event_cb, LV_EVENT_SCROLL, obj);
    lv_obj_set_flex_grow(year_dd, 1);

    lv_obj_t * month_dd = lv_dropdown_create(obj);
    lv_dropdown_set_options(month_dd, month_list);
    lv_obj_add_event_cb(month_dd, month_event_cb, LV_EVENT_VALUE_CHANGED, calendar);
    lv_obj_set_flex_grow(month_dd, 1);

    lv_obj_add_event_cb(obj, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /*Refresh the drop down*/
    lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
}

static void month_event_cb(lv_event_t * e)
{
    lv_obj_t * dropdown = lv_event_get_current_target(e);
    lv_obj_t * calendar = lv_event_get_user_data(e);

    uint32_t sel = lv_dropdown_get_selected(dropdown);

    const lv_calendar_date_t * d;
    d = lv_calendar_get_showed_date(calendar);
    lv_calendar_date_t newd = *d;
    newd.month = sel + 1;

    lv_calendar_set_month_shown(calendar, newd.year, newd.month);
}

static void year_event_cb(lv_event_t * e)
{
    lv_obj_t * dropdown = lv_event_get_current_target(e);
    lv_obj_t * calendar = lv_event_get_user_data(e);

    char year_buf[LV_CALENDAR_HEADER_DROPDOWN_YEAR_TEXT_LEN];
    lv_dropdown_get_selected_str(dropdown, year_buf, sizeof(year_buf));
    int32_t year;
    if(!parse_year_text(year_buf, &year) || year < 0 || year > UINT16_MAX) {
        return;
    }

    const lv_calendar_date_t * d;
    d = lv_calendar_get_showed_date(calendar);
    lv_calendar_date_t newd = *d;
    newd.year = (uint16_t)year;

    lv_calendar_set_month_shown(calendar, newd.year, newd.month);
}

static void year_dropdown_ready_event_cb(lv_event_t * e)
{
    lv_obj_t * header = lv_event_get_user_data(e);
    lv_calendar_header_dropdown_t * header_dropdown = (lv_calendar_header_dropdown_t *)header;
    if(header_dropdown->custom_year_list) {
        return;
    }

    lv_obj_t * calendar = lv_obj_get_parent(header);
    const lv_calendar_date_t * cur_date = lv_calendar_get_showed_date(calendar);

    header_dropdown->rebuilding_list = 1;
    rebuild_auto_year_window(header, cur_date->year);
    sync_auto_year_dropdown_state(header, false);
    header_dropdown->rebuilding_list = 0;
}

static void year_list_scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * list = lv_event_get_current_target(e);
    lv_obj_t * header = lv_event_get_user_data(e);
    lv_calendar_header_dropdown_t * header_dropdown = (lv_calendar_header_dropdown_t *)header;
    if(header_dropdown->custom_year_list || header_dropdown->rebuilding_list) {
        return;
    }

    lv_obj_t * year_dropdown = get_year_dropdown(header);
    if(year_dropdown == NULL || lv_dropdown_get_list(year_dropdown) != list) {
        return;
    }

    int32_t item_height = get_year_option_height(year_dropdown);
    if(item_height <= 0) {
        return;
    }

    const int32_t threshold = item_height * LV_CALENDAR_HEADER_DROPDOWN_SCROLL_THRESHOLD_ITEMS;
    const int32_t scroll_y = lv_obj_get_scroll_y(list);

    if(lv_obj_get_scroll_bottom(list) <= threshold) {
        int32_t old_top_year = header_dropdown->auto_top_year;
        header_dropdown->rebuilding_list = 1;
        int32_t new_top_year = rebuild_auto_year_window(header, old_top_year - LV_CALENDAR_HEADER_DROPDOWN_YEAR_SHIFT_STEP);
        int32_t actual_shift_years = old_top_year - new_top_year;
        sync_auto_year_dropdown_state(header, true);
        if(actual_shift_years > 0) {
            lv_obj_scroll_to_y(list, LV_MAX(0, scroll_y - actual_shift_years * item_height), LV_ANIM_OFF);
        }
        header_dropdown->rebuilding_list = 0;
    }
    else if(lv_obj_get_scroll_top(list) <= threshold) {
        int32_t old_top_year = header_dropdown->auto_top_year;
        header_dropdown->rebuilding_list = 1;
        int32_t new_top_year = rebuild_auto_year_window(header, old_top_year + LV_CALENDAR_HEADER_DROPDOWN_YEAR_SHIFT_STEP);
        int32_t actual_shift_years = new_top_year - old_top_year;
        sync_auto_year_dropdown_state(header, true);
        if(actual_shift_years > 0) {
            lv_obj_scroll_to_y(list, scroll_y + actual_shift_years * item_height, LV_ANIM_OFF);
        }
        header_dropdown->rebuilding_list = 0;
    }
}

static void value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * header = lv_event_get_current_target(e);
    lv_calendar_header_dropdown_t * header_dropdown = (lv_calendar_header_dropdown_t *)header;
    lv_obj_t * calendar = lv_obj_get_parent(header);
    const lv_calendar_date_t * cur_date = lv_calendar_get_showed_date(calendar);
    lv_obj_t * year_dd = get_year_dropdown(header);
    lv_obj_t * month_dd = get_month_dropdown(header);

    if(header_dropdown->custom_year_list) {
        char year_buf[LV_CALENDAR_HEADER_DROPDOWN_YEAR_TEXT_LEN];
        lv_dropdown_set_selected_highlight(year_dd, true);
        lv_dropdown_set_text(year_dd, NULL);
        lv_snprintf(year_buf, sizeof(year_buf), "%d", cur_date->year);
        int32_t year_idx = lv_dropdown_get_option_index(year_dd, year_buf);
        if(year_idx >= 0) {
            lv_dropdown_set_selected(year_dd, year_idx);
        }
    }
    else {
        header_dropdown->rebuilding_list = 1;
        rebuild_auto_year_window(header, cur_date->year);
        sync_auto_year_dropdown_state(header, false);
        header_dropdown->rebuilding_list = 0;
    }

    lv_dropdown_set_selected(month_dd, cur_date->month - 1);
}

static int32_t rebuild_auto_year_window(lv_obj_t * header, int32_t top_year)
{
    lv_calendar_header_dropdown_t * header_dropdown = (lv_calendar_header_dropdown_t *)header;
    lv_obj_t * year_dropdown = get_year_dropdown(header);
    if(year_dropdown == NULL) {
        return 0;
    }

    top_year = clamp_auto_top_year(top_year);
    header_dropdown->auto_top_year = top_year;

    size_t offset = 0;
    header_dropdown->year_list_buf[0] = '\0';

    for(int32_t i = 0; i < LV_CALENDAR_HEADER_DROPDOWN_YEAR_WINDOW_SIZE; i++) {
        int32_t year = top_year - i;
        int written = lv_snprintf(header_dropdown->year_list_buf + offset,
                                  sizeof(header_dropdown->year_list_buf) - offset,
                                  i == 0 ? "%d" : "\n%d",
                                  year);
        if(written < 0) {
            break;
        }

        offset += (size_t)written;
        if(offset >= sizeof(header_dropdown->year_list_buf)) {
            header_dropdown->year_list_buf[sizeof(header_dropdown->year_list_buf) - 1] = '\0';
            break;
        }
    }

    lv_dropdown_set_options_static(year_dropdown, header_dropdown->year_list_buf);
    refresh_open_year_list(year_dropdown);

    return top_year;
}

static void refresh_open_year_list(lv_obj_t * year_dropdown)
{
    if(!lv_dropdown_is_open(year_dropdown)) {
        return;
    }

    lv_obj_t * list = lv_dropdown_get_list(year_dropdown);
    if(list == NULL) {
        return;
    }

    lv_obj_t * label = lv_obj_get_child(list, 0);
    if(label == NULL) {
        return;
    }

    lv_label_set_text_static(label, lv_dropdown_get_options(year_dropdown));
    lv_obj_update_layout(label);
    lv_obj_update_layout(list);
    lv_obj_invalidate(list);
}

static void sync_auto_year_dropdown_state(lv_obj_t * header, bool preserve_scroll)
{
    lv_calendar_header_dropdown_t * header_dropdown = (lv_calendar_header_dropdown_t *)header;
    lv_obj_t * year_dropdown = get_year_dropdown(header);
    lv_obj_t * calendar = lv_obj_get_parent(header);
    const lv_calendar_date_t * cur_date = lv_calendar_get_showed_date(calendar);
    int32_t selected_idx = header_dropdown->auto_top_year - cur_date->year;

    if(selected_idx >= 0 && selected_idx < LV_CALENDAR_HEADER_DROPDOWN_YEAR_WINDOW_SIZE) {
        if(lv_dropdown_get_text(year_dropdown) != NULL) {
            lv_dropdown_set_text(year_dropdown, NULL);
        }
        lv_dropdown_set_selected_highlight(year_dropdown, true);
        set_year_dropdown_selected(year_dropdown, (uint32_t)selected_idx, preserve_scroll);
    }
    else {
        if(lv_dropdown_get_text(year_dropdown) != NULL) {
            lv_dropdown_set_text(year_dropdown, NULL);
        }
        lv_dropdown_set_selected_highlight(year_dropdown, false);
    }
}

static void set_year_dropdown_selected(lv_obj_t * year_dropdown, uint32_t sel, bool preserve_scroll)
{
    if(!preserve_scroll || !lv_dropdown_is_open(year_dropdown)) {
        lv_dropdown_set_selected(year_dropdown, sel);
        return;
    }

    lv_dropdown_t * dropdown = (lv_dropdown_t *)year_dropdown;
    if(dropdown->option_cnt == 0) {
        return;
    }

    if(sel >= dropdown->option_cnt) {
        sel = dropdown->option_cnt - 1;
    }

    dropdown->sel_opt_id = sel;
    dropdown->sel_opt_id_orig = sel;
    lv_obj_invalidate(year_dropdown);
    if(dropdown->list) {
        lv_obj_invalidate(dropdown->list);
    }
}

static bool parse_year_text(const char * text, int32_t * year)
{
    int32_t parsed_year = 0;
    bool negative = false;

    if(text == NULL) {
        return false;
    }

    if(text[0] == '-') {
        negative = true;
        text++;
    }

    if(*text == '\0') {
        return false;
    }

    while(*text >= '0' && *text <= '9') {
        parsed_year = parsed_year * 10 + (*text - '0');
        text++;
    }

    if(*text != '\0') {
        return false;
    }

    *year = negative ? -parsed_year : parsed_year;
    return true;
}

static int32_t get_year_option_height(lv_obj_t * year_dropdown)
{
    lv_obj_t * list = lv_dropdown_get_list(year_dropdown);
    if(list == NULL) {
        return 0;
    }

    lv_obj_t * label = lv_obj_get_child(list, 0);
    if(label == NULL) {
        return 0;
    }

    const lv_font_t * font = lv_obj_get_style_text_font(label, LV_PART_MAIN);
    return lv_font_get_line_height(font) + lv_obj_get_style_text_line_space(label, LV_PART_MAIN);
}

static int32_t clamp_auto_top_year(int32_t top_year)
{
    const int32_t min_top_year = LV_CALENDAR_HEADER_DROPDOWN_YEAR_WINDOW_SIZE - 1;
    const int32_t max_top_year = UINT16_MAX;

    if(top_year < min_top_year) {
        return min_top_year;
    }
    if(top_year > max_top_year) {
        return max_top_year;
    }

    return top_year;
}

static lv_obj_t * get_year_dropdown(lv_obj_t * header)
{
    return lv_obj_get_child(header, 0);
}

static lv_obj_t * get_month_dropdown(lv_obj_t * header)
{
    return lv_obj_get_child(header, 1);
}

#endif /*LV_USE_CALENDAR_HEADER_ARROW*/
