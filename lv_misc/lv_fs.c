/**
 * @file lv_fs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_fs.h"
#if USE_LV_FILESYSTEM

#include "lv_ll.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static const char * lv_fs_get_real_path(const char * path);
static lv_fs_drv_t * lv_fs_get_drv(char letter);


/**********************
 *  STATIC VARIABLES
 **********************/
static lv_ll_t drv_ll;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the File system interface
 */
void lv_fs_init(void)
{
    lv_ll_init(&drv_ll, sizeof(lv_fs_drv_t));
}



/**
 * Open a file
 * @param file_p pointer to a lv_fs_file_t variable
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_open(lv_fs_file_t * file_p, const char * path, lv_fs_mode_t mode)
{
    file_p->drv = NULL;
    file_p->file_d = NULL;

    if(path == NULL) return LV_FS_RES_INV_PARAM;

    char letter = path[0];

    file_p->drv = lv_fs_get_drv(letter);

    if(file_p->drv == NULL) {
        file_p->file_d = NULL;
        return LV_FS_RES_NOT_EX;
    }

    if(file_p->drv->ready != NULL) {
        if(file_p->drv->ready() == false) {
            file_p->drv = NULL;
            file_p->file_d = NULL;
            return LV_FS_RES_HW_ERR;
        }
    }

    file_p->file_d = lv_mem_alloc(file_p->drv->file_size);
    lv_mem_assert(file_p->file_d);
    if(file_p->file_d == NULL) {
        file_p->drv = NULL;
        return LV_FS_RES_OUT_OF_MEM;  /* Out of memory */
    }

    if(file_p->drv->open == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    const char * real_path = lv_fs_get_real_path(path);
    lv_fs_res_t res = file_p->drv->open(file_p->file_d, real_path, mode);

    if(res != LV_FS_RES_OK) {
        lv_mem_free(file_p->file_d);
        file_p->file_d = NULL;
        file_p->drv = NULL;
    }

    return res;
}

/**
 * Close an already opened file
 * @param file_p pointer to a lv_fs_file_t variable
 * @return  LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_close(lv_fs_file_t * file_p)
{
    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->close == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    lv_fs_res_t res = file_p->drv->close(file_p->file_d);

    lv_mem_free(file_p->file_d);   /*Clean up*/
    file_p->file_d = NULL;
    file_p->drv = NULL;
    file_p->file_d = NULL;

    return res;
}

/**
 * Delete a file
 * @param path path of the file to delete
 * @return  LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_remove(const char * path)
{
    if(path == NULL) return LV_FS_RES_INV_PARAM;
    lv_fs_drv_t * drv = NULL;

    char letter = path[0];

    drv = lv_fs_get_drv(letter);
    if(drv == NULL) return LV_FS_RES_NOT_EX;
    if(drv->ready != NULL) {
        if(drv->ready() == false) return LV_FS_RES_HW_ERR;
    }

    if(drv->remove == NULL) return LV_FS_RES_NOT_IMP;

    const char * real_path = lv_fs_get_real_path(path);
    lv_fs_res_t res = drv->remove(real_path);

    return res;
}

/**
 * Read from a file
 * @param file_p pointer to a lv_fs_file_t variable
 * @param buf pointer to a buffer where the read bytes are stored
 * @param btr Bytes To Read
 * @param br the number of real read bytes (Bytes Read). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_read(lv_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    if(br != NULL) *br = 0;
    if(file_p->drv == NULL) return LV_FS_RES_INV_PARAM;
    if(file_p->drv->read == NULL) return LV_FS_RES_NOT_IMP;

    uint32_t br_tmp = 0;
    lv_fs_res_t res = file_p->drv->read(file_p->file_d, buf, btr, &br_tmp);
    if(br != NULL) *br = br_tmp;

    return res;
}

/**
 * Write into a file
 * @param file_p pointer to a lv_fs_file_t variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btr Bytes To Write
 * @param br the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_write(lv_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    if(bw != NULL) *bw = 0;

    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->write == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    uint32_t bw_tmp = 0;
    lv_fs_res_t res = file_p->drv->write(file_p->file_d, buf, btw, &bw_tmp);
    if(bw != NULL)  *bw = bw_tmp;

    return res;
}

/**
 * Set the position of the 'cursor' (read write pointer) in a file
 * @param file_p pointer to a lv_fs_file_t variable
 * @param pos the new position expressed in bytes index (0: start of file)
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_seek(lv_fs_file_t * file_p, uint32_t pos)
{
    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->seek == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    lv_fs_res_t res = file_p->drv->seek(file_p->file_d, pos);

    return res;
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a lv_fs_file_t variable
 * @param pos_p pointer to store the position of the read write pointer
 * @return LV_FS_RES_OK or any error from 'fs_res_t'
 */
