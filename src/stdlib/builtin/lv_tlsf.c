#include "../../lv_conf_internal.h"
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN

#include "lv_tlsf_private.h"
#include "../../stdlib/lv_string.h"
#include "../../misc/lv_log.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_types.h"

#undef  printf
#define printf LV_LOG_ERROR

#define TLSF_MAX_POOL_SIZE (LV_MEM_SIZE + LV_MEM_POOL_EXPAND_SIZE)

#if !defined(_DEBUG)
    #define _DEBUG 0
#endif

#if defined(__cplusplus)
    #define tlsf_decl inline
#else
    #define tlsf_decl static
#endif

/*
** Architecture-specific bit manipulation routines.
**
** TLSF achieves O(1) cost for malloc and free operations by limiting
** the search for a free block to a free list of guaranteed size
** adequate to fulfill the request, combined with efficient free list
** queries using bitmasks and architecture-specific bit-manipulation
** routines.
**
** Most modern processors provide instructions to count leading zeroes
** in a word, find the lowest and highest set bit, etc. These
** specific implementations will be used when available, falling back
** to a reasonably efficient generic implementation.
**
** NOTE: TLSF spec relies on ffs/fls returning value 0..31.
** ffs/fls return 1-32 by default, returning 0 for error.
*/

/*
** Detect whether or not we are building for a 32- or 64-bit (LP/LLP)
** architecture. There is no reliable portable method at compile-time.
*/
#if defined (__alpha__) || defined (__ia64__) || defined (__x86_64__) \
    || defined (_WIN64) || defined (__LP64__) || defined (__LLP64__)
    #define TLSF_64BIT
#endif

/*
** Returns one plus the index of the most significant 1-bit of n,
** or if n is zero, returns zero.
*/
#ifdef TLSF_64BIT
    #define TLSF_FLS(n) ((n) & 0xffffffff00000000ull ? 32 + TLSF_FLS32((size_t)(n) >> 32) : TLSF_FLS32(n))
#else
    #define TLSF_FLS(n) TLSF_FLS32(n)
#endif

#define TLSF_FLS32(n) ((n) & 0xffff0000 ? 16 + TLSF_FLS16((n) >> 16) : TLSF_FLS16(n))
#define TLSF_FLS16(n) ((n) & 0xff00     ?  8 + TLSF_FLS8 ((n) >>  8) : TLSF_FLS8 (n))
#define TLSF_FLS8(n)  ((n) & 0xf0       ?  4 + TLSF_FLS4 ((n) >>  4) : TLSF_FLS4 (n))
#define TLSF_FLS4(n)  ((n) & 0xc        ?  2 + TLSF_FLS2 ((n) >>  2) : TLSF_FLS2 (n))
#define TLSF_FLS2(n)  ((n) & 0x2        ?  1 + TLSF_FLS1 ((n) >>  1) : TLSF_FLS1 (n))
#define TLSF_FLS1(n)  ((n) & 0x1        ?  1 : 0)

/*
** Returns round up value of log2(n).
** Note: it is used at compile time.
*/
#define TLSF_LOG2_CEIL(n) ((n) & (n - 1) ? TLSF_FLS(n) : TLSF_FLS(n) - 1)

/*
** gcc 3.4 and above have builtin support, specialized for architecture.
** Some compilers masquerade as gcc; patchlevel test filters them out.
*/
#if defined (__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)) \
    && defined (__GNUC_PATCHLEVEL__)

#if defined (__SNC__)
/* SNC for PlayStation 3. */

tlsf_decl int tlsf_ffs(unsigned int word)
{
    const unsigned int reverse = word & (~word + 1);
    const int bit = 32 - __builtin_clz(reverse);
    return bit - 1;
}

#else

tlsf_decl int tlsf_ffs(unsigned int word)
{
    return __builtin_ffs(word) - 1;
}

#endif

tlsf_decl int tlsf_fls(unsigned int word)
{
    const int bit = word ? 32 - __builtin_clz(word) : 0;
    return bit - 1;
}

#elif defined (_MSC_VER) && (_MSC_VER >= 1400) && (defined (_M_IX86) || defined (_M_X64))
/* Microsoft Visual C++ support on x86/X64 architectures. */

#include <intrin.h>

#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanForward)

tlsf_decl int tlsf_fls(unsigned int word)
{
    unsigned long index;
    return _BitScanReverse(&index, word) ? index : -1;
}

tlsf_decl int tlsf_ffs(unsigned int word)
{
    unsigned long index;
    return _BitScanForward(&index, word) ? index : -1;
}

#elif defined (_MSC_VER) && defined (_M_PPC)
/* Microsoft Visual C++ support on PowerPC architectures. */

#include <ppcintrinsics.h>

tlsf_decl int tlsf_fls(unsigned int word)
{
    const int bit = 32 - _CountLeadingZeros(word);
    return bit - 1;
}

tlsf_decl int tlsf_ffs(unsigned int word)
{
    const unsigned int reverse = word & (~word + 1);
    const int bit = 32 - _CountLeadingZeros(reverse);
    return bit - 1;
}

