/**nit
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"
#if LV_USE_DRAW_SW

#include "lv_draw_sw.h"
#include <errno.h>

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
 *  GLOBAL PROTOTYPES
 **********************/
static int thread(void * ptr);


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_init_ctx(lv_disp_t * disp, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(disp);


    draw_ctx->buffer_copy = lv_draw_sw_buffer_copy;
    draw_ctx->buffer_convert = lv_draw_sw_buffer_convert;
    draw_ctx->buffer_clear = lv_draw_sw_buffer_clear;
    draw_ctx->layer_init = lv_draw_sw_layer_create;
    draw_ctx->layer_adjust = lv_draw_sw_layer_adjust;
    draw_ctx->layer_blend = lv_draw_sw_layer_blend;
    draw_ctx->layer_destroy = lv_draw_sw_layer_destroy;
    draw_ctx->layer_instance_size = sizeof(lv_draw_sw_layer_ctx_t);


    for(int i = 0; i < 4; i++) {
        lv_draw_sw_unit_t * draw_sw_unit = lv_malloc(sizeof(*draw_sw_unit));
        lv_memzero(draw_sw_unit, sizeof(lv_draw_sw_unit_t));
        draw_sw_unit->base_unit.draw_ctx = draw_ctx;
        draw_sw_unit->base_unit.wait_for_finish = lv_draw_sw_wait_for_finish;
        draw_sw_unit->base_unit.dispatch = lv_draw_sw_dispatch;
        draw_sw_unit->idx = i;

        draw_sw_unit->base_unit.next = draw_ctx->draw_unit_head;
        draw_ctx->draw_unit_head = (lv_draw_unit_t *) draw_sw_unit;

        pthread_mutex_init(&draw_sw_unit->lock, NULL);
        pthread_cond_init(&draw_sw_unit->cond, NULL);

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, thread, draw_sw_unit);
    }
}

void lv_draw_sw_deinit_ctx(lv_disp_t * disp, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(disp);

    lv_draw_sw_unit_t * draw_sw_ctx = (lv_draw_sw_unit_t *) draw_ctx;
    lv_memzero(draw_sw_ctx, sizeof(lv_draw_sw_unit_t));
}

void lv_draw_sw_wait_for_finish(lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(draw_ctx);
    /*Nothing to wait for*/
}


uint32_t lv_draw_sw_dispatch(lv_draw_unit_t * draw_unit, lv_draw_ctx_t * draw_ctx)
{

    lv_draw_sw_unit_t * draw_sw_unit = (lv_draw_sw_unit_t *) draw_unit;
    if(draw_sw_unit->task_act) {
        return 0;
    }

    lv_draw_task_t * t = lv_draw_get_next_available_task(draw_ctx, NULL);
    while(t) {
        if(t->type == LV_DRAW_TASK_TYPE_RECTANGLE) {
            pthread_mutex_lock(&draw_sw_unit->lock);

            t->state = LV_DRAW_TASK_STATE_IN_PRGRESS;
            draw_sw_unit->task_act = t;
            printf("%d Taken: %d, %d, %d, %d\n", draw_sw_unit->idx, t->area.x1, t->area.y1,
                   lv_area_get_width(&t->area), lv_area_get_height(&t->area));

            pthread_cond_signal(&draw_sw_unit->cond);
            pthread_mutex_unlock(&draw_sw_unit->lock);
            return 1;
        }
        t = lv_draw_get_next_available_task(draw_ctx, t);
    }

    return 0;
}

void lv_draw_sw_buffer_copy(lv_draw_ctx_t * draw_ctx,
                            void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                            void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    LV_UNUSED(draw_ctx);

    uint8_t px_size = lv_color_format_get_size(draw_ctx->color_format);
    uint8_t * dest_bufc =  dest_buf;
    uint8_t * src_bufc =  src_buf;

    /*Got the first pixel of each buffer*/
    dest_bufc += dest_stride * px_size * dest_area->y1;
    dest_bufc += dest_area->x1 * px_size;

    src_bufc += src_stride * px_size * src_area->y1;
    src_bufc += src_area->x1 * px_size;

    uint32_t line_length = lv_area_get_width(dest_area) * px_size;
    lv_coord_t y;
    for(y = dest_area->y1; y <= dest_area->y2; y++) {
        lv_memcpy(dest_bufc, src_bufc, line_length);
        dest_bufc += dest_stride;
        src_bufc += src_stride;
    }
}

