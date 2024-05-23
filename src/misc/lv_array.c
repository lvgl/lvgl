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

void lv_array_deinit(lv_array_t * array)
{
    if(array->data) {
        lv_free(array->data);
        array->data = NULL;
    }

    array->size = 0;
    array->capacity = 0;
}

void lv_array_copy(lv_array_t * target, const lv_array_t * source)
{
    if(lv_array_is_empty(source)) {
        return;
    }
    lv_array_deinit(target);
    lv_array_init(target, source->capacity, source->element_size);
    lv_memcpy(target->data, source->data, source->size * source->element_size);
    target->size = source->size;
}

lv_result_t lv_array_remove(lv_array_t * array, uint32_t index)
{
    if(index >= array->size) {
        return LV_RESULT_INVALID;
    }

    /*Shortcut*/
    if(index == array->size - 1) {
        lv_array_resize(array, array->size - 1);
        return LV_RESULT_OK;
    }

    uint8_t * start = lv_array_at(array, index);
    uint8_t * remaining = start + array->element_size;
    uint32_t remaining_size = (array->size - index - 1) * array->element_size;
    lv_memmove(start, remaining, remaining_size);
    lv_array_resize(array, array->size - 1);
    return LV_RESULT_OK;
}

lv_result_t lv_array_erase(lv_array_t * array, uint32_t start, uint32_t end)
{
    if(end > array->size) {
        end = array->size;
    }

    if(start >= end) {
        return LV_RESULT_INVALID;
    }

    /*Shortcut*/
    if(end == array->size) {
        lv_array_resize(array, start);
        return LV_RESULT_OK;
    }

    uint8_t * start_p = lv_array_at(array, start);
    uint8_t * remaining = start_p + (end - start) * array->element_size;
    uint32_t remaining_size = (array->size - end) * array->element_size;
    lv_memcpy(start_p, remaining, remaining_size);
    lv_array_resize(array, array->size - (end - start));
    return LV_RESULT_OK;
}

void lv_array_resize(lv_array_t * array, uint32_t new_capacity)
{
    uint8_t * data = lv_realloc(array->data, new_capacity * array->element_size);
    LV_ASSERT_NULL(data);
    array->data = data;
    array->capacity = new_capacity;
    if(array->size > new_capacity) {
        array->size = new_capacity;
    }
}

lv_result_t lv_array_concat(lv_array_t * array, const lv_array_t * other)
{
    LV_ASSERT_NULL(array->data);
    uint32_t size = other->size;
    if(array->size + size > array->capacity) {
        /*array is full*/
        lv_array_resize(array, array->size + size);
    }

    uint8_t * data = array->data + array->size * array->element_size;
    lv_memcpy(data, other->data, array->element_size * size);
    array->size += size;
    return LV_RESULT_OK;
}

lv_result_t lv_array_push_back(lv_array_t * array, const void * element)
{
    LV_ASSERT_NULL(array->data);

    if(array->size == array->capacity) {
        /*array is full*/
        lv_array_resize(array, array->capacity + 1);
    }

    uint8_t * data = array->data + array->size * array->element_size;
    lv_memcpy(data, element, array->element_size);
    array->size++;
    return LV_RESULT_OK;
}

void * lv_array_at(const lv_array_t * array, uint32_t index)
{
    if(index >= array->size) {
        return NULL;
    }

    LV_ASSERT_NULL(array->data);
    return array->data + index * array->element_size;
}

lv_result_t lv_array_assign(lv_array_t * array, uint32_t index, const void * value)
{
    uint8_t * data = lv_array_at(array, index);
    if(data == NULL) return LV_RESULT_INVALID;

    lv_memcpy(data, value, array->element_size);
    return LV_RESULT_OK;
}
