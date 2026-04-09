/**
 * @file test_draw_buf_vram.c
 *
 * Tests for VRAM buffer residency (LV_USE_DRAW_VRAM).
 *
 * Uses a dummy "fake VRAM" draw unit that backs VRAM allocations with
 * regular heap memory. This lets us exercise the full ensure_resident
 * state machine, flag handling, canvas interactions, and cross-unit
 * migration without any real hardware.
 */

#if LV_BUILD_TEST

#include "../lvgl.h"

#if LV_USE_DRAW_VRAM
#include "../../lvgl_private.h"

#include "unity/unity.h"

#include <string.h>

/**********************
 *  FAKE VRAM UNIT
 **********************/

/** Extended vram_res for the fake VRAM draw unit */
typedef struct {
    lv_draw_buf_vram_res_t base;   /**< Must be first */
    void * fake_vram;              /**< Heap block pretending to be VRAM */
    uint32_t alloc_size;
    bool valid;                    /**< false = simulate VRAM lost */
} fake_vram_res_t;

/** Counters to track callback invocations */
typedef struct {
    int alloc_count;
    int free_count;
    int upload_count;
    int download_count;
    int check_count;
    int dup_count;
    int font_free_count;
    bool fail_alloc;               /**< If true, vram_alloc_cb returns false */
    bool fail_upload;
    bool fail_download;
    bool fail_dup;
} fake_vram_stats_t;

static lv_draw_unit_t s_fake_unit_a;
static lv_draw_unit_t s_fake_unit_b;   /**< Second unit for cross-unit migration tests */
static fake_vram_stats_t s_stats_a;
static fake_vram_stats_t s_stats_b;

static fake_vram_stats_t * get_stats(lv_draw_unit_t * unit)
{
    if(unit == &s_fake_unit_a) return &s_stats_a;
    if(unit == &s_fake_unit_b) return &s_stats_b;
    return NULL;
}

static bool fake_vram_alloc_cb(lv_draw_unit_t * unit, lv_draw_buf_t * buf)
{
    fake_vram_stats_t * stats = get_stats(unit);
    stats->alloc_count++;
    if(stats->fail_alloc) return false;

    uint32_t w = buf->header.w;
    uint32_t h = buf->header.h;
    lv_color_format_t cf = (lv_color_format_t)buf->header.cf;
    uint32_t stride = buf->header.stride;
    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, cf);
    uint32_t size = stride * h;

    fake_vram_res_t * vr = lv_malloc_zeroed(sizeof(fake_vram_res_t));
    if(vr == NULL) return false;

    vr->fake_vram = lv_malloc(size);
    if(vr->fake_vram == NULL) {
        lv_free(vr);
        return false;
    }
    lv_memzero(vr->fake_vram, size);

    vr->alloc_size = size;
    vr->valid = true;
    vr->base.unit = unit;
    vr->base.size = size;
    buf->vram_res = &vr->base;
    return true;
}

static void fake_vram_free_cb(lv_draw_unit_t * unit, lv_draw_buf_t * buf)
{
    fake_vram_stats_t * stats = get_stats(unit);
    stats->free_count++;

    if(buf->vram_res == NULL) return;
    fake_vram_res_t * vr = (fake_vram_res_t *)buf->vram_res;
    if(vr->fake_vram) {
        lv_free(vr->fake_vram);
        vr->fake_vram = NULL;
    }
    lv_free(vr);
    buf->vram_res = NULL;
}

static bool fake_vram_upload_cb(lv_draw_unit_t * unit, lv_draw_buf_t * buf)
{
    fake_vram_stats_t * stats = get_stats(unit);
    stats->upload_count++;
    if(stats->fail_upload) return false;

    /* Alloc VRAM first */
    if(!fake_vram_alloc_cb(unit, buf)) return false;
    stats->alloc_count--;  /* Don't double-count the alloc inside upload */

    /* Copy CPU data to fake VRAM */
    fake_vram_res_t * vr = (fake_vram_res_t *)buf->vram_res;
    if(buf->data != NULL && vr->fake_vram != NULL) {
        uint32_t copy_size = vr->alloc_size;
        if(buf->data_size < copy_size) copy_size = buf->data_size;
        lv_memcpy(vr->fake_vram, buf->data, copy_size);
    }
    return true;
}

static bool fake_vram_download_cb(lv_draw_unit_t * unit, lv_draw_buf_t * buf)
{
    fake_vram_stats_t * stats = get_stats(unit);
    stats->download_count++;
    if(stats->fail_download) return false;

    if(buf->vram_res == NULL || buf->data == NULL) return false;
    fake_vram_res_t * vr = (fake_vram_res_t *)buf->vram_res;
    if(vr->fake_vram == NULL) return false;

    uint32_t copy_size = vr->alloc_size;
    if(buf->data_size < copy_size) copy_size = buf->data_size;
    lv_memcpy(buf->data, vr->fake_vram, copy_size);
    return true;
}

