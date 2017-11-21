/**
 * @file linuxfs.c
 * Functions to give an API to the standard
 * file operation functions to be compatible
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_LINUXFS != 0

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "linuxfs.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static fs_res_t linuxfs_res_trans(int linuxfs_res);

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
 * Create a driver for the linuxfs device and initilaize it.
 */
void linuxfs_init(void)
{
    /*Create the driver*/
    fs_drv_t linux_drv;
    memset(&linux_drv, 0, sizeof(fs_drv_t));    /*Initialization*/

    linux_drv.file_size = sizeof(FILE *);
    linux_drv.rddir_size = sizeof(DIR *);
    linux_drv.letter = LINUXFS_LETTER;
    linux_drv.ready = linuxfs_ready;

    linux_drv.open = linuxfs_open;
    linux_drv.close = linuxfs_close;
    linux_drv.remove = linuxfs_remove;
    linux_drv.read = linuxfs_read;
    linux_drv.write = linuxfs_write;
    linux_drv.seek = linuxfs_seek;
    linux_drv.tell = linuxfs_tell;
    linux_drv.size = linuxfs_size;
    linux_drv.trunc = NULL;

    linux_drv.rddir_init = linuxfs_readdir_init;
    linux_drv.rddir = linuxfs_readdir;
    linux_drv.rddir_close = linuxfs_readdir_close;

    linux_drv.free = NULL;
    fs_add_drv(&linux_drv);
}

/**
 * Give the state of the linuxfs
 * @return true if it is already initialized
 */
bool linuxfs_ready(void)
{
    return true;
}

/**
 * Open a file
 * @param file_p pointer to a FIL type variable
 * @param path path of the file (e.g. "dir1/dir2/file1.txt")
 * @param mode open mode (FS_MODE_RD or FS_MODE_WR or both with | (or connection))
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_open (void * file_p, const char * path, fs_mode_t mode)
{
    FILE ** fp = file_p;

    errno = 0;
    const char * linuxfs_mode = 0;
    if(mode == FS_MODE_RD) linuxfs_mode = "r";
    if(mode == FS_MODE_WR) linuxfs_mode = "a";
    if(mode == (FS_MODE_WR | FS_MODE_RD)) linuxfs_mode = "a+";

    *fp = fopen(path, linuxfs_mode);
    return linuxfs_res_trans(errno);
}

/**
 * Close an already opened file
 * @param file_p pointer to a FIL type variable
 * @return FS_RES_OK or any error from fs_res_t
 */
fs_res_t linuxfs_close (void * file_p)
{
    FILE ** fp = file_p;
    if(*fp == NULL) return FS_RES_INV_PARAM;
    errno = 0;
    fclose(*fp);
    return linuxfs_res_trans(errno);
}

/**
 * Remove (delete) a file
 * @param path the path of the file ("dir1/file1.txt")
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_remove(const char * path)
{
    errno = 0;
    remove(path);
    return linuxfs_res_trans(errno);
}

/**
 * Read data from an opened file
 * @param file_p pointer to a FIL type variable
 * @param buf pointer a buffer to store the read bytes
 * @param btr the number of Bytes To Read
 * @param br the number of real read bytes (Bytes Read)
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    FILE ** fp = file_p;
    if(*fp == NULL) return FS_RES_INV_PARAM;

    errno = 0;
    (*br) = fread(buf, 1, btr, *fp);
    return linuxfs_res_trans(errno);
}

/**
 * Write data to an opened file
 * @param file_p pointer to a FIL type variable
 * @param buf pointer to buffer where the data to write is located
 * @param btw the number of Bytes To Write
 * @param bw the number of real written bytes (Bytes Written)
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_write (void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    FILE ** fp = file_p;
    if(*fp == NULL) return FS_RES_INV_PARAM;

    errno = 0;
    (*bw) = fwrite(buf, 1, btw, *fp);
    return linuxfs_res_trans(errno);
}

/**
 * Position the read write pointer to given position
 * @param file_p pointer to a FIL type variable
 * @param pos the new position expressed in bytes index (0: start of file)
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_seek (void * file_p, uint32_t pos)
{
    FILE ** fp = file_p;
    if(*fp == NULL) return FS_RES_INV_PARAM;

    errno = 0;
    fseek(*fp, pos, SEEK_SET);
    return linuxfs_res_trans(errno);
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a FIL type variable
 * @param pos_p pointer to store the position of the read write pointer
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_tell (void * file_p, uint32_t * pos_p)
{
    FILE ** fp = file_p;
    if(*fp == NULL) return FS_RES_INV_PARAM;

    errno = 0;
    long int x = ftell(*fp);
    if(x < 0) *pos_p = 0;
    else *pos_p = (uint32_t)x;

    return linuxfs_res_trans(errno);
}


/**
 * Give the size of a file
 * @param file_p pointer to a FIL type variable
 * @param size_p pointer to store the size
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t linuxfs_size (void * file_p, uint32_t * size_p)
{
    FILE ** fp = file_p;
    if(*fp == NULL) return FS_RES_INV_PARAM;

    errno = 0;
    /*Save the current position*/
    long int ori = ftell(*fp);
    if(errno) return linuxfs_res_trans(errno);

    /* Seek to the and read the position.
     * It is equal to the size*/
    fseek(*fp, 0, SEEK_END);
    if(errno) return linuxfs_res_trans(errno);
    long int x;
    x = ftell(*fp);
    if(errno) return linuxfs_res_trans(errno);

    *size_p = (uint32_t)x;

    /*Revert the position*/
    fseek(*fp, ori, SEEK_SET); // seek back read write pointer

    return linuxfs_res_trans(errno);
}


/**
 * Initialize a variable for directory reading
 * @param rddir_p pointer to a 'DIR' variable
 * @param path path to a directory
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t linuxfs_readdir_init(void * rddir_p, const char * path)
{
    errno = 0;
    char path_buf[512];
    sprintf(path_buf,"%s%s" , LINUXFS_ROOT_DIR, path);

    DIR ** rd = rddir_p;
    *rd = opendir(path_buf);

    return linuxfs_res_trans(errno);
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param rddir_p pointer to an initialized 'DIR' variable
 * @param fn pointer to a buffer to store the filename
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t linuxfs_readdir(void * rddir_p, char * fn)
{
    errno = 0;
    DIR ** rd = rddir_p;
    if(*rd == NULL) return FS_RES_INV_PARAM;

    struct dirent *dirp;

    do {
        dirp = readdir(*rd);
        if(dirp == NULL){
            fn[0] = '\0';
            break;
        }
    } while(dirp->d_name[0] == '.');  /*Ignore "." and ".."*/

    /*Save the filename*/
    if(dirp != NULL) {
        if(dirp->d_type == DT_DIR)  sprintf(fn, "/%s", dirp->d_name);
        else sprintf(fn, "%s", dirp->d_name);
    }

    return linuxfs_res_trans(errno);
}

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'DIR' variable
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t linuxfs_readdir_close(void * rddir_p)
{
    DIR ** rd = rddir_p;
    if(*rd == NULL) return FS_RES_INV_PARAM;

    errno = 0;
    closedir(*rd);
    return FS_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Translate the return codes to fs_res_t
 * @param linuxfs_res the original fat_32 result
 * @return the converted, fs_res_t return code
 */

static fs_res_t linuxfs_res_trans(int linuxfs_res)
{
    if(linuxfs_res == 0) return FS_RES_OK;
    return FS_RES_UNKNOWN;
}

#endif
