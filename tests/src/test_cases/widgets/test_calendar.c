#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

/* This function runs before each test */
void setUp(void);
/* This function runs after every test */
void tearDown(void);

void test_calendar_creation_successful(void);
void test_calendar_set_today_date(void);
void test_calendar_set_today_date_gui(void);
void test_calendar_set_showed_date_gui(void);
void test_calendar_set_highlighted_dates(void);
void test_calendar_set_highlighted_dates_gui(void);
void test_calendar_set_day_names_gui(void);
void test_calendar_get_highlighted_dates_num(void);
void test_calendar_header_dropdown_create_gui(void);
void test_calendar_header_arrow_create_gui(void);
void test_calendar_event_key_down_gui(void);
void test_calendar_get_pressed_date_null(void);
void test_calendar_get_btnmatrix(void);

static lv_obj_t * g_active_screen = NULL;
static lv_obj_t * g_calendar = NULL;

void setUp(void)
{
    g_active_screen = lv_screen_active();
    g_calendar = lv_calendar_create(g_active_screen);
}

void tearDown(void)
{
    lv_obj_clean(g_active_screen);
}

void test_calendar_creation_successful(void)
{
    TEST_ASSERT_NOT_NULL(g_calendar);
}

void test_calendar_set_today_date(void)
{
    /* Work with 2022-09-21 as today (start of spring in Southern hemisphere) */
    lv_calendar_date_t today;
    today.year = 2022;
    today.month = 9;
    today.day = 21;

    lv_calendar_set_today_date(g_calendar, today.year, today.month, today.day);

    const lv_calendar_date_t * date_after_test = lv_calendar_get_today_date(g_calendar);

    TEST_ASSERT_EQUAL_INT16(today.year, date_after_test->year);
    TEST_ASSERT_EQUAL_INT16(today.month, date_after_test->month);
    TEST_ASSERT_EQUAL_INT16(today.day, date_after_test->day);
}

void test_calendar_set_today_date_gui(void)
{
    /* Work with 2022-09-21 as today (start of spring in Southern hemisphere) */
    lv_calendar_date_t today;
    today.year = 2022;
    today.month = 9;
    today.day = 21;

    lv_calendar_set_today_date(g_calendar, today.year, today.month, today.day);
    lv_calendar_set_showed_date(g_calendar, 2022, 9);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/calendar_01.png");
}

void test_calendar_set_showed_date_gui(void)
{
    lv_calendar_set_showed_date(g_calendar, 2022, 9);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/calendar_02.png");
}

void test_calendar_set_highlighted_dates(void)
{
    /*Highlight a few days*/
    static lv_calendar_date_t highlighted_days[3];       /*Only its pointer will be saved so should be static*/
    highlighted_days[0].year = 2022;
    highlighted_days[0].month = 2;
    highlighted_days[0].day = 6;

    highlighted_days[1].year = 2022;
    highlighted_days[1].month = 2;
    highlighted_days[1].day = 11;

    highlighted_days[2].year = 2022;
    highlighted_days[2].month = 2;
    highlighted_days[2].day = 22;

    lv_calendar_set_highlighted_dates(g_calendar, highlighted_days, 3);

    const lv_calendar_date_t * highlighted_days_after_test = lv_calendar_get_highlighted_dates(g_calendar);

    for(int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_INT16(highlighted_days[i].year, highlighted_days_after_test[i].year);
        TEST_ASSERT_EQUAL_INT16(highlighted_days[i].month, highlighted_days_after_test[i].month);
        TEST_ASSERT_EQUAL_INT16(highlighted_days[i].day, highlighted_days_after_test[i].day);
    }
}

void test_calendar_set_highlighted_dates_gui(void)
{
    /*Highlight a few days*/
    static lv_calendar_date_t highlighted_days[3];       /*Only its pointer will be saved so should be static*/
    highlighted_days[0].year = 2022;
    highlighted_days[0].month = 2;
    highlighted_days[0].day = 6;

    highlighted_days[1].year = 2022;
    highlighted_days[1].month = 2;
    highlighted_days[1].day = 11;

    highlighted_days[2].year = 2022;
    highlighted_days[2].month = 2;
    highlighted_days[2].day = 22;

    lv_calendar_set_highlighted_dates(g_calendar, highlighted_days, 3);

    lv_calendar_set_showed_date(g_calendar, 2022, 2);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/calendar_03.png");
}

void test_calendar_set_day_names_gui(void)
{
    static const char * day_names[7] = {"Do", "Lu", "Ma", "Mi", "Ju", "Vi", "Sa"};

    lv_calendar_set_day_names(g_calendar, day_names);

    lv_calendar_set_showed_date(g_calendar, 2022, 9);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/calendar_04.png");
}

void test_calendar_get_highlighted_dates_num(void)
{
    /*Highlight a few days*/
    static lv_calendar_date_t highlighted_days[3];       /*Only its pointer will be saved so should be static*/
    highlighted_days[0].year = 2022;
    highlighted_days[0].month = 2;
    highlighted_days[0].day = 6;

    highlighted_days[1].year = 2022;
    highlighted_days[1].month = 2;
    highlighted_days[1].day = 11;

    highlighted_days[2].year = 2022;
    highlighted_days[2].month = 2;
    highlighted_days[2].day = 22;

    lv_calendar_set_highlighted_dates(g_calendar, highlighted_days, 3);

    TEST_ASSERT_EQUAL_INT16(3, lv_calendar_get_highlighted_dates_num(g_calendar));
}

void test_calendar_header_dropdown_create_gui(void)
{
    lv_calendar_header_dropdown_create(g_calendar);

    lv_calendar_set_showed_date(g_calendar, 2022, 9);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/calendar_05.png");
}

void test_calendar_header_arrow_create_gui(void)
{
    lv_calendar_header_arrow_create(g_calendar);

    lv_calendar_set_showed_date(g_calendar, 2022, 10);    // Use October to avoid month name sliding

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/calendar_06.png");
}

void test_calendar_event_key_down_gui(void)
{
    uint32_t key = LV_KEY_DOWN;

    lv_calendar_set_showed_date(g_calendar, 2022, 9);

    lv_obj_send_event(g_calendar, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/calendar_07.png");
}

void test_calendar_get_pressed_date_null(void)
{
    lv_calendar_set_showed_date(g_calendar, 2022, 9);

    lv_calendar_date_t pressed_date;

    lv_result_t result = lv_calendar_get_pressed_date(g_calendar, &pressed_date);

    TEST_ASSERT_EQUAL(result, LV_RESULT_INVALID);
}

void test_calendar_get_btnmatrix(void)
{
    lv_obj_t * btnm = lv_calendar_get_btnmatrix(g_calendar);

    TEST_ASSERT_NOT_NULL(btnm);
}

void test_calendar_custom_year_list(void)
{
    lv_obj_t  * calendar = lv_calendar_create(lv_screen_active());

    lv_calendar_header_dropdown_create(calendar);

    const char * years = "2024\n2023\n2022\n2021\n2020\n2019";
    lv_calendar_header_dropdown_set_year_list(calendar, years);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/calendar_08.png");
}

#endif