static bool fake_vram_check_cb(lv_draw_unit_t * unit, lv_draw_buf_t * buf)
{
    fake_vram_stats_t * stats = get_stats(unit);
    stats->check_count++;

    if(buf->vram_res == NULL) return false;
    fake_vram_res_t * vr = (fake_vram_res_t *)buf->vram_res;
    return vr->valid;
}

static void fake_vram_font_free_cb(lv_draw_unit_t * unit, void * font)
{
    fake_vram_stats_t * stats = get_stats(unit);
    stats->font_free_count++;

    lv_font_t * f = (lv_font_t *)font;
    lv_font_dsc_base_t * dsc_base = (lv_font_dsc_base_t *)f->dsc;
    if(dsc_base && dsc_base->vram_res) {
        fake_vram_res_t * vr = (fake_vram_res_t *)dsc_base->vram_res;
        if(vr->fake_vram) {
            lv_free(vr->fake_vram);
            vr->fake_vram = NULL;
        }
        lv_free(vr);
        dsc_base->vram_res = NULL;
    }
}

static bool fake_vram_dup_cb(lv_draw_unit_t * unit, lv_draw_buf_t * dest, const lv_draw_buf_t * src)
{
    fake_vram_stats_t * stats = get_stats(unit);
    stats->dup_count++;
    if(stats->fail_dup) return false;

    if(src->vram_res == NULL) return false;
    fake_vram_res_t * src_vr = (fake_vram_res_t *)src->vram_res;
    if(src_vr->fake_vram == NULL) return false;

    /* Allocate VRAM for dest */
    if(!fake_vram_alloc_cb(unit, dest)) return false;
    stats->alloc_count--;  /* Don't double-count the alloc inside dup */

    /* Copy VRAM to VRAM */
    fake_vram_res_t * dest_vr = (fake_vram_res_t *)dest->vram_res;
    uint32_t copy_size = LV_MIN(src_vr->alloc_size, dest_vr->alloc_size);
    lv_memcpy(dest_vr->fake_vram, src_vr->fake_vram, copy_size);
    return true;
}

static void init_fake_unit(lv_draw_unit_t * unit, fake_vram_stats_t * stats)
{
    lv_memzero(unit, sizeof(lv_draw_unit_t));
    lv_memzero(stats, sizeof(fake_vram_stats_t));
    unit->vram_alloc_cb = fake_vram_alloc_cb;
    unit->vram_free_cb = fake_vram_free_cb;
    unit->vram_upload_cb = fake_vram_upload_cb;
    unit->vram_download_cb = fake_vram_download_cb;
    unit->vram_check_cb = fake_vram_check_cb;
    unit->vram_dup_cb = fake_vram_dup_cb;
    unit->vram_font_free_cb = fake_vram_font_free_cb;
}

/**********************
 *  HELPERS
 **********************/

/** Write a recognizable pattern into a CPU buffer */
static void fill_pattern(lv_draw_buf_t * buf, uint8_t value)
{
    if(buf->data == NULL) return;
    uint32_t size = buf->header.stride * buf->header.h;
    lv_memset(buf->data, value, size);
}

/** Check that the fake VRAM contains a given byte pattern */
static bool vram_contains_pattern(lv_draw_buf_t * buf, uint8_t value)
{
    if(buf->vram_res == NULL) return false;
    fake_vram_res_t * vr = (fake_vram_res_t *)buf->vram_res;
    if(vr->fake_vram == NULL) return false;
    uint8_t * p = (uint8_t *)vr->fake_vram;
    for(uint32_t i = 0; i < vr->alloc_size; i++) {
        if(p[i] != value) return false;
    }
    return true;
}

/** Invalidate the fake VRAM (simulate GPU reclaim) */
static void invalidate_vram(lv_draw_buf_t * buf)
{
    if(buf->vram_res == NULL) return;
    fake_vram_res_t * vr = (fake_vram_res_t *)buf->vram_res;
    vr->valid = false;
}

/**********************
 *  SETUP / TEARDOWN
 **********************/

void setUp(void)
{
    init_fake_unit(&s_fake_unit_a, &s_stats_a);
    init_fake_unit(&s_fake_unit_b, &s_stats_b);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/**********************
 *   TEST CASES
 **********************/

/*----------------------------------------------------------------------
 * 1. Lazy allocation
 *----------------------------------------------------------------------*/

/** lv_draw_buf_create with VRAM enabled produces header-only buffer */
void test_vram_lazy_create_is_header_only(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(40, 30, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);
    TEST_ASSERT(buf->header.w == 40);
    TEST_ASSERT(buf->header.h == 30);
    TEST_ASSERT(buf->data_size > 0);
    lv_draw_buf_destroy(buf);
}

/** ensure_resident(buf, NULL) on a lazy buffer allocates CPU memory */
void test_vram_lazy_alloc_cpu(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(20, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_NULL(buf->data);

    bool ok = lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);

    lv_draw_buf_destroy(buf);
}

/** ensure_resident(buf, vram_unit) on a lazy buffer allocates VRAM */
void test_vram_lazy_alloc_vram(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(20, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    TEST_ASSERT_EQUAL_PTR(&s_fake_unit_a, buf->vram_res->unit);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.alloc_count);
    /* CPU data should not be allocated (or freed after VRAM alloc) */

    lv_draw_buf_destroy(buf);
}

/** Lazy VRAM allocation failure returns false, buffer stays header-only */
void test_vram_lazy_alloc_vram_failure(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(20, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    s_stats_a.fail_alloc = true;
    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);

    lv_draw_buf_destroy(buf);
}

/*----------------------------------------------------------------------
 * 2. CPU <-> VRAM transitions
 *----------------------------------------------------------------------*/

/** CPU buffer uploaded to VRAM preserves content */
void test_vram_cpu_to_vram_upload(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* First get CPU resident */
    lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_NOT_NULL(buf->data);
    fill_pattern(buf, 0xAB);

    /* Now move to VRAM */
    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.upload_count);

    /* VRAM should contain the pattern */
    TEST_ASSERT_TRUE(vram_contains_pattern(buf, 0xAB));

    /* CPU data should be freed (ALLOCATED flag was set) */
    TEST_ASSERT_NULL(buf->data);

    lv_draw_buf_destroy(buf);
}

