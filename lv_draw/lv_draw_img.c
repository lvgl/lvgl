/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_img.h"
#include "../lv_misc/lv_fs.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if USE_LV_IMG
/**
 * Draw an image
 * @param coords the coordinates of the image
 * @param mask the image will be drawn only in this area
 * @param src pointer to a lv_color_t array which contains the pixels of the image
 * @param style style of the image
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_img(const lv_area_t * coords, const lv_area_t * mask,
                 const void * src, const lv_style_t * style, lv_opa_t opa_scale)
{

    if(src == NULL) {
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL);
        return;
    }


    lv_opa_t opa = (uint16_t)((uint16_t) style->image.opa * opa_scale) >> 8;

    const uint8_t * u8_p = (uint8_t *) src;
    if(u8_p[0] >= 'A' &&  u8_p[0] <= 'Z') { /*It will be a path of a file*/
#if USE_LV_FILESYSTEM
        lv_fs_file_t file;
        lv_fs_res_t res = lv_fs_open(&file, src, LV_FS_MODE_RD);
        if(res == LV_FS_RES_OK) {
            lv_img_t img_data;
            uint32_t br;
            res = lv_fs_read(&file, &img_data, sizeof(lv_img_t), &br);

            lv_area_t mask_com;    /*Common area of mask and cords*/
            bool union_ok;
            union_ok = lv_area_intersect(&mask_com, mask, coords);
            if(union_ok == false) {
                lv_fs_close(&file);
                return;
            }

            uint8_t px_size = 0;
            switch(img_data.header.format) {
                case LV_IMG_FORMAT_FILE_RAW_RGB332:
                    px_size = 1;
                    break;
                case LV_IMG_FORMAT_FILE_RAW_RGB565:
                    px_size = 2;
                    break;
                case LV_IMG_FORMAT_FILE_RAW_RGB888:
                    px_size = 4;
                    break;
                default:
                    return;
            }

            if(img_data.header.alpha_byte) {    /*Correction with the alpha byte*/
                px_size++;
                if(img_data.header.format == LV_IMG_FORMAT_FILE_RAW_RGB888) px_size--; /*Stored in the 4 byte anyway*/
            }


            /* Move the file pointer to the start address according to mask*/
            uint32_t start_offset = sizeof(img_data.header);
            start_offset += (lv_area_get_width(coords)) * (mask_com.y1 - coords->y1) * px_size;      /*First row*/
            start_offset += (mask_com.x1 - coords->x1) * px_size;                                    /*First col*/
            lv_fs_seek(&file, start_offset);

            uint32_t useful_data = lv_area_get_width(&mask_com) * px_size;
            uint32_t next_row = lv_area_get_width(coords) * px_size - useful_data;

            lv_area_t line;
            lv_area_copy(&line, &mask_com);
            lv_area_set_height(&line, 1);

            lv_coord_t row;
            uint32_t act_pos;

#if LV_COMPILER_VLA_SUPPORTED
            uint8_t buf[lv_area_get_width(&mask_com) * px_size];
#else
# if LV_HOR_RES > LV_VER_RES
            uint8_t buf[LV_HOR_RES * ((LV_COLOR_DEPTH >> 8) + 1)];  /*+1 because of the possible alpha byte*/
# else
            uint8_t buf[LV_VER_RES * ((LV_COLOR_DEPTH >> 8) + 1)];
# endif
#endif
            for(row = mask_com.y1; row <= mask_com.y2; row ++) {
                res = lv_fs_read(&file, buf, useful_data, &br);

                map_fp(&line, &mask_com, (uint8_t *)buf, opa, img_data.header.chroma_keyed, img_data.header.alpha_byte,
                       style->image.color, style->image.intense);

                lv_fs_tell(&file, &act_pos);
                lv_fs_seek(&file, act_pos + next_row);
                line.y1++;    /*Go down a line*/
                line.y2++;
            }

            lv_fs_close(&file);

            if(res != LV_FS_RES_OK) {
                lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
                lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No data", LV_TXT_FLAG_NONE, NULL);
            }
        }
#endif
    } else {
        const lv_img_t * img_var = src;
        lv_area_t mask_com;    /*Common area of mask and coords*/
        bool union_ok;
        union_ok = lv_area_intersect(&mask_com, mask, coords);
        if(union_ok == false) {
            return;         /*Out of mask*/
        }

        map_fp(coords, mask, img_var->pixel_map, opa, img_var->header.chroma_keyed, img_var->header.alpha_byte, style->image.color, style->image.intense);

    }

}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/
