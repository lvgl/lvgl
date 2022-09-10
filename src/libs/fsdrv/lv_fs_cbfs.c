#include "lv_fs_cbfs.h"
#include "../../../lvgl.h"
#if defined(ARDUINO) && !defined(ESP32)
#include <avr/pgmspace.h>
#endif
typedef struct cbfs_info {
    lv_fs_drv_t drv;
    const void* data;
    size_t position;
    size_t size;
} cbfs_info_t;

static cbfs_info_t infos[CONST_BUFFER_FS_DRV_MAX_SLOTS];
static bool ready_cb(lv_fs_drv_t* drv) {
    return true;
}
static void* open_cb(lv_fs_drv_t* drv,const char* path, lv_fs_mode_t mode) {
    if(mode!=LV_FS_MODE_RD) {
        return NULL;
    }
    return (cbfs_info_t*)drv->user_data;
}
static lv_fs_res_t close_cb(lv_fs_drv_t* drv,void* handle) {
    return LV_FS_RES_OK;
}
static lv_fs_res_t read_cb(lv_fs_drv_t* drv,void* handle,void* buf,uint32_t btr,uint32_t* br) {
    cbfs_info_t* info = (cbfs_info_t*)drv->user_data;
    uint32_t remaining = ((uint32_t)info->size)-((uint32_t)info->position);
    if(btr>remaining) {
        btr = remaining;
    }
#if defined(ARDUINO) && !defined(ESP32)

    size_t left=btr;
    const uint8_t* src = (const uint8_t*)info->data+info->position;
    uint8_t* dst = (uint8_t*)buf;
    while(left--) {
        *dst++=pgm_read_byte(src++);
    }
#else
    memcpy(buf,info->data+info->position,btr);
#endif
    info->position+=(size_t)btr;
    *br = btr;
    return LV_FS_RES_OK;
}
static lv_fs_res_t write_cb(lv_fs_drv_t* drv,void* handle,const void* buf,uint32_t btw,uint32_t* bw) {
    return LV_FS_RES_DENIED;
}
static lv_fs_res_t seek_cb(lv_fs_drv_t* drv,void* handle,uint32_t pos,lv_fs_whence_t whence) {
    cbfs_info_t* info = (cbfs_info_t*)drv->user_data;
    switch(whence) {
        case LV_FS_SEEK_CUR:
            if(pos+info->position>info->size) {
                pos=info->size-info->position;
            }
            info->position = pos;
            return LV_FS_RES_OK;
        break;
        case LV_FS_SEEK_END:
            if(pos>info->size) {
                pos=info->size;
            }
            info->position = info->size-pos;
            return LV_FS_RES_OK;
        break;
        case LV_FS_SEEK_SET:
            if(pos>info->size) {
                pos=info->size;
            }
            info->position = pos;
            return LV_FS_RES_OK;
        break;
    }
    return LV_FS_RES_INV_PARAM;
}

static lv_fs_res_t tell_cb(lv_fs_drv_t* drv,void* handle,uint32_t *pos) {
    cbfs_info_t* info = (cbfs_info_t*)drv->user_data;
    *pos = (uint32_t)info->position;
    return LV_FS_RES_OK;
}
static void* dir_open_cb(lv_fs_drv_t* drv,const char *path) {
    return NULL;
}
static lv_fs_res_t dir_read_cb(lv_fs_drv_t* drv,void* handle,char* fn) {
    return LV_FS_RES_NOT_IMP;
}
static lv_fs_res_t dir_close_cb(lv_fs_drv_t* drv,void* handle) {
    return LV_FS_RES_NOT_IMP;
}
void lv_cbfs_init(size_t slot,char drive,const void* data, size_t size) {
    if(slot<0 || slot>=CONST_BUFFER_FS_DRV_MAX_SLOTS) {
        return;
    }
    cbfs_info_t* info = infos+slot;
    lv_fs_drv_init(&info->drv);                     /*Basic initialization*/
    info->data = data;
    info->size = size;
    info->position = 0;
    info->drv.letter = drive;                         /*An uppercase letter to identify the drive */
    info->drv.cache_size =0;           /*Cache size for reading in bytes. 0 to not cache.*/

    info->drv.ready_cb = ready_cb;               /*Callback to tell if the drive is ready to use */
    info->drv.open_cb = open_cb;                 /*Callback to open a file */
    info->drv.close_cb = close_cb;               /*Callback to close a file */
    info->drv.read_cb = read_cb;                 /*Callback to read a file */
    info->drv.write_cb = write_cb;               /*Callback to write a file */
    info->drv.seek_cb = seek_cb;                 /*Callback to seek in a file (Move cursor) */
    info->drv.tell_cb = tell_cb;                 /*Callback to tell the cursor position  */

    info->drv.dir_open_cb = dir_open_cb;         /*Callback to open directory to read its content */
    info->drv.dir_read_cb = dir_read_cb;         /*Callback to read a directory's content */
    info->drv.dir_close_cb = dir_close_cb;       /*Callback to close a directory */

    info->drv.user_data = info;             /*Any custom data if required*/

    lv_fs_drv_register(&info->drv);                 /*Finally register the drive*/
}