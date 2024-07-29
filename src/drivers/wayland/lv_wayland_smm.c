/**
 * @file lv_wayland_smm.c
 *
 */

typedef int dummy_t;     /* Make GCC on windows happy, avoid empty translation unit */

#ifndef _WIN32

#include "lv_wayland_smm.h"
#include "../../display/lv_display.h"

#if LV_USE_WAYLAND

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_NAME_ATTEMPTS (5)
#define PREFER_NUM_BUFFERS (3)

#define ROUND_UP(n, b) (((((n) ? (n) : 1) + (b) - 1) / (b)) * (b))
#define LLHEAD(type) \
    struct { \
        struct type *first; \
        struct type *last; \
    }

#define LLLINK(type) \
    struct { \
        struct type *next; \
        struct type *prev; \
    }

#define LL_FIRST(head) ((head)->first)
#define LL_LAST(head) ((head)->last)
#define LL_IS_EMPTY(head) (LL_FIRST(head) == NULL)
#define LL_NEXT(src, member) ((src)->member.next)
#define LL_PREV(src, member) ((src)->member.prev)

#define LL_INIT(head) do { \
        (head)->first = NULL; \
        (head)->last = NULL; \
    } while (0)

#define LL_ENQUEUE(head, src, member) do { \
        (src)->member.next = NULL; \
        (src)->member.prev = (head)->last; \
        if ((head)->last == NULL) { \
            (head)->first = (src); \
        } else { \
            (head)->last->member.next = (src); \
        } \
        (head)->last = (src); \
    } while (0)

#define LL_DEQUEUE(entry, head, member) do { \
        (entry) = LL_FIRST(head); \
        LL_REMOVE(head, entry, member); \
    } while (0)

#define LL_INSERT_AFTER(head, dest, src, member) do { \
        (src)->member.prev = (dest); \
        (src)->member.next = (dest)->member.next; \
        if ((dest)->member.next != NULL) { \
            (dest)->member.next->member.prev = (src); \
        } else { \
            (head)->last = (src); \
        } \
        (dest)->member.next = (src); \
    } while (0)

#define LL_REMOVE(head, src, member) do { \
        if ((src)->member.prev != NULL) { \
            (src)->member.prev->member.next = (src)->member.next; \
        } else { \
            (head)->first = (src)->member.next; \
        } \
        if ((src)->member.next != NULL) { \
            (src)->member.next->member.prev = (src)->member.prev; \
        } else { \
            (head)->last = (src)->member.prev; \
        } \
    } while (0)

#define LL_FOREACH(entry, head, member) \
    for ((entry) = LL_FIRST(head); \
         (entry) != NULL; \
         (entry) = LL_NEXT(entry, member))

#define WAYLAND_FD_NAME "/" SMM_FD_NAME "-XXXXX"

struct smm_pool {
    struct smm_pool_properties props;
    LLHEAD(smm_buffer) allocd;
    void * map;
    size_t map_size;
    bool map_outdated;
};

struct smm_buffer {
    struct smm_buffer_properties props;
    bool group_resized;
    LLLINK(smm_buffer) pool;
    LLLINK(smm_buffer) use;
    LLLINK(smm_buffer) age;
};

struct smm_group {
    struct smm_group_properties props;
    size_t size;
    unsigned char num_buffers;
    LLHEAD(smm_buffer) unused;
    LLHEAD(smm_buffer) inuse;
    LLHEAD(smm_buffer) history;
    LLLINK(smm_group) link;
};

static size_t calc_buffer_size(struct smm_buffer * buf);
static void purge_history(struct smm_buffer * buf);
static struct smm_buffer * get_from_pool(struct smm_group * grp);
static void return_to_pool(struct smm_buffer * buf);
static struct smm_pool * alloc_pool(void);
static void free_pool(struct smm_pool * pool);
static struct smm_buffer * alloc_buffer(struct smm_buffer * last, size_t offset);
static void free_buffer(struct smm_buffer * buf);

static struct {
    unsigned long page_sz;
    struct smm_events cbs;
    struct smm_pool * active;
    LLHEAD(smm_group) groups;
    struct {
        size_t active_used;
    } statistics;
} smm_instance;


void smm_init(struct smm_events * evs)
{
    memcpy(&smm_instance.cbs, evs, sizeof(struct smm_events));
    srand((unsigned int)clock());
    smm_instance.page_sz = (unsigned long)sysconf(_SC_PAGESIZE);
    LL_INIT(&smm_instance.groups);
}


void smm_deinit(void)
{
    struct smm_group * grp;

    /* Destroy all buffer groups */
    while(!LL_IS_EMPTY(&smm_instance.groups)) {
        LL_DEQUEUE(grp, &smm_instance.groups, link);
        smm_destroy(grp);
    }
}


