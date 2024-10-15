/**
 * @file lv_circle_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_assert.h"

#include "lv_circle_buf.h"
#include "lv_array.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_circle_buf_t {
    lv_array_t array;
    uint32_t head;
    uint32_t tail;    /**< The next write position */

    bool inner_alloc; /**< true: the array is allocated by the buffer, false: the array is created from an external buffer */
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void circle_buf_prepare_empty(lv_circle_buf_t * circle_buf);

/**********************
 *  GLOBAL VARIABLES
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

lv_circle_buf_t * lv_circle_buf_create(const uint32_t capacity, const uint32_t element_size)
{
    lv_circle_buf_t * circle_buf = lv_malloc(sizeof(lv_circle_buf_t));
    LV_ASSERT_MALLOC(circle_buf);

    if(circle_buf == NULL) {
        return NULL;
    }

    lv_array_init(&circle_buf->array, capacity, element_size);
    circle_buf->head = 0;
    circle_buf->tail = 0;
    circle_buf->inner_alloc = true;

    circle_buf_prepare_empty(circle_buf);

    return circle_buf;
}

lv_circle_buf_t * lv_circle_buf_create_from_buf(void * buf, const uint32_t capacity, const uint32_t element_size)
{
    LV_ASSERT_NULL(buf);

    lv_circle_buf_t * circle_buf = lv_malloc(sizeof(lv_circle_buf_t));
    LV_ASSERT_MALLOC(circle_buf);

    if(circle_buf == NULL) {
        return NULL;
    }

    lv_array_init_from_buf(&circle_buf->array, buf, capacity, element_size);
    circle_buf->head = 0;
    circle_buf->tail = 0;
    circle_buf->inner_alloc = false;

    circle_buf_prepare_empty(circle_buf);

    return circle_buf;
}

lv_circle_buf_t * lv_circle_buf_create_from_array(const lv_array_t * array)
{
    LV_ASSERT_NULL(array);
    if(array == NULL) {
        return NULL;
    }

    lv_circle_buf_t * circle_buf = lv_malloc(sizeof(lv_circle_buf_t));
    LV_ASSERT_MALLOC(circle_buf);

    if(circle_buf == NULL) {
        return NULL;
    }

    circle_buf->array = *array;
    circle_buf->head = 0;
    circle_buf->tail = 0;
    circle_buf->inner_alloc = false;

    circle_buf_prepare_empty(circle_buf);

    return circle_buf;
}

lv_result_t lv_circle_buf_resize(lv_circle_buf_t * circle_buf, const uint32_t capacity)
{
    LV_ASSERT_NULL(circle_buf);

    if(lv_array_resize(&circle_buf->array, capacity) == false) {
        return LV_RESULT_INVALID;
    }

    circle_buf->head = 0;
    circle_buf->tail = 0;

    circle_buf_prepare_empty(circle_buf);

    return LV_RESULT_OK;
}

void lv_circle_buf_destroy(lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    lv_array_deinit(&circle_buf->array);

    lv_free(circle_buf);
}

uint32_t lv_circle_buf_size(const lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    return circle_buf->tail - circle_buf->head;
}

uint32_t lv_circle_buf_capacity(const lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    return lv_array_capacity(&circle_buf->array);
}

uint32_t lv_circle_buf_remain(const lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    return lv_circle_buf_capacity(circle_buf) - lv_circle_buf_size(circle_buf);
}

bool lv_circle_buf_is_empty(const lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    return !lv_circle_buf_size(circle_buf);
}

bool lv_circle_buf_is_full(const lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    return !lv_circle_buf_remain(circle_buf);
}

void lv_circle_buf_reset(lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    lv_array_clear(&circle_buf->array);
    circle_buf->head = 0;
    circle_buf->tail = 0;
}

void * lv_circle_buf_head(const lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    return lv_array_at(&circle_buf->array,
                       circle_buf->head % lv_circle_buf_capacity(circle_buf));
}

void * lv_circle_buf_tail(const lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    return lv_array_at(&circle_buf->array,
                       circle_buf->tail % lv_circle_buf_capacity(circle_buf));
}

lv_result_t lv_circle_buf_read(lv_circle_buf_t * circle_buf, void * data)
{
    LV_ASSERT_NULL(circle_buf);

    if(lv_circle_buf_is_empty(circle_buf)) {
        circle_buf->head = 0;
        circle_buf->tail = 0;
        return LV_RESULT_INVALID;
    }

    lv_circle_buf_peek_at(circle_buf, 0, data);
    circle_buf->head++;

    return LV_RESULT_OK;
}

lv_result_t lv_circle_buf_write(lv_circle_buf_t * circle_buf, const void * data)
{
    LV_ASSERT_NULL(circle_buf);

    if(lv_circle_buf_is_full(circle_buf)) {
        return LV_RESULT_INVALID;
    }

    lv_array_assign(&circle_buf->array, circle_buf->tail % lv_circle_buf_capacity(circle_buf), data);
    circle_buf->tail++;

    return LV_RESULT_OK;
}

uint32_t lv_circle_buf_fill(lv_circle_buf_t * circle_buf, uint32_t count, lv_circle_buf_fill_cb_t fill_cb,
                            void * user_data)
{
    LV_ASSERT_NULL(circle_buf);
    LV_ASSERT_NULL(fill_cb);

    uint32_t filled = 0;
    while(count > 0 && !lv_circle_buf_is_full(circle_buf)) {
        void * data = lv_circle_buf_tail(circle_buf);
        if(fill_cb(data, circle_buf->array.element_size, (int32_t)filled, user_data) == LV_RESULT_OK) {
            circle_buf->tail++;
            filled++;
        }
        else break;

        count--;
    }

    return filled;
}

lv_result_t lv_circle_buf_skip(lv_circle_buf_t * circle_buf)
{
    LV_ASSERT_NULL(circle_buf);

    if(lv_circle_buf_is_empty(circle_buf)) {
        circle_buf->head = 0;
        circle_buf->tail = 0;
        return LV_RESULT_INVALID;
    }

    circle_buf->head++;

    return LV_RESULT_OK;
}

lv_result_t lv_circle_buf_peek(const lv_circle_buf_t * circle_buf, void * data)
{
    LV_ASSERT_NULL(circle_buf);
    LV_ASSERT_NULL(data);

    return lv_circle_buf_peek_at(circle_buf, 0, data);
}

lv_result_t lv_circle_buf_peek_at(const lv_circle_buf_t * circle_buf, const uint32_t index, void * data)
{
    LV_ASSERT_NULL(circle_buf);
    LV_ASSERT_NULL(data);

    const uint32_t real_index = (index % lv_circle_buf_size(circle_buf) + circle_buf->head) % lv_circle_buf_capacity(
                                    circle_buf);
    lv_memcpy(data, lv_array_at(&circle_buf->array, real_index), circle_buf->array.element_size);

    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void circle_buf_prepare_empty(lv_circle_buf_t * circle_buf)
{
    const uint32_t required = lv_array_capacity(&circle_buf->array) - lv_array_size(&circle_buf->array);
    for(uint32_t i = 0; i < required; i++) lv_array_push_back(&circle_buf->array, NULL);
}