#elif defined (__ARMCC_VERSION)
/* RealView Compilation Tools for ARM */

tlsf_decl int tlsf_ffs(unsigned int word)
{
    const unsigned int reverse = word & (~word + 1);
    const int bit = 32 - __clz(reverse);
    return bit - 1;
}

tlsf_decl int tlsf_fls(unsigned int word)
{
    const int bit = word ? 32 - __clz(word) : 0;
    return bit - 1;
}

#elif defined (__ghs__)
/* Green Hills support for PowerPC */

#include <ppc_ghs.h>

tlsf_decl int tlsf_ffs(unsigned int word)
{
    const unsigned int reverse = word & (~word + 1);
    const int bit = 32 - __CLZ32(reverse);
    return bit - 1;
}

tlsf_decl int tlsf_fls(unsigned int word)
{
    const int bit = word ? 32 - __CLZ32(word) : 0;
    return bit - 1;
}

#else
/* Fall back to generic implementation. */

/* Implement ffs in terms of fls. */
tlsf_decl int tlsf_ffs(unsigned int word)
{
    const unsigned int reverse = word & (~word + 1);
    return TLSF_FLS32(reverse) - 1;
}

tlsf_decl int tlsf_fls(unsigned int word)
{
    return TLSF_FLS32(word) - 1;
}

#endif

/* Possibly 64-bit version of tlsf_fls. */
#if defined (TLSF_64BIT)
tlsf_decl int tlsf_fls_sizet(size_t size)
{
    int high = (int)(size >> 32);
    int bits = 0;
    if(high) {
        bits = 32 + tlsf_fls(high);
    }
    else {
        bits = tlsf_fls((int)size & 0xffffffff);

    }
    return bits;
}
#else
#define tlsf_fls_sizet tlsf_fls
#endif

#undef tlsf_decl

/*
** Constants.
*/

/* Public constants: may be modified. */
typedef enum {
    /* log2 of number of linear subdivisions of block sizes. Larger
    ** values require more memory in the control structure. Values of
    ** 4 or 5 are typical.
    */
    SL_INDEX_COUNT_LOG2 = 5,
} tlsf_public;

/* Private constants: do not modify. */
typedef enum {
#if defined (TLSF_64BIT)
    /* All allocation sizes and addresses are aligned to 8 bytes. */
    ALIGN_SIZE_LOG2 = 3,
#else
    /* All allocation sizes and addresses are aligned to 4 bytes. */
    ALIGN_SIZE_LOG2 = 2,
#endif
    ALIGN_SIZE = (1 << ALIGN_SIZE_LOG2),

    /*
    ** We support allocations of sizes up to (1 << FL_INDEX_MAX) bits.
    ** However, because we linearly subdivide the second-level lists, and
    ** our minimum size granularity is 4 bytes, it doesn't make sense to
    ** create first-level lists for sizes smaller than SL_INDEX_COUNT * 4,
    ** or (1 << (SL_INDEX_COUNT_LOG2 + 2)) bytes, as there we will be
    ** trying to split size ranges into more slots than we have available.
    ** Instead, we calculate the minimum threshold size, and place all
    ** blocks below that size into the 0th first-level list.
    */

#if defined (TLSF_MAX_POOL_SIZE)
    FL_INDEX_MAX = TLSF_LOG2_CEIL(TLSF_MAX_POOL_SIZE),
#elif defined (TLSF_64BIT)
    /*
    ** TODO: We can increase this to support larger sizes, at the expense
    ** of more overhead in the TLSF structure.
    */
    FL_INDEX_MAX = 32,
#else
    FL_INDEX_MAX = 30,
#endif
    SL_INDEX_COUNT = (1 << SL_INDEX_COUNT_LOG2),
    FL_INDEX_SHIFT = (SL_INDEX_COUNT_LOG2 + ALIGN_SIZE_LOG2),
    FL_INDEX_COUNT = (FL_INDEX_MAX - FL_INDEX_SHIFT + 1),

    SMALL_BLOCK_SIZE = (1 << FL_INDEX_SHIFT),
} tlsf_private;

/*
** Cast and min/max macros.
*/

#define tlsf_cast(t, exp)   ((t) (exp))
#define tlsf_min(a, b)      ((a) < (b) ? (a) : (b))
#define tlsf_max(a, b)      ((a) > (b) ? (a) : (b))

/*
** Set assert macro, if it has not been provided by the user.
*/
#define tlsf_assert LV_ASSERT

#if !defined (tlsf_assert)
    #define tlsf_assert assert
#endif

/*
** Static assertion mechanism.
*/

#define _tlsf_glue2(x, y) x ## y
#define _tlsf_glue(x, y) _tlsf_glue2(x, y)
#define tlsf_static_assert(exp) \
    typedef char _tlsf_glue(static_assert, __LINE__) [(exp) ? 1 : -1]

/* This code has been tested on 32- and 64-bit (LP/LLP) architectures. */
tlsf_static_assert(sizeof(int) * CHAR_BIT == 32);
tlsf_static_assert(sizeof(size_t) * CHAR_BIT >= 32);
tlsf_static_assert(sizeof(size_t) * CHAR_BIT <= 64);

