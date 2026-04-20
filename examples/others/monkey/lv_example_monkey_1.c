#include "../../lv_examples.h"
#if LV_USE_MONKEY && LV_BUILD_EXAMPLES

/**
 * @title Pointer monkey input
 * @brief Generate random pointer events with `LV_INDEV_TYPE_POINTER`.
 *
 * `lv_monkey_config_init` initializes a config which is then set to
 * `LV_INDEV_TYPE_POINTER` with a period between 10 ms and 100 ms.
 * `lv_monkey_create` registers the synthetic input device and
 * `lv_monkey_set_enable(monkey, true)` starts the stream of random clicks.
 */
void lv_example_monkey_1(void)
{
    /*Create pointer monkey test*/
    lv_monkey_config_t config;
    lv_monkey_config_init(&config);
    config.type = LV_INDEV_TYPE_POINTER;
    config.period_range.min = 10;
    config.period_range.max = 100;
    lv_monkey_t * monkey = lv_monkey_create(&config);

    /*Start monkey test*/
    lv_monkey_set_enable(monkey, true);
}

#endif
