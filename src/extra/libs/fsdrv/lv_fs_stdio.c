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

/*********************
 *      DEFINES
 *********************/

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
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn);
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

    /*Add a simple drive to open images*/
    static lv_fs_drv_t fs_drv; /*A driver descriptor*/
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = LV_FS_STDIO_LETTER;
    fs_drv.cache_size = LV_FS_STDIO_CACHE_SIZE;

    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Open a file
 * @param drv pointer to a driver where this function belongs
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
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

    char buf[256];
    lv_snprintf(buf, sizeof(buf), LV_FS_STDIO_PATH "%s", path);

    return fopen(buf, flags);
}

/**
 * Close an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FILE variable. (opened with fs_open)
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
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FILE variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
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
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FILE variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btw Bytes To Write
 * @param bw the number of real written bytes (Bytes Written). NULL if unused.
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
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FILE variable. (opened with fs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    fseek(file_p, pos, whence);
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FILE variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    *pos_p = ftell(file_p);
    return LV_FS_RES_OK;
}

#ifdef WIN32
    static char next_fn[256];
#endif

/**
 * Initialize a 'DIR' or 'HANDLE' variable for directory reading
 * @param drv pointer to a driver where this function belongs
 * @param path path to a directory
 * @return pointer to an initialized 'DIR' or 'HANDLE' variable
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);
#ifndef WIN32
    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    lv_snprintf(buf, sizeof(buf), LV_FS_STDIO_PATH "%s", path);
    return opendir(buf);
#else
    HANDLE d = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA fdata;

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    lv_snprintf(buf, sizeof(buf), LV_FS_STDIO_PATH "%s\\*", path);

    strcpy(next_fn, "");
    d = FindFirstFile(buf, &fdata);
    do {
        if(strcmp(fdata.cFileName, ".") == 0 || strcmp(fdata.cFileName, "..") == 0) {
            continue;
        }
        else {
            if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                sprintf(next_fn, "/%s", fdata.cFileName);
            }
            else {
                sprintf(next_fn, "%s", fdata.cFileName);
            }
            break;
        }
    } while(FindNextFileA(d, &fdata));

    return d;
#endif
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'DIR' or 'HANDLE' variable
 * @param fn pointer to a buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn)
{
    LV_UNUSED(drv);

#ifndef WIN32
    struct dirent * entry;
    do {
        entry = readdir(dir_p);
        if(entry) {
            if(entry->d_type == DT_DIR) sprintf(fn, "/%s", entry->d_name);
            else strcpy(fn, entry->d_name);
        }
        else {
            strcpy(fn, "");
        }
    } while(strcmp(fn, "/.") == 0 || strcmp(fn, "/..") == 0);
#else
    strcpy(fn, next_fn);

    strcpy(next_fn, "");
    WIN32_FIND_DATA fdata;

    if(FindNextFile(dir_p, &fdata) == false) return LV_FS_RES_OK;
    do {
        if(strcmp(fdata.cFileName, ".") == 0 || strcmp(fdata.cFileName, "..") == 0) {
            continue;
        }
        else {
            if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                sprintf(next_fn, "/%s", fdata.cFileName);
            }
            else {
                sprintf(next_fn, "%s", fdata.cFileName);
            }
            break;
        }
    } while(FindNextFile(dir_p, &fdata));

#endif
    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'DIR' or 'HANDLE' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p)
{
    LV_UNUSED(drv);
#ifndef WIN32
    closedir(dir_p);
#else
    FindClose(dir_p);
#endif
    return LV_FS_RES_OK;
}

#else /*LV_USE_FS_STDIO == 0*/

#if defined(LV_FS_STDIO_LETTER) && LV_FS_STDIO_LETTER != '\0'
    #warning "LV_USE_FS_STDIO is not enabled but LV_FS_STDIO_LETTER is set"
#endif

#endif /*LV_USE_FS_POSIX*/

