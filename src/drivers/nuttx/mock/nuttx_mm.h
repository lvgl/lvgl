/**
 * @file nuttx_mm.h
 *
 */

#ifndef NUTTX_MM_H
#define NUTTX_MM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stddef.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/* Special PID to query the info about alloc, free and mempool */

#define PID_MM_ORPHAN  (-6)
#define PID_MM_BIGGEST (-5)
#define PID_MM_FREE    (-4)
#define PID_MM_ALLOC   (-3)
#define PID_MM_LEAK    (-2)
#define PID_MM_MEMPOOL (-1)

#ifndef ULONG_MAX
#define ULONG_MAX   4294967295UL
#endif

#define CONFIG_MM_BACKTRACE 1

/**********************
 *      TYPEDEFS
 **********************/

struct mm_heap_s {
    /* This is the size of the heap provided to mm */

    size_t mm_heapsize;

    /* This is the heap maximum used memory size */

    size_t mm_maxused;

    /* This is the current used size of the heap */

    size_t mm_curused;

    /* Kasan is disable or enable for this heap */

    bool mm_nokasan;
};

struct malltask {
    /* Negative pid means differently. See include/malloc.h */

    int pid; /* Process id */
    unsigned long seqmin; /* The minimum sequence */
    unsigned long seqmax; /* The maximum sequence */
};

#define mm_memdump_s malltask

struct mallinfo {
    int arena;    /* non-mmapped space allocated from system */
    int ordblks;  /* number of free chunks */
    int smblks;   /* number of fastbin blocks */
    int hblks;    /* number of mmapped regions */
    int hblkhd;   /* space in mmapped regions */
    int usmblks;  /* always 0, preserved for backwards compatibility */
    int fsmblks;  /* space available in freed fastbin blocks */
    int uordblks; /* total allocated space */
    int fordblks; /* total free space */
    int keepcost; /* top-most, releasable (via malloc_trim) space */
    int aordblks; /* This is the number of allocated (in use) chunks for task */
    int mxordblk; /* size of the largest free chunk */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline struct mm_heap_s * mm_initialize(const char * name, void * mem, size_t size)
{
    (void)name;
    (void)mem;
    (void)size;
    return NULL;
}

static inline void mm_uninitialize(struct mm_heap_s * heap)
{
    (void)heap;
}

static inline void * mm_malloc(struct mm_heap_s * heap, size_t size)
{
    (void)heap;
    (void)size;
    return NULL;
}

static inline void mm_free(struct mm_heap_s * heap, void * ptr)
{
    (void)heap;
    (void)ptr;
}

static inline void mm_memdump(struct mm_heap_s * heap, const struct mm_memdump_s * dump)
{
    (void)heap;
    (void)dump;
}

static inline struct mallinfo mm_mallinfo(struct mm_heap_s * heap)
{
    (void)heap;
    struct mallinfo info = {0};
    return info;
}

static inline int gettid(void)
{
    return -1;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*NUTTX_MM_H*/