/* SL_INDEX_COUNT must be <= number of bits in sl_bitmap's storage type. */
tlsf_static_assert(sizeof(unsigned int) * CHAR_BIT >= SL_INDEX_COUNT);

/* Ensure we've properly tuned our sizes. */
tlsf_static_assert(ALIGN_SIZE == SMALL_BLOCK_SIZE / SL_INDEX_COUNT);

/*
** Data structures and associated constants.
*/

/*
** Block header structure.
**
** There are several implementation subtleties involved:
** - The prev_phys_block field is only valid if the previous block is free.
** - The prev_phys_block field is actually stored at the end of the
**   previous block. It appears at the beginning of this structure only to
**   simplify the implementation.
** - The next_free / prev_free fields are only valid if the block is free.
*/
typedef struct block_header_t {
    /* Points to the previous physical block. */
    struct block_header_t * prev_phys_block;

    /* The size of this block, excluding the block header. */
    size_t size;

    /* Next and previous free blocks. */
    struct block_header_t * next_free;
    struct block_header_t * prev_free;
} block_header_t;

/*
** Since block sizes are always at least a multiple of 4, the two least
** significant bits of the size field are used to store the block status:
** - bit 0: whether block is busy or free
** - bit 1: whether previous block is busy or free
*/
static const size_t block_header_free_bit = 1 << 0;
static const size_t block_header_prev_free_bit = 1 << 1;

/*
** The size of the block header exposed to used blocks is the size field.
** The prev_phys_block field is stored *inside* the previous free block.
*/
static const size_t block_header_overhead = sizeof(size_t);

/* User data starts directly after the size field in a used block. */
static const size_t block_start_offset =
    offsetof(block_header_t, size) + sizeof(size_t);

/*
** A free block must be large enough to store its header minus the size of
** the prev_phys_block field, and no larger than the number of addressable
** bits for FL_INDEX.
*/
static const size_t block_size_min =
    sizeof(block_header_t) - sizeof(block_header_t *);
static const size_t block_size_max = tlsf_cast(size_t, 1) << FL_INDEX_MAX;

/* The TLSF control structure. */
typedef struct control_t {
    /* Empty lists point at this block to indicate they are free. */
    block_header_t block_null;

    /* Bitmaps for free lists. */
    unsigned int fl_bitmap;
    unsigned int sl_bitmap[FL_INDEX_COUNT];

    /* Head of free lists. */
    block_header_t * blocks[FL_INDEX_COUNT][SL_INDEX_COUNT];
} control_t;

/* A type used for casting when doing pointer arithmetic. */
typedef ptrdiff_t tlsfptr_t;

/*
** block_header_t member functions.
*/

static size_t block_size(const block_header_t * block)
{
    return block->size & ~(block_header_free_bit | block_header_prev_free_bit);
}

static void block_set_size(block_header_t * block, size_t size)
{
    const size_t oldsize = block->size;
    block->size = size | (oldsize & (block_header_free_bit | block_header_prev_free_bit));
}

static int block_is_last(const block_header_t * block)
{
    return block_size(block) == 0;
}

static int block_is_free(const block_header_t * block)
{
    return tlsf_cast(int, block->size & block_header_free_bit);
}

static void block_set_free(block_header_t * block)
{
    block->size |= block_header_free_bit;
}

static void block_set_used(block_header_t * block)
{
    block->size &= ~block_header_free_bit;
}

static int block_is_prev_free(const block_header_t * block)
{
    return tlsf_cast(int, block->size & block_header_prev_free_bit);
}

static void block_set_prev_free(block_header_t * block)
{
    block->size |= block_header_prev_free_bit;
}

static void block_set_prev_used(block_header_t * block)
{
    block->size &= ~block_header_prev_free_bit;
}

static block_header_t * block_from_ptr(const void * ptr)
{
    return tlsf_cast(block_header_t *,
                     tlsf_cast(unsigned char *, ptr) - block_start_offset);
}

static void * block_to_ptr(const block_header_t * block)
{
    return tlsf_cast(void *,
                     tlsf_cast(unsigned char *, block) + block_start_offset);
}

/* Return location of next block after block of given size. */
static block_header_t * offset_to_block(const void * ptr, size_t size)
{
    return tlsf_cast(block_header_t *, tlsf_cast(tlsfptr_t, ptr) + size);
}

/* Return location of previous block. */
static block_header_t * block_prev(const block_header_t * block)
{
    tlsf_assert(block_is_prev_free(block) && "previous block must be free");
    return block->prev_phys_block;
}

/* Return location of next existing block. */
static block_header_t * block_next(const block_header_t * block)
{
    block_header_t * next = offset_to_block(block_to_ptr(block),
                                            block_size(block) - block_header_overhead);
    tlsf_assert(!block_is_last(block));
    return next;
}

/* Link a new block with its physical neighbor, return the neighbor. */
static block_header_t * block_link_next(block_header_t * block)
{
    block_header_t * next = block_next(block);
    next->prev_phys_block = block;
    return next;
}

static void block_mark_as_free(block_header_t * block)
{
    /* Link the block to the next block, first. */
    block_header_t * next = block_link_next(block);
    block_set_prev_free(next);
    block_set_free(block);
}

