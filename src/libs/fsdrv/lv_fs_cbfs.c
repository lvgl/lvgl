#include "../../../lvgl.h"
#ifdef LV_USE_FS_CBFS
#if LV_USE_FS_CBFS !=0
#include <stdio.h>
#include "lv_fs_cbfs.h"
#if defined(ARDUINO) && !defined(ESP32)
    #include <avr/pgmspace.h>
#endif

typedef struct cbfs_handle {
    const void * data;
    size_t position;
    size_t size;
} cbfs_handle_t;
static char drive = '\0';
static bool ready_cb(lv_fs_drv_t * drv)
{
    LV_UNUSED(drv);
    return true;
}
static void * open_cb(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    if(mode != LV_FS_MODE_RD) {
        return NULL;
    }
    const char * sz = path;
    if(*sz != '/' && *sz != '\\') {
        return NULL;
    }
    ++sz;
    if(*sz != '0') {
        return NULL;
    }
    ++sz;
    if(*sz != 'x' && *sz != 'X') {
        return NULL;
    }
    ++sz;
    long l = 0;
    char ch = *sz;
    while((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')) {
        int i;
        if(ch <= '9') {
            i = ch - '0';
        }
        else if(ch <= 'F') {
            i = ch - 'A' + 0xA;
        }
        else {
            i = ch - 'a' + 0xa;
        }
        l *= 0x10;
        l += i;
        ++sz;
        ch = *sz;
    }
    if(l == 0 || ch != '-') {
        return NULL;
    }
    void * p = (void *)l;
    ++sz;
    l = 0;
    ch = *sz;
    while(ch >= '0' && ch <= '9') {
        int i = ch - '0';
        l *= 10;
        l += i;
        ++sz;
        ch = *sz;
    }
    size_t size = (size_t)l;
    cbfs_handle_t * h = (cbfs_handle_t *)lv_malloc(sizeof(cbfs_handle_t));
    h->data = p;
    h->size = size;
    h->position = 0;
    return (void *)h;
}
static lv_fs_res_t close_cb(lv_fs_drv_t * drv, void * handle)
{
    LV_UNUSED(drv);
    if(handle != NULL) {
        lv_free(handle);
        return LV_FS_RES_OK;
    }
    return LV_FS_RES_INV_PARAM;
}
static lv_fs_res_t read_cb(lv_fs_drv_t * drv, void * handle, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    if(handle == NULL) {
        return LV_FS_RES_INV_PARAM;
    }
    cbfs_handle_t * h = (cbfs_handle_t *)handle;
    uint32_t remaining = ((uint32_t)h->size) - ((uint32_t)h->position);
    if(btr > remaining) {
        btr = remaining;
    }
#if defined(ARDUINO) && !defined(ESP32)

    size_t left = btr;
    const uint8_t * src = (const uint8_t *)h->data + h->position;
    uint8_t * dst = (uint8_t *)buf;
    while(left--) {
        *dst++ = pgm_read_byte(src++);
    }
#else

    memcpy(buf, ((const uint8_t *)h->data) + h->position, btr);
#endif
    h->position += (size_t)btr;
    *br = btr;
    return LV_FS_RES_OK;
}
static lv_fs_res_t write_cb(lv_fs_drv_t * drv, void * handle, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    LV_UNUSED(handle);
    LV_UNUSED(buf);
    LV_UNUSED(btw);
    LV_UNUSED(bw);
    return LV_FS_RES_DENIED;
}
static lv_fs_res_t seek_cb(lv_fs_drv_t * drv, void * handle, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    if(handle == NULL) {
        return LV_FS_RES_INV_PARAM;
    }
    cbfs_handle_t * h = (cbfs_handle_t *)handle;
    switch(whence) {
        case LV_FS_SEEK_CUR:
            if(pos + h->position > h->size) {
                pos = h->size - h->position;
            }
            h->position = pos;
            return LV_FS_RES_OK;
            break;
        case LV_FS_SEEK_END:
            if(pos > h->size) {
                pos = h->size;
            }
            h->position = h->size - pos;
            return LV_FS_RES_OK;
            break;
        case LV_FS_SEEK_SET:
            if(pos > h->size) {
                pos = h->size;
            }
            h->position = pos;
            return LV_FS_RES_OK;
            break;
    }
    return LV_FS_RES_INV_PARAM;
}

static lv_fs_res_t tell_cb(lv_fs_drv_t * drv, void * handle, uint32_t * pos)
{
    LV_UNUSED(drv);
    if(handle == NULL) {
        return LV_FS_RES_INV_PARAM;
    }
    cbfs_handle_t * h = (cbfs_handle_t *)handle;
    *pos = (uint32_t)h->position;
    return LV_FS_RES_OK;
}
static void * dir_open_cb(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);
    LV_UNUSED(path);
    return NULL;
}
static lv_fs_res_t dir_read_cb(lv_fs_drv_t * drv, void * handle, char * fn)
{
    LV_UNUSED(drv);
    LV_UNUSED(handle);
    LV_UNUSED(fn);
    return LV_FS_RES_NOT_IMP;
}
static lv_fs_res_t dir_close_cb(lv_fs_drv_t * drv, void * handle)
{
    LV_UNUSED(drv);
    LV_UNUSED(handle);
    return LV_FS_RES_NOT_IMP;
}
lv_fs_res_t lv_fs_cbfs_create(char * out_path, size_t out_path_size, const void * data, size_t data_size)
{
    if(drive == 0) {
        return LV_FS_RES_NOT_EX;
    }
    if(out_path == NULL || data == NULL) {
        return LV_FS_RES_INV_PARAM;
    }
    snprintf(out_path, out_path_size, "%c:/0x%lx-%li", drive, (long)data, (long)data_size);
    return LV_FS_RES_INV_PARAM;


}
void lv_fs_cbfs_init(void)
{
    if(drive != '\0') {
        LV_LOG_ERROR("CBFS is already initialized");
        return;
    }
    int i;
#if defined(LV_FS_CBFS_LETTER) && LV_FS_CBFS_LETTER >= 'A' && LV_FS_CBFS_LETTER <= 'Z'
    i = LV_FS_CBFS_LETTER;
#else
    char drives[27];
    lv_fs_get_letters(drives);
    int drive_count = strlen(drives);
    for(i = 'A'; i <= 'Z'; ++i) {
        int found = 0;
        for(int j = 0; j < drive_count; ++j) {
            if(drives[j] == i) {
                found = 1;
                break;
            }
        }
        if(!found) {
            break;
        }
    }
    if(i > 'Z') {
        LV_LOG_ERROR("CBFS found no more drives available");
        return;
    }
#endif
    drive = (char)i;

    static lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);                     /*Basic initialization*/
    drv.letter = drive;                         /*An uppercase letter to identify the drive */
    drv.cache_size = 0;          /*Cache size for reading in bytes. 0 to not cache.*/

    drv.ready_cb = ready_cb;               /*Callback to tell if the drive is ready to use */
    drv.open_cb = open_cb;                 /*Callback to open a file */
    drv.close_cb = close_cb;               /*Callback to close a file */
    drv.read_cb = read_cb;                 /*Callback to read a file */
    drv.write_cb = write_cb;               /*Callback to write a file */
    drv.seek_cb = seek_cb;                 /*Callback to seek in a file (Move cursor) */
    drv.tell_cb = tell_cb;                 /*Callback to tell the cursor position  */

    drv.dir_open_cb = dir_open_cb;         /*Callback to open directory to read its content */
    drv.dir_read_cb = dir_read_cb;         /*Callback to read a directory's content */
    drv.dir_close_cb = dir_close_cb;       /*Callback to close a directory */
    lv_fs_drv_register(&drv);                 /*Finally register the drive*/
}
#endif // LV_USE_FS_CBFS != 0 
#endif // #ifdef LV_USE_FS_CBFS