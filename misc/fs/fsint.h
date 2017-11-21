/**
 * @file fsint.h
 * 
 */

#ifndef FSINT_H
#define FSINT_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_FSINT != 0

#include <stdint.h>
#include <stdbool.h>
#include "../mem/dyn_mem.h"

/*********************
 *      DEFINES
 *********************/
#define FSINT_MAX_FN_LENGTH  64

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    FS_RES_OK = 0,
    FS_RES_HW_ERR,      /*Low level hardwer error*/
    FS_RES_FS_ERR,      /*Error in the file system structure */
    FS_RES_NOT_EX,      /*Driver, file or direcory is not exists*/
    FS_RES_FULL,        /*Disk full*/
    FS_RES_LOCKED,      /*The file is already opened*/
    FS_RES_DENIED,      /*Access denied. Check 'fs_open' modes and write protect*/
    FS_BUSY,            /*The filesystem now can't handle it, try later*/
    FS_RES_TOUT,        /*Process timeouted*/
    FS_RES_NOT_IMP,     /*Requested function is not implemented*/
    FS_RES_OUT_OF_MEM,  /*Not enough memory for an internal opretion*/
    FS_RES_INV_PARAM,   /*Invalid parameter among arguments*/
    FS_RES_UNKNOWN,     /*Other unknown error*/
}fs_res_t;

struct __fs_drv_struct;
        
typedef struct
{
    void * file_d;
    struct __fs_drv_struct* drv;
}fs_file_t;


typedef struct
{
    void * rddir_d;
    struct __fs_drv_struct * drv;
}fs_readdir_t;

typedef enum
{
    FS_MODE_WR = 0x01,
    FS_MODE_RD = 0x02,
}fs_mode_t;

typedef struct __fs_drv_struct
{
    char letter;
    uint16_t file_size;
    uint16_t rddir_size;
    bool (*ready) (void);
    
    fs_res_t (*open) (void * file_p, const char * path, fs_mode_t mode);
    fs_res_t (*close) (void * file_p);
    fs_res_t (*remove) (const char * fn);
    fs_res_t (*read) (void * file_p, void * buf, uint32_t btr, uint32_t * br);
    fs_res_t (*write) (void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
    fs_res_t (*seek) (void * file_p, uint32_t pos);
    fs_res_t (*tell) (void * file_p, uint32_t * pos_p);
    fs_res_t (*trunc) (void * file_p);
    fs_res_t (*size) (void * file_p, uint32_t * size_p);
    fs_res_t (*free) (uint32_t * total_p, uint32_t * free_p);
    
    fs_res_t (*rddir_init) (void * rddir_p, const char * path);
    fs_res_t (*rddir) (void * rddir_p, char * fn);
    fs_res_t (*rddir_close) (void * rddir_p);
}fs_drv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the File system interface
 */
void fs_init(void);

/**
 * Add a new drive
 * @param drv_p pointer to an fs_drv_t structure which is inited with the
 * corresponding function pointer
 */
void fs_add_drv(fs_drv_t * drv_p);

/**
 * Open a file
 * @param file_p pointer to a fs_file_t variable
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_open (fs_file_t * file_p, const char * path, fs_mode_t mode);

/**
 * Close an already opened file
 * @param file_p pointer to a fs_file_t variable
 * @return  FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_close (fs_file_t * file_p);

/**
 * Delete a file
 * @param path path of the file to delete
 * @return  FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_remove (const char * path);

/**
 * Read from a file
 * @param file_p pointer to a fs_file_t variable
 * @param buf pointer to a buffer where the read bytes are stored
 * @param btr Bytes To Read
 * @param br the number of real read bytes (Bytes Read). NULL if unused.
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_read (fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br);

/**
 * Write into a file
 * @param file_p pointer to a fs_file_t variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btr Bytes To Write
 * @param br the number of real written bytes (Bytes Written). NULL if unused.
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_write (fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw);

/**
 * Set the position of the 'cursor' (read write pointer) in a file
 * @param file_p pointer to a fs_file_t variable
 * @param pos the new position expressed in bytes index (0: start of file)
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_seek (fs_file_t * file_p, uint32_t pos);

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a fs_file_t variable
 * @param pos_p pointer to store the position of the read write pointer
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fs_tell (fs_file_t * file_p, uint32_t  * pos);

/**
 * Give the size of a file bytes
 * @param file_p pointer to a fs_file_t variable
 * @param size pointer to a variable to store the size
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_size (fs_file_t * file_p, uint32_t * size);

/**
 * Initialize a 'fs_read_dir_t' variable for directory reading
 * @param rddir_p pointer to a 'fs_read_dir_t' variable
 * @param path path to a directory
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_readdir_init(fs_readdir_t * rddir_p, const char * path);

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param rddir_p pointer to an initialized 'fs_read_dir_t' variable
 * @param fn pointer to a buffer to store the filename
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_readdir (fs_readdir_t * rddir_p, char * fn);

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'fs_read_dir_t' variable
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_readdir_close (fs_readdir_t * rddir_p);

/**
 * Get the free and total size of a driver in kB
 * @param letter the driver letter
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free size [kB]
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fs_free (char letter, uint32_t * total_p, uint32_t * free_p);

/**
 * Fill a buffer with the letters of existing drivers
 * @param buf buffer to store the letters ('\0' added after the last letter)
 * @return the buffer
 */
char *  fs_get_letters(char * buf);

/**
 * Return with the extension of the filename
 * @param fn string with a filename
 * @return pointer to the beginning extension or empty string if no extension
 */
const char * fs_get_ext(const char * fn);

/**
 * Step up one level
 * @param path pointer to a file name
 * @return the truncated file name
 */
char * fs_up(char * path);

/**
 * Get the last element of a path (e.g. U:/folder/file -> file)
 * @param buf buffer to store the letters ('\0' added after the last letter)
 * @return pointer to the beginning of the last element in the path
 */
const char * fs_get_last(const char * path);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
