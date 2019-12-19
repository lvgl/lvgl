/**
 * @file lv_types.h
 *
 */

#ifndef LV_TYPES_H
#define LV_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/
// Check windows
#ifdef _WIN64
#define LV_ARCH_64
#endif

// Check GCC
#ifdef __GNUC__
#if defined(__x86_64__) || defined(__ppc64__)
#define LV_ARCH_64
#endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * LittlevGL error codes.
 */
enum {
    LV_RES_INV = 0, /*Typically indicates that the object is deleted (become invalid) in the action
                       function or an operation was failed*/
    LV_RES_OK,      /*The object is valid (no deleted) after the action*/
};
typedef uint8_t lv_res_t;

#ifdef LV_ARCH_64
typedef uint64_t lv_uintptr_t;
#else
typedef uint32_t lv_uintptr_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TYPES_H*/