lv_fs_res_t lv_fs_tell(lv_fs_file_t * file_p, uint32_t  * pos)
{
    if(file_p->drv == NULL) {
        pos = 0;
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->tell == NULL) {
        pos = 0;
        return LV_FS_RES_NOT_IMP;
    }

    lv_fs_res_t res = file_p->drv->tell(file_p->file_d, pos);

    return res;
}

/**
 * Truncate the file size to the current position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_fs_open )
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_trunc(lv_fs_file_t * file_p)
{
    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->tell == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    lv_fs_res_t res = file_p->drv->trunc(file_p->file_d);

    return res;
}
/**
 * Give the size of a file bytes
 * @param file_p pointer to a lv_fs_file_t variable
 * @param size pointer to a variable to store the size
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_size(lv_fs_file_t * file_p, uint32_t * size)
{
    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->size == NULL) return LV_FS_RES_NOT_IMP;


    if(size == NULL) return LV_FS_RES_INV_PARAM;

    lv_fs_res_t res = file_p->drv->size(file_p->file_d, size);

    return res;
}

/**
 * Rename a file
 * @param oldname path to the file
 * @param newname path with the new name
 * @return LV_FS_RES_OK or any error from 'fs_res_t'
 */
lv_fs_res_t lv_fs_rename(const char * oldname, const char * newname)
{
    if(!oldname || !newname) return LV_FS_RES_INV_PARAM;

    char letter = oldname[0];

    lv_fs_drv_t * drv = lv_fs_get_drv(letter);

    if(!drv) {
        return LV_FS_RES_NOT_EX;
    }

    if(drv->ready != NULL) {
        if(drv->ready() == false) {
            return LV_FS_RES_HW_ERR;
        }
    }

    if(drv->rename == NULL) return LV_FS_RES_NOT_IMP;

    const char * old_real = lv_fs_get_real_path(oldname);
    const char * new_real = lv_fs_get_real_path(newname);
    lv_fs_res_t res = drv->rename(old_real, new_real);

    return res;
}