static void block_mark_as_used(block_header_t * block)
{
    block_header_t * next = block_next(block);
    block_set_prev_used(next);
    block_set_used(block);
}

static size_t align_up(size_t x, size_t align)
{
    tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
    return (x + (align - 1)) & ~(align - 1);
}

static size_t align_down(size_t x, size_t align)
{
    tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
    return x - (x & (align - 1));
}

static void * align_ptr(const void * ptr, size_t align)
{
    const tlsfptr_t aligned =
        (tlsf_cast(tlsfptr_t, ptr) + (align - 1)) & ~(align - 1);
    tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
    return tlsf_cast(void *, aligned);
}

/*
** Adjust an allocation size to be aligned to word size, and no smaller
** than internal minimum.
*/
static size_t adjust_request_size(size_t size, size_t align)
{
    size_t adjust = 0;
    if(size) {
        const size_t aligned = align_up(size, align);

        /* aligned sized must not exceed block_size_max or we'll go out of bounds on sl_bitmap */
        if(aligned < block_size_max) {
            adjust = tlsf_max(aligned, block_size_min);
        }
    }
    return adjust;
}

/*
** TLSF utility functions. In most cases, these are direct translations of
** the documentation found in the white paper.
*/

static void mapping_insert(size_t size, int * fli, int * sli)
{
    int fl, sl;
    if(size < SMALL_BLOCK_SIZE) {
        /* Store small blocks in first list. */
        fl = 0;
        sl = tlsf_cast(int, size) / (SMALL_BLOCK_SIZE / SL_INDEX_COUNT);
    }
    else {
        fl = tlsf_fls_sizet(size);
        sl = tlsf_cast(int, size >> (fl - SL_INDEX_COUNT_LOG2)) ^ (1 << SL_INDEX_COUNT_LOG2);
        fl -= (FL_INDEX_SHIFT - 1);
    }
    *fli = fl;
    *sli = sl;
}

/* This version rounds up to the next block size (for allocations) */
static void mapping_search(size_t size, int * fli, int * sli)
{
    if(size >= SMALL_BLOCK_SIZE) {
        const size_t round = (1 << (tlsf_fls_sizet(size) - SL_INDEX_COUNT_LOG2)) - 1;
        size += round;
    }
    mapping_insert(size, fli, sli);
}

static block_header_t * search_suitable_block(control_t * control, int * fli, int * sli)
{
    int fl = *fli;
    int sl = *sli;

    /*
    ** First, search for a block in the list associated with the given
    ** fl/sl index.
    */
    unsigned int sl_map = control->sl_bitmap[fl] & (~0U << sl);
    if(!sl_map) {
        /* No block exists. Search in the next largest first-level list. */
        const unsigned int fl_map = control->fl_bitmap & (~0U << (fl + 1));
        if(!fl_map) {
            /* No free blocks available, memory has been exhausted. */
            return 0;
        }

        fl = tlsf_ffs(fl_map);
        *fli = fl;
        sl_map = control->sl_bitmap[fl];
    }
    tlsf_assert(sl_map && "internal error - second level bitmap is null");
    sl = tlsf_ffs(sl_map);
    *sli = sl;

    /* Return the first block in the free list. */
    return control->blocks[fl][sl];
}

/* Remove a free block from the free list.*/
static void remove_free_block(control_t * control, block_header_t * block, int fl, int sl)
{
    block_header_t * prev = block->prev_free;
    block_header_t * next = block->next_free;
    tlsf_assert(prev && "prev_free field cannot be null");
    tlsf_assert(next && "next_free field cannot be null");
    next->prev_free = prev;
    prev->next_free = next;

    /* If this block is the head of the free list, set new head. */
    if(control->blocks[fl][sl] == block) {
        control->blocks[fl][sl] = next;

        /* If the new head is null, clear the bitmap. */
        if(next == &control->block_null) {
            control->sl_bitmap[fl] &= ~(1U << sl);

            /* If the second bitmap is now empty, clear the fl bitmap. */
            if(!control->sl_bitmap[fl]) {
                control->fl_bitmap &= ~(1U << fl);
            }
        }
    }
}

/* Insert a free block into the free block list. */
static void insert_free_block(control_t * control, block_header_t * block, int fl, int sl)
{
    block_header_t * current = control->blocks[fl][sl];
    tlsf_assert(current && "free list cannot have a null entry");
    tlsf_assert(block && "cannot insert a null entry into the free list");
    block->next_free = current;
    block->prev_free = &control->block_null;
    current->prev_free = block;

    tlsf_assert(block_to_ptr(block) == align_ptr(block_to_ptr(block), ALIGN_SIZE)
                && "block not aligned properly");
    /*
    ** Insert the new block at the head of the list, and mark the first-
    ** and second-level bitmaps appropriately.
    */
    control->blocks[fl][sl] = block;
    control->fl_bitmap |= (1U << fl);
    control->sl_bitmap[fl] |= (1U << sl);
}

