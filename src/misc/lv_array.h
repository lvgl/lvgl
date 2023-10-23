/**
 * @file lv_array.h
 * Array. The elements are dynamically allocated by the 'lv_mem' module.
 */

#ifndef LV_ARRAY_H
#define LV_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
#define DEFAULT_CAPS 8

/**********************
 *      TYPEDEFS
 **********************/

/** Description of a array*/
typedef struct {
    uint8_t * data;
    uint32_t size;
    uint32_t capacity;
    uint32_t element_size;
} lv_array_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_array_init(lv_array_t * array, uint32_t capacity, uint32_t element_size);

void lv_array_copy(lv_array_t * target, const lv_array_t * array);

void lv_array_clear(lv_array_t * array);

void lv_array_resize(lv_array_t * array, uint32_t new_capacity);

void lv_array_destroy(lv_array_t * array);

bool lv_array_append(lv_array_t * array, const uint8_t * element);

uint8_t * lv_array_get(const lv_array_t * array, uint32_t index);

uint32_t lv_array_length(const lv_array_t * array);

uint32_t lv_array_capacity(const lv_array_t * array);

bool lv_array_is_empty(const lv_array_t * array);

bool lv_array_is_full(const lv_array_t * array);
/**********************
 *      MACROS
 **********************/

#define LV_ARRAY_INIT(array, type) lv_array_init((array), DEFAULT_CAPS, sizeof(type))

#define LV_ARRAY_INIT_CAPACITY(array, caps, type) lv_array_init((array), (caps), sizeof(type))

#define LV_ARRAY_APPEND_VALUE(array, element) lv_array_append((array), (uint8_t*)&(element))

#define LV_ARRAY_APPEND(array, element) lv_array_append((array), (uint8_t*)(element))

#define LV_ARRAY_GET(array, index, type) (type*)lv_array_get((array), (index))

#define LV_ARRAY_SET(array, index, data, type) \
    do { \
        type* elem = (type*)lv_array_get((array), (index)); \
        *elem = *((type*)(data)); \
    } while(0)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
