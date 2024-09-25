/**
 * @file nanosvgrast.h
 *
 */

#ifndef NANOSVGRAST_H
#define NANOSVGRAST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "nanosvg.h"
#if LV_USE_NANOSVG

/*********************
 *      DEFINES
 *********************/

#define NSVG__MEMPAGE_SIZE  1024

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    float x0, y0, x1, y1;
    int dir;
    struct NSVGedge * next;
} NSVGedge;

typedef struct NSVGactiveEdge {
    int x, dx;
    float ey;
    int dir;
    struct NSVGactiveEdge * next;
} NSVGactiveEdge;

typedef struct {
    float x, y;
    float dx, dy;
    float len;
    float dmx, dmy;
    unsigned char flags;
} NSVGpoint;

typedef struct NSVGmemPage {
    unsigned char mem[NSVG__MEMPAGE_SIZE];
    int size;
    struct NSVGmemPage * next;
} NSVGmemPage;

typedef struct {
    float px, py;

    float tessTol;
    float distTol;

    NSVGedge * edges;
    int nedges;
    int cedges;

    NSVGpoint * points;
    int npoints;
    int cpoints;

    NSVGpoint * points2;
    int npoints2;
    int cpoints2;

    NSVGactiveEdge * freelist;
    NSVGmemPage * pages;
    NSVGmemPage * curpage;

    unsigned char * scanline;
    int cscanline;

    unsigned char * bitmap;
    int width, height, stride;
} NSVGrasterizer;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

// Allocated rasterizer context.
NSVGrasterizer * nsvgCreateRasterizer(void);

// Rasterizes SVG image, returns RGBA image (non-premultiplied alpha)
//   r - pointer to rasterizer context
//   image - pointer to image to rasterize
//   tx,ty - image offset (applied after scaling)
//   scale - image scale
//   dst - pointer to destination image data, 4 bytes per pixel (RGBA)
//   w - width of the image to render
//   h - height of the image to render
//   stride - number of bytes per scaleline in the destination buffer
void nsvgRasterize(NSVGrasterizer * r,
                   NSVGimage * image, float tx, float ty, float scale,
                   unsigned char * dst, int w, int h, int stride);

// Deletes rasterizer context.
void nsvgDeleteRasterizer(NSVGrasterizer *);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_NANOSVG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*NANOSVGRAST_H*/