/** VRAM buffer downloaded to CPU preserves content */
void test_vram_vram_to_cpu_download(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Allocate in VRAM first */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_NOT_NULL(buf->vram_res);

    /* Write pattern to fake VRAM */
    fake_vram_res_t * vr = (fake_vram_res_t *)buf->vram_res;
    lv_memset(vr->fake_vram, 0xCD, vr->alloc_size);

    /* Move to CPU */
    bool ok = lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.download_count);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.free_count);

    /* CPU should have the pattern */
    uint8_t * p = buf->data;
    uint32_t stride = buf->header.stride;
    TEST_ASSERT_EQUAL_UINT8(0xCD, p[0]);
    TEST_ASSERT_EQUAL_UINT8(0xCD, p[stride * buf->header.h - 1]);

    lv_draw_buf_destroy(buf);
}

/** Repeated ensure_resident on same unit is a no-op */
void test_vram_same_unit_noop(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.alloc_count);

    /* Second call should not alloc/upload again */
    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.alloc_count);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.upload_count);

    lv_draw_buf_destroy(buf);
}

/** Repeated ensure_resident on CPU is a no-op */
void test_vram_cpu_noop(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, NULL);
    uint8_t * data1 = buf->data;
    TEST_ASSERT_NOT_NULL(data1);

    bool ok = lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_PTR(data1, buf->data);

    lv_draw_buf_destroy(buf);
}

/*----------------------------------------------------------------------
 * 3. Cross-unit migration
 *----------------------------------------------------------------------*/

/** Buffer migrates from unit A to unit B, content preserved */
void test_vram_cross_unit_migration(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Put in unit A */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    fake_vram_res_t * vr_a = (fake_vram_res_t *)buf->vram_res;
    lv_memset(vr_a->fake_vram, 0x42, vr_a->alloc_size);

    /* Migrate to unit B */
    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_b);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    TEST_ASSERT_EQUAL_PTR(&s_fake_unit_b, buf->vram_res->unit);

    /* Unit A should have been freed */
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.free_count);

    /* Download from A + upload to B should have happened */
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.download_count);
    TEST_ASSERT_EQUAL_INT(1, s_stats_b.upload_count);

    /* Content should be preserved in unit B's VRAM */
    TEST_ASSERT_TRUE(vram_contains_pattern(buf, 0x42));

    lv_draw_buf_destroy(buf);
}

/** Cross-unit migration with DISCARDABLE skips download */
void test_vram_cross_unit_discard_skips_download(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    lv_draw_buf_set_flag(buf, LV_IMAGE_FLAGS_DISCARDABLE);

    /* Migrate to unit B — should skip download and just alloc */
    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_b);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.download_count);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.free_count);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    TEST_ASSERT_EQUAL_PTR(&s_fake_unit_b, buf->vram_res->unit);

    lv_draw_buf_destroy(buf);
}

/*----------------------------------------------------------------------
 * 4. CLEARZERO flag
 *----------------------------------------------------------------------*/

/** CLEARZERO flag on lazy buffer: VRAM alloc, flag cleared */
void test_vram_clearzero_lazy_to_vram(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);
    lv_draw_buf_set_flag(buf, LV_IMAGE_FLAGS_CLEARZERO);

    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    /* CLEARZERO should be cleared after handling */
    TEST_ASSERT_FALSE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_CLEARZERO));
    /* Should have used alloc (not upload) since content is zero */
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.alloc_count);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.upload_count);

    lv_draw_buf_destroy(buf);
}

/** CLEARZERO flag on lazy buffer: CPU alloc zeros the buffer */
void test_vram_clearzero_lazy_to_cpu(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);
    lv_draw_buf_set_flag(buf, LV_IMAGE_FLAGS_CLEARZERO);

    bool ok = lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_FALSE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_CLEARZERO));

    /* Buffer should be zeroed */
    uint8_t * p = buf->data;
    uint32_t total = buf->header.stride * buf->header.h;
    for(uint32_t i = 0; i < total; i++) {
        if(p[i] != 0) {
            TEST_FAIL_MESSAGE("CPU buffer not zeroed after CLEARZERO lazy alloc");
            break;
        }
    }

    lv_draw_buf_destroy(buf);
}