void smm_setctx(void * ctx)
{
    smm_instance.cbs.ctx = ctx;
}


smm_group_t * smm_create(void)
{
    struct smm_group * grp;

    /* Allocate and intialize a new buffer group */
    grp = malloc(sizeof(struct smm_group));
    if(grp != NULL) {
        grp->size = smm_instance.page_sz;
        grp->num_buffers = 0;
        LL_INIT(&grp->unused);
        LL_INIT(&grp->inuse);
        LL_INIT(&grp->history);

        /* Add to instance groups queue */
        LL_ENQUEUE(&smm_instance.groups, grp, link);
    }

    return grp;
}


void smm_resize(smm_group_t * grp, size_t sz)
{
    struct smm_buffer * buf;
    struct smm_group * rgrp = grp;

    /* Round allocation size up to a sysconf(_SC_PAGE_SIZE) boundary */
    rgrp->size = ROUND_UP(sz, smm_instance.page_sz);

    /* Return all unused buffers to pool (to be re-allocated at the new size) */
    while(!LL_IS_EMPTY(&rgrp->unused)) {
        LL_DEQUEUE(buf, &rgrp->unused, use);
        return_to_pool(buf);
    }

    /* Mark all buffers in use to be freed to pool when possible */
    LL_FOREACH(buf, &rgrp->inuse, use) {
        buf->group_resized = true;
        purge_history(buf);
    }
}


void smm_destroy(smm_group_t * grp)
{
    struct smm_buffer * buf;
    struct smm_group * dgrp = grp;

    /* Return unused buffers */
    while(!LL_IS_EMPTY(&dgrp->unused)) {
        LL_DEQUEUE(buf, &dgrp->unused, use);
        return_to_pool(buf);
    }

    /* Return buffers that are still in use (ideally this queue should be empty
     * at this time)
     */
    while(!LL_IS_EMPTY(&dgrp->inuse)) {
        LL_DEQUEUE(buf, &dgrp->inuse, use);
        return_to_pool(buf);
    }

    /* Remove from instance groups queue */
    LL_REMOVE(&smm_instance.groups, dgrp, link);
    free(dgrp);
}


smm_buffer_t * smm_acquire(smm_group_t * grp)
{
    struct smm_buffer * buf;
    struct smm_group * agrp = grp;

    if(LL_IS_EMPTY(&agrp->unused)) {
        /* No unused buffer available, so get a new one from pool */
        buf = get_from_pool(agrp);
    }
    else {
        /* Otherwise, reuse an unused buffer */
        LL_DEQUEUE(buf, &agrp->unused, use);
    }

    if(buf != NULL) {
        /* Add buffer to in-use queue */
        LL_ENQUEUE(&agrp->inuse, buf, use);

        /* Emit 'init buffer' event */
        if(smm_instance.cbs.init_buffer != NULL) {
            if(smm_instance.cbs.init_buffer(smm_instance.cbs.ctx, &buf->props)) {
                smm_release(buf);
                buf = NULL;
            }
        }

        if(buf != NULL) {
            /* Remove from history */
            purge_history(buf);

            /* Add to history a-new */
            LL_ENQUEUE(&agrp->history, buf, age);
        }
    }

    return buf;
}


void * smm_map(smm_buffer_t * buf)
{
    struct smm_buffer * mbuf = buf;
    struct smm_pool * pool = mbuf->props.pool;
    void * map = pool->map;

    if(pool->map_outdated) {
        /* Update mapping to current pool size */
        if(pool->map != NULL) {
            munmap(pool->map, pool->map_size);
        }

        map = mmap(NULL,
                   pool->props.size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED,
                   pool->props.fd,
                   0);

        if(map == MAP_FAILED) {
            map = NULL;
            pool->map = NULL;
        }
        else {
            pool->map = map;
            pool->map_size = pool->props.size;
            pool->map_outdated = false;
        }
    }

    /* Calculate buffer mapping (from offset in pool) */
    if(map != NULL) {
        map = (((char *)map) + mbuf->props.offset);
    }

    return map;
}


void smm_release(smm_buffer_t * buf)
{
    struct smm_buffer * rbuf = buf;
    struct smm_group * grp = rbuf->props.group;

    /* Remove from in-use queue */
    LL_REMOVE(&grp->inuse, rbuf, use);

    if(rbuf->group_resized) {
        /* Buffer group was resized while this buffer was in-use, thus it must be
         * returned to it's pool
         */
        rbuf->group_resized = false;
        return_to_pool(rbuf);
    }
    else {
        /* Move to unused queue */
        LL_ENQUEUE(&grp->unused, rbuf, use);

        /* Try to limit total number of buffers to preferred number */
        while((grp->num_buffers > PREFER_NUM_BUFFERS) &&
              (!LL_IS_EMPTY(&grp->unused))) {
            LL_DEQUEUE(rbuf, &grp->unused, use);
            return_to_pool(rbuf);
        }
    }
}


