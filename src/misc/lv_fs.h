/**
 * @file lv_fs.h
 *
 */

#ifndef LV_FS_H
#define LV_FS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stdint.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
#define LV_FS_MAX_FN_LENGTH 64
#define LV_FS_MAX_PATH_LENGTH 256

#define LV_FS_CACHE_FROM_BUFFER   UINT32_MAX

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Errors in the file system module.
 */
enum _lv_fs_res_t {
    LV_FS_RES_OK = 0,
    LV_FS_RES_HW_ERR,     /*Low level hardware error*/
    LV_FS_RES_FS_ERR,     /*Error in the file system structure*/
    LV_FS_RES_NOT_EX,     /*Driver, file or directory is not exists*/
    LV_FS_RES_FULL,       /*Disk full*/
    LV_FS_RES_LOCKED,     /*The file is already opened*/
    LV_FS_RES_DENIED,     /*Access denied. Check 'fs_open' modes and write protect*/
    LV_FS_RES_BUSY,       /*The file system now can't handle it, try later*/
    LV_FS_RES_TOUT,       /*Process time outed*/
    LV_FS_RES_NOT_IMP,    /*Requested function is not implemented*/
    LV_FS_RES_OUT_OF_MEM, /*Not enough memory for an internal operation*/
    LV_FS_RES_INV_PARAM,  /*Invalid parameter among arguments*/
    LV_FS_RES_UNKNOWN,    /*Other unknown error*/
};

#ifdef DOXYGEN
typedef _lv_fs_res_t lv_fs_res_t;
#else
typedef uint8_t lv_fs_res_t;
#endif /*DOXYGEN*/

/**
 * File open mode.
 */
enum _lv_fs_mode_t {
    LV_FS_MODE_WR = 0x01,
    LV_FS_MODE_RD = 0x02,
};

#ifdef DOXYGEN
typedef _lv_fs_mode_t lv_fs_mode_t;
#else
typedef uint8_t lv_fs_mode_t;
#endif /*DOXYGEN*/

/**
 * Seek modes.
 */
typedef enum {
    LV_FS_SEEK_SET = 0x00,      /**< Set the position from absolutely (from the start of file)*/
    LV_FS_SEEK_CUR = 0x01,      /**< Set the position from the current position*/
    LV_FS_SEEK_END = 0x02,      /**< Set the position from the end of the file*/
} lv_fs_whence_t;

struct _lv_fs_drv_t;
typedef struct _lv_fs_drv_t lv_fs_drv_t;
struct _lv_fs_drv_t {
    char letter;
    uint32_t cache_size;
    bool (*ready_cb)(lv_fs_drv_t * drv);

