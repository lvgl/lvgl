/**
 * @file lv_fs_stdio.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_FS_STDIO != '\0'

#include <stdio.h>
#ifndef WIN32
    #include <dirent.h>
    #include <unistd.h>
#else
    #include <windows.h>
#endif

#include "../../core/lv_global.h"
/*********************
 *      DEFINES
 *********************/
#if LV_FS_STDIO_LETTER == '\0'
    #error "LV_FS_STDIO_LETTER must be set to a valid value"
#else
    #if (LV_FS_STDIO_LETTER < 'A') || (LV_FS_STDIO_LETTER > 'Z')
        #if LV_FS_DEFAULT_DRIVE_LETTER != '\0' /*When using default drive letter, strict format (X:) is mandatory*/
            #error "LV_FS_STDIO_LETTER must be an upper case ASCII letter"
        #else /*Lean rules for backward compatibility*/
            #warning LV_FS_STDIO_LETTER should be an upper case ASCII letter. \
            Using a slash symbol as drive letter should be replaced with LV_FS_DEFAULT_DRIVE_LETTER mechanism
        #endif
    #endif
#endif

#define MAX_PATH_LEN 256

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
#ifdef _WIN32
    HANDLE dir_p;
    char next_fn[MAX_PATH_LEN];
#else
    DIR * dir_p;
#endif
} dir_handle_t;

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
void lv_fs_stdio_init(void)
{
    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    lv_fs_drv_t * fs_drv_p = &(LV_GLOBAL_DEFAULT()->stdio_fs_drv);
    lv_fs_drv_init(fs_drv_p);

    /*Set up fields...*/
    fs_drv_p->letter = LV_FS_STDIO_LETTER;
    fs_drv_p->cache_size = LV_FS_STDIO_CACHE_SIZE;

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
 * @return pointer to FIL struct or NULL in case of fail
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);

    const char * flags = "";

    if(mode == LV_FS_MODE_WR) flags = "wb";
    else if(mode == LV_FS_MODE_RD) flags = "rb";
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = "rb+";

    /*Make the path relative to the current directory (the projects root folder)*/

    char buf[MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_STDIO_PATH "%s", path);

    return fopen(buf, flags);
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FILE variable. (opened with fs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    fclose(file_p);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FILE variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    *br = fread(buf, 1, btr, file_p);
    return (int32_t)(*br) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FILE variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    *bw = fwrite(buf, 1, btw, file_p);
    return (int32_t)(*bw) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FILE variable. (opened with fs_open )
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

    fseek(file_p, pos, w);
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FILE variable
 * @param pos_p     pointer to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    *pos_p = ftell(file_p);
    return LV_FS_RES_OK;
}

/**
 * Initialize a 'DIR' or 'HANDLE' variable for directory reading
 * @param drv   pointer to a driver where this function belongs
 * @param path  path to a directory
 * @return pointer to an initialized 'DIR' or 'HANDLE' variable
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);
    dir_handle_t * handle = (dir_handle_t *)lv_malloc(sizeof(dir_handle_t));
#ifndef WIN32
    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_STDIO_PATH "%s", path);
    handle->dir_p = opendir(buf);
    if(handle->dir_p == NULL) {
        lv_free(handle);
        return NULL;
    }
    return handle;
#else
    handle->dir_p = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA fdata;

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_STDIO_PATH "%s\\*", path);

    lv_strcpy(handle->next_fn, "");
    handle->dir_p = FindFirstFileA(buf, &fdata);
    do {
        if(lv_strcmp(fdata.cFileName, ".") == 0 || lv_strcmp(fdata.cFileName, "..") == 0) {
            continue;
        }
        else {
            if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                lv_snprintf(handle->next_fn, sizeof(handle->next_fn), "/%s", fdata.cFileName);
            }
            else {
                lv_snprintf(handle->next_fn, sizeof(handle->next_fn), "%s", fdata.cFileName);
            }
            break;
        }
    } while(FindNextFileA(handle->dir_p, &fdata));

    if(handle->dir_p == INVALID_HANDLE_VALUE) {
        lv_free(handle);
        return INVALID_HANDLE_VALUE;
    }
    return handle;
#endif
}

/**
 * Read the next filename form a directory.
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

    dir_handle_t * handle = (dir_handle_t *)dir_p;
#ifndef WIN32
    struct dirent * entry;
    do {
        entry = readdir(handle->dir_p);
        if(entry) {
            /*Note, DT_DIR is not defined in C99*/
            if(entry->d_type == DT_DIR) lv_snprintf(fn, fn_len, "/%s", entry->d_name);
            else lv_strlcpy(fn, entry->d_name, fn_len);
        }
        else {
            lv_strlcpy(fn, "", fn_len);
        }
    } while(lv_strcmp(fn, "/.") == 0 || lv_strcmp(fn, "/..") == 0);
#else
    lv_strlcpy(fn, handle->next_fn, fn_len);

    lv_strcpy(handle->next_fn, "");
    WIN32_FIND_DATAA fdata;

    if(FindNextFileA(handle->dir_p, &fdata) == false) return LV_FS_RES_OK;
    do {
        if(lv_strcmp(fdata.cFileName, ".") == 0 || lv_strcmp(fdata.cFileName, "..") == 0) {
            continue;
        }
        else {
            if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                lv_snprintf(handle->next_fn, sizeof(handle->next_fn), "/%s", fdata.cFileName);
            }
            else {
                lv_snprintf(handle->next_fn, sizeof(handle->next_fn), "%s", fdata.cFileName);
            }
            break;
        }
    } while(FindNextFileA(handle->dir_p, &fdata));

#endif
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
    dir_handle_t * handle = (dir_handle_t *)dir_p;
#ifndef WIN32
    closedir(handle->dir_p);
#else
    FindClose(handle->dir_p);
#endif
    lv_free(handle);
    return LV_FS_RES_OK;
}

#else /*LV_USE_FS_STDIO == 0*/

#if defined(LV_FS_STDIO_LETTER) && LV_FS_STDIO_LETTER != '\0'
    #warning "LV_USE_FS_STDIO is not enabled but LV_FS_STDIO_LETTER is set"
#endif

#endif /*LV_USE_FS_POSIX*/
