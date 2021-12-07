/**
 * @file lv_monkey.h
 *
 */
#ifndef LV_MONKEY_H
#define LV_MONKEY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_MONKEY != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_monkey;
typedef struct _lv_monkey lv_monkey_t;

typedef struct {
    /**< Input device type*/
    lv_indev_type_t type;
    struct {
        uint32_t min;
        uint32_t max;
    } period_range;
    struct {
        int32_t min;
        int32_t max;
    } input_range;
} lv_monkey_config_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a monkey config with default values
 * @param config pointer to 'lv_monkey_config_t' variable to initialize
 */
void lv_monkey_config_init(lv_monkey_config_t * config);

/**
 * Create monkey for test
 * @param config pointer to 'lv_monkey_config_t' variable
 * @return pointer to the created monkey
 */
lv_monkey_t * lv_monkey_create(const lv_monkey_config_t * config);

/**
 * Enable monkey
 * @param monkey pointer to monkey
 * @param en set to true to enable
 */
void lv_monkey_set_enable(lv_monkey_t * monkey, bool en);

/**
 * Get whether monkey is enabled
 * @param monkey pointer to monkey
 * @return return true if monkey enabled
 */
bool lv_monkey_get_enable(lv_monkey_t * monkey);

/**
 * Get monkey input device
 * @param monkey pointer to monkey
 * @return pointer to the input device
 */
lv_indev_t * lv_monkey_get_indev(lv_monkey_t * monkey);

/**
 * Delete monkey
 * @param monkey pointer to monkey
 */
void lv_monkey_del(lv_monkey_t * monkey);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MONKEY*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MONKEY_H*/
