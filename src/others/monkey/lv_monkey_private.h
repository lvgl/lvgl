/**
 * @file lv_monkey_private.h
 *
 */

#ifndef LV_MONKEY_PRIVATE_H
#define LV_MONKEY_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_monkey.h"

#if LV_USE_MONKEY != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_monkey_config_t {
    /**< Input device type*/
    lv_indev_type_t type;

    /**< Monkey execution period*/
    struct {
        //! @cond Doxygen_Suppress
        uint32_t min;
        uint32_t max;
        //! @endcond
    } period_range;

    /**< The range of input value*/
    struct {
        int32_t min;
        int32_t max;
    } input_range;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_MONKEY != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MONKEY_PRIVATE_H*/
