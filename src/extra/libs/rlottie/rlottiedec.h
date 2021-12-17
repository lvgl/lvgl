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
    uint16_t create_height : 15;
    uint16_t should_free : 1;
    struct Lottie_Animation_S * cache;
    const void * create_src;
} rlottiedec_ctx_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_rlottie_init(void);

void lv_rlottie_set_max_buffer_size(size_t size_bytes);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLOTTIE*/

#endif /* RLOTTIEDEC_H */
