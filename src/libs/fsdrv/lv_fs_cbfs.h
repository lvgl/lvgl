#ifndef LV_FS_CBFS_H
#define LV_FS_CBFS_H
#ifdef LV_USE_FS_CBFS
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
// declared in lv_fsdrv.h:
//void lv_fs_cbfs_init();
lv_fs_res_t lv_fs_cbfs_create_file(char* out_path,size_t out_path_size, const void* data, size_t data_size);
#ifdef __cplusplus
}
#endif
#endif // LV_USE_FS_CBFS
#endif // LV_FS_CBFS_H