    void * (*open_cb)(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
    lv_fs_res_t (*close_cb)(lv_fs_drv_t * drv, void * file_p);
    lv_fs_res_t (*read_cb)(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
    lv_fs_res_t (*write_cb)(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
    lv_fs_res_t (*seek_cb)(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
    lv_fs_res_t (*tell_cb)(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

    void * (*dir_open_cb)(lv_fs_drv_t * drv, const char * path);
    lv_fs_res_t (*dir_read_cb)(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len);
    lv_fs_res_t (*dir_close_cb)(lv_fs_drv_t * drv, void * rddir_p);

    void * user_data; /**< Custom file user data*/
};

typedef struct {
    uint32_t start;
    uint32_t end;
    uint32_t file_position;
    void * buffer;
} lv_fs_file_cache_t;

typedef struct {
    void * file_d;
    lv_fs_drv_t * drv;
    lv_fs_file_cache_t * cache;
} lv_fs_file_t;

/* Extended path object to specify the buffer for memory-mapped files */
typedef struct {
    char path[4];   /* This is needed to make it compatible with a normal path */
    const void * buffer;
    uint32_t size;
} lv_fs_path_ex_t;

typedef struct {
    void * dir_d;
    lv_fs_drv_t * drv;
} lv_fs_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the File system interface
 */
void _lv_fs_init(void);

/**
 * Deinitialize the File system interface
 */
void _lv_fs_deinit(void);

/**
 * Initialize a file system driver with default values.
 * It is used to ensure all fields have known values and not memory junk.
 * After it you can set the fields.
 * @param drv     pointer to driver variable to initialize
 */
void lv_fs_drv_init(lv_fs_drv_t * drv);

/**
 * Add a new drive
 * @param drv       pointer to an lv_fs_drv_t structure which is inited with the
 *                  corresponding function pointers. Only pointer is saved, so the
 *                  driver should be static or dynamically allocated.
 */
void lv_fs_drv_register(lv_fs_drv_t * drv);

/**
 * Give a pointer to a driver from its letter
 * @param letter    the driver letter
 * @return          pointer to a driver or NULL if not found
 */
lv_fs_drv_t * lv_fs_get_drv(char letter);

/**
 * Test if a drive is ready or not. If the `ready` function was not initialized `true` will be
 * returned.
 * @param letter    letter of the drive
 * @return          true: drive is ready; false: drive is not ready
 */
bool lv_fs_is_ready(char letter);

/**
 * Open a file
 * @param file_p    pointer to a lv_fs_file_t variable
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_open(lv_fs_file_t * file_p, const char * path, lv_fs_mode_t mode);

/**
 * Make a path object for the memory-mapped file compatible with the file system interface
 * @param path      path to a lv_fs_path_ex object
 * @param letter    the letter of the driver. E.g. `LV_FS_MEMFS_LETTER`
 * @param buf       address of the memory buffer
 * @param size      size of the memory buffer in bytes
 */
void lv_fs_make_path_from_buffer(lv_fs_path_ex_t * path, char letter, const void * buf, uint32_t size);

/**
 * Close an already opened file
 * @param file_p    pointer to a lv_fs_file_t variable
 * @return          LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_close(lv_fs_file_t * file_p);

/**
 * Read from a file
 * @param file_p    pointer to a lv_fs_file_t variable
 * @param buf       pointer to a buffer where the read bytes are stored
 * @param btr       Bytes To Read
 * @param br        the number of real read bytes (Bytes Read). NULL if unused.
 * @return          LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_read(lv_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br);

/**
 * Write into a file
 * @param file_p    pointer to a lv_fs_file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return          LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_write(lv_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw);

/**
 * Set the position of the 'cursor' (read write pointer) in a file
 * @param file_p    pointer to a lv_fs_file_t variable
 * @param pos       the new position expressed in bytes index (0: start of file)
 * @param whence    tells from where set the position. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_seek(lv_fs_file_t * file_p, uint32_t pos, lv_fs_whence_t whence);

/**
 * Give the position of the read write pointer
 * @param file_p    pointer to a lv_fs_file_t variable
 * @param pos       pointer to store the position of the read write pointer
 * @return          LV_FS_RES_OK or any error from 'fs_res_t'
 */
lv_fs_res_t lv_fs_tell(lv_fs_file_t * file_p, uint32_t * pos);

/**
 * Initialize a 'fs_dir_t' variable for directory reading
 * @param rddir_p   pointer to a 'lv_fs_dir_t' variable
 * @param path      path to a directory
 * @return          LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_dir_open(lv_fs_dir_t * rddir_p, const char * path);

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param rddir_p   pointer to an initialized 'fs_dir_t' variable
 * @param fn        pointer to a buffer to store the filename
 * @param fn_len    length of the buffer to store the filename
 * @return          LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_dir_read(lv_fs_dir_t * rddir_p, char * fn, uint32_t fn_len);

/**
 * Close the directory reading
 * @param rddir_p   pointer to an initialized 'fs_dir_t' variable
 * @return          LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_dir_close(lv_fs_dir_t * rddir_p);

/**
 * Fill a buffer with the letters of existing drivers
 * @param buf       buffer to store the letters ('\0' added after the last letter)
 * @return          the buffer
 */
char * lv_fs_get_letters(char * buf);

/**
 * Return with the extension of the filename
 * @param fn        string with a filename
 * @return          pointer to the beginning extension or empty string if no extension
 */
const char * lv_fs_get_ext(const char * fn);

/**
 * Step up one level
 * @param path      pointer to a file name
 * @return          the truncated file name
 */
char * lv_fs_up(char * path);

/**
 * Get the last element of a path (e.g. U:/folder/file -> file)
 * @param path      pointer to a file name
 * @return          pointer to the beginning of the last element in the path
 */
const char * lv_fs_get_last(const char * path);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FS_H*/
