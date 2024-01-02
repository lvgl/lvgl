#ifndef ZLIB_H
#define ZLIB_H

#include "_fake_defines.h"
#include "_fake_typedefs.h"

typedef int uInt;
typedef int uLong;
#if !defined(__MACTYPES__)
typedef int Byte;
#endif

typedef int Bytef;
typedef int charf;
typedef int intf;
typedef int uIntf;
typedef int uLongf;

typedef int voidpc;
typedef int voidpf;
typedef int voidp;

#if !defined(Z_U4) && !defined(Z_SOLO) && defined(STDC)
typedef int Z_U4;
#endif

typedef int z_crc_t;
typedef int z_size_t;

typedef int alloc_func;
typedef int free_func;

#endif
