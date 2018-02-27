/**
 * @file lv_draw.h
 * 
 */

#ifndef LV_DRAW_H
#define LV_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_core/lv_style.h"
#include "../lv_misc/lv_txt.h"

/*********************
 *      DEFINES
 *********************/
/*If image pixels contains alpha we need to know how much byte is a pixel*/
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
# define LV_IMG_PX_SIZE_ALPHA_BYTE   2
#elif LV_COLOR_DEPTH == 16
# define LV_IMG_PX_SIZE_ALPHA_BYTE   3
#elif LV_COLOR_DEPTH == 24
# define LV_IMG_PX_SIZE_ALPHA_BYTE   4
#endif

/**********************
 *      TYPEDEFS
 **********************/ 

/* Image header it is compatible with
 * the result image converter utility*/
typedef struct
{
    union{
        struct {
            uint32_t chroma_keyed:1;    /*1: The image contains transparent pixels with LV_COLOR_TRANSP color*/
            uint32_t alpha_byte  :1;    /*Every pixel is extended with a 8 bit alpha channel*/
            uint32_t format      :6;    /*See: lv_img_px_format*/
            uint32_t w:12;              /*Width of the image map*/
            uint32_t h:12;              /*Height of the image map*/
        }header;
        uint8_t src_type;
    };

    union {
        const uint8_t * pixel_map;  /*For internal images (c arrays) pointer to the pixels array*/
        uint8_t first_pixel;        /*For external images (binary) the first byte of the pixels (just for convenient)*/
    };
}lv_img_t;

typedef enum {
    LV_IMG_FORMAT_UNKOWN = 0,
    LV_IMG_FORMAT_INTERNAL_RAW,       /*'lv_img_t' variable compiled with the code*/
    LV_IMG_FORMAT_FILE_RAW_RGB332,    /*8 bit*/
    LV_IMG_FORMAT_FILE_RAW_RGB565,    /*16 bit*/
    LV_IMG_FORMAT_FILE_RAW_RGB888,    /*24 bit (stored on 32 bit)*/
}lv_img_format_t;


typedef enum {
    LV_IMG_SRC_VARIABLE,
    LV_IMG_SRC_FILE,
    LV_IMG_SRC_SYMBOL,
    LV_IMG_SRC_UNKNOWN,
}lv_img_src_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Draw a rectangle
 * @param cords_p the coordinates of the rectangle
 * @param mask_p the rectangle will be drawn only in this mask
 * @param style_p pointer to a style
 */
void lv_draw_rect(const lv_area_t * cords_p, const lv_area_t * mask_p, const lv_style_t * style_p);


/*Experimental use for 3D modeling*/
#define USE_LV_TRIANGLE 0
#if USE_LV_TRIANGLE != 0
/**
 *
 * @param points pointer to an array with 3 points
 * @param mask_p the triangle will be drawn only in this mask
 * @param color color of the triangle
 */
void lv_draw_triangle(const lv_point_t * points, const lv_area_t * mask_p, lv_color_t color);
#endif

/**
 * Write a text
 * @param cords_p coordinates of the label
 * @param mask_p the label will be drawn only in this area
 * @param style_p pointer to a style
 * @param txt 0 terminated text to write
 * @param flags settings for the text from 'txt_flag_t' enum
 * @param offset text offset in x and y direction (NULL if unused)
 */
void lv_draw_label(const lv_area_t * cords_p,const lv_area_t * mask_p, const lv_style_t * style_p,
                    const char * txt, lv_txt_flag_t flag, lv_point_t * offset);

#if USE_LV_IMG
/**
 * Draw an image
 * @param cords_p the coordinates of the image
 * @param mask_p the image will be drawn only in this area
 * @param map_p pointer to a lv_color_t array which contains the pixels of the image
 */
void lv_draw_img(const lv_area_t * coords, const lv_area_t * mask,
             const lv_style_t * style, const void * src);
#endif

/**
 * Draw a line
 * @param p1 first point of the line
 * @param p2 second point of the line
 * @param mask_pthe line will be drawn only on this area
 * @param style_p pointer to a style
 */
void lv_draw_line(const lv_point_t * p1, const lv_point_t * p2, const lv_area_t * mask_p,
                  const lv_style_t * style_p);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_DRAW_H*/
