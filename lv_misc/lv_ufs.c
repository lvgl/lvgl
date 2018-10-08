/**
 * @file lv_ufs.c
 * Implementation of RAM file system which do NOT support directories.
 * The API is compatible with the lv_fs_int module.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_ufs.h"
#if USE_LV_FILESYSTEM

#include "lv_ll.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_ufs_ent_t * lv_ufs_ent_get(const char * fn);
static lv_ufs_ent_t * lv_ufs_ent_new(const char * fn);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_ll_t file_ll;
static bool inited = false;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a driver for ufs and initialize it.
 */
void lv_ufs_init(void)
{
    lv_ll_init(&file_ll, sizeof(lv_ufs_ent_t));

    lv_fs_drv_t ufs_drv;
    memset(&ufs_drv, 0, sizeof(lv_fs_drv_t));    /*Initialization*/

    ufs_drv.file_size = sizeof(lv_ufs_file_t);
    ufs_drv.rddir_size = sizeof(lv_ufs_dir_t);
    ufs_drv.letter = UFS_LETTER;
    ufs_drv.ready = lv_ufs_ready;

    ufs_drv.open = lv_ufs_open;
    ufs_drv.close = lv_ufs_close;
    ufs_drv.remove = lv_ufs_remove;
    ufs_drv.read = lv_ufs_read;
    ufs_drv.write = lv_ufs_write;
    ufs_drv.seek = lv_ufs_seek;
    ufs_drv.tell = lv_ufs_tell;
    ufs_drv.size = lv_ufs_size;
    ufs_drv.trunc = lv_ufs_trunc;
    ufs_drv.free = lv_ufs_free;

    ufs_drv.dir_open = lv_ufs_dir_open;
    ufs_drv.dir_read = lv_ufs_dir_read;
    ufs_drv.dir_close = lv_ufs_dir_close;

    lv_fs_add_drv(&ufs_drv);

    inited = true;
}

/**
 * Give the state of the ufs
 * @return true if ufs is initialized and can be used else false
 */
bool lv_ufs_ready(void)
{
    return inited;
}

/**
 * Open a file in ufs
 * @param file_p pointer to a lv_ufs_file_t variable
 * @param fn name of the file. There are no directories so e.g. "myfile.txt"
 * @param mode element of 'fs_mode_t' enum or its 'OR' connection (e.g. FS_MODE_WR | FS_MODE_RD)
 * @return LV_FS_RES_OK: no error, the file is opened
 *         any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_open(void * file_p, const char * fn, lv_fs_mode_t mode)
{
    lv_ufs_file_t * fp = file_p;    /*Convert type*/
    lv_ufs_ent_t * ent = lv_ufs_ent_get(fn);

    fp->ent = NULL;

    /*If the file not exists ...*/
    if(ent == NULL) {
        if((mode & LV_FS_MODE_WR) != 0) {  /*Create the file if opened for write*/
            ent = lv_ufs_ent_new(fn);
            if(ent == NULL) return LV_FS_RES_FULL; /*No space for the new file*/
        } else {
            return LV_FS_RES_NOT_EX;       /*Can not read not existing file*/
        }
    }

    /*Can not write already opened and const data files*/
    if((mode & LV_FS_MODE_WR) != 0) {
        if(ent->oc != 0) return LV_FS_RES_LOCKED;
        if(ent->const_data != 0) return LV_FS_RES_DENIED;
    }

    /*No error, the file can be opened*/
    fp->ent = ent;
    fp->ar = mode & LV_FS_MODE_RD ? 1 : 0;
    fp->aw = mode & LV_FS_MODE_WR ? 1 : 0;
    fp->rwp = 0;
    ent->oc ++;

    return LV_FS_RES_OK;
}


/**
 * Create a file with a constant data
 * @param fn name of the file (directories are not supported)
 * @param const_p pointer to a constant data
 * @param len length of the data pointed by 'const_p' in bytes
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_create_const(const char * fn, const void * const_p, uint32_t len)
{
    lv_ufs_file_t file;
    lv_fs_res_t res;

    /*Error if the file already exists*/
    res = lv_ufs_open(&file, fn, LV_FS_MODE_RD);
    if(res == LV_FS_RES_OK) {
        lv_ufs_close(&file);
        return LV_FS_RES_DENIED;
    }

    lv_ufs_close(&file);

    res = lv_ufs_open(&file, fn, LV_FS_MODE_WR);
    if(res != LV_FS_RES_OK) return res;

    lv_ufs_ent_t * ent = file.ent;

    if(ent->data_d != NULL) return LV_FS_RES_DENIED;

    ent->data_d = (void *) const_p;
    ent->size = len;
    ent->const_data = 1;

    res = lv_ufs_close(&file);
    if(res != LV_FS_RES_OK) return res;

    return LV_FS_RES_OK;
}

