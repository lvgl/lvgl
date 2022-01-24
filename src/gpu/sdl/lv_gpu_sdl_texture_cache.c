/**
 * @file lv_gpu_sdl_texture_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_gpu_sdl_texture_cache.h"

#include "../../misc/lv_log.h"
#include "../../draw/lv_draw_label.h"
#include "../../draw/lv_draw_img.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    SDL_Texture * texture;
    void * userdata;
    lv_lru_free_t * userdata_free;
    lv_gpu_sdl_cache_flag_t flags;
} draw_cache_value_t;

typedef struct {
    lv_gpu_cache_key_magic_t magic;
} temp_texture_key_t;

typedef struct {
    lv_coord_t width, height;
} temp_texture_userdata_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_cache_free_value(draw_cache_value_t *);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_lru_t * lv_sdl_texture_cache;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_gpu_sdl_texture_cache_init()
{
    lv_sdl_texture_cache = lv_lru_new(1024 * 1024 * 8, 65536, (lv_lru_free_t *) draw_cache_free_value,
                                      NULL);
}

void _lv_gpu_sdl_texture_cache_deinit()
{
    lv_lru_free(lv_sdl_texture_cache);
}

SDL_Texture * lv_gpu_draw_cache_get(const void * key, size_t key_length, bool * found)
{
    return lv_gpu_draw_cache_get_with_userdata(key, key_length, found, NULL);
}

SDL_Texture * lv_gpu_draw_cache_get_with_userdata(const void * key, size_t key_length, bool * found, void ** userdata)
{
    draw_cache_value_t * value = NULL;
    lv_lru_get(lv_sdl_texture_cache, key, key_length, (void **) &value);
    if(!value) {
        if(found) {
            *found = false;
        }
        return NULL;
    }
    else {
        if(userdata) {
            *userdata = value->userdata;
        }
    }
    if(found) {
        *found = true;
    }
    return value->texture;
}

void lv_gpu_draw_cache_put(const void * key, size_t key_length, SDL_Texture * texture)
{
    lv_gpu_draw_cache_put_advanced(key, key_length, texture, NULL, NULL, 0);
}

void lv_gpu_draw_cache_put_advanced(const void * key, size_t key_length, SDL_Texture * texture, void * userdata,
                                    lv_lru_free_t userdata_free, lv_gpu_sdl_cache_flag_t flags)
{
    draw_cache_value_t * value = SDL_malloc(sizeof(draw_cache_value_t));
    value->texture = texture;
    value->userdata = userdata;
    value->userdata_free = userdata_free;
    value->flags = flags;
    if(!texture) {
        lv_lru_set(lv_sdl_texture_cache, key, key_length, value, 1);
        return;
    }
    if(flags & LV_GPU_SDL_CACHE_FLAG_MANAGED) {
        /* Managed texture doesn't count into cache size */
        LV_LOG_INFO("cache texture %p, %d*%d@%dbpp", texture, width, height, SDL_BITSPERPIXEL(format));
        lv_lru_set(lv_sdl_texture_cache, key, key_length, value, 1);
        return;
    }
    Uint32 format;
    int access, width, height;
    if(SDL_QueryTexture(texture, &format, &access, &width, &height) != 0) {
        return;
    }
    LV_LOG_INFO("cache texture %p, %d*%d@%dbpp", texture, width, height, SDL_BITSPERPIXEL(format));
    lv_lru_set(lv_sdl_texture_cache, key, key_length, value, width * height * SDL_BITSPERPIXEL(format) / 8);
}

SDL_Texture * lv_gpu_temp_texture_obtain(SDL_Renderer * renderer, lv_coord_t width, lv_coord_t height)
{
    temp_texture_key_t key;
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_TEMP;
    temp_texture_userdata_t * userdata = NULL;
    SDL_Texture * texture = lv_gpu_draw_cache_get_with_userdata(&key, sizeof(key), NULL, (void **) &userdata);
    if(texture && userdata->width >= width && userdata->height >= height) {
        return texture;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
    userdata = SDL_malloc(sizeof(temp_texture_userdata_t));
    userdata->width = width;
    userdata->height = height;
    lv_gpu_draw_cache_put_advanced(&key, sizeof(key), texture, userdata, SDL_free, 0);
    return texture;
}

lv_gpu_sdl_cache_key_head_img_t * lv_gpu_sdl_img_cache_key_create(const void * src, int32_t frame_id, size_t * size)
{
    lv_gpu_sdl_cache_key_head_img_t header;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&header, 0, sizeof(header));
    header.magic = LV_GPU_CACHE_KEY_MAGIC_IMG;
    header.type = lv_img_src_get_type(src);
    header.frame_id = frame_id;
    void * key;
    size_t key_size;
    if(header.type == LV_IMG_SRC_FILE || header.type == LV_IMG_SRC_SYMBOL) {
        size_t srclen = SDL_strlen(src);
        key_size = sizeof(header) + srclen;
        key = SDL_malloc(key_size);
        SDL_memcpy(key, &header, sizeof(header));
        /*Copy string content as key value*/
        SDL_memcpy(key + sizeof(header), src, srclen);
    }
    else {
        key_size = sizeof(header) + sizeof(void *);
        key = SDL_malloc(key_size);
        SDL_memcpy(key, &header, sizeof(header));
        /*Copy address number as key value*/
        SDL_memcpy(key + sizeof(header), &src, sizeof(void *));
    }
    *size = key_size;
    return (lv_gpu_sdl_cache_key_head_img_t *) key;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_cache_free_value(draw_cache_value_t * value)
{
    if(value->texture && !(value->flags & LV_GPU_SDL_CACHE_FLAG_MANAGED)) {
        LV_LOG_INFO("destroy texture %p", value->texture);
        SDL_DestroyTexture(value->texture);
    }
    if(value->userdata_free) {
        value->userdata_free(value->userdata);
    }
    SDL_free(value);
}


#endif /*LV_USE_GPU_SDL*/

