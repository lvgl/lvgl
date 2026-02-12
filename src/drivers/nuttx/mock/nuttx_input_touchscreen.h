/**
 * @file nuttx_input_touchscreen.h
 *
 */

#ifndef NUTTX_INPUT_TOUCHSCREEN_H
#define NUTTX_INPUT_TOUCHSCREEN_H

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

#define _TSIOC(x) (x)

/* Common TSC IOCTL commands */

#define TSIOC_SETXRCAL       _TSIOC(0x0001) /* arg: Pointer to
                                             * int Xplate R calibration value
                                             */
#define TSIOC_GETXRCAL       _TSIOC(0x0002) /* arg: Pointer to
                                             * int Xplate R calibration value
                                             */
#define TSIOC_SETFREQUENCY   _TSIOC(0x0003) /* arg: Pointer to
                                             * uint32_t frequency value
                                             */
#define TSIOC_GETFREQUENCY   _TSIOC(0x0004) /* arg: Pointer to
                                             *  uint32_t frequency value
                                             */
#define TSIOC_GETFWVERSION   _TSIOC(0x0005) /* arg: Pointer to
                                             * uint32_t firmware version
                                             * value
                                             * */
#define TSIOC_ENABLEGESTURE  _TSIOC(0x0006) /* arg: Pointer to
                                             * int for enable gesture feature
                                             */
#define TSIOC_DOACALIB       _TSIOC(0x0007) /* arg: none.
                                             * Initiate TS auto calibration
                                             */
#define TSIOC_CALDATA        _TSIOC(0x0008) /* arg: Pointer to
                                             * struct g_tscaldata_s
                                             */
#define TSIOC_USESCALED      _TSIOC(0x0009) /* arg: bool, yes/no */
#define TSIOC_GETOFFSETX     _TSIOC(0x000a) /* arg: Pointer to
                                             * int X offset value
                                             */
#define TSIOC_GETOFFSETY     _TSIOC(0x000b) /* arg: Pointer to
                                             * int Y offset value
                                             */
#define TSIOC_GETTHRESHX     _TSIOC(0x000c) /* arg: Pointer to
                                             * int X threshold value
                                             */
#define TSIOC_GETTHRESHY     _TSIOC(0x000d) /* arg: Pointer to
                                             * int Y threshold value
                                             */

#define TSIOC_GRAB           _TSIOC(0x000e) /* arg: Pointer to
                                             * int for enable grab
                                             */

#define TSIOC_GETMAXPOINTS   _TSIOC(0x000f) /* arg: Pointer to
                                             * uint8_t max touch point
                                             */
#define TSIOC_GETRESOLUTION  _TSIOC(0x0010) /* arg: Pointer to
                                             * struct touch_resolution_s
                                             */

/* These definitions provide the meaning of all of the bits that may be
 * reported in the struct touch_point_s flags.
 */

#define TOUCH_DOWN           (1 << 0) /* A new touch contact is established */
#define TOUCH_MOVE           (1 << 1) /* Movement occurred with previously reported contact */
#define TOUCH_UP             (1 << 2) /* The touch contact was lost */
#define TOUCH_ID_VALID       (1 << 3) /* Touch ID is certain */
#define TOUCH_POS_VALID      (1 << 4) /* Hardware provided a valid X/Y position */
#define TOUCH_PRESSURE_VALID (1 << 5) /* Hardware provided a valid pressure */
#define TOUCH_SIZE_VALID     (1 << 6) /* Hardware provided a valid H/W contact size */
#define TOUCH_GESTURE_VALID  (1 << 7) /* Hardware provided a valid gesture */

/**********************
 *      TYPEDEFS
 **********************/

struct touch_point_s {
    uint8_t  id;        /* Unique identifies contact; Same in all reports for the contact */
    uint8_t  flags;     /* See TOUCH_* definitions above */
    int16_t  x;         /* X coordinate of the touch point (uncalibrated) */
    int16_t  y;         /* Y coordinate of the touch point (uncalibrated) */
    int16_t  h;         /* Height of touch point (uncalibrated) */
    int16_t  w;         /* Width of touch point (uncalibrated) */
    uint16_t gesture;   /* Gesture of touchscreen contact */
    uint16_t pressure;  /* Touch pressure */
    uint16_t dummy;     /* Padded with 2 bytes here */
    uint64_t timestamp; /* Touch event time stamp, in microseconds */
};

struct touch_sample_s {
    int32_t npoints;               /* The number of touch points in point[] */
    int32_t dummy;                 /* Padded with 4 bytes here */
    struct touch_point_s point[1]; /* Actual dimension is npoints */
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

#endif /*NUTTX_INPUT_TOUCHSCREEN_H*/