void lv_draw_sw_buffer_convert(lv_draw_ctx_t * draw_ctx)
{
    /*Keep the rendered image as it is*/
    if(draw_ctx->color_format == LV_COLOR_FORMAT_NATIVE) return;

#if LV_COLOR_DEPTH == 8
    if(draw_ctx->color_format == LV_COLOR_FORMAT_L8) return;
#endif

#if LV_COLOR_DEPTH == 16
    if(draw_ctx->color_format == LV_COLOR_FORMAT_RGB565) return;

    /*Make both the clip and buf area relative to the buf area*/
    if(draw_ctx->color_format == LV_COLOR_FORMAT_NATIVE_REVERSED) {
        uint32_t px_cnt = lv_area_get_size(draw_ctx->buf_area);
        uint32_t u32_cnt = px_cnt / 2;
        uint16_t * buf16 = draw_ctx->buf;
        uint32_t * buf32 = (uint32_t *) buf16 ;

        /*Swap all byte pairs*/
        while(u32_cnt >= 8) {
            buf32[0] = ((uint32_t)(buf32[0] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[0] & 0x00ff00ff) << 8);
            buf32[1] = ((uint32_t)(buf32[1] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[1] & 0x00ff00ff) << 8);
            buf32[2] = ((uint32_t)(buf32[2] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[2] & 0x00ff00ff) << 8);
            buf32[3] = ((uint32_t)(buf32[3] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[3] & 0x00ff00ff) << 8);
            buf32[4] = ((uint32_t)(buf32[4] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[4] & 0x00ff00ff) << 8);
            buf32[5] = ((uint32_t)(buf32[5] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[5] & 0x00ff00ff) << 8);
            buf32[6] = ((uint32_t)(buf32[6] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[6] & 0x00ff00ff) << 8);
            buf32[7] = ((uint32_t)(buf32[7] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[7] & 0x00ff00ff) << 8);
            buf32 += 8;
            u32_cnt -= 8;
        }

        while(u32_cnt) {
            *buf32 = ((uint32_t)(*buf32 & 0xff00ff00) >> 8) + ((uint32_t)(*buf32 & 0x00ff00ff) << 8);
            buf32++;
            u32_cnt--;
        }

        if(px_cnt & 0x1) {
            uint32_t e = px_cnt - 1;
            buf16[e] = ((buf16[e] & 0xff00) >> 8) + ((buf16[e] & 0x00ff) << 8);
        }

        return;
    }
    else {
        size_t buf_size_px = lv_area_get_size(draw_ctx->buf_area);
        bool has_alpha = lv_color_format_has_alpha(draw_ctx->color_format);
        uint8_t px_size_in = lv_color_format_get_size(has_alpha ? LV_COLOR_FORMAT_NATIVE_ALPHA : LV_COLOR_FORMAT_NATIVE);
        uint8_t px_size_out = lv_color_format_get_size(draw_ctx->color_format);

        /*In-plpace conversation can happen only when converting to a smaller pixel size*/
        if(px_size_in >= px_size_out) {
            if(has_alpha) lv_color_from_native_alpha(draw_ctx->buf, draw_ctx->buf, draw_ctx->color_format, buf_size_px);
            else lv_color_from_native(draw_ctx->buf, draw_ctx->buf, draw_ctx->color_format, buf_size_px);
        }
        else {
            /*TODO What to to do when can't perform in-place conversion?*/
            LV_LOG_WARN("Can't convert to the desired color format (%d)", draw_ctx->color_format);
        }
        return;
    }
#endif

    LV_LOG_WARN("Couldn't convert the image to the desired format");
}

void lv_draw_sw_buffer_clear(lv_draw_ctx_t * draw_ctx)
{
    uint8_t px_size = lv_color_format_get_size(draw_ctx->color_format);
    uint8_t * buf8 = draw_ctx->buf;
    lv_area_t a;
    lv_area_copy(&a, draw_ctx->clip_area);
    lv_area_move(&a, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
    lv_coord_t w = lv_area_get_width(&a);
    buf8 += a.y1 * w * px_size;
    buf8 += a.x1 * px_size;

    lv_coord_t y;
    for(y = a.y1; y <= a.y2; y++) {
        lv_memzero(buf8, w * px_size);
        buf8 += w * px_size;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Very simple thread - counts 0 to 9 delaying 50ms between increments */
static int thread(void * ptr)
{
    lv_draw_sw_unit_t * u = ptr;

    while(1) {
        pthread_mutex_lock(&u->lock);

        while(u->task_act == NULL) {
            pthread_cond_wait(&u->cond, &u->lock);
        }

        if(u->task_act->type == LV_DRAW_TASK_TYPE_RECTANGLE) {
            const lv_area_t * coords = &u->task_act->area;
            printf("%d Draw : %d, %d, %d, %d\n", u->idx, coords->x1, coords->y1,
                   lv_area_get_width(coords), lv_area_get_height(coords));

            u->base_unit.clip_area = &u->task_act->clip_area;

            lv_draw_sw_rect((lv_draw_unit_t *)u, u->task_act->draw_dsc, &u->task_act->area);

            printf("%d Ready: %d, %d, %d, %d\n", u->idx, coords->x1, coords->y1,
                   lv_area_get_width(coords), lv_area_get_height(coords));
            u->task_act->state = LV_DRAW_TASK_STATE_READY;
            u->task_act = NULL;
            lv_draw_dispatch_request(u->base_unit.draw_ctx);

        }

        pthread_mutex_unlock(&u->lock);
    }

    return 0;
}

#endif /*LV_USE_DRAW_SW*/
