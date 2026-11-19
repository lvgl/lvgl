/**
 * @file lv_fs_freertos_fat.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_fs_private.h"
#include "../core/lv_global.h"

#if LV_USE_FS_FREERTOS_FAT
#include <ff_stdio.h>

/*********************
 *      DEFINES
 *********************/
#if !LV_FS_IS_VALID_LETTER(LV_FS_FREERTOS_FAT_LETTER)
    #error "Invalid drive letter"
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    FF_FindData_t find_data;
    char path[LV_FS_MAX_PATH_LEN];
    bool first;
    bool end;
} freertos_fat_dir_t;

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
 * Register the FreeRTOS+FAT file system driver.
 * Initialize and mount the storage device in the application before accessing files.
 */
void lv_fs_freertos_fat_init(void)
{
    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    lv_fs_drv_t * fs_drv_p = &(LV_GLOBAL_DEFAULT()->freertos_fat_fs_drv);
    lv_fs_drv_init(fs_drv_p);

    fs_drv_p->letter = LV_FS_FREERTOS_FAT_LETTER;
    fs_drv_p->cache_size = LV_FS_FREERTOS_FAT_CACHE_SIZE;

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
 * @param mode  read: LV_FS_MODE_RD, write: LV_FS_MODE_WR, both: LV_FS_MODE_RD | LV_FS_MODE_WR
 * @return      pointer to an FF_FILE or NULL on error
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_ASSERT(path != NULL);
    LV_UNUSED(drv);
    const char * flags = "";

    if(mode == LV_FS_MODE_WR) flags = "w";
    else if(mode == LV_FS_MODE_RD) flags = "r";
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = "r+";
    else return NULL;

    char buf[LV_FS_MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_FREERTOS_FAT_PATH "%s", path);

    return ff_fopen(buf, flags);
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to an FF_FILE variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_ASSERT(file_p != NULL);
    LV_UNUSED(drv);
    ff_fclose(file_p);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to an FF_FILE variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_ASSERT(file_p != NULL);
    LV_ASSERT(br != NULL);
    LV_ASSERT(buf != NULL || btr == 0);
    LV_UNUSED(drv);
    *br = 0;
    if(btr == 0) return LV_FS_RES_OK;
    *br = (uint32_t)ff_fread(buf, 1, btr, file_p);
    if(stdioGET_ERRNO() == 0) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to an FF_FILE variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return          LV_FS_RES_OK: no error or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    LV_ASSERT(file_p != NULL);
    LV_ASSERT(buf != NULL || btw == 0);
    LV_ASSERT(bw != NULL);
    *bw = 0;
    if(btw == 0) return LV_FS_RES_OK;
    *bw = (uint32_t)ff_fwrite(buf, 1, btw, file_p);
    if(stdioGET_ERRNO() == 0) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * Set the read write pointer.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to an FF_FILE variable. (opened with fs_open)
 * @param pos       the offset of the read write pointer; for LV_FS_SEEK_END, bytes before the end
 * @param whence    tells from where to interpret the `pos`. See lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_ASSERT(file_p != NULL);
    LV_UNUSED(drv);
    if(pos > INT32_MAX) return LV_FS_RES_INV_PARAM;
    int w;
    long offset = (long)pos;
    switch(whence) {
        case LV_FS_SEEK_SET:
            w = FF_SEEK_SET;
            break;
        case LV_FS_SEEK_CUR:
            w = FF_SEEK_CUR;
            break;
        case LV_FS_SEEK_END:
            w = FF_SEEK_END;
            offset = -(long)pos;
            break;
        default:
            return LV_FS_RES_INV_PARAM;
    }
    int res = ff_fseek(file_p, offset, w);
    return res < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to an FF_FILE variable
 * @param pos_p     pointer to store the result
 * @return          LV_FS_RES_OK: no error or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_ASSERT(file_p != NULL);
    LV_ASSERT(pos_p != NULL);
    LV_UNUSED(drv);
    long pos = ff_ftell(file_p);
    if(pos < 0) return LV_FS_RES_UNKNOWN;
    *pos_p = (uint32_t)pos;
    return LV_FS_RES_OK;
}

/**
 * Initialize a 'freertos_fat_dir_t' variable for directory reading
 * @param drv   pointer to a driver where this function belongs
 * @param path  path to a directory
 * @return      pointer to an initialized 'freertos_fat_dir_t' variable
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_ASSERT(path != NULL);
    LV_UNUSED(drv);
    freertos_fat_dir_t * dir = lv_malloc(sizeof(*dir));
    if(dir == NULL) return NULL;

    /* FreeRTOS+FAT may retain a pointer to the path during directory iteration. */
    lv_snprintf(dir->path, sizeof(dir->path), LV_FS_FREERTOS_FAT_PATH "%s", path);

    int res = ff_findfirst(dir->path, &dir->find_data);
    if(res != 0 && FF_GETERROR(res) != FF_ERR_DIR_END_OF_DIR) {
        lv_free(dir);
        return NULL;
    }
    dir->first = res == 0;
    dir->end = res != 0;
    return dir;
}

/**
 * Read the next filename from a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param dir_p     pointer to an initialized 'freertos_fat_dir_t' variable
 * @param fn        pointer to a buffer to store the filename
 * @param fn_len    length of the buffer to store the filename
 * @return          LV_FS_RES_OK: no error or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len)
{
    LV_ASSERT(dir_p != NULL);
    LV_ASSERT(fn != NULL);
    LV_UNUSED(drv);
    LV_ASSERT(fn_len > 0);

    freertos_fat_dir_t * dir = dir_p;
    fn[0] = '\0';
    if(dir->end) return LV_FS_RES_OK;

    do {
        if(dir->first) dir->first = false;
        else {
            int res = ff_findnext(&dir->find_data);
            if(res != 0) {
                if(FF_GETERROR(res) != FF_ERR_DIR_END_OF_DIR) return LV_FS_RES_UNKNOWN;
                dir->end = true;
                return LV_FS_RES_OK;
            }
        }
        const char * name = dir->find_data.pcFileName;
        if(name == NULL || name[0] == '\0') return LV_FS_RES_UNKNOWN;

        if(dir->find_data.ucAttributes & FF_FA_DIREC) {
            lv_snprintf(fn, fn_len, "/%s", name);
        }
        else lv_strlcpy(fn, name, fn_len);

    } while(lv_strcmp(fn, "/.") == 0 || lv_strcmp(fn, "/..") == 0);

    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param dir_p     pointer to an initialized 'freertos_fat_dir_t' variable
 * @return          LV_FS_RES_OK: no error or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p)
{
    LV_ASSERT(dir_p != NULL);
    LV_UNUSED(drv);
    /* The ff_findfirst/ff_findnext API has no close operation. */
    lv_free(dir_p);
    return LV_FS_RES_OK;
}

#else /*LV_USE_FS_FREERTOS_FAT == 0*/

#if defined(LV_FS_FREERTOS_FAT_LETTER) && LV_FS_FREERTOS_FAT_LETTER != '\0'
    #warning "LV_USE_FS_FREERTOS_FAT is not enabled but LV_FS_FREERTOS_FAT_LETTER is set"
#endif

#endif /*LV_USE_FS_FREERTOS_FAT*/
