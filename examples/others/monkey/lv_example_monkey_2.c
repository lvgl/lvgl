#include "../../lv_examples.h"
#if LV_USE_MONKEY && LV_BUILD_EXAMPLES

/**
 * @title Encoder monkey input
 * @brief Feed random encoder steps into a new default group for focus testing.
 *
 * The config is set to `LV_INDEV_TYPE_ENCODER` with a 50 to 500 ms period and
 * input range of -5 to 5. A fresh `lv_group_t` is created, bound to the
 * monkey's indev with `lv_indev_set_group`, and installed as the default
 * group. `lv_monkey_set_enable` then starts the random encoder events.
 */
void lv_example_monkey_2(void)
{
    /*Create encoder monkey test*/
    lv_monkey_config_t config;
    lv_monkey_config_init(&config);
    config.type = LV_INDEV_TYPE_ENCODER;
    config.period_range.min = 50;
    config.period_range.max = 500;
    config.input_range.min = -5;
    config.input_range.max = 5;
    lv_monkey_t * monkey = lv_monkey_create(&config);

    /*Set the default group*/
    lv_group_t * group = lv_group_create();
    lv_indev_set_group(lv_monkey_get_indev(monkey), group);
    lv_group_set_default(group);

    /*Start monkey test*/
    lv_monkey_set_enable(monkey, true);
}

#endif
