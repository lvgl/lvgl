#include "lv_memcpy.h"
#include <stdint.h>
#include <string.h>
#include "lv_types.h"

#ifdef LV_ARCH_64
#define ALIGN_MASK	0x7
#else
#define ALIGN_MASK	0x3
#endif

#define COPY32 *d32 = *s32; d32++; s32++;

void * lv_memcpy(void * dst, const void * src, size_t len)
{
	uint8_t * d8 = dst;
	const uint8_t * s8 = src;

	/*Fallback to simply memcpy for unaligned addresses*/
	if(((lv_uintptr_t)d8 & ALIGN_MASK) || ((lv_uintptr_t)s8 & ALIGN_MASK)) {
		memcpy(dst, src, len);
	}

	uint32_t * d32 = dst;
	const uint32_t * s32 = src;
	while(len > 32) {
		COPY32;
		COPY32;
		COPY32;
		COPY32;
		COPY32;
		COPY32;
		COPY32;
		COPY32;
		len -= 32;
	}

	while(len > 4) {
		COPY32;
		len--;
	}

	d8 = (uint8_t *)d32;
	s8 = (const uint8_t *)s32;
	while(len) {
		*d8 = *s8;
		d8++;
		s8++;
		len--;

	}

	return dst;
}
