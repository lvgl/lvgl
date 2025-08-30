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

#include "lv_types.h"

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

/**
 * @brief Write the profiling data for a function with the given tag and custom time
 * @param func Name of the function being profiled
 * @param task Name of the task being profiled
 * @param tag Tag to associate with the profiling data for the function
 * @param tick The tick count
 * @param tid Thread ID of the function
 * @param cpu CPU ID of the function
 */
void lv_profiler_builtin_write_custom(const char * func, const char * task, char tag, uint64_t tick, int tid, int cpu);

/**
 * @brief Get the current tick count
 * @return The current tick count
 */
uint64_t lv_profiler_builtin_tick_get(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_PROFILER_BUILTIN*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PROFILER_BUILTIN_H*/