/** CLEARZERO on VRAM-resident buffer migrating to CPU clears the flag */
void test_vram_clearzero_vram_to_cpu(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Put in VRAM */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);

    /* Set CLEARZERO (simulating a clear after VRAM rendering) */
    lv_draw_buf_set_flag(buf, LV_IMAGE_FLAGS_CLEARZERO);

    /* Move to CPU — should not download (CLEARZERO implies discard) */
    bool ok = lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.download_count);

    lv_draw_buf_destroy(buf);
}

/*----------------------------------------------------------------------
 * 5. DISCARDABLE flag
 *----------------------------------------------------------------------*/

/** DISCARDABLE on CPU buffer moving to VRAM: alloc, no upload */
void test_vram_discardable_cpu_to_vram(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, NULL);
    fill_pattern(buf, 0xEE);
    lv_draw_buf_set_flag(buf, LV_IMAGE_FLAGS_DISCARDABLE);

    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    /* Should have used alloc path, not upload */
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.alloc_count);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.upload_count);

    lv_draw_buf_destroy(buf);
}

/** DISCARDABLE flag is cleared after ensure_resident */
void test_vram_discardable_cleared_after_transition(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    lv_draw_buf_set_flag(buf, LV_IMAGE_FLAGS_DISCARDABLE);

    /* Move to CPU — discard should be cleared */
    lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_FALSE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_DISCARDABLE));

    lv_draw_buf_destroy(buf);
}

/*----------------------------------------------------------------------
 * 6. vram_check_cb — VRAM loss detection
 *----------------------------------------------------------------------*/

/** Lost VRAM is detected and freed on next ensure_resident */
void test_vram_check_lost_vram(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_NOT_NULL(buf->vram_res);

    /* Simulate VRAM reclaim */
    invalidate_vram(buf);

    /* ensure_resident should detect loss, free stale vram_res, and re-allocate */
    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    /* check_cb should have been called */
    TEST_ASSERT(s_stats_a.check_count >= 1);
    /* stale vram_res should have been freed, new one allocated */
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.free_count);
    TEST_ASSERT_EQUAL_INT(2, s_stats_a.alloc_count);

    lv_draw_buf_destroy(buf);
}

/** Lost VRAM detected when moving to CPU — no download attempted */
void test_vram_check_lost_then_cpu(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    invalidate_vram(buf);

    /* Moving to CPU after VRAM loss — should allocate fresh CPU memory */
    bool ok = lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);
    /* No download should have been attempted on invalid VRAM */
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.download_count);

    lv_draw_buf_destroy(buf);
}

/** Buffer with both CPU and VRAM lost — only CPU should remain after check */
void test_vram_check_lost_with_no_cpu(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Allocate to VRAM (CPU data freed) */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_NULL(buf->data);
    TEST_ASSERT_NOT_NULL(buf->vram_res);

    /* Simulate VRAM loss */
    invalidate_vram(buf);

    /* Now try to get CPU resident — should detect loss and allocate fresh */
    bool ok = lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);

    lv_draw_buf_destroy(buf);
}

/*----------------------------------------------------------------------
 * 7. destroy cleanup
 *----------------------------------------------------------------------*/

/** destroy frees VRAM via vram_free_cb */
void test_vram_destroy_frees_vram(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.free_count);

    lv_draw_buf_destroy(buf);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.free_count);
}

/** destroy on CPU-only buffer does not call vram_free */
void test_vram_destroy_cpu_only(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, NULL);
    lv_draw_buf_destroy(buf);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.free_count);
    TEST_ASSERT_EQUAL_INT(0, s_stats_b.free_count);
}

/*----------------------------------------------------------------------
 * 8. lv_draw_buf_clear interaction with VRAM
 *----------------------------------------------------------------------*/

/** lv_draw_buf_clear(NULL area) on CPU buffer sets CLEARZERO flag */
void test_vram_clear_sets_clearzero(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, NULL);
    fill_pattern(buf, 0xFF);

    lv_draw_buf_clear(buf, NULL);
    TEST_ASSERT_TRUE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_CLEARZERO));

    /* Moving to VRAM should use alloc (not upload) due to CLEARZERO */
    bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.alloc_count);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.upload_count);

    lv_draw_buf_destroy(buf);
}

/*----------------------------------------------------------------------
 * 9. Canvas interactions
 *----------------------------------------------------------------------*/

/** Canvas set_px pulls buffer from VRAM to CPU */
void test_vram_canvas_set_px_pulls_to_cpu(void)
{
    lv_obj_t * canvas_obj = lv_canvas_create(lv_screen_active());
    lv_draw_buf_t * buf = lv_draw_buf_create(20, 20, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Put buffer in VRAM first */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    TEST_ASSERT_NULL(buf->data);

    lv_canvas_set_draw_buf(canvas_obj, buf);

    /* set_px should trigger ensure_resident(buf, NULL) => download + free VRAM */
    lv_canvas_set_px(canvas_obj, 0, 0, lv_color_black(), LV_OPA_COVER);

    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.download_count);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.free_count);

    lv_draw_buf_destroy(buf);
}

