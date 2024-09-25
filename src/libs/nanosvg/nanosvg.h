/**
 * @file nanosvg.h
 *
 */

#ifndef NANOSVG_H
#define NANOSVG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_NANOSVG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    NSVG_PAINT_UNDEF = -1,
    NSVG_PAINT_NONE = 0,
    NSVG_PAINT_COLOR = 1,
    NSVG_PAINT_LINEAR_GRADIENT = 2,
    NSVG_PAINT_RADIAL_GRADIENT = 3
} NSVGpaintType;

typedef enum {
    NSVG_SPREAD_PAD = 0,
    NSVG_SPREAD_REFLECT = 1,
    NSVG_SPREAD_REPEAT = 2
} NSVGspreadType;

typedef enum {
    NSVG_JOIN_MITER = 0,
    NSVG_JOIN_ROUND = 1,
    NSVG_JOIN_BEVEL = 2
} NSVGlineJoin;

typedef enum {
    NSVG_CAP_BUTT = 0,
    NSVG_CAP_ROUND = 1,
    NSVG_CAP_SQUARE = 2
} NSVGlineCap;

typedef enum {
    NSVG_FILLRULE_NONZERO = 0,
    NSVG_FILLRULE_EVENODD = 1
} NSVGfillRule;

typedef enum {
    NSVG_FLAGS_VISIBLE = 0x01
} NSVGflags;

typedef struct {
    unsigned int color;
    float offset;
} NSVGgradientStop;

typedef struct {
    float xform[6];
    char spread;
    float fx, fy;
    int nstops;
    NSVGgradientStop stops[1];
} NSVGgradient;

typedef struct {
    signed char type;
    union {
        unsigned int color;
        NSVGgradient * gradient;
    } paint_data;
} NSVGpaint;

typedef struct NSVGpath {
    float * pts;                // Cubic bezier points: x0,y0, [cpx1,cpx1,cpx2,cpy2,x1,y1], ...
    int npts;                   // Total number of bezier points.
    char closed;                // Flag indicating if shapes should be treated as closed.
    float bounds[4];            // Tight bounding box of the shape [minx,miny,maxx,maxy].
    struct NSVGpath * next;     // Pointer to next path, or NULL if last element.
} NSVGpath;

typedef struct NSVGshape {
    char id[64];                // Optional 'id' attr of the shape or its group
    NSVGpaint fill;             // Fill paint
    NSVGpaint stroke;           // Stroke paint
    float opacity;              // Opacity of the shape.
    float strokeWidth;          // Stroke width (scaled).
    float strokeDashOffset;     // Stroke dash offset (scaled).
    float strokeDashArray[8];   // Stroke dash array (scaled).
    char strokeDashCount;       // Number of dash values in dash array.
    char strokeLineJoin;        // Stroke join type.
    char strokeLineCap;         // Stroke cap type.
    float miterLimit;           // Miter limit
    char fillRule;              // Fill rule, see NSVGfillRule.
    unsigned char flags;        // Logical or of NSVG_FLAGS_* flags
    float bounds[4];            // Tight bounding box of the shape [minx,miny,maxx,maxy].
    char fillGradient[64];      // Optional 'id' of fill gradient
    char strokeGradient[64];    // Optional 'id' of stroke gradient
    float xform[6];             // Root transformation for fill/stroke gradient
    NSVGpath * paths;           // Linked list of paths in the image.
    struct NSVGshape * next;    // Pointer to next shape, or NULL if last element.
} NSVGshape;

typedef struct {
    float width;                // Width of the image.
    float height;               // Height of the image.
    NSVGshape * shapes;         // Linked list of shapes in the image.
} NSVGimage;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

// Parses SVG file from a null terminated string, returns SVG image as paths.
// Important note: changes the string.
NSVGimage * nsvgParse(char * input, const char * units, float dpi);

// Parses SVG file from a file, returns SVG image as paths.
NSVGimage * nsvgParseFromFile(const char * filename, const char * units, float dpi);

// Duplicates a path.
NSVGpath * nsvgDuplicatePath(NSVGpath * p);

// Deletes an image.
void nsvgDelete(NSVGimage * image);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_NANOSVG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*NANOSVG_H*/