/**
 * Close an opened file
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_close(void * file_p)
{
    lv_ufs_file_t * fp = file_p;    /*Convert type*/

    if(fp->ent == NULL) return LV_FS_RES_OK;

    /*Decrement the Open counter*/
    if(fp->ent->oc > 0) {
        fp->ent->oc--;
    }

    return LV_FS_RES_OK;
}

/**
 * Remove a file. The file can not be opened.
 * @param fn '\0' terminated string
 * @return LV_FS_RES_OK: no error, the file is removed
 *         LV_FS_RES_DENIED: the file was opened, remove failed
 */
lv_fs_res_t lv_ufs_remove(const char * fn)
{
    lv_ufs_ent_t * ent = lv_ufs_ent_get(fn);
    if(ent == NULL) return LV_FS_RES_DENIED;    /*File not exists*/

    /*Can not be deleted is opened*/
    if(ent->oc != 0) return LV_FS_RES_DENIED;

    lv_ll_rem(&file_ll, ent);
    lv_mem_free(ent->fn_d);
    ent->fn_d = NULL;
    if(ent->const_data == 0) {
        lv_mem_free(ent->data_d);
        ent->data_d = NULL;
    }

    lv_mem_free(ent);

    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_ufs_open )
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_read(void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    lv_ufs_file_t * fp = file_p;    /*Convert type*/

    lv_ufs_ent_t * ent = fp->ent;
    *br = 0;

    if(ent->data_d == NULL || ent->size == 0) { /*Don't read empty files*/
        return LV_FS_RES_OK;
    } else if(fp->ar == 0) {    /*The file is not opened for read*/
        return LV_FS_RES_DENIED;
    }

    /*No error, read the file*/
    if(fp->rwp + btr > ent->size) {  /*Check too much bytes read*/
        *br =  ent->size - fp->rwp;
    } else {
        *br = btr;
    }

    /*Read the data*/
    uint8_t * data8_p;
    if(ent->const_data == 0) {
        data8_p = (uint8_t *) ent->data_d;
    } else {
        data8_p = ent->data_d;
    }

    data8_p += fp->rwp;
    memcpy(buf, data8_p, *br);

    fp->rwp += *br; /*Refresh the read write pointer*/

    return LV_FS_RES_OK;
}

/**
 * Write data to an opened file
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_ufs_open)
 * @param buf pointer to a memory block which content will be written
 * @param btw the number Bytes To Write
 * @param bw The real number of written bytes (Byte Written)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_write(void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    lv_ufs_file_t * fp = file_p;    /*Convert type*/
    *bw = 0;

    if(fp->aw == 0) return LV_FS_RES_DENIED; /*Not opened for write*/

    lv_ufs_ent_t * ent = fp->ent;

    /*Reallocate data array if it necessary*/
    uint32_t new_size = fp->rwp + btw;
    if(new_size > ent->size) {
        uint8_t * new_data = lv_mem_realloc(ent->data_d, new_size);
        lv_mem_assert(new_data);
        if(new_data == NULL) return LV_FS_RES_FULL; /*Cannot allocate the new memory*/

        ent->data_d = new_data;
        ent->size = new_size;
    }

    /*Write the file*/
    uint8_t * data8_p = (uint8_t *) ent->data_d;
    data8_p += fp->rwp;
    memcpy(data8_p, buf, btw);
    *bw = btw;
    fp->rwp += *bw;

    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_seek(void * file_p, uint32_t pos)
{
    lv_ufs_file_t * fp = file_p;    /*Convert type*/
    lv_ufs_ent_t * ent = fp->ent;

    /*Simply move the rwp before EOF*/
    if(pos < ent->size) {
        fp->rwp = pos;
    } else { /*Expand the file size*/
        if(fp->aw == 0) return LV_FS_RES_DENIED;       /*Not opened for write*/

        uint8_t * new_data = lv_mem_realloc(ent->data_d, pos);
        lv_mem_assert(new_data);
        if(new_data == NULL) return LV_FS_RES_FULL; /*Out of memory*/

        ent->data_d = new_data;
        ent->size = pos;
        fp->rwp = pos;
    }

    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_ufs_open )
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_tell(void * file_p, uint32_t * pos_p)
{
    lv_ufs_file_t * fp = file_p;    /*Convert type*/

    *pos_p = fp->rwp;

    return LV_FS_RES_OK;
}

