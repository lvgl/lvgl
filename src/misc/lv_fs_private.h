/**
 * @file lv_fs_private.h
 *
 */

#ifndef LV_FS_PRIVATE_H
#define LV_FS_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_fs.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_fs_file_cache_t {
    uint32_t start;
    uint32_t end;
    uint32_t file_position;
    void * buffer;
};

/** Extended path object to specify buffer for memory-mapped files */
struct lv_fs_path_ex_t {
    char path[4];   /**<  This is needed to make it compatible with a normal path */
    const void * buffer;
    uint32_t size;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the File system interface
 */
void lv_fs_init(void);

/**
 * Deinitialize the File system interface
 */
void lv_fs_deinit(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FS_PRIVATE_H*/
