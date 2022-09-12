#ifndef LV_FS_CBFS_H
#define LV_FS_CBFS_H
#include "../../../lvgl.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
lv_fs_res_t lv_fs_cbfs_create(char * out_path, size_t out_path_size, const void * data, size_t data_size);
#ifdef __cplusplus
}
#endif
#endif // LV_FS_CBFS_H