/** Canvas get_px pulls buffer from VRAM to CPU */
void test_vram_canvas_get_px_pulls_to_cpu(void)
{
    lv_obj_t * canvas_obj = lv_canvas_create(lv_screen_active());
    lv_draw_buf_t * buf = lv_draw_buf_create(20, 20, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    lv_canvas_set_draw_buf(canvas_obj, buf);

    lv_color32_t color32 = lv_canvas_get_px(canvas_obj, 0, 0);
    LV_UNUSED(color32);

    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.download_count);

    lv_draw_buf_destroy(buf);
}

/** Canvas fill_bg with transparent on ARGB sets CLEARZERO without CPU alloc */
void test_vram_canvas_fill_bg_transparent_sets_clearzero(void)
{
    lv_obj_t * canvas_obj = lv_canvas_create(lv_screen_active());
    lv_draw_buf_t * buf = lv_draw_buf_create(20, 20, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_canvas_set_draw_buf(canvas_obj, buf);

    /* Fill with transparent (opa=0 on alpha format = zero fill) */
    lv_canvas_fill_bg(canvas_obj, lv_color_black(), LV_OPA_TRANSP);

    /* Should set CLEARZERO without allocating CPU or VRAM memory */
    TEST_ASSERT_TRUE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_CLEARZERO));
    TEST_ASSERT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);

    lv_draw_buf_destroy(buf);
}

/** Canvas fill_bg with non-zero color pulls to CPU */
void test_vram_canvas_fill_bg_nonzero_pulls_to_cpu(void)
{
    lv_obj_t * canvas_obj = lv_canvas_create(lv_screen_active());
    lv_draw_buf_t * buf = lv_draw_buf_create(20, 20, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Put in VRAM */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    lv_canvas_set_draw_buf(canvas_obj, buf);

    /* Non-transparent fill should pull to CPU */
    lv_canvas_fill_bg(canvas_obj, lv_color_hex(0xFF0000), LV_OPA_COVER);

    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);

    lv_draw_buf_destroy(buf);
}

/** Canvas: buffer works correctly after VRAM->CPU->VRAM->CPU round trip */
void test_vram_canvas_round_trip(void)
{
    lv_obj_t * canvas_obj = lv_canvas_create(lv_screen_active());
    lv_draw_buf_t * buf = lv_draw_buf_create(20, 20, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_canvas_set_draw_buf(canvas_obj, buf);

    /* Step 1: Fill with transparent (lazy, CLEARZERO) */
    lv_canvas_fill_bg(canvas_obj, lv_color_black(), LV_OPA_TRANSP);
    TEST_ASSERT_TRUE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_CLEARZERO));

    /* Step 2: Move to VRAM (should alloc, not upload) */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    TEST_ASSERT_FALSE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_CLEARZERO));
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.upload_count);

    /* Step 3: set_px pulls back to CPU */
    lv_canvas_set_px(canvas_obj, 5, 5, lv_color_hex(0xFF0000), LV_OPA_COVER);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);

    /* Step 4: Move back to VRAM (should upload this time — content is valid) */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.upload_count);

    /* Step 5: Move back to CPU and verify we can still read */
    lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_NOT_NULL(buf->data);

    lv_draw_buf_destroy(buf);
}

/*----------------------------------------------------------------------
 * 10. Image descriptor (non-ALLOCATED, ROM-like) VRAM handling
 *----------------------------------------------------------------------*/

/** Image descriptor with CPU data uploads to VRAM (never discards ROM) */
void test_vram_image_dsc_upload(void)
{
    /* Simulate a ROM image descriptor (no ALLOCATED flag, no handlers) */
    static uint8_t img_data[40 * 4]; /* 10x1 ARGB8888, stride=40 */
    lv_memset(img_data, 0x77, sizeof(img_data));

    lv_draw_buf_t img_buf;
    lv_memzero(&img_buf, sizeof(img_buf));
    img_buf.header.magic = LV_IMAGE_HEADER_MAGIC;
    img_buf.header.w = 10;
    img_buf.header.h = 1;
    img_buf.header.cf = LV_COLOR_FORMAT_ARGB8888;
    img_buf.header.stride = 40;
    img_buf.header.flags = 0; /* No ALLOCATED, no MODIFIABLE — like const ROM */
    img_buf.data = img_data;
    img_buf.unaligned_data = img_data;
    img_buf.data_size = sizeof(img_data);
    /* handlers = NULL => image descriptor path */

    bool ok = lv_draw_buf_ensure_resident(&img_buf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(img_buf.vram_res);
    /* Should have uploaded (not just alloc) because ROM data is always valid */
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.upload_count);
    /* ROM data pointer should NOT be freed (no ALLOCATED flag) */
    TEST_ASSERT_EQUAL_PTR(img_data, img_buf.data);

    /* Content preserved in VRAM */
    TEST_ASSERT_TRUE(vram_contains_pattern(&img_buf, 0x77));

    /* Clean up VRAM manually since this isn't a heap-allocated draw_buf */
    fake_vram_free_cb(&s_fake_unit_a, &img_buf);
}

