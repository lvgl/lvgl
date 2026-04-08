/**
 * @file lv_file_explorer_private.h
 *
 */

#ifndef LV_FILE_EXPLORER_PRIVATE_H
#define LV_FILE_EXPLORER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_file_explorer.h"

#if LV_USE_FILE_EXPLORER != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of canvas*/
struct _lv_file_explorer_t {
    lv_obj_t obj;
    lv_obj_t * cont;
    lv_obj_t * head_area;
    lv_obj_t * browser_area;
    lv_obj_t * file_table;
    lv_obj_t * path_label;
#if LV_FILE_EXPLORER_QUICK_ACCESS
    lv_obj_t * quick_access_area;
    lv_obj_t * list_device;
    lv_obj_t * list_places;
    char * home_dir;
    char * music_dir;
    char * pictures_dir;
    char * video_dir;
    char * docs_dir;
    char * fs_dir;
#endif
    const char * sel_fn;
    char   current_path[LV_FILE_EXPLORER_PATH_MAX_LEN];
    lv_file_explorer_sort_t sort;
    bool show_back_button;
};

typedef enum {
    LV_FILE_EXPLORER_FILE_KIND_DIR,
    LV_FILE_EXPLORER_FILE_KIND_IMAGE,
    LV_FILE_EXPLORER_FILE_KIND_AUDIO,
    LV_FILE_EXPLORER_FILE_KIND_VIDEO,
    LV_FILE_EXPLORER_FILE_KIND_FILE
} lv_file_explorer_file_kind_t;

typedef struct {
    lv_file_explorer_file_kind_t file_kind;
} lv_file_explorer_file_table_entry_data_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_FILE_EXPLORER != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FILE_EXPLORER_PRIVATE_H*/
