/**
 * @file lv_sdl2.h
 *
 */

#ifndef LV_SDL2_H
#define LV_SDL2_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if LV_USE_OS == LV_OS_SDL2
#include <SDL2/SDL.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    SDL_Thread * thread;
    void (*callback)(void *);
    void * user_data;
} lv_thread_t;

typedef SDL_mutex * lv_mutex_t;

typedef struct {
    SDL_mutex * mutex;
    SDL_cond * cond;
    bool v;
} lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_SDL2*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SDL2_H*/
