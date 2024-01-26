/**
 * @file lv_profiler_builtin.h
 *
 */

#ifndef LV_PROFILER_BUILTIN_H
#define LV_PROFILER_BUILTIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_conf_internal.h"

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

#define LV_PROFILER_BUILTIN_BEGIN_TAG(tag)  lv_profiler_builtin_write((tag), 'B')
#define LV_PROFILER_BUILTIN_END_TAG(tag)    lv_profiler_builtin_write((tag), 'E')
#define LV_PROFILER_BUILTIN_BEGIN           LV_PROFILER_BUILTIN_BEGIN_TAG(__func__)
#define LV_PROFILER_BUILTIN_END             LV_PROFILER_BUILTIN_END_TAG(__func__)

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief LVGL profiler built-in configuration structure
 */
typedef struct {
    size_t buf_size;                    /**< The size of the buffer used for profiling data */
    uint32_t tick_per_sec;              /**< The number of ticks per second */
    uint32_t (*tick_get_cb)(void);      /**< Callback function to get the current tick count */
    void (*flush_cb)(const char * buf); /**< Callback function to flush the profiling data */
    int (*tid_get_cb)(void);            /**< Callback function to get the current thread ID */
    int (*cpu_get_cb)(void);            /**< Callback function to get the current CPU */
} lv_profiler_builtin_config_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Initialize the configuration of the built-in profiler
 * @param config Pointer to the configuration structure of the built-in profiler
 */
void lv_profiler_builtin_config_init(lv_profiler_builtin_config_t * config);

/**
 * @brief Initialize the built-in profiler with the given configuration
 * @param config Pointer to the configuration structure of the built-in profiler
 */
void lv_profiler_builtin_init(const lv_profiler_builtin_config_t * config);

/**
 * @brief Uninitialize the built-in profiler
 */
void lv_profiler_builtin_uninit(void);

/**
 * @brief Enable or disable the built-in profiler
 * @param enable true to enable the built-in profiler, false to disable
 */
void lv_profiler_builtin_set_enable(bool enable);

/**
 * @brief Flush the profiling data to the console
 */
void lv_profiler_builtin_flush(void);

/**
 * @brief Write the profiling data for a function with the given tag
 * @param func Name of the function being profiled
 * @param tag Tag to associate with the profiling data for the function
 */
void lv_profiler_builtin_write(const char * func, char tag);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_PROFILER_BUILTIN*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PROFILER_BUILTIN_H*/
