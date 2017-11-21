/**
 * @file linuxfs.c
 * Functions to give an API to the standard
 * file operation functions to be compatible
 */


#ifndef LINUXFS_H
#define LINUXFS_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_LINUXFS != 0

#include "../fsint.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a driver for the linuxfs device and initilaize it.
 */
void linuxfs_init(void);

/**
 * Give the state of the linuxfs
 * @return true if it is already initialized
 */
bool linuxfs_ready(void);

/**
 * Open a file
 * @param file_p pointer to a FIL type variable
 * @param path path of the file (e.g. "dir1/dir2/file1.txt")
 * @param mode open mode (FS_MODE_RD or FS_MODE_WR or both with | (or connection))
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_open (void * file_p, const char * path, fs_mode_t mode);

/**
 * Close an already opened file
 * @param file_p pointer to a FIL type variable
 * @return FS_RES_OK or any error from fs_res_t
 */
fs_res_t linuxfs_close (void * file_p);

/**
 * Remove (delete) a file
 * @param path the path of the file ("dir1/file1.txt")
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_remove(const char * path);

/**
 * Read data from an opened file
 * @param file_p pointer to a FIL type variable
 * @param buf pointer a buffer to store the read bytes
 * @param btr the number of Bytes To Read
 * @param br the number of real read bytes (Bytes Read)
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br);

/**
 * Write data to an opened file
 * @param file_p pointer to a FIL type variable
 * @param buf pointer to buffer where the data to write is located
 * @param btw the number of Bytes To Write
 * @param bw the number of real written bytes (Bytes Written)
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_write (void * file_p, const void * buf, uint32_t btw, uint32_t * bw);

/**
 * Position the read write pointer to given position
 * @param file_p pointer to a FIL type variable
 * @param pos the new position expressed in bytes index (0: start of file)
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_seek (void * file_p, uint32_t pos);

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a FIL type variable
 * @param pos_p pointer to store the position of the read write pointer
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_tell (void * file_p, uint32_t * pos_p);

/**
 * Give the size of a file
 * @param file_p pointer to a FIL type variable
 * @param size_p pointer to store the size
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_size (void * file_p, uint32_t * size_p);

/**
 * Initialize a variable for directory reading
 * @param rddir_p pointer to a 'DIR' variable
 * @param path path to a directory
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t linuxfs_readdir_init(void * rddir_p, const char * path);

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param rddir_p pointer to an initialized 'DIR' variable
 * @param fn pointer to a buffer to store the filename
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t linuxfs_readdir(void * rddir_p, char * fn);

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'DIR' variable
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t linuxfs_readdir_close(void * rddir_p);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
