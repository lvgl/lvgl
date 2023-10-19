/**
 * @file lv_fs_littlefs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_FS_LITTLEFS
#include "lfs.h"

/*********************
 *      DEFINES
 *********************/

#if LV_FS_LITTLEFS_LETTER == '\0'
    #error "LV_FS_LITTLEFS_LETTER must be an upper case ASCII letter"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void fs_init(void);

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);

static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);

static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p);

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

void lv_fs_littlefs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    /*Add a simple drive to open images*/
    static lv_fs_drv_t fs_drv; /*A driver descriptor*/
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = LV_FS_LITTLEFS_LETTER;
    fs_drv.cache_size = LV_FS_LITTLEFS_CACHE_SIZE;

    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_read_cb = fs_dir_read;

    /*#if LV_USE_USER_DATA*/
    fs_drv.user_data = NULL;
    /*#endif*/

    lv_fs_drv_register(&fs_drv);
}

/**
 * Convenience function to attach registered driver to lfs_t structure by driver-label
 * @param label     the label assigned to the driver when it was registered
 * @param lfs_p     the pointer to the lfs_t structure initialized by external code/library
 * @return          pointer to a driver descriptor or NULL on error
 */
lv_fs_drv_t * lv_fs_littlefs_set_driver(char label, void * lfs_p)
{
    lv_fs_drv_t * drv_p = lv_fs_get_drv(label);
    if(drv_p != NULL) drv_p->user_data = (lfs_t *) lfs_p;
    return drv_p;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your Storage device and File system.*/
static void fs_init(void)
{
    /* Initialize the internal flash or SD-card and LittleFS itself.
     * Better to do it in your code to keep this library untouched for easy updating */
}

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          pointer to a file descriptor or NULL on error
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    lfs_t * lfs_p = drv->user_data;
    uint32_t flags = 0;

    flags = mode == LV_FS_MODE_RD ? LFS_O_RDONLY
            : mode == LV_FS_MODE_WR ? LFS_O_WRONLY
            : mode == (LV_FS_MODE_WR | LV_FS_MODE_RD) ? LFS_O_RDWR : 0;

    lfs_file_t * file_p = lv_mem_alloc(sizeof(lfs_file_t));
    if(file_p == NULL) return NULL;

    int result = lfs_file_open(lfs_p, file_p, path, flags);

    if(result != LFS_ERR_OK) {
        lv_mem_free(file_p);
        return NULL;
    }

    return file_p;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    lfs_t * lfs_p = drv->user_data;

    int result = lfs_file_close(lfs_p, file_p);
    lv_mem_free(file_p);
    /*lv_mem_free( lfs_p );*/ /*allocated and freed by outside-code*/

    if(result != LFS_ERR_OK) return LV_FS_RES_UNKNOWN;
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
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    lfs_t * lfs_p = drv->user_data;

    lfs_ssize_t result = lfs_file_read(lfs_p, file_p, buf, btr);
    if(result < 0) return LV_FS_RES_UNKNOWN;

    *br = (uint32_t) result;
    return LV_FS_RES_OK;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
#ifndef LFS_READONLY
    lfs_t * lfs_p = drv->user_data;

    lfs_ssize_t result = lfs_file_write(lfs_p, file_p, buf, btw);
    if(result < 0 || lfs_file_sync(lfs_p, file_p) < 0) return LV_FS_RES_UNKNOWN;

    *bw = (uint32_t) result;
    return LV_FS_RES_OK;
#else
    return LV_FS_RES_NOT_IMP;
#endif
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
    lfs_t * lfs_p = drv->user_data;

    int lfs_whence = whence == LV_FS_SEEK_SET ? LFS_SEEK_SET
                     : whence == LV_FS_SEEK_CUR ? LFS_SEEK_CUR
                     : whence == LV_FS_SEEK_END ? LFS_SEEK_END : 0;

    lfs_soff_t result = lfs_file_seek(lfs_p, file_p, pos, lfs_whence);
    if(result < 0) return LV_FS_RES_UNKNOWN;

    /*pos = result;*/ /*not supported by lv_fs*/
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param pos_p     pointer to where to store the result
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    lfs_t * lfs_p = drv->user_data;

    lfs_soff_t result = lfs_file_tell(lfs_p, file_p);
    if(result < 0) return LV_FS_RES_UNKNOWN;

    *pos_p = (uint32_t) result;
    return LV_FS_RES_OK;
}

/**
 * Initialize a 'lv_fs_dir_t' variable for directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to a directory
 * @return          pointer to the directory read descriptor or NULL on error
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    lfs_t * lfs_p = drv->user_data;

    lfs_dir_t * dir_p = lv_mem_alloc(sizeof(lfs_dir_t));
    if(dir_p == NULL) return NULL;

    int result = lfs_dir_open(lfs_p, dir_p, path);
    if(result != LFS_ERR_OK) {
        lv_mem_free(dir_p);
        return NULL;
    }

    return dir_p;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @param fn        pointer to a buffer to store the filename
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn)
{
    lfs_t * lfs_p = drv->user_data;
    struct lfs_info info;
    int result;

    info.name[0] = '\0';

    do {
        result = lfs_dir_read(lfs_p, rddir_p, &info);
        if(result > 0) {
            if(info.type == LFS_TYPE_DIR) {
                fn[0] = '/';
                strcpy(&fn[1], info.name);
            }
            else strcpy(fn, info.name);
        }
        else if(result == 0) fn[0] = '\0'; /*dir-scan ended*/
        else return LV_FS_RES_UNKNOWN;

    } while(!strcmp(fn, "/.") || !strcmp(fn, "/.."));

    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p)
{
    lfs_t * lfs_p = drv->user_data;

    int result = lfs_dir_close(lfs_p, rddir_p);
    lv_mem_free(rddir_p);

    if(result != LFS_ERR_OK) return LV_FS_RES_UNKNOWN;
    return LV_FS_RES_OK;
}

#else /*LV_USE_FS_LITTLEFS == 0*/

#if defined(LV_FS_LITTLEFS_LETTER) && LV_FS_LITTLEFS_LETTER != '\0'
    #warning "LV_USE_FS_LITTLEFS is not enabled but LV_FS_LITTLEFS_LETTER is set"
#endif

#endif /*LV_USE_FS_POSIX*/
