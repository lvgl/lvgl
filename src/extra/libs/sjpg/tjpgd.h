/*----------------------------------------------------------------------------/
/ TJpgDec - Tiny JPEG Decompressor R0.03 include file         (C)ChaN, 2021
/----------------------------------------------------------------------------*/
#ifndef DEF_TJPGDEC
#define DEF_TJPGDEC

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../lv_conf_internal.h"
#if LV_USE_SJPG

#include "tjpgdcnf.h"
#include <string.h>

#if defined(_WIN32)	/* VC++ or some compiler without stdint.h */
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef short			int16_t;
typedef unsigned long	uint32_t;
typedef long			int32_t;
#else				/* Embedded platform */
#include <stdint.h>
#endif

#if JD_FASTDECODE >= 1
typedef int16_t jd_yuv_t;
#else
typedef uint8_t jd_yuv_t;
#endif


/* Error code */
typedef enum {
	JDR_OK = 0,	/* 0: Succeeded */
	JDR_INTR,	/* 1: Interrupted by output function */	
	JDR_INP,	/* 2: Device error or wrong termination of input stream */
	JDR_MEM1,	/* 3: Insufficient memory pool for the image */
	JDR_MEM2,	/* 4: Insufficient stream input buffer */
	JDR_PAR,	/* 5: Parameter error */
	JDR_FMT1,	/* 6: Data format error (may be broken data) */
	JDR_FMT2,	/* 7: Right format but not supported */
	JDR_FMT3	/* 8: Not supported JPEG standard */
} JRESULT;

/* Rectangular region in the output image */
typedef struct {
	uint16_t left;		/* Left end */
	uint16_t right;		/* Right end */
	uint16_t top;		/* Top end */
	uint16_t bottom;	/* Bottom end */
} JRECT;

/* Decompressor object structure */
typedef struct JDEC JDEC;


/* TJpgDec API functions */
JRESULT jd_prepare (JDEC* jd, size_t (*infunc)(JDEC*,uint8_t*,size_t), void* pool, size_t sz_pool, void* dev);
JRESULT jd_decomp (JDEC* jd, int (*outfunc)(JDEC*,void*,JRECT*), uint8_t scale);

#endif /*LV_USE_SJPG*/

#ifdef __cplusplus
}
#endif

#endif /* _TJPGDEC */
