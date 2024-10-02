/**
 * @file lv_iter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_assert.h"

#include "lv_iter.h"

#include "lv_circle_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_iter_t {
    /* Iterator state */
    void  *  instance;        /**< Pointer to the object to iterate over */
    uint32_t elem_size;       /**< Size of one element in bytes */
    void  *  context;         /**< Custom context for the iteration */
    uint32_t context_size;    /**< Size of the custom context in bytes */

    /* Peeking */
    lv_circle_buf_t * peek_buf;   /**< Circular buffer for peeking */
    uint32_t peek_offset;         /**< Offset in the peek buffer */

    /* Callbacks */
    lv_iter_next_cb next_cb;  /**< Callback to get the next element */
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool peek_fill_cb(void * buf, uint32_t buf_len, int32_t index, void * user_data);

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

lv_iter_t * lv_iter_create(void * instance, const uint32_t elem_size, const uint32_t context_size,
                           lv_iter_next_cb next_cb)
{
    lv_iter_t * iter = lv_malloc_zeroed(sizeof(lv_iter_t));
    LV_ASSERT_MALLOC(iter);

    if(iter == NULL) {
        LV_LOG_ERROR("Could not allocate memory for iterator");
        return NULL;
    }

    iter->instance = instance;
    iter->elem_size = elem_size;
    iter->context_size = context_size;
    iter->next_cb = next_cb;

    if(context_size > 0) {
        iter->context = lv_malloc_zeroed(context_size);
        LV_ASSERT_MALLOC(iter->context);
    }

    return iter;
}

void * lv_iter_get_context(const lv_iter_t * iter)
{
    LV_ASSERT_NULL(iter);

    return iter ? iter->context : NULL;
}

void lv_iter_destroy(lv_iter_t * iter)
{
    LV_ASSERT_NULL(iter);
    if(iter == NULL) return;

    if(iter->context_size > 0) lv_free(iter->context);
    if(iter->peek_buf != NULL) lv_circle_buf_destroy(iter->peek_buf);

    iter->context = NULL;
    iter->peek_buf = NULL;

    lv_free(iter);
}

void lv_iter_make_peekable(lv_iter_t * iter, const uint32_t capacity)
{
    LV_ASSERT_NULL(iter);
    if(iter == NULL) return;

    if(capacity == 0 || iter->peek_buf != NULL) return;
    iter->peek_buf = lv_circle_buf_create(capacity, iter->elem_size);
    LV_ASSERT_NULL(iter->peek_buf);
}

lv_result_t lv_iter_next(lv_iter_t * iter, void * elem)
{
    LV_ASSERT_NULL(iter);
    if(iter == NULL) return LV_RESULT_INVALID;

    lv_circle_buf_t * c_buf = iter->peek_buf;
    if(c_buf != NULL && !lv_circle_buf_is_empty(c_buf)) {
        if(elem) lv_circle_buf_read(c_buf, elem);
        else lv_circle_buf_skip(c_buf);
        iter->peek_offset = 0;
        return LV_RESULT_OK;
    }

    const lv_result_t iter_res = iter->next_cb(iter->instance, iter->context, elem);
    if(iter_res == LV_RESULT_INVALID) return LV_RESULT_INVALID;

    if(c_buf != NULL) iter->peek_offset = 0;

    return iter_res;
}

lv_result_t lv_iter_peek(lv_iter_t * iter, void * elem)
{
    LV_ASSERT_NULL(iter);
    if(iter == NULL) return LV_RESULT_INVALID;

    lv_circle_buf_t * c_buf = iter->peek_buf;
    if(c_buf == NULL) return LV_RESULT_INVALID;

    const uint32_t peek_count = lv_circle_buf_size(c_buf);
    if(iter->peek_offset >= peek_count) {
        const uint32_t required = iter->peek_offset + 1 - peek_count;
        const uint32_t filled = lv_circle_buf_fill(c_buf, required, peek_fill_cb, iter);
        if(filled != required) return LV_RESULT_INVALID;
    }

    lv_circle_buf_peek_at(c_buf, iter->peek_offset, elem);

    return LV_RESULT_OK;
}

lv_result_t lv_iter_peek_advance(lv_iter_t * iter)
{
    LV_ASSERT_NULL(iter);
    if(iter == NULL) return LV_RESULT_INVALID;

    if(iter->peek_buf == NULL || iter->peek_offset + 1 >= lv_circle_buf_capacity(iter->peek_buf))
        return LV_RESULT_INVALID;
    iter->peek_offset++;
    return LV_RESULT_OK;
}

lv_result_t lv_iter_peek_reset(lv_iter_t * iter)
{
    LV_ASSERT_NULL(iter);
    if(iter == NULL) return LV_RESULT_INVALID;

    if(iter->peek_buf == NULL) return LV_RESULT_INVALID;

    iter->peek_offset = 0;
    return LV_RESULT_OK;
}

void lv_iter_inspect(lv_iter_t * iter, const lv_iter_inspect_cb inspect_cb)
{
    LV_ASSERT_NULL(iter);
    if(iter == NULL) return;

    void * elem = lv_malloc_zeroed(iter->elem_size);
    LV_ASSERT_MALLOC(elem);

    if(elem == NULL) {
        LV_LOG_ERROR("Could not allocate memory for element");
        return;
    }

    while(lv_iter_next(iter, elem) == LV_RESULT_OK) {
        inspect_cb(elem);
    }

    lv_free(elem);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool peek_fill_cb(void * buf, const uint32_t buf_len, const int32_t index, void * user_data)
{
    LV_UNUSED(buf_len);
    LV_UNUSED(index);

    const lv_iter_t * iter = user_data;
    const lv_result_t iter_res = iter->next_cb(iter->instance, iter->context, buf);
    if(iter_res == LV_RESULT_INVALID) return false;

    return true;
}
