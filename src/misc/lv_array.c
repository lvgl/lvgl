/**
 * @file lv_array.c
 * Array.
 * The nodes are dynamically allocated by the 'lv_mem' module,
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_array.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"

#include "lv_assert.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_array_init(lv_array_t * array, uint32_t capacity, uint32_t element_size)
{
    array->size = 0;
    array->capacity = capacity;
    array->element_size = element_size;

    array->data = lv_malloc(capacity * element_size);
    LV_ASSERT_MALLOC(array->data);
}

void lv_array_destroy(lv_array_t * array)
{
    if(array->data) {
        lv_free(array->data);
        array->data = NULL;
    }
}

void lv_array_copy(lv_array_t * target, const lv_array_t * array)
{
    if(lv_array_is_empty(array)) {
        return;
    }
    lv_array_destroy(target);
    lv_array_init(target, array->capacity, array->element_size);
    lv_memcpy(target->data, array->data, array->size * array->element_size);
    target->size = array->size;
}

void lv_array_clear(lv_array_t * array)
{
    array->size = 0;
}

void lv_array_resize(lv_array_t * array, uint32_t new_capacity)
{
    if(new_capacity > array->size) {
        if(new_capacity > array->capacity) {
            uint8_t * data = lv_malloc(new_capacity * array->element_size);
            lv_memcpy(data, array->data, array->size * array->element_size);
            lv_free(array->data);
            array->data = data;
            array->capacity = new_capacity;
        }
    }
    else {
        array->size = new_capacity;
    }
}

bool lv_array_append(lv_array_t * array, const uint8_t * element)
{
    if(array->size >= array->capacity) {
        // array is full
        return false;
    }

    LV_ASSERT_NULL(array->data);
    uint8_t * data = (uint8_t *)(array->data + array->size * array->element_size);
    lv_memcpy(data, element, array->element_size);
    array->size++;
    return true;
}

uint8_t * lv_array_get(const lv_array_t * array, uint32_t index)
{
    if(index > (array->size - 1)) {
        return NULL;
    }

    LV_ASSERT_NULL(array->data);
    return array->data + index * array->element_size;
}

uint32_t lv_array_length(const lv_array_t * array)
{
    return array->size;
}

uint32_t lv_array_capacity(const lv_array_t * array)
{
    return array->capacity;
}

bool lv_array_is_full(const lv_array_t * array)
{
    return array->size == array->capacity;
}

bool lv_array_is_empty(const lv_array_t * array)
{
    return array->size == 0;
}