/* Remove a given block from the free list. */
static void block_remove(control_t * control, block_header_t * block)
{
    int fl, sl;
    mapping_insert(block_size(block), &fl, &sl);
    remove_free_block(control, block, fl, sl);
}

/* Insert a given block into the free list. */
static void block_insert(control_t * control, block_header_t * block)
{
    int fl, sl;
    mapping_insert(block_size(block), &fl, &sl);
    insert_free_block(control, block, fl, sl);
}

static int block_can_split(block_header_t * block, size_t size)
{
    return block_size(block) >= sizeof(block_header_t) + size;
}

/* Split a block into two, the second of which is free. */
static block_header_t * block_split(block_header_t * block, size_t size)
{
    /* Calculate the amount of space left in the remaining block. */
    block_header_t * remaining =
        offset_to_block(block_to_ptr(block), size - block_header_overhead);

    const size_t remain_size = block_size(block) - (size + block_header_overhead);

    tlsf_assert(block_to_ptr(remaining) == align_ptr(block_to_ptr(remaining), ALIGN_SIZE)
                && "remaining block not aligned properly");

    tlsf_assert(block_size(block) == remain_size + size + block_header_overhead);
    block_set_size(remaining, remain_size);
    tlsf_assert(block_size(remaining) >= block_size_min && "block split with invalid size");

    block_set_size(block, size);
    block_mark_as_free(remaining);

    return remaining;
}

/* Absorb a free block's storage into an adjacent previous free block. */
static block_header_t * block_absorb(block_header_t * prev, block_header_t * block)
{
    tlsf_assert(!block_is_last(prev) && "previous block can't be last");
    /* Note: Leaves flags untouched. */
    prev->size += block_size(block) + block_header_overhead;
    block_link_next(prev);
    return prev;
}

/* Merge a just-freed block with an adjacent previous free block. */
static block_header_t * block_merge_prev(control_t * control, block_header_t * block)
{
    if(block_is_prev_free(block)) {
        block_header_t * prev = block_prev(block);
        tlsf_assert(prev && "prev physical block can't be null");
        tlsf_assert(block_is_free(prev) && "prev block is not free though marked as such");
        block_remove(control, prev);
        block = block_absorb(prev, block);
    }

    return block;
}

/* Merge a just-freed block with an adjacent free block. */
static block_header_t * block_merge_next(control_t * control, block_header_t * block)
{
    block_header_t * next = block_next(block);
    tlsf_assert(next && "next physical block can't be null");

    if(block_is_free(next)) {
        tlsf_assert(!block_is_last(block) && "previous block can't be last");
        block_remove(control, next);
        block = block_absorb(block, next);
    }

    return block;
}

/* Trim any trailing block space off the end of a block, return to pool. */
static void block_trim_free(control_t * control, block_header_t * block, size_t size)
{
    tlsf_assert(block_is_free(block) && "block must be free");
    if(block_can_split(block, size)) {
        block_header_t * remaining_block = block_split(block, size);
        block_link_next(block);
        block_set_prev_free(remaining_block);
        block_insert(control, remaining_block);
    }
}

/* Trim any trailing block space off the end of a used block, return to pool. */
static void block_trim_used(control_t * control, block_header_t * block, size_t size)
{
    tlsf_assert(!block_is_free(block) && "block must be used");
    if(block_can_split(block, size)) {
        /* If the next block is free, we must coalesce. */
        block_header_t * remaining_block = block_split(block, size);
        block_set_prev_used(remaining_block);

        remaining_block = block_merge_next(control, remaining_block);
        block_insert(control, remaining_block);
    }
}

static block_header_t * block_trim_free_leading(control_t * control, block_header_t * block, size_t size)
{
    block_header_t * remaining_block = block;
    if(block_can_split(block, size)) {
        /* We want the 2nd block. */
        remaining_block = block_split(block, size - block_header_overhead);
        block_set_prev_free(remaining_block);

        block_link_next(block);
        block_insert(control, block);
    }

    return remaining_block;
}

static block_header_t * block_locate_free(control_t * control, size_t size)
{
    int fl = 0, sl = 0;
    block_header_t * block = 0;

    if(size) {
        mapping_search(size, &fl, &sl);

        /*
        ** mapping_search can futz with the size, so for excessively large sizes it can sometimes wind up
        ** with indices that are off the end of the block array.
        ** So, we protect against that here, since this is the only callsite of mapping_search.
        ** Note that we don't need to check sl, since it comes from a modulo operation that guarantees it's always in range.
        */
        if(fl < FL_INDEX_COUNT) {
            block = search_suitable_block(control, &fl, &sl);
        }
    }

    if(block) {
        tlsf_assert(block_size(block) >= size);
        remove_free_block(control, block, fl, sl);
    }

    return block;
}

static void * block_prepare_used(control_t * control, block_header_t * block, size_t size)
{
    void * p = 0;
    if(block) {
        tlsf_assert(size && "size must be non-zero");
        block_trim_free(control, block, size);
        block_mark_as_used(block);
        p = block_to_ptr(block);
    }
    return p;
}