/**
 * Initialize a 'fs_read_dir_t' variable for directory reading
 * @param rddir_p pointer to a 'fs_read_dir_t' variable
 * @param path path to a directory
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_dir_open(lv_fs_dir_t * rddir_p, const char * path)
{
    if(path == NULL) return LV_FS_RES_INV_PARAM;

    char letter = path[0];

    rddir_p->drv = lv_fs_get_drv(letter);

    if(rddir_p->drv == NULL) {
        rddir_p->dir_d = NULL;
        return LV_FS_RES_NOT_EX;
    }

    rddir_p->dir_d = lv_mem_alloc(rddir_p->drv->rddir_size);
    lv_mem_assert(rddir_p->dir_d);
    if(rddir_p->dir_d == NULL) {
        rddir_p->dir_d = NULL;
        return LV_FS_RES_OUT_OF_MEM;  /* Out of memory */
    }

    if(rddir_p->drv->dir_open == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    const char * real_path = lv_fs_get_real_path(path);
    lv_fs_res_t res = rddir_p->drv->dir_open(rddir_p->dir_d, real_path);

    return res;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param rddir_p pointer to an initialized 'fs_read_dir_t' variable
 * @param fn pointer to a buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_dir_read(lv_fs_dir_t * rddir_p, char * fn)
{
    if(rddir_p->drv == NULL || rddir_p->dir_d == NULL) {
        fn[0] = '\0';
        return LV_FS_RES_INV_PARAM;
    }

    if(rddir_p->drv->dir_read == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    lv_fs_res_t res = rddir_p->drv->dir_read(rddir_p->dir_d, fn);

    return res;
}

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'fs_read_dir_t' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_dir_close(lv_fs_dir_t * rddir_p)
{
    if(rddir_p->drv == NULL || rddir_p->dir_d == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    lv_fs_res_t res;

    if(rddir_p->drv->dir_close == NULL) {
        res =  LV_FS_RES_NOT_IMP;
    } else {
        res = rddir_p->drv->dir_close(rddir_p->dir_d);
    }

    lv_mem_free(rddir_p->dir_d);   /*Clean up*/
    rddir_p->dir_d = NULL;
    rddir_p->drv = NULL;
    rddir_p->dir_d = NULL;

    return res;
}

/**
 * Get the free and total size of a driver in kB
 * @param letter the driver letter
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free size [kB]
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
lv_fs_res_t lv_fs_free(char letter, uint32_t * total_p, uint32_t * free_p)
{
    lv_fs_drv_t * drv = lv_fs_get_drv(letter);

    if(drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    lv_fs_res_t res;

    if(drv->free == NULL) {
        res =  LV_FS_RES_NOT_IMP;
    } else {
        uint32_t total_tmp = 0;
        uint32_t free_tmp = 0;
        res = drv->free(&total_tmp, &free_tmp);

        if(total_p != NULL) *total_p = total_tmp;
        if(free_p != NULL) *free_p = free_tmp;
    }

    return res;
}

/**
 * Add a new drive
 * @param drv_p pointer to an lv_fs_drv_t structure which is inited with the
 * corresponding function pointers. The data will be copied so the variable can be local.
 */
void lv_fs_add_drv(lv_fs_drv_t * drv_p)
{
    /*Save the new driver*/
    lv_fs_drv_t * new_drv;
    new_drv =  lv_ll_ins_head(&drv_ll);
    lv_mem_assert(new_drv);
    if(new_drv == NULL) return;

    memcpy(new_drv, drv_p, sizeof(lv_fs_drv_t));
}

/**
 * Fill a buffer with the letters of existing drivers
 * @param buf buffer to store the letters ('\0' added after the last letter)
 * @return the buffer
 */
char  * lv_fs_get_letters(char * buf)
{
    lv_fs_drv_t * drv;
    uint8_t i = 0;

    LL_READ(drv_ll, drv) {
        buf[i] = drv->letter;
        i++;
    }

    buf[i] = '\0';

    return buf;
}


/**
 * Return with the extension of the filename
 * @param fn string with a filename
 * @return pointer to the beginning extension or empty string if no extension
 */
const char * lv_fs_get_ext(const char * fn)
{
    uint16_t i;
    for(i = strlen(fn); i > 0; i --) {
        if(fn[i] == '.') {
            return &fn[i + 1];
        } else if(fn[i] == '/' || fn[i] == '\\') {
            return "";  /*No extension if a '\' or '/' found*/
        }
    }

    return ""; /*Empty string if no '.' in the file name. */
}

/**
 * Step up one level
 * @param path pointer to a file name
 * @return the truncated file name
 */
char * lv_fs_up(char * path)
{
    uint16_t len = strlen(path);
    if(len == 0) return path;

    len --; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        path[len] = '\0';
        if(len > 0) len --;
        else return path;
    }

    uint16_t i;
    for(i = len; i > 0; i --) {
        if(path[i] == '/' || path[i] == '\\') break;
    }

    path[i] = '\0';

    return path;
}

/**
 * Get the last element of a path (e.g. U:/folder/file -> file)
 * @param path a character sting with the path to search in
 * @return pointer to the beginning of the last element in the path
 */
const char * lv_fs_get_last(const char * path)
{
    uint16_t len = strlen(path);
    if(len == 0) return path;

    len --; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        if(len > 0) len --;
        else return path;
    }

    uint16_t i;
    for(i = len; i > 0; i --) {
        if(path[i] == '/' || path[i] == '\\') break;
    }

    /*No '/' or '\' in the path so return with path itself*/
    if(i == 0) return path;

    return &path[i + 1];
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Leave the driver letters and / or \ letters from beginning of the path
 * @param path path string (E.g. S:/folder/file.txt)
 * @return pointer to the beginning of the real path (E.g. folder/file.txt)
 */
static const char * lv_fs_get_real_path(const char * path)
{
    /* Example path: "S:/folder/file.txt"
     * Leave the letter and the : / \ characters*/

    path ++; /*Ignore the driver letter*/

    while(*path != '\0') {
        if(*path == ':' || *path == '\\' || *path == '/') {
            path ++;
        } else {
            break;
        }
    }

    return path;
}

/**
 * Give a pointer to a driver from its letter
 * @param letter the driver letter
 * @return pointer to a driver or NULL if not found
 */
static lv_fs_drv_t * lv_fs_get_drv(char letter)
{
    lv_fs_drv_t * drv;

    LL_READ(drv_ll, drv) {
        if(drv->letter == letter) {
            return drv;
        }
    }

    return NULL;
}

#endif /*USE_LV_FILESYSTEM*/
