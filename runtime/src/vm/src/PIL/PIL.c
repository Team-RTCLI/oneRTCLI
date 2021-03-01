#define DLL_IMPLEMENTATION
#include "rtcli/internal/log.h"
#include "rtcli/PIL.h"
#include <stdlib.h>

#define RTCLI_FREE_INTERNAL free
#define RTCLI_REALLOC_INTERNAL realloc
#define RTCLI_CALLOC_INTERNAL calloc
#define RTCLI_MALLOC_INTERNAL malloc

RTCLI_EXTERN_C RTCLI_API void* rtcli_calloc(rtcli_usize n, rtcli_usize x)
{
    void* ptr = RTCLI_NULL;
	if (!x)
		return 0;
	ptr = RTCLI_CALLOC_INTERNAL (n, x);
	if (ptr) 
		return ptr;
	rtcli_error("Could not allocate %i bytes", x);
	return RTCLI_NULL;
}