/* Clear structure and point all empty lists at the null block. */
static void control_constructor(control_t * control)
{
    int i, j;

    control->block_null.next_free = &control->block_null;
    control->block_null.prev_free = &control->block_null;

    control->fl_bitmap = 0;
    for(i = 0; i < FL_INDEX_COUNT; ++i) {
        control->sl_bitmap[i] = 0;
        for(j = 0; j < SL_INDEX_COUNT; ++j) {
            control->blocks[i][j] = &control->block_null;
        }
    }
}

/*
** Debugging utilities.
*/

typedef struct integrity_t {
    int prev_status;
    int status;
} integrity_t;

#define tlsf_insist(x) { tlsf_assert(x); if (!(x)) { status--; } }

static void integrity_walker(void * ptr, size_t size, int used, void * user)
{
    block_header_t * block = block_from_ptr(ptr);
    integrity_t * integ = tlsf_cast(integrity_t *, user);
    const int this_prev_status = block_is_prev_free(block) ? 1 : 0;
    const int this_status = block_is_free(block) ? 1 : 0;
    const size_t this_block_size = block_size(block);

    int status = 0;
    LV_UNUSED(used);
    tlsf_insist(integ->prev_status == this_prev_status && "prev status incorrect");
    tlsf_insist(size == this_block_size && "block size incorrect");

    integ->prev_status = this_status;
    integ->status += status;
}

int lv_tlsf_check(lv_tlsf_t tlsf)
{
    int i, j;

    control_t * control = tlsf_cast(control_t *, tlsf);
    int status = 0;

    /* Check that the free lists and bitmaps are accurate. */
    for(i = 0; i < FL_INDEX_COUNT; ++i) {
        for(j = 0; j < SL_INDEX_COUNT; ++j) {
            const int fl_map = control->fl_bitmap & (1U << i);
            const int sl_list = control->sl_bitmap[i];
            const int sl_map = sl_list & (1U << j);
            const block_header_t * block = control->blocks[i][j];

            /* Check that first- and second-level lists agree. */
            if(!fl_map) {
                tlsf_insist(!sl_map && "second-level map must be null");
            }

            if(!sl_map) {
                tlsf_insist(block == &control->block_null && "block list must be null");
                continue;
            }

            /* Check that there is at least one free block. */
            tlsf_insist(sl_list && "no free blocks in second-level map");
            tlsf_insist(block != &control->block_null && "block should not be null");

            while(block != &control->block_null) {
                int fli, sli;
                tlsf_insist(block_is_free(block) && "block should be free");
                tlsf_insist(!block_is_prev_free(block) && "blocks should have coalesced");
                tlsf_insist(!block_is_free(block_next(block)) && "blocks should have coalesced");
                tlsf_insist(block_is_prev_free(block_next(block)) && "block should be free");
                tlsf_insist(block_size(block) >= block_size_min && "block not minimum size");

                mapping_insert(block_size(block), &fli, &sli);
                tlsf_insist(fli == i && sli == j && "block size indexed in wrong list");
                block = block->next_free;
            }
        }
    }

    return status;
}

#undef tlsf_insist

static void default_walker(void * ptr, size_t size, int used, void * user)
{
    LV_UNUSED(user);
    printf("\t%p %s size: %x (%p)\n", ptr, used ? "used" : "free", (unsigned int)size, (void *)block_from_ptr(ptr));
}

void lv_tlsf_walk_pool(lv_pool_t pool, lv_tlsf_walker walker, void * user)
{
    lv_tlsf_walker pool_walker = walker ? walker : default_walker;
    block_header_t * block =
        offset_to_block(pool, -(int)block_header_overhead);

    while(block && !block_is_last(block)) {
        pool_walker(
            block_to_ptr(block),
            block_size(block),
            !block_is_free(block),
            user);
        block = block_next(block);
    }
}

size_t lv_tlsf_block_size(void * ptr)
{
    size_t size = 0;
    if(ptr) {
        const block_header_t * block = block_from_ptr(ptr);
        size = block_size(block);
    }
    return size;
}

int lv_tlsf_check_pool(lv_pool_t pool)
{
    /* Check that the blocks are physically correct. */
    integrity_t integ = { 0, 0 };
    lv_tlsf_walk_pool(pool, integrity_walker, &integ);

    return integ.status;
}

/*
** Size of the TLSF structures in a given memory block passed to
** lv_tlsf_create, equal to the size of a control_t
*/
size_t lv_tlsf_size(void)
{
    return sizeof(control_t);
}

size_t lv_tlsf_align_size(void)
{
    return ALIGN_SIZE;
}

size_t lv_tlsf_block_size_min(void)
{
    return block_size_min;
}

size_t lv_tlsf_block_size_max(void)
{
    return block_size_max;
}

/*
** Overhead of the TLSF structures in a given memory block passed to
** lv_tlsf_add_pool, equal to the overhead of a free block and the
** sentinel block.
*/
size_t lv_tlsf_pool_overhead(void)
{
    return 2 * block_header_overhead;
}

size_t lv_tlsf_alloc_overhead(void)
{
    return block_header_overhead;
}

