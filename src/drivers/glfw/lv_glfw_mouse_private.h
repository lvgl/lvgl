/**
 * @file lv_glfw_mouse_private.h
 *
 */

#ifndef LV_GLFW_MOUSE_PRIVATE_H
#define LV_GLFW_MOUSE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#if LV_USE_OPENGLES

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    int16_t last_x;
    int16_t last_y;
    bool left_button_down;
    int32_t diff;
} lv_glfw_mouse_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_glfw_mouse_move_handler(lv_display_t * disp, int x, int y);

void lv_glfw_mouse_btn_handler(lv_display_t * disp, int btn_down);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_GLFW_MOUSE_PRIVATE_H */
