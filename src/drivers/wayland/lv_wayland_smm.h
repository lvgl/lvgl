/**
 * @file lv_wayland_smm.h
 *
 */
#ifndef LV_WAYLAND_SMM_H
#define LV_WAYLAND_SMM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include LV_STDDEF_INCLUDE
#include LV_STDBOOL_INCLUDE

#if LV_USE_WAYLAND

/*********************
 *      DEFINES
 *********************/

#define SMM_FD_NAME  "lvgl-wayland"
#define SMM_POOL_TAGS   (1)
#define SMM_BUFFER_TAGS (2)
#define SMM_GROUP_TAGS  (1)

/**********************
 *      TYPEDEFS
 **********************/

typedef void smm_pool_t;
typedef void smm_buffer_t;
typedef void smm_group_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

struct smm_events {
    void * ctx;
    bool (*new_pool)(void * ctx, smm_pool_t * pool);
    void (*expand_pool)(void * ctx, smm_pool_t * pool);
    void (*free_pool)(void * ctx, smm_pool_t * pool);
    bool (*new_buffer)(void * ctx, smm_buffer_t * buf);
    bool (*init_buffer)(void * ctx, smm_buffer_t * buf);
    void (*free_buffer)(void * ctx, smm_buffer_t * buf);
};

struct smm_pool_properties {
    void * tag[SMM_POOL_TAGS];
    size_t size;
    int fd;
};

struct smm_buffer_properties {
    void * tag[SMM_BUFFER_TAGS];
    smm_group_t * const group;
    smm_pool_t * const pool;
    size_t offset;
};

struct smm_group_properties {
    void * tag[SMM_GROUP_TAGS];
};

void smm_init(const struct smm_events * evs);
void smm_setctx(void * ctx);
void smm_deinit(void);
smm_group_t * smm_create(void);
void smm_resize(smm_group_t * grp, size_t sz);
void smm_destroy(smm_group_t * grp);
smm_buffer_t * smm_acquire(smm_group_t * grp);
void * smm_map(smm_buffer_t * buf);
void smm_release(smm_buffer_t * buf);
smm_buffer_t * smm_latest(smm_group_t * grp);
smm_buffer_t * smm_next(smm_buffer_t * buf);

/**********************
 *      MACROS
 **********************/

#define SMM_POOL_PROPERTIES(p) ((const struct smm_pool_properties *)(p))
#define SMM_BUFFER_PROPERTIES(b) ((const struct smm_buffer_properties *)(b))
#define SMM_GROUP_PROPERTIES(g) ((const struct smm_group_properties *)(g))
#define SMM_TAG(o, n, v) \
    do { \
        void **smm_tag = (void **)((char *)o + (n * sizeof(void *))); \
        *smm_tag = (v); \
    } while(0)


#endif /* LV_USE_WAYLAND */
#endif /* _WIN32 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_WAYLAND_SMM_H */
