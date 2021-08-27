#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../../misc/lv_log.h"
#include "../../draw/lv_draw_label.h"

#include "lv_gpu_sdl_texture_cache.h"

static lv_lru_t *lv_sdl_texture_cache;

typedef struct {
    SDL_Texture *texture;
    void *userdata;
    lv_lru_free_t *userdata_free;
} draw_cache_value_t;

typedef struct {
    lv_gpu_cache_key_magic_t magic;
} temp_texture_key_t;

typedef struct {
    lv_coord_t width, height;
} temp_texture_userdata_t;

static void draw_cache_free_value(draw_cache_value_t *);

void _lv_gpu_sdl_texture_cache_init() {
    lv_sdl_texture_cache = lv_lru_new(1024 * 1024 * 128, 65536, (lv_lru_free_t *) draw_cache_free_value,
                                      SDL_free);
}

void _lv_gpu_sdl_texture_cache_deinit() {
    lv_lru_free(lv_sdl_texture_cache);
}

SDL_Texture *lv_gpu_draw_cache_get(const void *key, size_t key_length, bool *found) {
    return lv_gpu_draw_cache_get_with_userdata(key, key_length, found, NULL);
}

SDL_Texture *lv_gpu_draw_cache_get_with_userdata(const void *key, size_t key_length, bool *found, void **userdata) {
    draw_cache_value_t *value = NULL;
    lv_lru_get(lv_sdl_texture_cache, key, key_length, (void **) &value);
    if (!value) {
        if (found) {
            *found = false;
        }
        return NULL;
    } else {
        if (userdata) {
            *userdata = value->userdata;
        }
    }
    if (found) {
        *found = true;
    }
    return value->texture;
}

void lv_gpu_draw_cache_put(const void *key, size_t key_length, SDL_Texture *texture) {
    lv_gpu_draw_cache_put_with_userdata(key, key_length, texture, NULL, NULL);
}

void lv_gpu_draw_cache_put_with_userdata(const void *key, size_t key_length, SDL_Texture *texture, void *userdata,
                                         lv_lru_free_t userdata_free) {
    draw_cache_value_t *value = SDL_malloc(sizeof(draw_cache_value_t));
    value->texture = texture;
    value->userdata = userdata;
    value->userdata_free = userdata_free;
    if (!texture) {
        lv_lru_set(lv_sdl_texture_cache, key, key_length, value, 1);
        return;
    }
    Uint32 format;
    int access, width, height;
    if (SDL_QueryTexture(texture, &format, &access, &width, &height) != 0) {
        return;
    }
    LV_LOG_INFO("cache texture %p, %d*%d@%dbpp", texture, width, height, SDL_BITSPERPIXEL(format));
    lv_lru_set(lv_sdl_texture_cache, key, key_length, value, width * height * SDL_BITSPERPIXEL(format) / 8);
}

SDL_Texture *lv_gpu_temp_texture_obtain(SDL_Renderer *renderer, lv_coord_t width, lv_coord_t height) {
    temp_texture_key_t key;
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_TEMP;
    temp_texture_userdata_t *userdata = NULL;
    SDL_Texture *texture = lv_gpu_draw_cache_get_with_userdata(&key, sizeof(key), NULL, (void **) &userdata);
    if (texture && userdata->width >= width && userdata->height >= height) {
        return texture;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
    userdata = SDL_malloc(sizeof(temp_texture_userdata_t));
    userdata->width = width;
    userdata->height = height;
    lv_gpu_draw_cache_put_with_userdata(&key, sizeof(key), texture, userdata, SDL_free);
    return texture;
}

static void draw_cache_free_value(draw_cache_value_t *value) {
    if (value->texture) {
        LV_LOG_INFO("destroy texture %p", value->texture);
        SDL_DestroyTexture(value->texture);
    }
    if (value->userdata_free) {
        value->userdata_free(value->userdata);
    }
    SDL_free(value);
}

#endif /*LV_USE_GPU_SDL*/
