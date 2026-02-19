/**
 * @file lv_fs_romfs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_FS_ROMFS

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_global.h"
#include "../romfs/romfs.h"
/*********************
 *      DEFINES
 *********************/

#if LV_FS_ROMFS_LETTER == '\0'
    #error "LV_FS_ROMFS_LETTER must be an upper case ASCII letter"
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct RomFile {
    romfs_file_t file;
} RomFile;
/**********************

 *  STATIC PROTOTYPES
 **********************/

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf,
                           uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos,
                           lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int lv_fs_romfs_init(void)
{
    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/
    lv_fs_drv_t * fs_drv = &(LV_GLOBAL_DEFAULT()->romfs_fs_drv);

    lv_fs_drv_init(fs_drv);

    rom_fs_t * rfs = romfs_filebackend_init(LV_FS_ROMFS_PATH, LV_FS_ROMFS_OFFSET);
    if(rfs == NULL) {
        return -1;
    }

    drv->user_data = rfs;

    /*Set up fields...*/
    fs_drv->letter = LV_FS_ROMFS_LETTER;
    fs_drv->open_cb = fs_open;
    fs_drv->close_cb = fs_close;
    fs_drv->read_cb = fs_read;
    fs_drv->seek_cb = fs_seek;
    fs_drv->tell_cb = fs_tell;

    lv_fs_drv_register(fs_drv);

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    /*Make the path relative to the current directory (the projects root folder)*/
    RomFile * rf = (RomFile *)lv_malloc(sizeof(RomFile));
    LV_ASSERT_NULL(rf);

    rom_fs_t * rfs = (rom_fs_t *)drv->user_data;

    int err = romfs_open(rfs, &rf->file, path);
    if(err < 0) {
        LV_LOG_ERROR("open path %s failed\n", path);
        return NULL;
    }

    return (void *)rf;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    RomFile * rf = (RomFile *)file_p;

    rom_fs_t * rfs = (rom_fs_t *)drv->user_data;
    romfs_close(rfs, &rf->file);
    lv_free(rf);

    return LV_FS_RES_OK;
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
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf,
                           uint32_t btr, uint32_t * br)
{
    RomFile * rf = (RomFile *)file_p;

    rom_fs_t * rfs = (rom_fs_t *)drv->user_data;
    *br = romfs_read(rfs, &rf->file, buf, btr);

    return (int32_t)(*br) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos,
                           lv_fs_whence_t whence)
{
    int w;

    if(whence == LV_FS_SEEK_SET)
        w = SEEK_SET;
    else if(whence == LV_FS_SEEK_CUR)
        w = SEEK_CUR;
    else if(whence == LV_FS_SEEK_END)
        w = SEEK_END;
    else
        return LV_FS_RES_INV_PARAM;

    RomFile * rf = (RomFile *)file_p;

    rom_fs_t * rfs = (rom_fs_t *)drv->user_data;
    off_t offset = romfs_lseek(rfs, &rf->file, pos, w);

    return offset < 0 ? LV_FS_RES_FS_ERR : LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param pos_p     pointer to store the result
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    RomFile * rf = (RomFile *)file_p;

    rom_fs_t * rfs = (rom_fs_t *)drv->user_data;
    off_t offset = romfs_tell(rfs, &rf->file);

    *pos_p = offset;

    return offset < 0 ? LV_FS_RES_FS_ERR : LV_FS_RES_OK;
}
#endif
