/**
 * @file ufs.h
 * Implementation of RAM file system which do NOT support directories.
 * The API is compatible with the fs_int module.
 */

#ifndef UFS_H
#define UFS_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_UFS != 0

#include <stdbool.h>
#include "lv_fs.h"
#include "lv_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Description of a file entry */
typedef struct
{
    char * fn_d;
    void * data_d;
    uint32_t size;  /*Data length in bytes*/
    uint16_t oc;    /*Open Count*/
    uint8_t const_data :1;
}ufs_ent_t;

/*File descriptor, used to handle opening an entry more times simultaneously 
 Contains unique informations about the specific opening*/
typedef struct
{
    ufs_ent_t* ent; /*Pointer to the entry*/
    uint32_t rwp;   /*Read Write Pointer*/
    uint8_t ar :1;  /*1: Access for read is enabled */
    uint8_t aw :1;  /*1: Access for write is enabled */
}ufs_file_t;

/* Read directory descriptor.
 * It is used to to iterate through the entries in a directory*/
typedef struct
{
    ufs_ent_t * last_ent;
}ufs_read_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a driver for ufs and initialize it.
 */
void ufs_init(void);

/**
 * Give the state of the ufs
 * @return true if ufs is initialized and can be used else false
 */
bool ufs_ready(void);

/**
 * Open a file in ufs
 * @param file_p pointer to a ufs_file_t variable
 * @param fn name of the file. There are no directories so e.g. "myfile.txt"
 * @param mode element of 'fs_mode_t' enum or its 'OR' connection (e.g. FS_MODE_WR | FS_MODE_RD)
 * @return FS_RES_OK: no error, the file is opened
 *         any error from fs_res_t enum
 */
fs_res_t ufs_open (void * file_p, const char * fn, fs_mode_t mode);

/**
 * Create a file with a constant data
 * @param fn name of the file (directories are not supported)
 * @param const_p pointer to a constant data
 * @param len length of the data pointed by 'const_p' in bytes
 * @return FS_RES_OK: no error, the file is read
 *         any error from fs_res_t enum
 */
fs_res_t ufs_create_const(const char * fn, const void * const_p, uint32_t len);

/**
 * Close an opened file
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ufs_open)
 * @return FS_RES_OK: no error, the file is read
 *         any error from fs_res_t enum
 */
fs_res_t ufs_close (void * file_p);

/**
 * Remove a file. The file can not be opened.
 * @param fn '\0' terminated string
 * @return FS_RES_OK: no error, the file is removed
 *         FS_RES_DENIED: the file was opened, remove failed
 */
fs_res_t ufs_remove(const char * fn);

/**
 * Read data from an opened file
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ufs_open )
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return FS_RES_OK: no error, the file is read
 *         any error from fs_res_t enum
 */
fs_res_t ufs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br);

/**
 * Write data to an opened file
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ufs_open)
 * @param buf pointer to a memory block which content will be written
 * @param btw the number Bytes To Write
 * @param bw The real number of written bytes (Byte Written)
 * @return FS_RES_OK: no error, the file is read
 *         any error from fs_res_t enum
 */
fs_res_t ufs_write (void * file_p, const void * buf, uint32_t btw, uint32_t * bw);

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ufs_open )
 * @param pos the new position of read write pointer
 * @return FS_RES_OK: no error, the file is read
 *         any error from fs_res_t enum
 */
fs_res_t ufs_seek (void * file_p, uint32_t pos);

/**
 * Give the position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ufs_open )
 * @param pos_p pointer to to store the result
 * @return FS_RES_OK: no error, the file is read
 *         any error from fs_res_t enum
 */
fs_res_t ufs_tell (void * file_p, uint32_t * pos_p);

/**
 * Truncate the file size to the current position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ufs_open )
 * @return FS_RES_OK: no error, the file is read
 *         any error from fs_res_t enum
 */
fs_res_t ufs_trunc (void * file_p);

/**
 * Give the size of the file in bytes
 * @param file_p file_p pointer to an 'ufs_file_t' variable. (opened with ufs_open )
 * @param size_p pointer to store the size
 * @return FS_RES_OK: no error, the file is read
 *         any error from fs_res_t enum
 */
fs_res_t ufs_size (void * file_p, uint32_t * size_p);

/**
 * Initialize a ufs_read_dir_t variable to directory reading
 * @param rddir_p pointer to a 'ufs_read_dir_t' variable
 * @param path uFS doesn't support folders so it has to be ""
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t ufs_readdir_init(void * rddir_p, const char * path);

/**
 * Read the next file name
 * @param rddir_p pointer to an initialized 'ufs_read_dir_t' variable
 * @param fn pointer to buffer to sore the file name
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t ufs_readdir(void * rddir_p, char * fn);

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'ufs_read_dir_t' variable
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t ufs_readdir_close(void * rddir_p);

/**
 * Give the size of a drive
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free site [kB]
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t ufs_free (uint32_t * total_p, uint32_t * free_p);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
