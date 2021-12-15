#ifndef RLOTTIEDEC_H
#define RLOTTIEDEC_H

#if LV_USE_RLOTTIE

/*********************
 *      DEFINES
 *********************/
struct Lottie_Animation;
struct lv_img_header_t;

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    Lottie_Animation * animation;
    lv_img_header_t * header;
    size_t total_frames;
    size_t current_frame;
    uint32_t * allocated_buf;
    size_t lines_in_buf;
    size_t scanline_width;
    size_t top;
    size_t last_rendered_frame;
} lv_rlottie_dec_context_t;

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
