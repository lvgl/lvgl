/**
 * @file lv_uefi_std_wrapper.h
 *
 */

#ifndef LV_UEFI_STD_WRAPPER_H
#define LV_UEFI_STD_WRAPPER_H

#if LV_USE_UEFI

    #include LV_USE_UEFI_INCLUDE

    /*************************************
    * TYPES
    *************************************/
    typedef UINT8 uint8_t;
    typedef UINT16 uint16_t;
    typedef UINT32 uint32_t;
    typedef UINT64 uint64_t;
    typedef INT8 int8_t;
    typedef INT16 int16_t;
    typedef INT32 int32_t;
    typedef INT64 int64_t;

    typedef uint32_t uint_fast32_t;
    typedef UINTN uintptr_t;
    typedef UINTN size_t;
    typedef INTN intptr_t;
    typedef INTN intmax_t;
    typedef INTN ptrdiff_t;

    typedef UINT8 bool;

    /*************************************
    * DEFINES
    *************************************/
    #define false 0
    #define true 1

    #define PRId8       "d"
    #define PRId16      "d"
    #define PRId32      "d"
    #define PRId64      "d"

    #define PRIu8       "u"
    #define PRIu16      "u"
    #define PRIu32      "u"
    #define PRIu64      "u"

    #define PRIx8       "x"
    #define PRIx16      "x"
    #define PRIx32      "x"
    #define PRIx64      "x"

    #define PRIX8       "X"
    #define PRIX16      "X"
    #define PRIX32      "X"
    #define PRIX64      "X"

    /*************************************
    * LIMITS
    *************************************/
    #ifndef INT8_MAX
        #define INT8_MAX    (0x7F)
    #endif

    #ifndef UINT8_MAX
        #define UINT8_MAX   (0xFF)
    #endif

    #ifndef INT16_MAX
        #define INT16_MAX   (0x7FFF)
    #endif

    #ifndef UINT16_MAX
        #define UINT16_MAX  (0xFFFF)
    #endif

    #ifndef INT32_MAX
        #define INT32_MAX   (0x7FFFFFFF)
    #endif

    #ifndef UINT32_MAX
        #define UINT32_MAX  (0xFFFFFFFF)
    #endif

    #ifndef INT64_MAX
        #define INT64_MAX   (0x7FFFFFFFFFFFFFFFULL)
    #endif

    #ifndef UINT64_MAX
        #define UINT64_MAX  (0xFFFFFFFFFFFFFFFFULL)
    #endif

    #ifndef INT_MAX
        #define INT_MAX   (0x7FFFFFFFFFFFFFFFULL)
    #endif

    #ifndef UINT_MAX
        #define UINT_MAX  (0xFFFFFFFFFFFFFFFFULL)
    #endif

    ///
    /// Minimum values for the signed UEFI Data Types
    ///
    #ifndef INT8_MIN
        #define INT8_MIN   ((  -127) - 1)
    #endif

    #ifndef INT16_MIN
        #define INT16_MIN  (( -32767) - 1)
    #endif

    #ifndef INT32_MIN
        #define INT32_MIN  (( -2147483647) - 1)
    #endif

    #ifndef INT64_MIN
        #define INT64_MIN  (( -9223372036854775807LL) - 1)
    #endif

    #ifndef SIZE_MAX
        #define SIZE_MAX (0xFFFFFFFF)
    #endif

    #ifndef LONG_MAX
        #define LONG_MAX (0x7FFFFFFF)
    #endif

    #ifndef ULONG_MAX
        #define ULONG_MAX (0xFFFFFFFF)
    #endif

    #ifndef USHRT_MAX
        #define USHRT_MAX (0xFFFF)
    #endif

    #ifndef CHAR_BIT
        #define CHAR_BIT 8
    #endif

    /*************************************
    * VA_ARG
    *************************************/
    #if LV_UEFI_EDK2_HEADERS
        #define va_list VA_LIST
        #define va_start(Marker, Parameter) VA_START(Marker, Parameter)
        #define va_arg(Marker, TYPE)  VA_ARG(Marker, TYPE)
        #define va_end(Marker)  VA_END(Marker)
        #define va_copy(Dest, Start)  VA_COPY(Dest, Start)
    #endif

#endif

#endif