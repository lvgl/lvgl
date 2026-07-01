/**
 * @file lv_file_explorer.h
 *
 */

#ifndef LV_FILE_EXPLORER_H
#define LV_FILE_EXPLORER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../config/lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_FILE_EXPLORER != 0

/*********************
 *      DEFINES
 *********************/

/**
 * @deprecated The `lv_file_explorer` widget is deprecated and kept only for
 * backward compatibility. A file explorer is a path header plus a table of
 * directory entries read with the `lv_fs` API, so build it directly instead. See
 * the `lv_example_table_file_browser` example. All `lv_file_explorer_*`
 * functions below are deprecated.
 */
#define LV_FILE_EXPLORER_DEPRECATED_MSG \
    "lv_file_explorer is deprecated; build a file browser from a table + lv_fs instead. See the lv_example_table_file_browser example."

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_EXPLORER_SORT_NONE,
    LV_EXPLORER_SORT_KIND,
} lv_file_explorer_sort_t;

#if LV_FILE_EXPLORER_QUICK_ACCESS
typedef enum {
    LV_EXPLORER_HOME_DIR,
    LV_EXPLORER_MUSIC_DIR,
    LV_EXPLORER_PICTURES_DIR,
    LV_EXPLORER_VIDEO_DIR,
    LV_EXPLORER_DOCS_DIR,
    LV_EXPLORER_FS_DIR,
} lv_file_explorer_dir_t;
#endif

extern const lv_obj_class_t lv_file_explorer_class;

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a file explorer object
 * @param parent    pointer to an object, it will be the parent of the new explorer
 * @return          pointer to the created file explorer
 * @deprecated The `lv_file_explorer` widget is deprecated. Build a file browser from
 *             a table and the `lv_fs` API instead. See `lv_example_table_file_browser`.
 */
LV_DEPRECATED(LV_FILE_EXPLORER_DEPRECATED_MSG)
lv_obj_t * lv_file_explorer_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

#if LV_FILE_EXPLORER_QUICK_ACCESS
/**
 * Set file_explorer
 * @param obj   pointer to a label object
 * @param dir   the dir from 'lv_file_explorer_dir_t' enum.
 * @param path   path

 */
void lv_file_explorer_set_quick_access_path(lv_obj_t * obj, lv_file_explorer_dir_t dir, const char * path);
#endif

/**
 * Set file_explorer sort
 * @param obj   pointer to a file explorer object
 * @param sort  the sort from 'lv_file_explorer_sort_t' enum.
 */
void lv_file_explorer_set_sort(lv_obj_t * obj, lv_file_explorer_sort_t sort);

/**
 * Set the visibility of the "< Back" button
 * @param obj   pointer to a file explorer object
 * @param show  bool true/false, enable or disable button
 */
void lv_file_explorer_show_back_button(lv_obj_t * obj, bool show);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get file explorer Selected file
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer selected file name
 */
const char * lv_file_explorer_get_selected_file_name(const lv_obj_t * obj);

/**
 * Get file explorer cur path
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer cur path
 */
const char * lv_file_explorer_get_current_path(const lv_obj_t * obj);

/**
 * Get file explorer file list obj(lv_table)
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer file table obj(lv_table)
 */
lv_obj_t * lv_file_explorer_get_file_table(lv_obj_t * obj);

/**
 * Get file explorer head area obj
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer head area obj(lv_obj)
 */
lv_obj_t * lv_file_explorer_get_header(lv_obj_t * obj);

/**
 * Get file explorer path obj(label)
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer path obj(lv_label)
 */
lv_obj_t * lv_file_explorer_get_path_label(lv_obj_t * obj);

#if LV_FILE_EXPLORER_QUICK_ACCESS
/**
 * Get file explorer head area obj
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer quick access area obj(lv_obj)
 */
lv_obj_t * lv_file_explorer_get_quick_access_area(lv_obj_t * obj);

/**
 * Get file explorer places list obj(lv_list)
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer places list obj(lv_list)
 */
lv_obj_t * lv_file_explorer_get_places_list(lv_obj_t * obj);

/**
 * Get file explorer device list obj(lv_list)
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer device list obj(lv_list)
 */
lv_obj_t * lv_file_explorer_get_device_list(lv_obj_t * obj);
#endif

/**
 * Set file_explorer sort
 * @param obj   pointer to a file explorer object
 * @return the current mode from 'lv_file_explorer_sort_t'
 */
lv_file_explorer_sort_t lv_file_explorer_get_sort(const lv_obj_t * obj);

/*=====================
 * Other functions
 *====================*/

/**
 * Open a specified path
 * @param obj   pointer to a file explorer object
 * @param dir   pointer to the path
 */
void lv_file_explorer_open_dir(lv_obj_t * obj, const char * dir);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_FILE_EXPLORER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FILE_EXPLORER_H*/
