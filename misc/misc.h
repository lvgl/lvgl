/**
 * @file misc.h
 * 
 */

#ifndef MISC_H
#define MISC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/
#define MISC_VERSION_MAJOR  5
#define MISC_VERSION_MINOR  0
#define MISC_VERSION_PATCH  0
#define MISC_VERSION_INFO   "beta"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the enabled misc. modules
 */
void misc_init(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