lv_pool_t lv_tlsf_add_pool(lv_tlsf_t tlsf, void * mem, size_t bytes)
{
    block_header_t * block;
    block_header_t * next;

    const size_t pool_overhead = lv_tlsf_pool_overhead();
    const size_t pool_bytes = align_down(bytes - pool_overhead, ALIGN_SIZE);

    if(((ptrdiff_t)mem % ALIGN_SIZE) != 0) {
        printf("lv_tlsf_add_pool: Memory must be aligned by %u bytes.\n",
               (unsigned int)ALIGN_SIZE);
        return 0;
    }

    if(pool_bytes < block_size_min || pool_bytes > block_size_max) {
#if defined (TLSF_64BIT)
        printf("lv_tlsf_add_pool: Memory size must be between 0x%x and 0x%x00 bytes.\n",
               (unsigned int)(pool_overhead + block_size_min),
               (unsigned int)((pool_overhead + block_size_max) / 256));
#else
        printf("lv_tlsf_add_pool: Memory size must be between %u and %u bytes.\n",
               (unsigned int)(pool_overhead + block_size_min),
               (unsigned int)(pool_overhead + block_size_max));
#endif
        return 0;
    }

    /*
    ** Create the main free block. Offset the start of the block slightly
    ** so that the prev_phys_block field falls outside of the pool -
    ** it will never be used.
    */
    block = offset_to_block(mem, -(tlsfptr_t)block_header_overhead);
    block_set_size(block, pool_bytes);
    block_set_free(block);
    block_set_prev_used(block);
    block_insert(tlsf_cast(control_t *, tlsf), block);

    /* Split the block to create a zero-size sentinel block. */
    next = block_link_next(block);
    block_set_size(next, 0);
    block_set_used(next);
    block_set_prev_free(next);

    return mem;
}

void lv_tlsf_remove_pool(lv_tlsf_t tlsf, lv_pool_t pool)
{
    control_t * control = tlsf_cast(control_t *, tlsf);
    block_header_t * block = offset_to_block(pool, -(int)block_header_overhead);

    int fl = 0, sl = 0;

    tlsf_assert(block_is_free(block) && "block should be free");
    tlsf_assert(!block_is_free(block_next(block)) && "next block should not be free");
    tlsf_assert(block_size(block_next(block)) == 0 && "next block size should be zero");

    mapping_insert(block_size(block), &fl, &sl);
    remove_free_block(control, block, fl, sl);
}

/*
** TLSF main interface.
*/

#if _DEBUG
int test_ffs_fls()
{
    /* Verify ffs/fls work properly. */
    int rv = 0;
    rv += (tlsf_ffs(0) == -1) ? 0 : 0x1;
    rv += (tlsf_fls(0) == -1) ? 0 : 0x2;
    rv += (tlsf_ffs(1) == 0) ? 0 : 0x4;
    rv += (tlsf_fls(1) == 0) ? 0 : 0x8;
    rv += (tlsf_ffs(0x80000000) == 31) ? 0 : 0x10;
    rv += (tlsf_ffs(0x80008000) == 15) ? 0 : 0x20;
    rv += (tlsf_fls(0x80000008) == 31) ? 0 : 0x40;
    rv += (tlsf_fls(0x7FFFFFFF) == 30) ? 0 : 0x80;

#if defined (TLSF_64BIT)
    rv += (tlsf_fls_sizet(0x80000000) == 31) ? 0 : 0x100;
    rv += (tlsf_fls_sizet(0x100000000) == 32) ? 0 : 0x200;
    rv += (tlsf_fls_sizet(0xffffffffffffffff) == 63) ? 0 : 0x400;
#endif

    if(rv) {
        printf("test_ffs_fls: %x ffs/fls tests failed.\n", rv);
    }
    return rv;
}
#endif

lv_tlsf_t lv_tlsf_create(void * mem)
{
#if _DEBUG
    if(test_ffs_fls()) {
        return 0;
    }
#endif

    if(((tlsfptr_t)mem % ALIGN_SIZE) != 0) {
        printf("lv_tlsf_create: Memory must be aligned to %u bytes.\n",
               (unsigned int)ALIGN_SIZE);
        return 0;
    }

    control_constructor(tlsf_cast(control_t *, mem));

    return tlsf_cast(lv_tlsf_t, mem);
}

lv_tlsf_t lv_tlsf_create_with_pool(void * mem, size_t bytes)
{
    lv_tlsf_t tlsf = lv_tlsf_create(mem);
    lv_tlsf_add_pool(tlsf, (char *)mem + lv_tlsf_size(), bytes - lv_tlsf_size());
    return tlsf;
}

void lv_tlsf_destroy(lv_tlsf_t tlsf)
{
    /* Nothing to do. */
    LV_UNUSED(tlsf);
}

lv_pool_t lv_tlsf_get_pool(lv_tlsf_t tlsf)
{
    return tlsf_cast(lv_pool_t, (char *)tlsf + lv_tlsf_size());
}

void * lv_tlsf_malloc(lv_tlsf_t tlsf, size_t size)
{
    control_t * control = tlsf_cast(control_t *, tlsf);
    const size_t adjust = adjust_request_size(size, ALIGN_SIZE);
    block_header_t * block = block_locate_free(control, adjust);
    return block_prepare_used(control, block, adjust);
}

