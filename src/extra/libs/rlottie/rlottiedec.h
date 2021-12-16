#ifndef RLOTTIEDEC_H
#define RLOTTIEDEC_H

#if LV_USE_RLOTTIE

/*********************
 *      DEFINES
 *********************/
struct Lottie_Animation_S;
struct lv_img_header_t;

/**********************
 *      TYPEDEFS
 **********************/
/** The decoder initialiation context to use as the last parameter of
 *  lv_img_set_src_ex
 */
typedef struct
{
    size_t total_frames;
    size_t current_frame;
    uint16_t create_width;
    uint16_t create_height : 15;
    uint16_t should_free : 1;
    struct Lottie_Animation_S * cache;
    void * create_src;
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
