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
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_ARRAY_DEFAULT_CAPACITY
#define LV_ARRAY_DEFAULT_CAPACITY  4
#endif

#ifndef LV_ARRAY_DEFAULT_SHRINK_RATIO
#define LV_ARRAY_DEFAULT_SHRINK_RATIO 2
#endif

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

/**
 * Init an array.
 * @param array pointer to an `lv_array_t` variable to initialize
 * @param capacity the initial capacity of the array
 * @param element_size the size of an element in bytes
 */
void lv_array_init(lv_array_t * array, uint32_t capacity, uint32_t element_size);

/**
 * Resize the array to the given capacity.
 * @note if the new capacity is smaller than the current size, the array will be truncated.
 * @param array pointer to an `lv_array_t` variable
 * @param new_capacity the new capacity of the array
 */
void lv_array_resize(lv_array_t * array, uint32_t new_capacity);

/**
 * Deinit the array, and free the allocated memory
 * @param array pointer to an `lv_array_t` variable to deinitialize
 */
void lv_array_deinit(lv_array_t * array);

/**
 * Return how many elements are stored in the array.
 * @param array pointer to an `lv_array_t` variable
 * @return the number of elements stored in the array
 */
uint32_t lv_array_size(const lv_array_t * array);

/**
 * Return the capacity of the array, i.e. how many elements can be stored.
 * @param array pointer to an `lv_array_t` variable
 * @return the capacity of the array
 */
uint32_t lv_array_capacity(const lv_array_t * array);

/**
 * Return if the array is empty
 * @param array pointer to an `lv_array_t` variable
 * @return true: array is empty; false: array is not empty
 */
bool lv_array_is_empty(const lv_array_t * array);

/**
 * Return if the array is full
 * @param array pointer to an `lv_array_t` variable
 * @return true: array is full; false: array is not full
 */
bool lv_array_is_full(const lv_array_t * array);

/**
 * Copy an array to another.
 * @note this will create a new array with the same capacity and size as the source array.
 * @param target pointer to an `lv_array_t` variable to copy to
 * @param source pointer to an `lv_array_t` variable to copy from
 */
void lv_array_copy(lv_array_t * target, const lv_array_t * source);

/**
 * Remove all elements in array.
 * @param array pointer to an `lv_array_t` variable
 */
void lv_array_clear(lv_array_t * array);

/**
 * Shrink the memory capacity of array if necessary.
 * @param array pointer to an `lv_array_t` variable
 */
void lv_array_shrink(lv_array_t * array);

/**
 * Remove the element at the specified position in the array.
 * @param array pointer to an `lv_array_t` variable
 * @param index the index of the element to remove
 * @return LV_RESULT_OK: success, otherwise: error
 */
lv_result_t lv_array_remove(lv_array_t * array, uint32_t index);

/**
 * Remove from the array either a single element or a range of elements ([start, end)).
 * @note This effectively reduces the container size by the number of elements removed.
 * @note When start equals to end, the function has no effect.
 * @param array pointer to an `lv_array_t` variable
 * @param start the index of the first element to be removed
 * @param end the index of the first element that is not to be removed
 * @return LV_RESULT_OK: success, otherwise: error
 */
lv_result_t lv_array_erase(lv_array_t * array, uint32_t start, uint32_t end);

/**
 * Concatenate two arrays. Adds new elements to the end of the array.
 * @note The destination array is automatically expanded as necessary.
 * @param array pointer to an `lv_array_t` variable
 * @param other pointer to the array to concatenate
 * @return LV_RESULT_OK: success, otherwise: error
 */
lv_result_t lv_array_concat(lv_array_t * array, const lv_array_t * other);

/**
 * Push back element. Adds a new element to the end of the array.
 * If the array capacity is not enough for the new element, the array will be resized automatically.
 * @param array pointer to an `lv_array_t` variable
 * @param element pointer to the element to add
 * @return LV_RESULT_OK: success, otherwise: error
 */
lv_result_t lv_array_push_back(lv_array_t * array, const void * element);

/**
 * Assigns one content to the array, replacing its current content.
 * @param array pointer to an `lv_array_t` variable
 * @param index the index of the element to replace
 * @param value pointer to the elements to add
 * @return true: success; false: error
 */
lv_result_t lv_array_assign(lv_array_t * array, uint32_t index, const void * value);

/**
 * Returns a pointer to the element at position n in the array.
 * @param array pointer to an `lv_array_t` variable
 * @param index the index of the element to return
 * @return a pointer to the requested element, NULL if `index` is out of range
 */
void * lv_array_at(const lv_array_t * array, uint32_t index);

/**
 * Returns a pointer to the first element in the array.
 * @param array pointer to an `lv_array_t` variable
 * @return a pointer to the first element in the array
 */
void * lv_array_front(const lv_array_t * array);

/**
 * Returns a pointer to the last element in the array.
 * @param array pointer to an `lv_array_t` variable
 */
void * lv_array_back(const lv_array_t * array);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
