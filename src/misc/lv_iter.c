/**
 * @file lv_iter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_iter.h"

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
    void  *  peek_buff;       /**< Buffer to store the next n element */
    uint32_t peek_capacity;   /**< Size of the peek buffer in count of elements */
    uint32_t peek_count;      /**< Number of elements currently in the peek buffer */
    uint32_t peek_offset;     /**< Offset in the peek buffer */
    uint32_t peek_iter_offset;/**< Offset in the peek buffer for the current iteration */

    /* Callbacks */
    lv_iter_next_cb next_cb;  /**< Callback to get the next element */
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#define PEEK_BUFF_OFFSET(iter, i) (void*)((uint8_t *)(iter)->peek_buff + (i) * (iter)->elem_size)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_iter_t * lv_iter_create(void * instance, uint32_t elem_size, uint32_t context_size, lv_iter_next_cb next_cb)
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

void * lv_iter_get_context(lv_iter_t * iter)
{
    return iter->context;
}

void lv_iter_destroy(lv_iter_t * iter)
{
    if(iter->context_size > 0) lv_free(iter->context);
    if(iter->peek_buff != NULL) lv_free(iter->peek_buff);

    lv_free(iter);
}

void lv_iter_make_peekable(lv_iter_t * iter, uint32_t capacity)
{

    if(capacity == 0 || iter->peek_buff != NULL) return;
    iter->peek_capacity = capacity;
    iter->peek_buff = lv_malloc_zeroed(iter->peek_capacity * iter->elem_size);
    LV_ASSERT_MALLOC(iter->peek_buff);
}

lv_result_t lv_iter_next(lv_iter_t * iter, void * elem)
{
    if(iter->peek_buff != NULL) {
        if(iter->peek_iter_offset >= iter->peek_capacity) return LV_RESULT_INVALID;
        if(iter->peek_iter_offset < iter->peek_count) {
            void * buff = PEEK_BUFF_OFFSET(iter, iter->peek_iter_offset);
            if(elem) lv_memcpy(elem, buff, iter->elem_size);
            lv_memmove(iter->peek_buff, PEEK_BUFF_OFFSET(iter, iter->peek_iter_offset + 1),
                       (iter->peek_count - iter->peek_iter_offset - 1) * iter->elem_size);
            iter->peek_iter_offset = 0;
            iter->peek_offset = 0;
            iter->peek_count--;
            return LV_RESULT_OK;
        }
    }

    lv_result_t iter_res = iter->next_cb(iter->instance, iter->context, elem);
    if(iter_res == LV_RESULT_INVALID) return LV_RESULT_INVALID;

    if(iter->peek_buff != NULL) {
        iter->peek_count = 0;
        iter->peek_offset = 0;
        iter->peek_iter_offset = 0;
    }

    return iter_res;
}

lv_result_t lv_iter_peek(lv_iter_t * iter, void * elem)
{
    if(iter->peek_buff == NULL || iter->peek_count > iter->peek_capacity) return LV_RESULT_INVALID;

    if(iter->peek_offset >= iter->peek_count) {
        uint32_t required = iter->peek_offset + 1 - iter->peek_count;
        while(required --> 0) {
            void * buff = PEEK_BUFF_OFFSET(iter, iter->peek_count);
            lv_result_t iter_res = iter->next_cb(iter->instance, iter->context, buff);
            if(iter_res == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            iter->peek_count++;
        }
    }

    void * buff = PEEK_BUFF_OFFSET(iter, iter->peek_offset);
    lv_memcpy(elem, buff, iter->elem_size);

    return LV_RESULT_OK;
}

lv_result_t lv_iter_peek_advance(lv_iter_t * iter)
{
    if(iter->peek_buff == NULL || iter->peek_offset + 1 >= iter->peek_capacity) return LV_RESULT_INVALID;
    iter->peek_offset++;
    return LV_RESULT_OK;
}

lv_result_t lv_iter_peek_reset(lv_iter_t * iter)
{
    if(iter->peek_buff == NULL) return LV_RESULT_INVALID;

    iter->peek_offset = 0;
    return LV_RESULT_OK;
}

void lv_iter_inspect(lv_iter_t * iter, lv_iter_inspect_cb inspect_cb)
{
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
