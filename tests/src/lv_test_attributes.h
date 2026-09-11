/**
 * @file lv_test_attributes.h
 *
 * Pulled in by lv_conf_internal.h through LV_ATTRIBUTE_CUSTOM_INCLUDE for the
 * configurations that need aligned static data, see configs/vg_lite.defconfig.
 */

#ifndef LV_TEST_ATTRIBUTES_H
#define LV_TEST_ATTRIBUTES_H

/* VG-Lite refuses to work on unaligned image and glyph data. The generated font
 * and image sources apply LV_ATTRIBUTE_MEM_ALIGN to their data arrays, so this
 * is what aligns the test assets. */
#ifdef _MSC_VER
    #define LV_ATTRIBUTE_MEM_ALIGN __declspec(align(LV_DRAW_BUF_ALIGN))
#else
    #define LV_ATTRIBUTE_MEM_ALIGN __attribute__((aligned(LV_DRAW_BUF_ALIGN)))
#endif

#endif /*LV_TEST_ATTRIBUTES_H*/
