/** @file */
#ifndef TEST_API_FIXTURE_H
#define TEST_API_FIXTURE_H

#include <stddef.h>
#include <stdint.h>
#include "lv_conf.h"

/** TEST primitive typedef documentation. */
typedef int test_number_t;
/** TEST qualified typedef documentation. */
typedef const test_number_t test_const_number_t;

/** TEST enum documentation. */
typedef enum test_flags {
    TEST_ZERO, /**< TEST zero member documentation. */
    TEST_EXPLICIT = 3, /**< TEST explicit member documentation. */
    TEST_SHIFT = (1u << 4), /**< TEST shift member documentation. */
    TEST_COMBINED = (TEST_EXPLICIT | TEST_SHIFT) /**< TEST combined member documentation. */
} test_flags_t;

/** TEST integer suffix and bitmask regression. */
typedef enum {
    TEST_FLAG_1 = (1u << 4),
    TEST_FLAG_2 = (1u << 8),
    TEST_FLAG_COMBINED = (TEST_FLAG_1 | TEST_FLAG_2)
} test_flag_t;

/** TEST struct documentation. */
typedef struct test_record {
    int count; /**< TEST count field documentation. */
    const char * text; /**< TEST text field documentation. */
    int samples[4]; /**< TEST array field documentation. */
    unsigned int bits : 3; /**< TEST bit field documentation. */
    test_number_t number; /**< TEST typedef field documentation. */
    test_flags_t flags; /**< TEST enum field documentation. */
} test_record_t;

/** TEST union documentation. */
typedef union test_value {
    int integer; /**< TEST integer union field documentation. */
    float real; /**< TEST real union field documentation. */
} test_value_t;

/** TEST incomplete struct documentation. */
typedef struct test_incomplete test_incomplete;

/** TEST callback documentation.
 * @param code TEST callback argument documentation.
 * @return TEST callback return documentation.
 */
typedef int (*test_callback_t)(int code);

/** TEST void function documentation. */
void test_reset(void);
/** TEST primitive function documentation.
 * @param number TEST number argument documentation.
 * @param flags TEST flags argument documentation.
 * @return TEST primitive return documentation.
 */
int test_compute(test_number_t number, test_flags_t flags);
/** TEST pointer function documentation.
 * @param record TEST record argument documentation.
 * @param text TEST text argument documentation.
 * @param cursor TEST cursor argument documentation.
 * @param callback TEST callback parameter documentation.
 * @param length TEST stdlib argument documentation.
 * @return TEST pointer return documentation.
 */
const char * test_read(test_record_t * record, const char * text,
                       int * const cursor, test_callback_t callback, size_t length);
/** TEST variadic function documentation.
 * @param format TEST format argument documentation.
 * @return TEST variadic return documentation.
 */
int test_format(const char * format, ...);
/** TEST typedef pointer function documentation.
 * @param number TEST typedef pointer argument documentation.
 */
void test_update(test_number_t * number);

/** TEST global variable documentation. */
extern const test_number_t test_global;

/** TEST other macro documentation. */
#define TEST_OTHER 7
/** TEST number macro documentation. */
#define TEST_NUMBER 123
/** TEST string macro documentation. */
#define TEST_STRING "hello  world"
/** TEST escaped string macro documentation. */
#define TEST_ESCAPED "quote: \" slash: \\ tab: \t"
/** TEST alias macro documentation. */
#define TEST_ALIAS TEST_OTHER
/** TEST expression macro documentation. */
#define TEST_EXPR (TEST_OTHER + 10)
/** TEST function reference macro documentation. */
#define TEST_FUNC_REF test_reset
/** TEST function-like macro documentation. */
#define TEST_FUNCTION_LIKE(x) ((x) + 1)
/** TEST multiple parameter macro documentation. */
#define TEST_MULTI(first, second) ((first) + (second))
/** TEST call macro documentation. */
#define TEST_CALL TEST_FUNCTION_LIKE(TEST_OTHER)
/** TEST zero parameter macro documentation. */
#define TEST_ZERO_ARGS() 42
/** TEST empty macro documentation. */
#define TEST_EMPTY

/**
 * TEST macro first paragraph.
 *
 * TEST macro second paragraph.
 */
#define TEST_MULTIPARAGRAPH 789
#define TEST_UNDOCUMENTED 456

#if TEST_FEATURE
    /** TEST conditional macro documentation. */
    #define TEST_CONDITIONAL_MACRO (TEST_OTHER + 20)
    /** TEST conditional declaration documentation. */
    void test_conditional(void);
#endif

#endif