smm_buffer_t * smm_latest(smm_group_t * grp)
{
    struct smm_group * lgrp = grp;

    return LL_LAST(&lgrp->history);
}


smm_buffer_t * smm_next(smm_buffer_t * buf)
{
    struct smm_buffer * ibuf;
    struct smm_buffer * nbuf = buf;
    struct smm_group * grp = nbuf->props.group;

    LL_FOREACH(ibuf, &grp->history, age) {
        if(ibuf == nbuf) {
            ibuf = LL_NEXT(ibuf, age);
            break;
        }
    }

    return ibuf;
}

void purge_history(struct smm_buffer * buf)
{
    struct smm_buffer * ibuf;
    struct smm_group * grp = buf->props.group;

    /* Remove from history (and any older) */
    LL_FOREACH(ibuf, &grp->history, age) {
        if(ibuf == buf) {
            do {
                LL_DEQUEUE(ibuf, &grp->history, age);
            } while(ibuf != buf);
            break;
        }
    }
}


size_t calc_buffer_size(struct smm_buffer * buf)
{
    size_t buf_sz;
    struct smm_pool * buf_pool = buf->props.pool;

    if(buf == LL_LAST(&buf_pool->allocd)) {
        buf_sz = (buf_pool->props.size - buf->props.offset);
    }
    else {
        buf_sz = (LL_NEXT(buf, pool)->props.offset - buf->props.offset);
    }

    return buf_sz;
}


struct smm_buffer * get_from_pool(struct smm_group * grp)
{
    int ret;
    size_t buf_sz;
    struct smm_buffer * buf;
    struct smm_buffer * last = NULL;

    /* TODO: Determine when to allocate a new active pool (i.e. memory shrink) */

    if(smm_instance.active == NULL) {
        /* Allocate a new active pool */
        smm_instance.active = alloc_pool();
        smm_instance.statistics.active_used = 0;
    }

    if(smm_instance.active == NULL) {
        buf = NULL;
    }
    else {
        /* Search for a free buffer large enough for allocation */
        LL_FOREACH(buf, &smm_instance.active->allocd, pool) {
            last = buf;
            if(buf->props.group == NULL) {
                buf_sz = calc_buffer_size(buf);
                if(buf_sz == grp->size) {
                    break;
                }
                else if(buf_sz > grp->size) {
                    if((buf != LL_LAST(&smm_instance.active->allocd)) &&
                       (LL_NEXT(buf, pool)->props.group == NULL)) {
                        /* Pull back next buffer to use unallocated size */
                        LL_NEXT(buf, pool)->props.offset -= (buf_sz - grp->size);
                    }
                    else {
                        /* Allocate another buffer to hold unallocated size */
                        alloc_buffer(buf, buf->props.offset + grp->size);
                    }

                    break;
                }
            }
        }

        if(buf == NULL) {
            /* No buffer found to meet allocation size, expand pool */
            if((last != NULL) &&
               (last->props.group == NULL)) {
                /* Use last free buffer */
                buf_sz = (grp->size - buf_sz);
            }
            else {
                /* Allocate new buffer */
                buf_sz = grp->size;
                if(last == NULL) {
                    buf = alloc_buffer(NULL, 0);
                }
                else {
                    buf = alloc_buffer(last, smm_instance.active->props.size);
                }
                last = buf;
            }

            if(last != NULL) {
                /* Expand pool backing memory */
                ret = ftruncate(smm_instance.active->props.fd,
                                smm_instance.active->props.size + buf_sz);
                if(ret) {
                    if(buf != NULL) {
                        free_buffer(buf);
                        buf = NULL;
                    }
                }
                else {
                    smm_instance.active->props.size += buf_sz;
                    smm_instance.active->map_outdated = true;
                    buf = last;

                    if(!(smm_instance.active->props.size - buf_sz)) {
                        /* Emit 'new pool' event */
                        if((smm_instance.cbs.new_pool != NULL) &&
                           (smm_instance.cbs.new_pool(smm_instance.cbs.ctx,
                                                      &smm_instance.active->props))) {
                            free_buffer(buf);
                            free_pool(smm_instance.active);
                            smm_instance.active = NULL;
                            buf = NULL;
                        }
                    }
                    else {
                        /* Emit 'expand pool' event */
                        if(smm_instance.cbs.expand_pool != NULL) {
                            smm_instance.cbs.expand_pool(smm_instance.cbs.ctx,
                                                         &smm_instance.active->props);
                        }
                    }
                }
            }
        }
    }