/** Image descriptor with DISCARDABLE still uploads (ROM is always valid) */
void test_vram_image_dsc_discard_still_uploads(void)
{
    static uint8_t img_data[40 * 4];
    lv_memset(img_data, 0x88, sizeof(img_data));

    lv_draw_buf_t img_buf;
    lv_memzero(&img_buf, sizeof(img_buf));
    img_buf.header.magic = LV_IMAGE_HEADER_MAGIC;
    img_buf.header.w = 10;
    img_buf.header.h = 1;
    img_buf.header.cf = LV_COLOR_FORMAT_ARGB8888;
    img_buf.header.stride = 40;
    img_buf.header.flags = LV_IMAGE_FLAGS_DISCARDABLE;
    img_buf.data = img_data;
    img_buf.unaligned_data = img_data;
    img_buf.data_size = sizeof(img_data);

    bool ok = lv_draw_buf_ensure_resident(&img_buf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(img_buf.vram_res);
    /* ROM images should still upload, not just alloc */
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.upload_count);

    fake_vram_free_cb(&s_fake_unit_a, &img_buf);
}

/*----------------------------------------------------------------------
 * 11. Font VRAM release
 *----------------------------------------------------------------------*/

/** lv_draw_buf_vram_font_release frees font VRAM */
void test_vram_font_release(void)
{
    /* Create a minimal font-like structure with vram_res */
    typedef struct {
        lv_font_dsc_base_t base;
        int dummy;
    } fake_font_dsc_t;

    fake_font_dsc_t dsc;
    lv_memzero(&dsc, sizeof(dsc));

    /* Manually create a fake VRAM res for the font */
    fake_vram_res_t * vr = lv_malloc_zeroed(sizeof(fake_vram_res_t));
    vr->fake_vram = lv_malloc(64);
    vr->alloc_size = 64;
    vr->valid = true;
    vr->base.unit = &s_fake_unit_a;
    vr->base.size = 64;
    dsc.base.vram_res = &vr->base;

    lv_font_t font;
    lv_memzero(&font, sizeof(font));
    font.dsc = &dsc;

    lv_draw_buf_vram_font_release(&dsc.base.vram_res, &font);
    TEST_ASSERT_NULL(dsc.base.vram_res);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.font_free_count);
}

/** lv_draw_buf_vram_font_release is safe with NULL */
void test_vram_font_release_null_safe(void)
{
    lv_draw_buf_vram_res_t * null_res = NULL;
    lv_draw_buf_vram_font_release(&null_res, NULL);
    /* Should not crash */
    TEST_ASSERT_NULL(null_res);
}

/*----------------------------------------------------------------------
 * 12. sys/vram alternation invariants
 *----------------------------------------------------------------------*/

/** After CPU->VRAM, data is NULL. After VRAM->CPU, vram_res is NULL. */
void test_vram_alternation_exactly_one_backing(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Start: both NULL (lazy) */
    TEST_ASSERT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);

    /* To CPU: data set, vram_res NULL */
    lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);

    /* To VRAM: data freed, vram_res set */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_NULL(buf->data);
    TEST_ASSERT_NOT_NULL(buf->vram_res);

    /* Back to CPU: data set, vram_res NULL */
    lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_NULL(buf->vram_res);

    /* Back to VRAM: data freed, vram_res set */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_NULL(buf->data);
    TEST_ASSERT_NOT_NULL(buf->vram_res);

    lv_draw_buf_destroy(buf);
}

/** Verify content survives multiple CPU<->VRAM round trips */
void test_vram_content_round_trip_integrity(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Allocate CPU and write pattern */
    lv_draw_buf_ensure_resident(buf, NULL);
    fill_pattern(buf, 0xDE);

    /* CPU -> VRAM -> CPU: content should survive */
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    lv_draw_buf_ensure_resident(buf, NULL);

    /* Verify pattern */
    uint8_t * p = buf->data;
    uint32_t total = buf->header.stride * buf->header.h;
    for(uint32_t i = 0; i < total; i++) {
        if(p[i] != 0xDE) {
            TEST_FAIL_MESSAGE("Content corrupted after CPU->VRAM->CPU round trip");
            break;
        }
    }

    /* CPU -> VRAM A -> VRAM B -> CPU: content should survive */
    fill_pattern(buf, 0xBE);
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_b);
    lv_draw_buf_ensure_resident(buf, NULL);

    p = buf->data;
    for(uint32_t i = 0; i < total; i++) {
        if(p[i] != 0xBE) {
            TEST_FAIL_MESSAGE("Content corrupted after CPU->VRAM_A->VRAM_B->CPU round trip");
            break;
        }
    }

    lv_draw_buf_destroy(buf);
}

/*----------------------------------------------------------------------
 * 13. Static draw buffers with VRAM
 *----------------------------------------------------------------------*/

