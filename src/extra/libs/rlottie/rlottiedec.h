#ifndef RLOTTIEDEC_H
#define RLOTTIEDEC_H

#if LV_USE_RLOTTIE

/*********************
 *      DEFINES
 *********************/
#define LV_RLOTTIE_ID  0x71

/**********************
 *      TYPEDEFS
 **********************/
struct Lottie_Animation_S;
struct lv_img_header_t;

/** The decoder initialiation context to use as the last parameter of
 *  lv_img_set_src_ex
 */
typedef struct {
    lv_img_dec_ctx_t ctx;
    uint16_t create_width;
    uint16_t create_height;
    uint32_t max_buf_size; /*Size of the ARGB image buffer until it's not longer cached and read in region*/
    struct Lottie_Animation_S * cache;
    const void * create_src;
} rlottiedec_ctx_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_rlottie_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLOTTIE*/

#endif /* RLOTTIEDEC_H */
