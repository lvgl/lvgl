#ifndef __LVGL_TESTS_MAKEFILE_UEFI_EFI_H__
#define __LVGL_TESTS_MAKEFILE_UEFI_EFI_H__

#ifndef __clang__
    #error This file is only for use with the clang compiler
#endif

/*************************************
 * TYPES
*************************************/
#if defined(__x86_64__)
    typedef unsigned long long UINT64;
    typedef long long INT64;
    typedef unsigned int UINT32;
    typedef int INT32;
    typedef unsigned short UINT16;
    typedef unsigned short CHAR16;
    typedef short INT16;
    typedef unsigned char BOOLEAN;
    typedef unsigned char UINT8;
    typedef char CHAR8;
    typedef signed char INT8;
    typedef UINT64 UINTN;
    typedef INT64 INTN;
    typedef INT64 INTMAX;
#elif defined(__i386__)
    typedef unsigned long long UINT64;
    typedef long long INT64;
    typedef unsigned int UINT32;
    typedef int INT32;
    typedef unsigned short UINT16;
    typedef unsigned short CHAR16;
    typedef short INT16;
    typedef unsigned char BOOLEAN;
    typedef unsigned char UINT8;
    typedef char CHAR8;
    typedef signed char INT8;
    typedef UINT32 UINTN;
    typedef INT32 INTN;
    typedef INT64 INTMAX;
#elif defined(__aarch64__)
    typedef unsigned long long UINT64;
    typedef long long INT64;
    typedef unsigned int UINT32;
    typedef int INT32;
    typedef unsigned short UINT16;
    typedef unsigned short CHAR16;
    typedef short INT16;
    typedef unsigned char BOOLEAN;
    typedef unsigned char UINT8;
    typedef char CHAR8;
    typedef signed char INT8;
    typedef UINT64 UINTN;
    typedef INT64 INTN;
    typedef INT64 INTMAX;
#else
    #error Architecture is not supported
#endif

typedef UINT8 uint8_t;
typedef UINT16 uint16_t;
typedef UINT32 uint32_t;
typedef UINT64 uint64_t;
typedef INT8 int8_t;
typedef INT16 int16_t;
typedef INT32 int32_t;
typedef INT64 int64_t;
typedef void VOID;

typedef uint32_t uint_fast32_t;
typedef UINTN uintptr_t;
typedef UINTN size_t;
typedef INTN intptr_t;
typedef INTMAX intmax_t;
typedef INTN ptrdiff_t;

typedef UINT8 bool;

/*************************************
 * DEFINES
*************************************/
#define false 0
#define true 1
#define NULL ((void*)0)

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

#define offsetof(TYPE, Field)     ((UINTN) __builtin_offsetof(TYPE, Field))

/*************************************
 * LIMITS
*************************************/
#define INT8_MAX    (0x7F)
#define UINT8_MAX   (0xFF)
#define INT16_MAX   (0x7FFF)
#define UINT16_MAX  (0xFFFF)
#define INT32_MAX   (0x7FFFFFFF)
#define UINT32_MAX  (0xFFFFFFFF)
#define INT64_MAX   (0x7FFFFFFFFFFFFFFFULL)
#define UINT64_MAX  (0xFFFFFFFFFFFFFFFFULL)
#define INT_MAX   (0x7FFFFFFFFFFFFFFFULL)
#define UINT_MAX  (0xFFFFFFFFFFFFFFFFULL)

///
/// Minimum values for the signed UEFI Data Types
///
#define INT8_MIN   ((  -127) - 1)
#define INT16_MIN  (( -32767) - 1)
#define INT32_MIN  (( -2147483647) - 1)
#define INT64_MIN  (( -9223372036854775807LL) - 1)

#define SIZE_MAX (0xFFFFFFFF)
#define LONG_MAX (0x7FFFFFFF)
#define CHAR_BIT 8

/*************************************
 * VA_ARG
*************************************/
typedef __builtin_va_list va_list;
#define va_start(Marker, Parameter)  __builtin_va_start (Marker, Parameter)
#define va_arg(Marker, TYPE)  ((sizeof (TYPE) < sizeof (UINTN)) ? (TYPE)(__builtin_va_arg (Marker, UINTN)) : (TYPE)(__builtin_va_arg (Marker, TYPE)))
#define va_end(Marker)  __builtin_va_end (Marker)
#define va_copy(Dest, Start)  __builtin_va_copy (Dest, Start)

/*************************************
 * VERIFICATION
*************************************/
_Static_assert(sizeof(bool)     == 1, "Size check for 'bool' failed.");
_Static_assert(sizeof(int8_t)   == 1, "Size check for 'int8_t' failed.");
_Static_assert(sizeof(uint8_t)  == 1, "Size check for 'uint8_t' failed.");
_Static_assert(sizeof(int16_t)  == 2, "Size check for 'int16_t' failed.");
_Static_assert(sizeof(uint16_t) == 2, "Size check for 'uint16_t' failed.");
_Static_assert(sizeof(int32_t)  == 4, "Size check for 'int32_t' failed.");
_Static_assert(sizeof(uint32_t) == 4, "Size check for 'uint32_t' failed.");
_Static_assert(sizeof(uint_fast32_t) == 4, "Size check for 'uint_fast32_t' failed.");
_Static_assert(sizeof(int64_t)  == 8, "Size check for 'int64_t' failed.");
_Static_assert(sizeof(uint64_t) == 8, "Size check for 'uint64_t' failed.");
_Static_assert(sizeof(intptr_t) == sizeof(void *), "Size check for 'intptr_t' failed.");
_Static_assert(sizeof(ptrdiff_t) == sizeof(void *), "Size check for 'ptrdiff_t' failed.");
_Static_assert(sizeof(uintptr_t) == sizeof(void *), "Size check for 'uintptr_t' failed.");

#endif
