#include "_fake_defines.h"
#include "_fake_typedefs.h"

void *memset(void *dest, int c, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
int memcmp(const void *buffer1, const void *buffer2, size_t count);