/**
 * @file nuttx_input_mouse.h
 *
 */

#ifndef NUTTX_INPUT_MOUSE_H
#define NUTTX_INPUT_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/* These definitions provide the meaning of all of the bits that may be
 * reported in the struct mouse_report_s buttons.
 */

#define MOUSE_BUTTON_1       (1 << 0) /* True: Left mouse button pressed */
#define MOUSE_BUTTON_2       (1 << 1) /* True: Right mouse button pressed */
#define MOUSE_BUTTON_3       (1 << 2) /* True: Middle mouse button pressed */

/**********************
 *      TYPEDEFS
 **********************/

struct mouse_report_s {
    uint8_t buttons;  /* See MOUSE_* definitions above */
    uint8_t dummy;    /* Padded with 1 byte here */
    int16_t x;        /* X coordinate of the mouse position */
    int16_t y;        /* Y coordinate of the mouse position */
    int16_t wheel;    /* Mouse wheel position */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*NUTTX_INPUT_MOUSE_H*/
