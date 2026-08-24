/**
 * @file lv_api_map_v9_5.h
 *
 */

#ifndef LV_API_MAP_V9_5_H
#define LV_API_MAP_V9_5_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Shipped in v9.5.0 through lv_api_map_v8.h, where it was an incorrect spelling of v8's
 * `lv_disp_rot_t`. Correcting that in #10095 also removed the only definition of this
 * name, so code written against v9.5.0 no longer compiles. Kept here for compatibility. */
typedef lv_display_rotation_t lv_disp_rotation_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#define lv_display_delete_event lv_display_remove_event
#define lv_observer_remove lv_observer_delete

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_API_MAP_V9_5_H */
