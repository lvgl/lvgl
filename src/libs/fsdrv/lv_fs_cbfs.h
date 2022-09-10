#ifndef LV_FS_CBFS_H
#define LV_FS_CBFS_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#ifndef CONST_BUFFER_FS_DRV_MAX_SLOTS
#define CONST_BUFFER_FS_DRV_MAX_SLOTS 3
#endif
void lv_fs_cbfs_init(size_t slot,char drive,const void* data, size_t size);
#ifdef __cplusplus
}
#endif
#endif // LV_FS_CBFS_H