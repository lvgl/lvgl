/**
 * @file lv_fs_posix.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_FS_POSIX

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#if LV_FS_POSIX_LETTER == '\0'
    #error "LV_FS_POSIX_LETTER must be set to a valid value"
#else
    #if (LV_FS_POSIX_LETTER < 'A') || (LV_FS_POSIX_LETTER > 'Z')
        #if LV_FS_DEFAULT_DRIVE_LETTER != '\0' /*When using default drive letter, strict format (X:) is mandatory*/
            #error "LV_FS_POSIX_LETTER must be an upper case ASCII letter"
        #else /*Lean rules for backward compatibility*/
            #warning LV_FS_POSIX_LETTER should be an upper case ASCII letter. \
            Using a slash symbol as drive letter should be replaced with LV_FS_DEFAULT_DRIVE_LETTER mechanism
        #endif
    #endif
#endif

/** The reason for 'fd + 1' is because open() may return a legal fd with a value of 0,
  * preventing it from being judged as NULL when converted to a pointer type.
  */
#define FILEP2FD(file_p) ((lv_uintptr_t)file_p - 1)
#define FD2FILEP(fd) ((void *)(lv_uintptr_t)(fd + 1))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register a driver for the File system interface
 */
void lv_fs_posix_init(void)
{
    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    lv_fs_drv_t * fs_drv_p = &(LV_GLOBAL_DEFAULT()->posix_fs_drv);
    lv_fs_drv_init(fs_drv_p);

    /*Set up fields...*/
    fs_drv_p->letter = LV_FS_POSIX_LETTER;
    fs_drv_p->cache_size = LV_FS_POSIX_CACHE_SIZE;

    fs_drv_p->open_cb = fs_open;
    fs_drv_p->close_cb = fs_close;
    fs_drv_p->read_cb = fs_read;
    fs_drv_p->write_cb = fs_write;
    fs_drv_p->seek_cb = fs_seek;
    fs_drv_p->tell_cb = fs_tell;

    fs_drv_p->dir_close_cb = fs_dir_close;
    fs_drv_p->dir_open_cb = fs_dir_open;
    fs_drv_p->dir_read_cb = fs_dir_read;

    lv_fs_drv_register(fs_drv_p);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Open a file
 * @param drv   pointer to a driver where this function belongs
 * @param path  path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode  read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return a file handle or -1 in case of fail
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);

    int flags = 0;
    if(mode == LV_FS_MODE_WR) flags = O_WRONLY | O_CREAT;
    else if(mode == LV_FS_MODE_RD) flags = O_RDONLY;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = O_RDWR | O_CREAT;

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    lv_snprintf(buf, sizeof(buf), LV_FS_POSIX_PATH "%s", path);

    int fd = open(buf, flags, 0666);
    if(fd < 0) {
        LV_LOG_WARN("Could not open file: %s, flags: 0x%x, errno: %d", buf, flags, errno);
        return NULL;
    }

    return FD2FILEP(fd);
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    a file handle. (opened with fs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);

    int fd = FILEP2FD(file_p);
    int ret = close(fd);
    if(ret < 0) {
        LV_LOG_WARN("Could not close file: %d, errno: %d", fd, errno);
        return LV_FS_RES_FS_ERR;
    }

    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    a file handle variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);

    int fd = FILEP2FD(file_p);
    ssize_t ret = read(fd, buf, btr);
    if(ret < 0) {
        LV_LOG_WARN("Could not read file: %d, errno: %d", fd, errno);
        return LV_FS_RES_FS_ERR;
    }

    *br = (uint32_t)ret;
    return LV_FS_RES_OK;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    a file handle variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);

    int fd = FILEP2FD(file_p);
    ssize_t ret = write(fd, buf, btw);
    if(ret < 0) {
        LV_LOG_WARN("Could not write file: %d, errno: %d", fd, errno);
        return LV_FS_RES_FS_ERR;
    }

    *bw = (uint32_t)ret;
    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    a file handle variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    int w;
    switch(whence) {
        case LV_FS_SEEK_SET:
            w = SEEK_SET;
            break;
        case LV_FS_SEEK_CUR:
            w = SEEK_CUR;
            break;
        case LV_FS_SEEK_END:
            w = SEEK_END;
            break;
        default:
            return LV_FS_RES_INV_PARAM;
    }

    int fd = FILEP2FD(file_p);
    off_t offset = lseek(fd, pos, w);
    if(offset < 0) {
        LV_LOG_WARN("Could not seek file: %d, errno: %d", fd, errno);
        return LV_FS_RES_FS_ERR;
    }

    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    a file handle variable
 * @param pos_p     pointer to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);

    int fd = FILEP2FD(file_p);
    off_t offset = lseek(fd, 0, SEEK_CUR);
    if(offset < 0) {
        LV_LOG_WARN("Could not get position of file: %d, errno: %d", fd, errno);
        return LV_FS_RES_FS_ERR;
    }

    *pos_p = (uint32_t)offset;
    return LV_FS_RES_OK;
}

/**
 * Initialize a 'fs_read_dir_t' variable for directory reading
 * @param drv   pointer to a driver where this function belongs
 * @param path  path to a directory
 * @return pointer to an initialized 'DIR' or 'HANDLE' variable
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    lv_snprintf(buf, sizeof(buf), LV_FS_POSIX_PATH "%s", path);

    void * dir = opendir(buf);
    if(!dir) {
        LV_LOG_WARN("Could not open directory: %s, errno: %d", buf, errno);
        return NULL;
    }

    return dir;
}

/**
 * Read the next filename from a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param dir_p     pointer to an initialized 'DIR' or 'HANDLE' variable
 * @param fn        pointer to a buffer to store the filename
 * @param fn_len    length of the buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len)
{
    LV_UNUSED(drv);
    if(fn_len == 0) return LV_FS_RES_INV_PARAM;

    struct dirent * entry;
    do {
        entry = readdir(dir_p);
        if(entry) {
            if(entry->d_type == DT_DIR) lv_snprintf(fn, fn_len, "/%s", entry->d_name);
            else lv_strlcpy(fn, entry->d_name, fn_len);
        }
        else {
            lv_strlcpy(fn, "", fn_len);
        }
    } while(lv_strcmp(fn, "/.") == 0 || lv_strcmp(fn, "/..") == 0);

    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv   pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'DIR' or 'HANDLE' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p)
{
    LV_UNUSED(drv);

    int ret = closedir(dir_p);
    if(ret < 0) {
        LV_LOG_WARN("Could not close directory: errno: %d", errno);
        return LV_FS_RES_FS_ERR;
    }

    return LV_FS_RES_OK;
}
#else /*LV_USE_FS_POSIX == 0*/

#if defined(LV_FS_POSIX_LETTER) && LV_FS_POSIX_LETTER != '\0'
    #warning "LV_USE_FS_POSIX is not enabled but LV_FS_POSIX_LETTER is set"
#endif

#endif /*LV_USE_FS_POSIX*/