    if(buf != NULL) {
        /* Set buffer group */
        memcpy((void *)&buf->props.group, &grp, sizeof(struct smm_group *));

        /* Emit 'new buffer' event */
        if(smm_instance.cbs.new_buffer != NULL) {
            if(smm_instance.cbs.new_buffer(smm_instance.cbs.ctx, &buf->props)) {
                grp = NULL;
                memcpy((void *)&buf->props.group, &grp, sizeof(struct smm_group *));
                buf = NULL;
            }
        }

        if(buf != NULL) {
            /* Update active pool usage statistic */
            smm_instance.statistics.active_used += grp->size;
            grp->num_buffers++;
        }
    }

    return buf;
}


void return_to_pool(struct smm_buffer * buf)
{
    struct smm_group * grp = buf->props.group;
    struct smm_pool * pool = buf->props.pool;

    /* Emit 'free buffer' event */
    if(smm_instance.cbs.free_buffer != NULL) {
        smm_instance.cbs.free_buffer(smm_instance.cbs.ctx, &buf->props);
    }

    /* Buffer is no longer part of history */
    purge_history(buf);

    /* Buffer is no longer part of group */
    grp->num_buffers--;
    grp = NULL;
    memcpy((void *)&buf->props.group, &grp, sizeof(struct smm_group *));

    /* Update active pool usage statistic */
    if(smm_instance.active == pool) {
        smm_instance.statistics.active_used -= calc_buffer_size(buf);
    }

    /* Coalesce with ungrouped buffers beside this one */
    if((buf != LL_LAST(&pool->allocd)) &&
       (LL_NEXT(buf, pool)->props.group == NULL)) {
        free_buffer(LL_NEXT(buf, pool));
    }
    if((buf != LL_FIRST(&pool->allocd)) &&
       (LL_PREV(buf, pool)->props.group == NULL)) {
        buf = LL_PREV(buf, pool);
        pool = buf->props.pool;
        free_buffer(LL_NEXT(buf, pool));
    }

    /* Free buffer (and pool), if only remaining buffer in pool */
    if((buf == LL_FIRST(&pool->allocd)) &&
       (buf == LL_LAST(&pool->allocd))) {
        free_buffer(buf);

        /* Emit 'free pool' event */
        if(smm_instance.cbs.free_pool != NULL) {
            smm_instance.cbs.free_pool(smm_instance.cbs.ctx, &pool->props);
        }

        free_pool(pool);
        if(smm_instance.active == pool) {
            smm_instance.active = NULL;
        }
    }
}


struct smm_pool * alloc_pool(void)
{
    struct smm_pool * pool;
    char name[] = WAYLAND_FD_NAME;
    unsigned char attempts = 0;
    bool opened = false;

    pool = malloc(sizeof(struct smm_pool));
    if(pool != NULL) {
        do {
            /* A randomized pool name should help reduce collisions */
            sprintf(name + sizeof(SMM_FD_NAME) + 1, "%05X", rand() & 0xFFFF);
            pool->props.fd = shm_open(name,
                                      O_RDWR | O_CREAT | O_EXCL,
                                      S_IRUSR | S_IWUSR);
            if(pool->props.fd >= 0) {
                shm_unlink(name);
                pool->props.size = 0;
                pool->map = NULL;
                pool->map_size = 0;
                pool->map_outdated = false;
                LL_INIT(&pool->allocd);
                opened = true;
                break;
            }
            else {
                if(errno != EEXIST) {
                    break;
                }
                attempts++;
            }
        } while(attempts < MAX_NAME_ATTEMPTS);

        if(!opened) {
            free(pool);
            pool = NULL;
        }
    }

    return pool;
}


void free_pool(struct smm_pool * pool)
{
    if(pool->map != NULL) {
        munmap(pool->map, pool->map_size);
    }

    close(pool->props.fd);
    free(pool);
}


struct smm_buffer * alloc_buffer(struct smm_buffer * last, size_t offset)
{
    struct smm_buffer * buf;
    struct smm_buffer_properties initial_props = {
        {NULL},
        NULL,
        smm_instance.active,
        offset
    };

    /* Allocate and intialize a new buffer (including linking in to pool) */
    buf = malloc(sizeof(struct smm_buffer));
    if(buf != NULL) {
        memcpy(&buf->props, &initial_props, sizeof(struct smm_buffer_properties));
        buf->group_resized = false;

        if(last == NULL) {
            LL_ENQUEUE(&smm_instance.active->allocd, buf, pool);
        }
        else {
            LL_INSERT_AFTER(&smm_instance.active->allocd, last, buf, pool);
        }
    }

    return buf;
}


void free_buffer(struct smm_buffer * buf)
{
    struct smm_pool * buf_pool = buf->props.pool;

    /* Remove from pool */
    LL_REMOVE(&buf_pool->allocd, buf, pool);
    free(buf);
}

#endif /* LV_USE_WAYLAND */
#endif /* _WIN32 */
