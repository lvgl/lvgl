/**
 * @file lv_calendar_chinese.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_calendar_private.h"
#if LV_USE_CALENDAR && LV_USE_CALENDAR_CHINESE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char festival_name[20];
    uint8_t month;
    uint8_t day;
} lv_calendar_festival_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static const uint32_t calendar_chinese_table[199] = {/*1901-2099*/
    0x04AE53, 0x0A5748, 0x5526BD, 0x0D2650, 0x0D9544, 0x46AAB9, 0x056A4D, 0x09AD42, 0x24AEB6, 0x04AE4A, /*1901-1910*/
    0x6A4DBE, 0x0A4D52, 0x0D2546, 0x5D52BA, 0x0B544E, 0x0D6A43, 0x296D37, 0x095B4B, 0x749BC1, 0x049754, /*1911-1920*/
    0x0A4B48, 0x5B25BC, 0x06A550, 0x06D445, 0x4ADAB8, 0x02B64D, 0x095742, 0x2497B7, 0x04974A, 0x664B3E, /*1921-1930*/
    0x0D4A51, 0x0EA546, 0x56D4BA, 0x05AD4E, 0x02B644, 0x393738, 0x092E4B, 0x7C96BF, 0x0C9553, 0x0D4A48, /*1931-1940*/
    0x6DA53B, 0x0B554F, 0x056A45, 0x4AADB9, 0x025D4D, 0x092D42, 0x2C95B6, 0x0A954A, 0x7B4ABD, 0x06CA51, /*1941-1950*/
    0x0B5546, 0x555ABB, 0x04DA4E, 0x0A5B43, 0x352BB8, 0x052B4C, 0x8A953F, 0x0E9552, 0x06AA48, 0x6AD53C, /*1951-1960*/
    0x0AB54F, 0x04B645, 0x4A5739, 0x0A574D, 0x052642, 0x3E9335, 0x0D9549, 0x75AABE, 0x056A51, 0x096D46, /*1961-1970*/
    0x54AEBB, 0x04AD4F, 0x0A4D43, 0x4D26B7, 0x0D254B, 0x8D52BF, 0x0B5452, 0x0B6A47, 0x696D3C, 0x095B50, /*1971-1980*/
    0x049B45, 0x4A4BB9, 0x0A4B4D, 0xAB25C2, 0x06A554, 0x06D449, 0x6ADA3D, 0x0AB651, 0x093746, 0x5497BB, /*1981-1990*/
    0x04974F, 0x064B44, 0x36A537, 0x0EA54A, 0x86B2BF, 0x05AC53, 0x0AB647, 0x5936BC, 0x092E50, 0x0C9645, /*1991-2000*/
    0x4D4AB8, 0x0D4A4C, 0x0DA541, 0x25AAB6, 0x056A49, 0x7AADBD, 0x025D52, 0x092D47, 0x5C95BA, 0x0A954E, /*2001-2010*/
    0x0B4A43, 0x4B5537, 0x0AD54A, 0x955ABF, 0x04BA53, 0x0A5B48, 0x652BBC, 0x052B50, 0x0A9345, 0x474AB9, /*2011-2020*/
    0x06AA4C, 0x0AD541, 0x24DAB6, 0x04B64A, 0x69573D, 0x0A4E51, 0x0D2646, 0x5E933A, 0x0D534D, 0x05AA43, /*2021-2030*/
    0x36B537, 0x096D4B, 0xB4AEBF, 0x04AD53, 0x0A4D48, 0x6D25BC, 0x0D254F, 0x0D5244, 0x5DAA38, 0x0B5A4C, /*2031-2040*/
    0x056D41, 0x24ADB6, 0x049B4A, 0x7A4BBE, 0x0A4B51, 0x0AA546, 0x5B52BA, 0x06D24E, 0x0ADA42, 0x355B37, /*2041-2050*/
    0x09374B, 0x8497C1, 0x049753, 0x064B48, 0x66A53C, 0x0EA54F, 0x06B244, 0x4AB638, 0x0AAE4C, 0x092E42, /*2051-2060*/
    0x3C9735, 0x0C9649, 0x7D4ABD, 0x0D4A51, 0x0DA545, 0x55AABA, 0x056A4E, 0x0A6D43, 0x452EB7, 0x052D4B, /*2061-2070*/
    0x8A95BF, 0x0A9553, 0x0B4A47, 0x6B553B, 0x0AD54F, 0x055A45, 0x4A5D38, 0x0A5B4C, 0x052B42, 0x3A93B6, /*2071-2080*/
    0x069349, 0x7729BD, 0x06AA51, 0x0AD546, 0x54DABA, 0x04B64E, 0x0A5743, 0x452738, 0x0D264A, 0x8E933E, /*2081-2090*/
    0x0D5252, 0x0DAA47, 0x66B53B, 0x056D4F, 0x04AE45, 0x4A4EB9, 0x0A4D4C, 0x0D1541, 0x2D92B5  /*2091-2099*/
};

