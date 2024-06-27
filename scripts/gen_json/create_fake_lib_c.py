import os

lib_c_files = (
    ('aio.h', ''),
    ('alloca.h', ''),
    ('ar.h', ''),
    ('argz.h', ''),
    ('assert.h', ''),
    ('complex.h', ''),
    ('cpio.h', ''),
    ('ctype.h', ''),
    ('dirent.h', ''),
    ('dlfcn.h', ''),
    ('emmintrin.h', ''),
    ('endian.h', ''),
    ('envz.h', ''),
    ('errno.h', ''),
    ('fastmath.h', ''),
    ('fcntl.h', ''),
    ('features.h', ''),
    ('fenv.h', ''),
    ('float.h', ''),
    ('fmtmsg.h', ''),
    ('fnmatch.h', ''),
    ('ftw.h', ''),
    ('getopt.h', ''),
    ('glob.h', ''),
    ('grp.h', ''),
    ('iconv.h', ''),
    ('ieeefp.h', ''),
    ('immintrin.h', ''),
    ('inttypes.h', ''),
    ('iso646.h', ''),
    ('langinfo.h', ''),
    ('libgen.h', ''),
    ('libintl.h', ''),
    ('limits.h', ''),
    ('locale.h', ''),
    ('malloc.h', 'void *malloc(size_t size);\nvoid *realloc(void *memblock, size_t size);\nvoid free(void *memblock);'),
    ('math.h', ''),
    ('memory.h', 'void *memset(void *dest, int c, size_t count);\nvoid *memcpy(void *dest, const void *src, size_t count);\nint memcmp(const void *buffer1, const void *buffer2, size_t count);'),
    ('monetary.h', ''),
    ('mqueue.h', ''),
    ('ndbm.h', ''),
    ('netdb.h', ''),
    ('newlib.h', ''),
    ('nl_types.h', ''),
    ('paths.h', ''),
    ('poll.h', ''),
    ('process.h', ''),
    ('pthread.h', ''),
    ('pwd.h', ''),
    ('reent.h', ''),
    ('regdef.h', ''),
    ('regex.h', ''),
    ('sched.h', ''),
    ('search.h', ''),
    ('semaphore.h', ''),
    ('setjmp.h', ''),
    ('signal.h', ''),
    ('smmintrin.h', ''),
    ('spawn.h', ''),
    ('stdalign.h', ''),
    ('stdarg.h', ''),
    ('stdatomic.h', '/* C11 stdatomic.h defines */\n#define ATOMIC_BOOL_LOCK_FREE       0\n#define ATOMIC_CHAR_LOCK_FREE       0\n#define ATOMIC_CHAR16_T_LOCK_FREE   0\n#define ATOMIC_CHAR32_T_LOCK_FREE   0\n#define ATOMIC_WCHAR_T_LOCK_FREE    0\n#define ATOMIC_SHORT_LOCK_FREE      0\n#define ATOMIC_INT_LOCK_FREE        0\n#define ATOMIC_LONG_LOCK_FREE       0\n#define ATOMIC_LLONG_LOCK_FREE      0\n#define ATOMIC_POINTER_LOCK_FREE    0\n#define ATOMIC_VAR_INIT(value) (value)\n#define ATOMIC_FLAG_INIT { 0 }\n\n\n/* C11 stdatomic.h types */\ntypedef _Atomic(_Bool)              atomic_bool;\ntypedef _Atomic(char)               atomic_char;\ntypedef _Atomic(signed char)        atomic_schar;\ntypedef _Atomic(unsigned char)      atomic_uchar;\ntypedef _Atomic(short)              atomic_short;\ntypedef _Atomic(unsigned short)     atomic_ushort;\ntypedef _Atomic(int)                atomic_int;\ntypedef _Atomic(unsigned int)       atomic_uint;\ntypedef _Atomic(long)               atomic_long;\ntypedef _Atomic(unsigned long)      atomic_ulong;\ntypedef _Atomic(long long)          atomic_llong;\ntypedef _Atomic(unsigned long long) atomic_ullong;\ntypedef _Atomic(uint_least16_t)     atomic_char16_t;\ntypedef _Atomic(uint_least32_t)     atomic_char32_t;\ntypedef _Atomic(wchar_t)            atomic_wchar_t;\ntypedef _Atomic(int_least8_t)       atomic_int_least8_t;\ntypedef _Atomic(uint_least8_t)      atomic_uint_least8_t;\ntypedef _Atomic(int_least16_t)      atomic_int_least16_t;\ntypedef _Atomic(uint_least16_t)     atomic_uint_least16_t;\ntypedef _Atomic(int_least32_t)      atomic_int_least32_t;\ntypedef _Atomic(uint_least32_t)     atomic_uint_least32_t;\ntypedef _Atomic(int_least64_t)      atomic_int_least64_t;\ntypedef _Atomic(uint_least64_t)     atomic_uint_least64_t;\ntypedef _Atomic(int_fast8_t)        atomic_int_fast8_t;\ntypedef _Atomic(uint_fast8_t)       atomic_uint_fast8_t;\ntypedef _Atomic(int_fast16_t)       atomic_int_fast16_t;\ntypedef _Atomic(uint_fast16_t)      atomic_uint_fast16_t;\ntypedef _Atomic(int_fast32_t)       atomic_int_fast32_t;\ntypedef _Atomic(uint_fast32_t)      atomic_uint_fast32_t;\ntypedef _Atomic(int_fast64_t)       atomic_int_fast64_t;\ntypedef _Atomic(uint_fast64_t)      atomic_uint_fast64_t;\ntypedef _Atomic(intptr_t)           atomic_intptr_t;\ntypedef _Atomic(uintptr_t)          atomic_uintptr_t;\ntypedef _Atomic(size_t)             atomic_size_t;\ntypedef _Atomic(ptrdiff_t)          atomic_ptrdiff_t;\ntypedef _Atomic(intmax_t)           atomic_intmax_t;\ntypedef _Atomic(uintmax_t)          atomic_uintmax_t;\ntypedef struct atomic_flag { atomic_bool _Value; } atomic_flag;\n\n\ntypedef enum memory_order {\n  memory_order_relaxed,\n  memory_order_consume,\n  memory_order_acquire,\n  memory_order_release,\n  memory_order_acq_rel,\n  memory_order_seq_cst\n} memory_order;'),
    ('stdbool.h', ''),
    ('stddef.h', ''),
    ('stdint.h', ''),
    ('stdio.h', ''),
    ('stdlib.h', ''),
    ('stdnoreturn.h', ''),
    ('string.h', 'size_t strlen(const char *str);\nchar *strncpy(char *strDest, const char *strSource, size_t count);\nchar *strcpy(char *strDestination, const char *strSource);'),
    ('strings.h', ''),
    ('stropts.h', ''),
    ('syslog.h', ''),
    ('tar.h', ''),
    ('termios.h', ''),
    ('tgmath.h', ''),
    ('threads.h', ''),
    ('time.h', ''),
    ('trace.h', ''),
    ('ulimit.h', ''),
    ('unctrl.h', ''),
    ('unistd.h', ''),
    ('utime.h', ''),
    ('utmp.h', ''),
    ('utmpx.h', ''),
    ('vadefs.h', 'typedef unsigned int uintptr_t;\ntypedef char* va_list;'),
    ('vcruntime.h', ''),
    ('wchar.h', ''),
    ('wctype.h', ''),
    ('wordexp.h', ''),
    ('zlib.h', 'typedef int uInt;\ntypedef int uLong;\n#if !defined(__MACTYPES__)\ntypedef int Byte;\n#endif\n\ntypedef int Bytef;\ntypedef int charf;\ntypedef int intf;\ntypedef int uIntf;\ntypedef int uLongf;\n\ntypedef int voidpc;\ntypedef int voidpf;\ntypedef int voidp;\n\n#if !defined(Z_U4) && !defined(Z_SOLO) && defined(STDC)\ntypedef int Z_U4;\n#endif\n\ntypedef int z_crc_t;\ntypedef int z_size_t;\n\ntypedef int alloc_func;\ntypedef int free_func;'),
    ('_ansi.h', ''),
    ('_fake_defines.h', '#define\tNULL\t0\n#define\tBUFSIZ\t\t1024\n#define\tFOPEN_MAX\t20\n#define\tFILENAME_MAX\t1024\n\n#ifndef SEEK_SET\n#define\tSEEK_SET\t0\t/* set file offset to offset */\n#endif\n#ifndef SEEK_CUR\n#define\tSEEK_CUR\t1\t/* set file offset to current plus offset */\n#endif\n#ifndef SEEK_END\n#define\tSEEK_END\t2\t/* set file offset to EOF plus offset */\n#endif\n\n#define __LITTLE_ENDIAN 1234\n#define LITTLE_ENDIAN __LITTLE_ENDIAN\n#define __BIG_ENDIAN 4321\n#define BIG_ENDIAN __BIG_ENDIAN\n#define __BYTE_ORDER __LITTLE_ENDIAN\n#define BYTE_ORDER __BYTE_ORDER\n\n#define EXIT_FAILURE 1\n#define EXIT_SUCCESS 0\n\n#define SCHAR_MIN -128\n#define SCHAR_MAX 127\n#define CHAR_MIN -128\n#define CHAR_MAX 127\n#define UCHAR_MAX 255\n#define SHRT_MIN -32768\n#define SHRT_MAX 32767\n#define USHRT_MAX 65535\n#define INT_MIN -2147483648\n#define INT_MAX 2147483647\n#define UINT_MAX 4294967295U\n#define LONG_MIN -9223372036854775808L\n#define LONG_MAX 9223372036854775807L\n#define ULONG_MAX 18446744073709551615UL\n#define RAND_MAX 32767\n\n/* C99 inttypes.h defines */\n#define PRId8 "d"\n#define PRIi8 "i"\n#define PRIo8 "o"\n#define PRIu8 "u"\n#define PRIx8 "x"\n#define PRIX8 "X"\n#define PRId16 "d"\n#define PRIi16 "i"\n#define PRIo16 "o"\n#define PRIu16 "u"\n#define PRIx16 "x"\n#define PRIX16 "X"\n#define PRId32 "d"\n#define PRIi32 "i"\n#define PRIo32 "o"\n#define PRIu32 "u"\n#define PRIx32 "x"\n#define PRIX32 "X"\n#define PRId64 "d"\n#define PRIi64 "i"\n#define PRIo64 "o"\n#define PRIu64 "u"\n#define PRIx64 "x"\n#define PRIX64 "X"\n#define PRIdLEAST8 "d"\n#define PRIiLEAST8 "i"\n#define PRIoLEAST8 "o"\n#define PRIuLEAST8 "u"\n#define PRIxLEAST8 "x"\n#define PRIXLEAST8 "X"\n#define PRIdLEAST16 "d"\n#define PRIiLEAST16 "i"\n#define PRIoLEAST16 "o"\n#define PRIuLEAST16 "u"\n#define PRIxLEAST16 "x"\n#define PRIXLEAST16 "X"\n#define PRIdLEAST32 "d"\n#define PRIiLEAST32 "i"\n#define PRIoLEAST32 "o"\n#define PRIuLEAST32 "u"\n#define PRIxLEAST32 "x"\n#define PRIXLEAST32 "X"\n#define PRIdLEAST64 "d"\n#define PRIiLEAST64 "i"\n#define PRIoLEAST64 "o"\n#define PRIuLEAST64 "u"\n#define PRIxLEAST64 "x"\n#define PRIXLEAST64 "X"\n#define PRIdFAST8 "d"\n#define PRIiFAST8 "i"\n#define PRIoFAST8 "o"\n#define PRIuFAST8 "u"\n#define PRIxFAST8 "x"\n#define PRIXFAST8 "X"\n#define PRIdFAST16 "d"\n#define PRIiFAST16 "i"\n#define PRIoFAST16 "o"\n#define PRIuFAST16 "u"\n#define PRIxFAST16 "x"\n#define PRIXFAST16 "X"\n#define PRIdFAST32 "d"\n#define PRIiFAST32 "i"\n#define PRIoFAST32 "o"\n#define PRIuFAST32 "u"\n#define PRIxFAST32 "x"\n#define PRIXFAST32 "X"\n#define PRIdFAST64 "d"\n#define PRIiFAST64 "i"\n#define PRIoFAST64 "o"\n#define PRIuFAST64 "u"\n#define PRIxFAST64 "x"\n#define PRIXFAST64 "X"\n#define PRIdPTR "d"\n#define PRIiPTR "i"\n#define PRIoPTR "o"\n#define PRIuPTR "u"\n#define PRIxPTR "x"\n#define PRIXPTR "X"\n#define PRIdMAX "d"\n#define PRIiMAX "i"\n#define PRIoMAX "o"\n#define PRIuMAX "u"\n#define PRIxMAX "x"\n#define PRIXMAX "X"\n#define SCNd8 "d"\n#define SCNi8 "i"\n#define SCNo8 "o"\n#define SCNu8 "u"\n#define SCNx8 "x"\n#define SCNd16 "d"\n#define SCNi16 "i"\n#define SCNo16 "o"\n#define SCNu16 "u"\n#define SCNx16 "x"\n#define SCNd32 "d"\n#define SCNi32 "i"\n#define SCNo32 "o"\n#define SCNu32 "u"\n#define SCNx32 "x"\n#define SCNd64 "d"\n#define SCNi64 "i"\n#define SCNo64 "o"\n#define SCNu64 "u"\n#define SCNx64 "x"\n#define SCNdLEAST8 "d"\n#define SCNiLEAST8 "i"\n#define SCNoLEAST8 "o"\n#define SCNuLEAST8 "u"\n#define SCNxLEAST8 "x"\n#define SCNdLEAST16 "d"\n#define SCNiLEAST16 "i"\n#define SCNoLEAST16 "o"\n#define SCNuLEAST16 "u"\n#define SCNxLEAST16 "x"\n#define SCNdLEAST32 "d"\n#define SCNiLEAST32 "i"\n#define SCNoLEAST32 "o"\n#define SCNuLEAST32 "u"\n#define SCNxLEAST32 "x"\n#define SCNdLEAST64 "d"\n#define SCNiLEAST64 "i"\n#define SCNoLEAST64 "o"\n#define SCNuLEAST64 "u"\n#define SCNxLEAST64 "x"\n#define SCNdFAST8 "d"\n#define SCNiFAST8 "i"\n#define SCNoFAST8 "o"\n#define SCNuFAST8 "u"\n#define SCNxFAST8 "x"\n#define SCNdFAST16 "d"\n#define SCNiFAST16 "i"\n#define SCNoFAST16 "o"\n#define SCNuFAST16 "u"\n#define SCNxFAST16 "x"\n#define SCNdFAST32 "d"\n#define SCNiFAST32 "i"\n#define SCNoFAST32 "o"\n#define SCNuFAST32 "u"\n#define SCNxFAST32 "x"\n#define SCNdFAST64 "d"\n#define SCNiFAST64 "i"\n#define SCNoFAST64 "o"\n#define SCNuFAST64 "u"\n#define SCNxFAST64 "x"\n#define SCNdPTR "d"\n#define SCNiPTR "i"\n#define SCNoPTR "o"\n#define SCNuPTR "u"\n#define SCNxPTR "x"\n#define SCNdMAX "d"\n#define SCNiMAX "i"\n#define SCNoMAX "o"\n#define SCNuMAX "u"\n#define SCNxMAX "x"\n\n/* C99 stdbool.h defines */\n#define __bool_true_false_are_defined 1\n#define false 0\n#define true 1\n\n/* va_arg macros and type*/\n#define va_start(_ap, _type) __builtin_va_start((_ap))\n#define va_arg(_ap, _type) __builtin_va_arg((_ap))\n#define va_end(_list)\n\n/* Vectors */\n#define __m128    int\n#define __m128_u  int\n#define __m128d   int\n#define __m128d_u int\n#define __m128i   int\n#define __m128i_u int\n#define __m256    int\n#define __m256_u  int\n#define __m256d   int\n#define __m256d_u int\n#define __m256i   int\n#define __m256i_u int\n#define __m512    int\n#define __m512_u  int\n#define __m512d   int\n#define __m512d_u int\n#define __m512i   int\n#define __m512i_u int\n\n/* C11 stdnoreturn.h defines */\n#define __noreturn_is_defined 1\n#define noreturn _Noreturn\n\n/* C11 threads.h defines */\n#define thread_local _Thread_local\n\n/* C11 assert.h defines */\n#define static_assert _Static_assert\n\n#define kill_dependency(y) (y)\n\n/* C11 stdalign.h defines */\n#define alignas _Alignas\n#define alignof _Alignof\n#define __alignas_is_defined 1\n#define __alignof_is_defined 1'),
    ('_fake_typedefs.h', 'typedef int size_t;\ntypedef int __builtin_va_list;\ntypedef int __gnuc_va_list;\ntypedef int va_list;\ntypedef int __int8_t;\ntypedef int __uint8_t;\ntypedef int __int16_t;\ntypedef int __uint16_t;\ntypedef int __int_least16_t;\ntypedef int __uint_least16_t;\ntypedef int __int32_t;\ntypedef int __uint32_t;\ntypedef int __int64_t;\ntypedef int __uint64_t;\ntypedef int __int_least32_t;\ntypedef int __uint_least32_t;\ntypedef int __s8;\ntypedef int __u8;\ntypedef int __s16;\ntypedef int __u16;\ntypedef int __s32;\ntypedef int __u32;\ntypedef int __s64;\ntypedef int __u64;\ntypedef int _LOCK_T;\ntypedef int _LOCK_RECURSIVE_T;\ntypedef int _off_t;\ntypedef int __dev_t;\ntypedef int __uid_t;\ntypedef int __gid_t;\ntypedef int _off64_t;\ntypedef int _fpos_t;\ntypedef int _ssize_t;\ntypedef int wint_t;\ntypedef int _mbstate_t;\ntypedef int _flock_t;\ntypedef int _iconv_t;\ntypedef int __ULong;\ntypedef int __FILE;\ntypedef int ptrdiff_t;\ntypedef int wchar_t;\ntypedef int char16_t;\ntypedef int char32_t;\ntypedef int __off_t;\ntypedef int __pid_t;\ntypedef int __loff_t;\ntypedef int u_char;\ntypedef int u_short;\ntypedef int u_int;\ntypedef int u_long;\ntypedef int ushort;\ntypedef int uint;\ntypedef int clock_t;\ntypedef int time_t;\ntypedef int daddr_t;\ntypedef int caddr_t;\ntypedef int ino_t;\ntypedef int off_t;\ntypedef int dev_t;\ntypedef int uid_t;\ntypedef int gid_t;\ntypedef int pid_t;\ntypedef int key_t;\ntypedef int ssize_t;\ntypedef int mode_t;\ntypedef int nlink_t;\ntypedef int fd_mask;\ntypedef int _types_fd_set;\ntypedef int clockid_t;\ntypedef int timer_t;\ntypedef int useconds_t;\ntypedef int suseconds_t;\ntypedef int FILE;\ntypedef int fpos_t;\ntypedef int cookie_read_function_t;\ntypedef int cookie_write_function_t;\ntypedef int cookie_seek_function_t;\ntypedef int cookie_close_function_t;\ntypedef int cookie_io_functions_t;\ntypedef int div_t;\ntypedef int ldiv_t;\ntypedef int lldiv_t;\ntypedef int sigset_t;\ntypedef int __sigset_t;\ntypedef int _sig_func_ptr;\ntypedef int sig_atomic_t;\ntypedef int __tzrule_type;\ntypedef int __tzinfo_type;\ntypedef int mbstate_t;\ntypedef int sem_t;\ntypedef int pthread_t;\ntypedef int pthread_attr_t;\ntypedef int pthread_mutex_t;\ntypedef int pthread_mutexattr_t;\ntypedef int pthread_cond_t;\ntypedef int pthread_condattr_t;\ntypedef int pthread_key_t;\ntypedef int pthread_once_t;\ntypedef int pthread_rwlock_t;\ntypedef int pthread_rwlockattr_t;\ntypedef int pthread_spinlock_t;\ntypedef int pthread_barrier_t;\ntypedef int pthread_barrierattr_t;\ntypedef int jmp_buf;\ntypedef int rlim_t;\ntypedef int sa_family_t;\ntypedef int sigjmp_buf;\ntypedef int stack_t;\ntypedef int siginfo_t;\ntypedef int z_stream;\n\n/* C99 exact-width integer types */\ntypedef int int8_t;\ntypedef int uint8_t;\ntypedef int int16_t;\ntypedef int uint16_t;\ntypedef int int32_t;\ntypedef int uint32_t;\ntypedef int int64_t;\ntypedef int uint64_t;\n\n/* C99 minimum-width integer types */\ntypedef int int_least8_t;\ntypedef int uint_least8_t;\ntypedef int int_least16_t;\ntypedef int uint_least16_t;\ntypedef int int_least32_t;\ntypedef int uint_least32_t;\ntypedef int int_least64_t;\ntypedef int uint_least64_t;\n\n/* C99 fastest minimum-width integer types */\ntypedef int int_fast8_t;\ntypedef int uint_fast8_t;\ntypedef int int_fast16_t;\ntypedef int uint_fast16_t;\ntypedef int int_fast32_t;\ntypedef int uint_fast32_t;\ntypedef int int_fast64_t;\ntypedef int uint_fast64_t;\n\n/* C99 integer types capable of holding object pointers */\ntypedef int intptr_t;\ntypedef int uintptr_t;\n\n/* C99 greatest-width integer types */\ntypedef int intmax_t;\ntypedef int uintmax_t;\n\n/* C99 stdbool.h bool type. _Bool is built-in in C99 */\ntypedef _Bool bool;\n\n/* Mir typedefs */\ntypedef void* MirEGLNativeWindowType;\ntypedef void* MirEGLNativeDisplayType;\ntypedef struct MirConnection MirConnection;\ntypedef struct MirSurface MirSurface;\ntypedef struct MirSurfaceSpec MirSurfaceSpec;\ntypedef struct MirScreencast MirScreencast;\ntypedef struct MirPromptSession MirPromptSession;\ntypedef struct MirBufferStream MirBufferStream;\ntypedef struct MirPersistentId MirPersistentId;\ntypedef struct MirBlob MirBlob;\ntypedef struct MirDisplayConfig MirDisplayConfig;\n\n/* xcb typedefs */\ntypedef struct xcb_connection_t xcb_connection_t;\ntypedef uint32_t xcb_window_t;\ntypedef uint32_t xcb_visualid_t;'),
    ('_syslist.h', ''),
    ('arpa/inet.h', ''),
    ('asm-generic/int-ll64.h', ''),
    ('linux/socket.h', ''),
    ('linux/version.h', ''),
    ('mir_toolkit/client_types.h', ''),
    ('net/if.h', ''),
    ('netinet/in.h', ''),
    ('netinet/tcp.h', ''),
    ('openssl/err.h', ''),
    ('openssl/evp.h', ''),
    ('openssl/hmac.h', ''),
    ('openssl/ssl.h', ''),
    ('openssl/x509v3.h', ''),
    ('sys/ioctl.h', ''),
    ('sys/ipc.h', ''),
    ('sys/mman.h', ''),
    ('sys/msg.h', ''),
    ('sys/poll.h', ''),
    ('sys/resource.h', ''),
    ('sys/select.h', ''),
    ('sys/sem.h', ''),
    ('sys/shm.h', ''),
    ('sys/socket.h', ''),
    ('sys/stat.h', ''),
    ('sys/statvfs.h', ''),
    ('sys/sysctl.h', ''),
    ('sys/time.h', ''),
    ('sys/times.h', ''),
    ('sys/types.h', ''),
    ('sys/uio.h', ''),
    ('sys/un.h', ''),
    ('sys/utsname.h', ''),
    ('sys/wait.h', ''),
    ('X11/Intrinsic.h', '#include "_X11_fake_defines.h"\n#include "_X11_fake_typedefs.h"'),
    ('X11/Xlib.h', '#include "_X11_fake_defines.h"\n#include "_X11_fake_typedefs.h"'),
    ('X11/_X11_fake_defines.h', '#define Atom CARD32\n#define Bool int\n#define KeySym CARD32\n#define Pixmap CARD32\n#define Time CARD32\n#define _XFUNCPROTOBEGIN\n#define _XFUNCPROTOEND\n#define _Xconst const\n\n#define _X_RESTRICT_KYWD\n#define Cardinal unsigned int\n#define Boolean int'),
    ('X11/_X11_fake_typedefs.h', 'typedef char* XPointer;\ntypedef unsigned char KeyCode;\ntypedef unsigned int  CARD32;\ntypedef unsigned long VisualID;\ntypedef unsigned long XIMResetState;\ntypedef unsigned long XID;\ntypedef XID Window;\ntypedef XID Colormap;\ntypedef XID Cursor;\ntypedef XID Drawable;\ntypedef void* XtPointer;\ntypedef XtPointer XtRequestId;\ntypedef struct Display Display;\ntypedef struct Screen Screen;\ntypedef struct Status Status;\ntypedef struct Visual Visual;\ntypedef struct Widget *Widget;\ntypedef struct XColor XColor;\ntypedef struct XClassHint XClassHint;\ntypedef struct XEvent XEvent;\ntypedef struct XFontStruct XFontStruct;\ntypedef struct XGCValues XGCValues;\ntypedef struct XKeyEvent XKeyEvent;\ntypedef struct XKeyPressedEvent XKeyPressedEvent;\ntypedef struct XPoint XPoint;\ntypedef struct XRectangle XRectangle;\ntypedef struct XSelectionRequestEvent XSelectionRequestEvent;\ntypedef struct XWindowChanges XWindowChanges;\ntypedef struct _XGC _XCG;\ntypedef struct _XGC *GC;\ntypedef struct _XIC *XIC;\ntypedef struct _XIM *XIM;\ntypedef struct _XImage XImage;'),
    ('xcb/xcb.h', ''),
)


contents = '''\
#include "_fake_defines.h"
#include "_fake_typedefs.h"
'''

define_template = '''
#ifndef {define_name}
#define {define_name}
{file_data}
#endif
'''


def run(temp_dir):
    fake_libc_path = os.path.join(temp_dir, 'fake_libc_include')
    os.mkdir(fake_libc_path)

    for file, file_data in lib_c_files:
        head, tail = os.path.split(file)
        file = os.path.join(fake_libc_path, file)
        file_name = os.path.split(file)[-1]
        define_name = f'__{file_name.replace(".", "_").upper()}__'

        if None not in (head, tail):
            pth = os.path.join(fake_libc_path, head)
            if not os.path.exists(pth):
                os.mkdir(pth)

        file_data = contents + file_data

        with open(file, 'w') as f:
            f.write(define_template.format(define_name=define_name, file_data=file_data))

    return fake_libc_path
