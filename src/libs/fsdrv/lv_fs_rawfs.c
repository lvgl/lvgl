/**
 * @file lv_fs_rawfs.c
 *
 */

/**
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_FS_RAWFS
#include <string.h>
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#if LV_FS_RAWFS_LETTER == '\0'
    #error "LV_FS_RAWFS_LETTER must be an upper case ASCII letter"
#endif

#if LV_FS_RAWFS_XIP
    #if LV_FS_RAWFS_XIP_BASE_ADDR == 0xFFFFFFFF
        #error "Base address for image binary (LV_FS_RAWFS_XIP_BASE_ADDR) must be valid"
    #endif
#endif

/**********************
 *      TYPEDEFS
 **********************/
/* Info for raw binaries: base, offset, size, name*/
extern const rawfs_size_t rawfs_file_count;
extern const rawfs_file_t rawfs_files[];

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_fs_res_t rawfs_file_find(const char * path, rawfs_file_t * file_p);

static void fs_init(void);

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static rawfs_file_t RFIL;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_rawfs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    lv_fs_drv_t * fs_drv_p = &(LV_GLOBAL_DEFAULT()->rawfs_fs_drv);
    lv_fs_drv_init(fs_drv_p);

    /*Set up fields...*/
    fs_drv_p->letter = LV_FS_RAWFS_LETTER;

    fs_drv_p->open_cb = fs_open;
    fs_drv_p->close_cb = fs_close;
    fs_drv_p->read_cb = fs_read;
    fs_drv_p->write_cb = NULL;
    fs_drv_p->seek_cb = fs_seek;
    fs_drv_p->tell_cb = fs_tell;

    fs_drv_p->dir_close_cb = NULL;
    fs_drv_p->dir_open_cb = NULL;
    fs_drv_p->dir_read_cb = NULL;

    lv_fs_drv_register(fs_drv_p);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Find file based on path
 * @param path      path to the file (e.g. /folder/file.txt)
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t rawfs_file_find(const char * path, rawfs_file_t * file_p)
{
    lv_fs_res_t res = LV_FS_RES_FS_ERR;
    /*Find file*/
    for(int i = 0; i < rawfs_file_count; i++) {
        if(0 == strcmp(path, rawfs_files[i].name)) {
            *file_p = rawfs_files[i];
            res = LV_FS_RES_OK;
            break;
        }
    }
    return res;
}

/*Initialize your Storage device and File system.*/
static void fs_init(void)
{
    RFIL.base = 0;
    RFIL.offset = 0;
}

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file (e.g. /folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    void * f = NULL;

    if(mode == LV_FS_MODE_RD) {
        /*Open a file for read*/
        res = rawfs_file_find(path, &RFIL);
    }
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) {
        /*Open a file for read and write*/
        res = rawfs_file_find(path, &RFIL);
    }

    if(res == LV_FS_RES_OK) {
        f = &RFIL;
    }

    return f;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    lv_fs_res_t res = LV_FS_RES_OK;

    fs_init();

    return res;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    lv_fs_res_t res = LV_FS_RES_OK;

    rawfs_file_t * p = (rawfs_file_t *)file_p;
#if LV_FS_RAWFS_XIP
    /*For XIP flash, copy directly*/
    lv_memcpy((uint8_t *)buf, (uint8_t *)(LV_FS_RAWFS_XIP_BASE_ADDR + p->base + p->offset), btr);
#else
    res = LV_FS_RES_NOT_IMP;
#endif
    p->offset += btr;
    *br = btr;

    return res;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    lv_fs_res_t res = LV_FS_RES_OK;

    rawfs_file_t * p = (rawfs_file_t *)file_p;
    if(whence == LV_FS_SEEK_SET) {
        p->offset = pos;
    }
    else if(whence == LV_FS_SEEK_CUR) {
        p->offset += pos;
    }
    else if(whence == LV_FS_SEEK_END) {
        p->offset = p->size + pos;
    }
    else {
        res = LV_FS_RES_NOT_IMP;
    }

    return res;
}

/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param pos_p     pointer to to store the result
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    lv_fs_res_t res = LV_FS_RES_OK;

    rawfs_file_t * p = (rawfs_file_t *)file_p;
    *pos_p = p->offset;

    return res;
}

#endif /*LV_USE_FS_RAWFS*/