/** Static draw buffers (stack-allocated data) can be uploaded to VRAM */
void test_vram_static_buf_upload(void)
{
    LV_DRAW_BUF_DEFINE_STATIC(sbuf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(sbuf);

    fill_pattern(&sbuf, 0x55);

    bool ok = lv_draw_buf_ensure_resident(&sbuf, &s_fake_unit_a);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(sbuf.vram_res);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.upload_count);
    TEST_ASSERT_TRUE(vram_contains_pattern(&sbuf, 0x55));

    /* Static buffer data should NOT be freed (no ALLOCATED flag) */
    TEST_ASSERT_NOT_NULL(sbuf.data);

    /* Clean up VRAM */
    fake_vram_free_cb(&s_fake_unit_a, &sbuf);
}

/** Static draw buffer: VRAM->CPU download works */
void test_vram_static_buf_download(void)
{
    LV_DRAW_BUF_DEFINE_STATIC(sbuf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(sbuf);

    /* Upload to VRAM */
    lv_draw_buf_ensure_resident(&sbuf, &s_fake_unit_a);
    TEST_ASSERT_NOT_NULL(sbuf.vram_res);

    /* Write different pattern to VRAM */
    fake_vram_res_t * vr = (fake_vram_res_t *)sbuf.vram_res;
    lv_memset(vr->fake_vram, 0xAA, vr->alloc_size);

    /* Move back to CPU */
    bool ok = lv_draw_buf_ensure_resident(&sbuf, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(sbuf.data);
    /* Static buffer still has its original pointer (not freed/reallocated) */

    /* Clean up VRAM if still present */
    if(sbuf.vram_res) fake_vram_free_cb(&s_fake_unit_a, &sbuf);
}

/*----------------------------------------------------------------------
 * 14. Edge cases
 *----------------------------------------------------------------------*/

/** ensure_resident with NULL buffer returns false */
void test_vram_ensure_resident_null_buf(void)
{
    bool ok = lv_draw_buf_ensure_resident(NULL, &s_fake_unit_a);
    TEST_ASSERT_FALSE(ok);
}

/** Multiple color formats work with VRAM */
void test_vram_multiple_formats(void)
{
    lv_color_format_t formats[] = {
        LV_COLOR_FORMAT_RGB565,
        LV_COLOR_FORMAT_RGB888,
        LV_COLOR_FORMAT_ARGB8888,
        LV_COLOR_FORMAT_XRGB8888,
        LV_COLOR_FORMAT_A8,
        LV_COLOR_FORMAT_L8,
    };

    for(unsigned i = 0; i < sizeof(formats) / sizeof(formats[0]); i++) {
        init_fake_unit(&s_fake_unit_a, &s_stats_a);
        lv_draw_buf_t * buf = lv_draw_buf_create(16, 8, formats[i], 0);
        TEST_ASSERT_NOT_NULL(buf);

        /* Lazy -> VRAM -> CPU -> VRAM round trip */
        bool ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
        TEST_ASSERT_TRUE(ok);
        TEST_ASSERT_NOT_NULL(buf->vram_res);

        ok = lv_draw_buf_ensure_resident(buf, NULL);
        TEST_ASSERT_TRUE(ok);
        TEST_ASSERT_NOT_NULL(buf->data);
        TEST_ASSERT_NULL(buf->vram_res);

        ok = lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
        TEST_ASSERT_TRUE(ok);
        TEST_ASSERT_NOT_NULL(buf->vram_res);

        lv_draw_buf_destroy(buf);
    }
}

/** lv_draw_buf_clear on a lazy buffer sets CLEARZERO without crash */
void test_vram_clear_lazy_buffer_sets_clearzero(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_NULL(buf->data);

    /* Clear on a lazy (header-only) buffer should not crash and should set CLEARZERO */
    lv_draw_buf_clear(buf, NULL);
    TEST_ASSERT_TRUE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_CLEARZERO));
    TEST_ASSERT_NULL(buf->data);  /* Still lazy — no CPU allocation */

    /* Subsequent ensure_resident to CPU should produce a zeroed buffer */
    bool ok = lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(buf->data);
    TEST_ASSERT_FALSE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_CLEARZERO));

    uint8_t * p = buf->data;
    uint32_t total = buf->header.stride * buf->header.h;
    for(uint32_t i = 0; i < total; i++) {
        if(p[i] != 0) {
            TEST_FAIL_MESSAGE("Buffer not zeroed after lazy clear + ensure_resident");
            break;
        }
    }

    lv_draw_buf_destroy(buf);
}

/** lv_draw_buf_dup on a lazy buffer ensures both src and dest are resident */
void test_vram_dup_lazy_buffer(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_NULL(buf->data);

    /* dup on a lazy buffer should succeed — both buffers become CPU-resident */
    lv_draw_buf_t * dup = lv_draw_buf_dup(buf);
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_NOT_NULL(dup->data);
    TEST_ASSERT_NOT_NULL(buf->data);  /* Source also ensured resident */

    lv_draw_buf_destroy(dup);
    lv_draw_buf_destroy(buf);
}