void * lv_tlsf_memalign(lv_tlsf_t tlsf, size_t align, size_t size)
{
    control_t * control = tlsf_cast(control_t *, tlsf);
    const size_t adjust = adjust_request_size(size, ALIGN_SIZE);

    /*
    ** We must allocate an additional minimum block size bytes so that if
    ** our free block will leave an alignment gap which is smaller, we can
    ** trim a leading free block and release it back to the pool. We must
    ** do this because the previous physical block is in use, therefore
    ** the prev_phys_block field is not valid, and we can't simply adjust
    ** the size of that block.
    */
    const size_t gap_minimum = sizeof(block_header_t);
    const size_t size_with_gap = adjust_request_size(adjust + align + gap_minimum, align);

    /*
    ** If alignment is less than or equals base alignment, we're done.
    ** If we requested 0 bytes, return null, as lv_tlsf_malloc(0) does.
    */
    const size_t aligned_size = (adjust && align > ALIGN_SIZE) ? size_with_gap : adjust;

    block_header_t * block = block_locate_free(control, aligned_size);

    /* This can't be a static assert. */
    tlsf_assert(sizeof(block_header_t) == block_size_min + block_header_overhead);

    if(block) {
        void * ptr = block_to_ptr(block);
        void * aligned = align_ptr(ptr, align);
        size_t gap = tlsf_cast(size_t,
                               tlsf_cast(tlsfptr_t, aligned) - tlsf_cast(tlsfptr_t, ptr));

        /* If gap size is too small, offset to next aligned boundary. */
        if(gap && gap < gap_minimum) {
            const size_t gap_remain = gap_minimum - gap;
            const size_t offset = tlsf_max(gap_remain, align);
            const void * next_aligned = tlsf_cast(void *,
                                                  tlsf_cast(tlsfptr_t, aligned) + offset);

            aligned = align_ptr(next_aligned, align);
            gap = tlsf_cast(size_t,
                            tlsf_cast(tlsfptr_t, aligned) - tlsf_cast(tlsfptr_t, ptr));
        }

        if(gap) {
            tlsf_assert(gap >= gap_minimum && "gap size too small");
            block = block_trim_free_leading(control, block, gap);
        }
    }

    return block_prepare_used(control, block, adjust);
}

size_t lv_tlsf_free(lv_tlsf_t tlsf, const void * ptr)
{
    size_t size = 0;
    /* Don't attempt to free a NULL pointer. */
    if(ptr) {
        control_t * control = tlsf_cast(control_t *, tlsf);
        block_header_t * block = block_from_ptr(ptr);
        tlsf_assert(!block_is_free(block) && "block already marked as free");
        size = block->size;
        block_mark_as_free(block);
        block = block_merge_prev(control, block);
        block = block_merge_next(control, block);
        block_insert(control, block);
    }

    return size;
}

/*
** The TLSF block information provides us with enough information to
** provide a reasonably intelligent implementation of realloc, growing or
** shrinking the currently allocated block as required.
**
** This routine handles the somewhat esoteric edge cases of realloc:
** - a non-zero size with a null pointer will behave like malloc
** - a zero size with a non-null pointer will behave like free
** - a request that cannot be satisfied will leave the original buffer
**   untouched
** - an extended buffer size will leave the newly-allocated area with
**   contents undefined
*/
void * lv_tlsf_realloc(lv_tlsf_t tlsf, void * ptr, size_t size)
{
    control_t * control = tlsf_cast(control_t *, tlsf);
    void * p = 0;

    /* Zero-size requests are treated as free. */
    if(ptr && size == 0) {
        lv_tlsf_free(tlsf, ptr);
    }
    /* Requests with NULL pointers are treated as malloc. */
    else if(!ptr) {
        p = lv_tlsf_malloc(tlsf, size);
    }
    else {
        block_header_t * block = block_from_ptr(ptr);
        block_header_t * next = block_next(block);

        const size_t cursize = block_size(block);
        const size_t combined = cursize + block_size(next) + block_header_overhead;
        const size_t adjust = adjust_request_size(size, ALIGN_SIZE);
        if(size > cursize && adjust == 0) {
            /* The request is probably too large, fail */
            return NULL;
        }

        tlsf_assert(!block_is_free(block) && "block already marked as free");

        /*
        ** If the next block is used, or when combined with the current
        ** block, does not offer enough space, we must reallocate and copy.
        */
        if(adjust > cursize && (!block_is_free(next) || adjust > combined)) {
            p = lv_tlsf_malloc(tlsf, size);
            if(p) {
                const size_t minsize = tlsf_min(cursize, size);
                lv_memcpy(p, ptr, minsize);
                lv_tlsf_free(tlsf, ptr);
            }
        }
        else {
            /* Do we need to expand to the next block? */
            if(adjust > cursize) {
                block_merge_next(control, block);
                block_mark_as_used(block);
            }

            /* Trim the resulting block and return the original pointer. */
            block_trim_used(control, block, adjust);
            p = ptr;
        }
    }

    return p;
}

#endif /*LV_STDLIB_BUILTIN*/
