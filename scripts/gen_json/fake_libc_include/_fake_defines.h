#ifndef _FAKE_DEFINES_H
#define _FAKE_DEFINES_H

#define	NULL	0
#define	BUFSIZ		1024
#define	FOPEN_MAX	20
#define	FILENAME_MAX	1024

#ifndef SEEK_SET
#define	SEEK_SET	0	/* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#endif

#define __LITTLE_ENDIAN 1234
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define __BIG_ENDIAN 4321
#define BIG_ENDIAN __BIG_ENDIAN
#define __BYTE_ORDER __LITTLE_ENDIAN
#define BYTE_ORDER __BYTE_ORDER

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define SCHAR_MIN -128
#define SCHAR_MAX 127
#define CHAR_MIN -128
#define CHAR_MAX 127
#define UCHAR_MAX 255
#define SHRT_MIN -32768
#define SHRT_MAX 32767
#define USHRT_MAX 65535
#define INT_MIN -2147483648
#define INT_MAX 2147483647
#define UINT_MAX 4294967295U
#define LONG_MIN -9223372036854775808L
#define LONG_MAX 9223372036854775807L
#define ULONG_MAX 18446744073709551615UL
#define RAND_MAX 32767

/* C99 inttypes.h defines */
#define PRId8 "d"
#define PRIi8 "i"
#define PRIo8 "o"
#define PRIu8 "u"
#define PRIx8 "x"
#define PRIX8 "X"
#define PRId16 "d"
#define PRIi16 "i"
#define PRIo16 "o"
#define PRIu16 "u"
#define PRIx16 "x"
#define PRIX16 "X"
#define PRId32 "d"
#define PRIi32 "i"
#define PRIo32 "o"
#define PRIu32 "u"
#define PRIx32 "x"
#define PRIX32 "X"
#define PRId64 "d"
#define PRIi64 "i"
#define PRIo64 "o"
#define PRIu64 "u"
#define PRIx64 "x"
#define PRIX64 "X"
#define PRIdLEAST8 "d"
#define PRIiLEAST8 "i"
#define PRIoLEAST8 "o"
#define PRIuLEAST8 "u"
#define PRIxLEAST8 "x"
#define PRIXLEAST8 "X"
#define PRIdLEAST16 "d"
#define PRIiLEAST16 "i"
#define PRIoLEAST16 "o"
#define PRIuLEAST16 "u"
#define PRIxLEAST16 "x"
#define PRIXLEAST16 "X"
#define PRIdLEAST32 "d"
#define PRIiLEAST32 "i"
#define PRIoLEAST32 "o"
#define PRIuLEAST32 "u"
#define PRIxLEAST32 "x"
#define PRIXLEAST32 "X"
#define PRIdLEAST64 "d"
#define PRIiLEAST64 "i"
#define PRIoLEAST64 "o"
#define PRIuLEAST64 "u"
#define PRIxLEAST64 "x"
#define PRIXLEAST64 "X"
#define PRIdFAST8 "d"
#define PRIiFAST8 "i"
#define PRIoFAST8 "o"
#define PRIuFAST8 "u"
#define PRIxFAST8 "x"
#define PRIXFAST8 "X"
#define PRIdFAST16 "d"
#define PRIiFAST16 "i"
#define PRIoFAST16 "o"
#define PRIuFAST16 "u"
#define PRIxFAST16 "x"
#define PRIXFAST16 "X"
#define PRIdFAST32 "d"
#define PRIiFAST32 "i"
#define PRIoFAST32 "o"
#define PRIuFAST32 "u"
#define PRIxFAST32 "x"
#define PRIXFAST32 "X"
#define PRIdFAST64 "d"
#define PRIiFAST64 "i"
#define PRIoFAST64 "o"
#define PRIuFAST64 "u"
#define PRIxFAST64 "x"
#define PRIXFAST64 "X"
#define PRIdPTR "d"
#define PRIiPTR "i"
#define PRIoPTR "o"
#define PRIuPTR "u"
#define PRIxPTR "x"
#define PRIXPTR "X"
#define PRIdMAX "d"
#define PRIiMAX "i"
#define PRIoMAX "o"
#define PRIuMAX "u"
#define PRIxMAX "x"
#define PRIXMAX "X"
#define SCNd8 "d"
#define SCNi8 "i"
#define SCNo8 "o"
#define SCNu8 "u"
#define SCNx8 "x"
#define SCNd16 "d"
#define SCNi16 "i"
#define SCNo16 "o"
#define SCNu16 "u"
#define SCNx16 "x"
#define SCNd32 "d"
#define SCNi32 "i"
#define SCNo32 "o"
#define SCNu32 "u"
#define SCNx32 "x"
#define SCNd64 "d"
#define SCNi64 "i"
#define SCNo64 "o"
#define SCNu64 "u"
#define SCNx64 "x"
#define SCNdLEAST8 "d"
#define SCNiLEAST8 "i"
#define SCNoLEAST8 "o"
#define SCNuLEAST8 "u"
#define SCNxLEAST8 "x"
#define SCNdLEAST16 "d"
#define SCNiLEAST16 "i"
#define SCNoLEAST16 "o"
#define SCNuLEAST16 "u"
#define SCNxLEAST16 "x"
#define SCNdLEAST32 "d"
#define SCNiLEAST32 "i"
#define SCNoLEAST32 "o"
#define SCNuLEAST32 "u"
#define SCNxLEAST32 "x"
#define SCNdLEAST64 "d"
#define SCNiLEAST64 "i"
#define SCNoLEAST64 "o"
#define SCNuLEAST64 "u"
#define SCNxLEAST64 "x"
#define SCNdFAST8 "d"
#define SCNiFAST8 "i"
#define SCNoFAST8 "o"
#define SCNuFAST8 "u"
#define SCNxFAST8 "x"
#define SCNdFAST16 "d"
#define SCNiFAST16 "i"
#define SCNoFAST16 "o"
#define SCNuFAST16 "u"
#define SCNxFAST16 "x"
#define SCNdFAST32 "d"
#define SCNiFAST32 "i"
#define SCNoFAST32 "o"
#define SCNuFAST32 "u"
#define SCNxFAST32 "x"
#define SCNdFAST64 "d"
#define SCNiFAST64 "i"
#define SCNoFAST64 "o"
#define SCNuFAST64 "u"
#define SCNxFAST64 "x"
#define SCNdPTR "d"
#define SCNiPTR "i"
#define SCNoPTR "o"
#define SCNuPTR "u"
#define SCNxPTR "x"
#define SCNdMAX "d"
#define SCNiMAX "i"
#define SCNoMAX "o"
#define SCNuMAX "u"
#define SCNxMAX "x"

/* C99 stdbool.h defines */
#define __bool_true_false_are_defined 1
#define false 0
#define true 1

/* va_arg macros and type*/
#define va_start(_ap, _type) __builtin_va_start((_ap))
#define va_arg(_ap, _type) __builtin_va_arg((_ap))
#define va_end(_list)

/* Vectors */
#define __m128    int
#define __m128_u  int
#define __m128d   int
#define __m128d_u int
#define __m128i   int
#define __m128i_u int
#define __m256    int
#define __m256_u  int
#define __m256d   int
#define __m256d_u int
#define __m256i   int
#define __m256i_u int
#define __m512    int
#define __m512_u  int
#define __m512d   int
#define __m512d_u int
#define __m512i   int
#define __m512i_u int

/* C11 stdnoreturn.h defines */
#define __noreturn_is_defined 1
#define noreturn _Noreturn

/* C11 threads.h defines */
#define thread_local _Thread_local

/* C11 assert.h defines */
#define static_assert _Static_assert

#define kill_dependency(y) (y)

/* C11 stdalign.h defines */
#define alignas _Alignas
#define alignof _Alignof
#define __alignas_is_defined 1
#define __alignof_is_defined 1

#endif
