/**
 * @file lv_rle.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../stdlib/lv_string.h"
#include "lv_rle.h"

#if LV_USE_RLE

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

uint32_t lv_rle_decompress(const uint8_t * input,
                           uint32_t input_buff_len, uint8_t * output,
                           uint32_t output_buff_len, uint8_t blk_size)
{
    uint32_t ctrl_byte;
    uint32_t rd_len = 0;
    uint32_t wr_len = 0;

    while(rd_len < input_buff_len) {
        ctrl_byte = input[0];
        rd_len++;
        input++;
        if(rd_len > input_buff_len)
            return 0;

        if(ctrl_byte & 0x80) {
            /* copy directly from input to output */
            uint32_t bytes = blk_size * (ctrl_byte & 0x7f);
            rd_len += bytes;
            if(rd_len > input_buff_len)
                return 0;

            wr_len += bytes;
            if(wr_len > output_buff_len) {
                if(wr_len > output_buff_len + blk_size)
                    return 0; /* Error */
                lv_memcpy(output, input, output_buff_len - (wr_len - bytes));
                return output_buff_len;
            }

            lv_memcpy(output, input, bytes);
            output += bytes;
            input += bytes;
        }
        else {
            rd_len += blk_size;
            if(rd_len > input_buff_len)
                return 0;

            wr_len += blk_size * ctrl_byte;
            if(wr_len > output_buff_len) {
                if(wr_len > output_buff_len + blk_size)
                    return 0; /* Error happened */

                /* Skip the last pixel, which could overflow output buffer.*/
                for(uint32_t i = 0; i < ctrl_byte - 1; i++) {
                    lv_memcpy(output, input, blk_size);
                    output += blk_size;
                }
                return output_buff_len;
            }

            if(blk_size == 1) {
                /* optimize the most common case. */
                lv_memset(output, input[0], ctrl_byte);
                output += ctrl_byte;
            }
            else {
                for(uint32_t i = 0; i < ctrl_byte; i++) {
                    lv_memcpy(output, input, blk_size);
                    output += blk_size;
                }
            }
            input += blk_size;
        }
    }

    return wr_len;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_RLE*/