static const uint16_t month_total_day[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

static const char * chinese_calendar_month_name[] = {"正月", "二月", "三月", "四月", "五月", "六月",
                                                     "七月", "八月", "九月", "十月", "十一月", "腊月"
                                                    };

static const char * chinese_calendar_leep_month_name[] = {"闰正月", "闰二月", "闰三月", "闰四月", "闰五月", "闰六月",
                                                          "闰七月", "闰八月", "闰九月", "闰十月", "闰十一月", "闰腊月"
                                                         };

static const char * chinese_calendar_day_name[] = {"初一", "初二", "初三", "初四", "初五",
                                                   "初六", "初七", "初八", "初九", "初十",
                                                   "十一", "十二", "十三", "十四", "十五",
                                                   "十六", "十七", "十八", "十九", "二十",
                                                   "廿一", "廿二", "廿三", "廿四", "廿五",
                                                   "廿六", "廿七", "廿八", "廿九", "三十"
                                                  };

static const lv_calendar_festival_t festivals_base_chinese[] = {
    {"春节", 1, 1},
    {"元宵节", 1, 15},
    {"端午节", 5, 5},
    {"七夕节", 7, 7},
    {"中元节", 7, 15},
    {"中秋节", 8, 15},
    {"重阳节", 9, 9},
    {"腊八节", 12, 8},
    {"除夕", 12, 29},/* To determine whether it is 12.29 or 12.30. */
    {"除夕", 12, 30},/* To determine whether it is 12.29 or 12.30. */
};

static const lv_calendar_festival_t festivals_base_gregorian[] = {
    {"元旦", 1, 1},
    {"情人节", 2, 14},
    {"妇女节", 3, 8},
    {"植树节", 3, 12},
    {"消费节", 3, 15},
    {"愚人节", 4, 1},
    {"劳动节", 5, 1},
    {"青年节", 5, 4},
    {"儿童节", 6, 1},
    {"建党节", 7, 1},
    {"建军节", 8, 1},
    {"教师节", 9, 10},
    {"国庆节", 10, 1},
    {"万圣节", 10, 31},
    {"平安夜", 12, 24},
    {"圣诞节", 12, 25},
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_calendar_set_chinese_mode(lv_obj_t * obj, bool en)
{
    lv_calendar_t * calendar = (lv_calendar_t *)obj;
    calendar->use_chinese_calendar = en;
    lv_calendar_set_month_shown(obj, calendar->today.year, calendar->today.month);
}

const char * lv_calendar_get_day_name(lv_calendar_date_t * gregorian)
{
    uint16_t i, len;
    lv_calendar_chinese_t chinese_calendar;
    lv_calendar_gregorian_to_chinese(gregorian, &chinese_calendar);

    if(gregorian->year > 2099 || gregorian->year < 1901)
        return NULL;

    len = sizeof(festivals_base_chinese) / sizeof(lv_calendar_festival_t);
    for(i = 0; i < len; i++) {
        if((chinese_calendar.today.month == festivals_base_chinese[i].month) &&
           (chinese_calendar.today.day == festivals_base_chinese[i].day) &&
           chinese_calendar.leep_month == false) {
            if(chinese_calendar.today.month == 12 && chinese_calendar.today.day == 29) {
                if((calendar_chinese_table[chinese_calendar.today.year - 1901] & 0xf00000) != 0) {
                    if((calendar_chinese_table[chinese_calendar.today.year - 1901] & 0x000080) == 0) {
                        return festivals_base_chinese[i].festival_name;
                    }
                }
                else {
                    if((calendar_chinese_table[chinese_calendar.today.year - 1901] & 0x000100) == 0) {
                        return festivals_base_chinese[i].festival_name;
                    }
                }
            }
            else {
                return festivals_base_chinese[i].festival_name;
            }
        }
    }

    len = sizeof(festivals_base_gregorian) / sizeof(lv_calendar_festival_t);
    for(i = 0; i < len; i++) {
        if((gregorian->month == festivals_base_gregorian[i].month) &&
           (gregorian->day == festivals_base_gregorian[i].day))
            return festivals_base_gregorian[i].festival_name;
    }

    if(chinese_calendar.today.day == 1) {
        if(chinese_calendar.leep_month == false)
            return chinese_calendar_month_name[chinese_calendar.today.month - 1];
        else {
            return chinese_calendar_leep_month_name[chinese_calendar.today.month - 1];
        }
    }

    return (char *)chinese_calendar_day_name[chinese_calendar.today.day - 1];
}

void lv_calendar_gregorian_to_chinese(lv_calendar_date_t * gregorian_time, lv_calendar_chinese_t * chinese_time)
{
    uint16_t year = gregorian_time->year;
    uint8_t month = gregorian_time->month;
    uint8_t day = gregorian_time->day;

    /*Record the number of days between the Spring Festival
    and the New Year's Day of that year.*/
    uint16_t by_spring;

    /*Record the number of days from the gregorian calendar
    to the New Year's Day of that year.*/
    uint16_t by_gregorian;

    /*Record the number of days in that month*/
    uint8_t days_per_month;

    /*Record from which month the calculation starts.*/
    uint8_t index;

    bool leep_month;

    if(year < 1901 || year > 2099) {
        chinese_time->leep_month = 0;
        chinese_time->today.year = 2000;
        chinese_time->today.month = 1;
        chinese_time->today.day = 1;
        return;
    }

    if(((calendar_chinese_table[year - 1901] & 0x0060) >> 5) == 1)
        by_spring = (calendar_chinese_table[year - 1901] & 0x001F) - 1;
    else
        by_spring = (calendar_chinese_table[year - 1901] & 0x001F) - 1 + 31;

    by_gregorian = month_total_day[month - 1] + day - 1;

    if((!(year % 4)) && (month > 2))
        by_gregorian++;

    if(by_gregorian >= by_spring) {/*Gregorian calendar days after the Spring Festival*/
        by_gregorian -= by_spring;
        month = 1;
        index = 1;
        leep_month = false;

        if((calendar_chinese_table[year - 1901] & (0x80000 >> (index - 1))) == 0)
            days_per_month = 29;
        else
            days_per_month = 30;

        while(by_gregorian >= days_per_month) {
            by_gregorian -= days_per_month;
            index++;

            if(month == ((calendar_chinese_table[year - 1901] & 0xF00000) >> 20)) {
                leep_month = !leep_month;
                if(leep_month == false)
                    month++;
            }
            else
                month++;

            if((calendar_chinese_table[year - 1901] & (0x80000 >> (index - 1))) == 0)
                days_per_month = 29;
            else
                days_per_month = 30;
        }
        day = by_gregorian + 1;
    }
    else {/*Solar day before the Spring Festival*/
        by_spring -= by_gregorian;
        year--;
        month = 12;
        if(((calendar_chinese_table[year - 1901] & 0xF00000) >> 20) == 0)
            index = 12;
        else
            index = 13;
        leep_month = false;

        if((calendar_chinese_table[year - 1901] & (0x80000 >> (index - 1))) == 0)
            days_per_month = 29;
        else
            days_per_month = 30;

        while(by_spring > days_per_month) {
            by_spring -= days_per_month;
            index--;

            if(leep_month == false)
                month--;

            if(month == ((calendar_chinese_table[year - 1901] & 0xF00000) >> 20))
                leep_month = !leep_month;

            if((calendar_chinese_table[year - 1901] & (0x80000 >> (index - 1))) == 0)
                days_per_month = 29;
            else
                days_per_month = 30;
        }

        day = days_per_month - by_spring + 1;
    }
    chinese_time->today.day = day;
    chinese_time->today.month = month;
    chinese_time->today.year = year;
    chinese_time->leep_month = leep_month;
}

/**********************
 *  STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_CALENDAR_CHINESE*/