/**
 * Truncate the file size to the current position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_ufs_open )
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_trunc(void * file_p)
{
    lv_ufs_file_t * fp = file_p;    /*Convert type*/
    lv_ufs_ent_t * ent = fp->ent;

    if(fp->aw == 0) return LV_FS_RES_DENIED; /*Not opened for write*/

    void * new_data = lv_mem_realloc(ent->data_d, fp->rwp);
    lv_mem_assert(new_data);
    if(new_data == NULL) return LV_FS_RES_FULL; /*Out of memory*/

    ent->data_d = new_data;
    ent->size = fp->rwp;

    return LV_FS_RES_OK;
}

/**
 * Give the size of the file in bytes
 * @param file_p file_p pointer to an 'ufs_file_t' variable. (opened with lv_ufs_open )
 * @param size_p pointer to store the size
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_size(void * file_p, uint32_t * size_p)
{
    lv_ufs_file_t * fp = file_p;    /*Convert type*/
    lv_ufs_ent_t * ent = fp->ent;

    *size_p = ent->size;

    return LV_FS_RES_OK;
}

/**
 * Initialize a lv_ufs_read_dir_t variable to directory reading
 * @param rddir_p pointer to a 'ufs_dir_t' variable
 * @param path uFS doesn't support folders so it has to be ""
 * @return LV_FS_RES_OK or any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_dir_open(void * rddir_p, const char * path)
{
    lv_ufs_dir_t * lv_ufs_rddir_p = rddir_p;

    lv_ufs_rddir_p->last_ent = NULL;

    if(path[0] != '\0') return LV_FS_RES_NOT_EX;       /*Must be "" */
    else return LV_FS_RES_OK;
}

/**
 * Read the next file name
 * @param dir_p pointer to an initialized 'ufs_dir_t' variable
 * @param fn pointer to buffer to sore the file name
 * @return LV_FS_RES_OK or any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_dir_read(void * dir_p, char * fn)
{
    lv_ufs_dir_t * ufs_dir_p = dir_p;

    if(ufs_dir_p->last_ent == NULL) {
        ufs_dir_p->last_ent = lv_ll_get_head(&file_ll);
    } else {
        ufs_dir_p->last_ent = lv_ll_get_next(&file_ll, ufs_dir_p->last_ent);
    }

    if(ufs_dir_p->last_ent != NULL) {
        strcpy(fn, ufs_dir_p->last_ent->fn_d);
    } else {
        fn[0] = '\0';
    }

    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'ufs_dir_t' variable
 * @return LV_FS_RES_OK or any error from lv__fs_res_t enum
 */
lv_fs_res_t lv_ufs_dir_close(void * rddir_p)
{
    (void)rddir_p;
    return LV_FS_RES_OK;
}

/**
 * Give the size of a drive
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free site [kB]
 * @return LV_FS_RES_OK or any error from 'lv_fs_res_t'
 */
lv_fs_res_t lv_ufs_free(uint32_t * total_p, uint32_t * free_p)
{

#if LV_MEM_CUSTOM == 0
    lv_mem_monitor_t mon;

    lv_mem_monitor(&mon);
    *total_p = LV_MEM_SIZE >> 10;    /*Convert bytes to kB*/
    *free_p = mon.free_size >> 10;
#else
    *free_p = 0;
#endif
    return LV_FS_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Gives the lv_ufs_entry from a filename
 * @param fn filename ('\0' terminated string)
 * @return pointer to the dynamically allocated entry with 'fn' filename.
 *         NULL if no entry found with that name.
 */
static lv_ufs_ent_t * lv_ufs_ent_get(const char * fn)
{
    lv_ufs_ent_t * fp;

    LL_READ(file_ll, fp) {
        if(strcmp(fp->fn_d, fn) == 0) {
            return fp;
        }
    }

    return NULL;
}

/**
 * Create a new entry with 'fn' filename
 * @param fn filename ('\0' terminated string)
 * @return pointer to the dynamically allocated new entry.
 *         NULL if no space for the entry.
 */
static lv_ufs_ent_t * lv_ufs_ent_new(const char * fn)
{
    lv_ufs_ent_t * new_ent = NULL;
    new_ent = lv_ll_ins_head(&file_ll);                 /*Create a new file*/
    lv_mem_assert(new_ent);
    if(new_ent == NULL) return NULL;

    new_ent->fn_d = lv_mem_alloc(strlen(fn)  + 1); /*Save the name*/
    lv_mem_assert(new_ent->fn_d);
    if(new_ent->fn_d == NULL) return NULL;

    strcpy(new_ent->fn_d, fn);
    new_ent->data_d = NULL;
    new_ent->size = 0;
    new_ent->oc = 0;
    new_ent->const_data = 0;

    return new_ent;
}

#endif /*USE_LV_FILESYSTEM*/