/** lv_draw_buf_dup on a CPU-resident buffer works normally */
void test_vram_dup_cpu_buffer(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, NULL);
    fill_pattern(buf, 0xAB);

    lv_draw_buf_t * dup = lv_draw_buf_dup(buf);
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_NOT_NULL(dup->data);

    /* Content should match */
    uint8_t * p = dup->data;
    uint32_t total = dup->header.stride * dup->header.h;
    for(uint32_t i = 0; i < total; i++) {
        if(p[i] != 0xAB) {
            TEST_FAIL_MESSAGE("dup content mismatch");
            break;
        }
    }

    lv_draw_buf_destroy(dup);
    lv_draw_buf_destroy(buf);
}

/** lv_draw_buf_dup on VRAM-resident buffer uses vram_dup_cb fast path */
void test_vram_dup_vram_resident_uses_callback(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Get CPU resident, write pattern, upload to VRAM */
    lv_draw_buf_ensure_resident(buf, NULL);
    fill_pattern(buf, 0xCD);
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);
    TEST_ASSERT_NOT_NULL(buf->vram_res);
    TEST_ASSERT_NULL(buf->data);  /* CPU freed after upload */

    /* dup should use vram_dup_cb — no download, no CPU allocation on source */
    lv_draw_buf_t * dup = lv_draw_buf_dup(buf);
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.dup_count);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.download_count);

    /* Dup should be VRAM-resident on the same unit */
    TEST_ASSERT_NOT_NULL(dup->vram_res);
    TEST_ASSERT_EQUAL_PTR(&s_fake_unit_a, dup->vram_res->unit);

    /* Source should remain untouched in VRAM */
    TEST_ASSERT_NULL(buf->data);
    TEST_ASSERT_NOT_NULL(buf->vram_res);

    /* VRAM content should match the original pattern */
    TEST_ASSERT_TRUE(vram_contains_pattern(dup, 0xCD));

    lv_draw_buf_destroy(dup);
    lv_draw_buf_destroy(buf);
}

/** vram_dup_cb failure falls back to CPU copy */
void test_vram_dup_vram_fallback_on_failure(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, NULL);
    fill_pattern(buf, 0xEF);
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);

    /* Force vram_dup_cb to fail */
    s_stats_a.fail_dup = true;

    lv_draw_buf_t * dup = lv_draw_buf_dup(buf);
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.dup_count);  /* Attempted */

    /* Should have fallen back to CPU: source downloaded, dup is CPU-resident */
    TEST_ASSERT_NOT_NULL(dup->data);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.download_count);

    /* Content should still be correct via the CPU fallback */
    uint8_t * p = dup->data;
    uint32_t total = dup->header.stride * dup->header.h;
    for(uint32_t i = 0; i < total; i++) {
        if(p[i] != 0xEF) {
            TEST_FAIL_MESSAGE("dup content mismatch after vram_dup_cb fallback");
            break;
        }
    }

    lv_draw_buf_destroy(dup);
    lv_draw_buf_destroy(buf);
}

/** dup without vram_dup_cb falls back to CPU copy */
void test_vram_dup_no_callback_falls_back(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, NULL);
    fill_pattern(buf, 0x33);
    lv_draw_buf_ensure_resident(buf, &s_fake_unit_a);

    /* Remove vram_dup_cb to simulate a unit that doesn't support it */
    s_fake_unit_a.vram_dup_cb = NULL;

    lv_draw_buf_t * dup = lv_draw_buf_dup(buf);
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_EQUAL_INT(0, s_stats_a.dup_count);  /* Not attempted */

    /* Should have used CPU fallback: download + CPU copy */
    TEST_ASSERT_NOT_NULL(dup->data);
    TEST_ASSERT_EQUAL_INT(1, s_stats_a.download_count);

    uint8_t * p = dup->data;
    uint32_t total = dup->header.stride * dup->header.h;
    for(uint32_t i = 0; i < total; i++) {
        if(p[i] != 0x33) {
            TEST_FAIL_MESSAGE("dup content mismatch without vram_dup_cb");
            break;
        }
    }

    /* Restore for teardown */
    s_fake_unit_a.vram_dup_cb = fake_vram_dup_cb;

    lv_draw_buf_destroy(dup);
    lv_draw_buf_destroy(buf);
}

/** Alpha format lazy alloc to CPU is zero-filled */
void test_vram_alpha_format_lazy_zeroed(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(16, 8, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_draw_buf_ensure_resident(buf, NULL);
    TEST_ASSERT_NOT_NULL(buf->data);

    /* Alpha formats should be zeroed on lazy alloc (even without CLEARZERO) */
    uint8_t * p = buf->data;
    uint32_t total = buf->header.stride * buf->header.h;
    bool all_zero = true;
    for(uint32_t i = 0; i < total; i++) {
        if(p[i] != 0) { all_zero = false; break; }
    }
    TEST_ASSERT_TRUE(all_zero);

    lv_draw_buf_destroy(buf);
}

#endif /* LV_USE_DRAW_VRAM */

typedef int _keep_pedantic_happy; /* avoid empty translation unit when VRAM is disabled */

#endif /* LV_BUILD_TEST */
