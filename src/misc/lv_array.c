/**
 * @file lv_array.c
 * Array.
 * The nodes are dynamically allocated by the 'lv_mem' module,
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"
#include "lv_array.h"

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
lv_result_t lv_array_init(lv_array_t * array, uint32_t capacity, uint32_t element_size)
{
    LV_ASSERT(array != NULL);
    lv_memset(array, 0,  sizeof(*array));

    void * data = lv_malloc(capacity * element_size);
    LV_ASSERT_MALLOC(data);
    if(!data) {
        return LV_RESULT_INVALID;
    }
    lv_array_init_from_buf(array, data, capacity, element_size);
    array->inner_alloc = true;
    return LV_RESULT_OK;
}

void lv_array_init_from_buf(lv_array_t * array, void * buf, uint32_t capacity, uint32_t element_size)
{
    LV_ASSERT(array != NULL);
    LV_ASSERT(buf != NULL);
    lv_memset(array, 0,  sizeof(*array));
    array->size = 0;
    array->capacity = capacity;
    array->element_size = element_size;

    array->data = buf;
    array->inner_alloc = false;
}

void lv_array_deinit(lv_array_t * array)
{
    if(array->data) {
        if(array->inner_alloc) lv_free(array->data);
        array->data = NULL;
    }

    array->size = 0;
    array->capacity = 0;
}

void lv_array_copy(lv_array_t * target, const lv_array_t * source)
{
    LV_ASSERT_NULL(target);
    LV_ASSERT_NULL(source);

    if(target == source) return;

    lv_array_deinit(target);

    /* Guard against multiplication overflow during allocation */
    if(source->capacity > 0 && source->element_size > 0) {
        if(source->capacity > UINT32_MAX / source->element_size) {
            lv_memzero(target, sizeof(lv_array_t));
            return;
        }

        void * data = lv_malloc(source->capacity * source->element_size);
        if(data == NULL) {
            lv_memzero(target, sizeof(lv_array_t));
            return;
        }

        target->data = data;
        target->capacity = source->capacity;
        target->element_size = source->element_size;
        target->inner_alloc = true;
        target->size = 0;
    }
    else {
        void * data = lv_malloc(0);
        if(data == NULL) {
            lv_memzero(target, sizeof(lv_array_t));
            return;
        }
        target->data = data;
        target->capacity = source->capacity;
        target->element_size = source->element_size;
        target->inner_alloc = true;
        target->size = 0;
    }

    if(lv_array_is_empty(source)) {
        return;
    }

    lv_memcpy(target->data, source->data, source->size * source->element_size);
    target->size = source->size;
}

void lv_array_shrink(lv_array_t * array)
{
    if(array->size <= array->capacity / LV_ARRAY_DEFAULT_SHRINK_RATIO) {
        lv_array_resize(array, array->size);
    }
}

lv_result_t lv_array_remove(lv_array_t * array, uint32_t index)
{
    if(!array) {
        return LV_RESULT_INVALID;
    }

    if(index >= array->size) {
        return LV_RESULT_INVALID;
    }

    /*Shortcut*/
    if(index == array->size - 1) {
        array->size--;
        lv_array_shrink(array);
        return LV_RESULT_OK;
    }

    uint8_t * start = lv_array_at(array, index);
    uint8_t * remaining = start + array->element_size;
    uint32_t remaining_size = (array->size - index - 1) * array->element_size;
    lv_memmove(start, remaining, remaining_size);
    array->size--;
    lv_array_shrink(array);
    return LV_RESULT_OK;
}

lv_result_t lv_array_remove_unordered(lv_array_t * array, uint32_t index)
{
    if(!array) {
        return LV_RESULT_INVALID;
    }

    if(index >= array->size) {
        return LV_RESULT_INVALID;
    }

    /*Shortcut*/
    if(index == array->size - 1) {
        array->size--;
        lv_array_shrink(array);
        return LV_RESULT_OK;
    }

    /* Copy the last element into the position to remove*/
    uint8_t * dst = lv_array_at(array, index);
    uint8_t * src = lv_array_at(array, array->size - 1);

    lv_memcpy(dst, src, array->element_size);
    array->size--;
    lv_array_shrink(array);
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
        array->size = start;
        lv_array_shrink(array);
        return LV_RESULT_OK;
    }

    uint8_t * start_p = lv_array_at(array, start);
    uint8_t * remaining = start_p + (end - start) * array->element_size;
    uint32_t remaining_size = (array->size - end) * array->element_size;
    lv_memmove(start_p, remaining, remaining_size);
    array->size -= (end - start);
    lv_array_shrink(array);
    return LV_RESULT_OK;
}

bool lv_array_resize(lv_array_t * array, uint32_t new_capacity)
{
    if(array->inner_alloc == false) {
        LV_LOG_WARN("Cannot resize array with external buffer");
        return false;
    }

    uint8_t * data = lv_realloc(array->data, new_capacity * array->element_size);
    LV_ASSERT_NULL(data);

    if(data == NULL) return false;

    array->data = data;
    array->capacity = new_capacity;
    if(array->size > new_capacity) {
        array->size = new_capacity;
    }
    return true;
}

lv_result_t lv_array_concat(lv_array_t * array, const lv_array_t * other)
{
    LV_ASSERT_NULL(array->data);
    uint32_t size = other->size;
    if(array->size + size > array->capacity) {
        /*array is full*/
        if(lv_array_resize(array, array->size + size) == false) {
            return LV_RESULT_INVALID;
        }
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
        if(lv_array_resize(array, array->capacity + LV_ARRAY_DEFAULT_CAPACITY) == false) {
            return LV_RESULT_INVALID;
        }
    }

    /**
     * When the element is NULL, it means that the user wants to add an empty element.
     */
    uint8_t * data = array->data + array->size * array->element_size;
    if(element) lv_memcpy(data, element, array->element_size);
    else lv_memzero(data, array->element_size);

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

/**********************
 *   STATIC FUNCTIONS
 **********************/
