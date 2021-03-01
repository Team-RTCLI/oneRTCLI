#define DLL_IMPLEMENTATION
#include "rtcli/internal/log.h"
#include "rtcli/PIL.h"
#include <stdlib.h>

#define RTCLI_FREE_INTERNAL free
#define RTCLI_REALLOC_INTERNAL realloc
#define RTCLI_CALLOC_INTERNAL calloc
#define RTCLI_MALLOC_INTERNAL malloc

RTCLI_EXTERN_C RTCLI_API 
void* rtcli_calloc(rtcli_usize n, rtcli_usize x)
{
    void* ptr = RTCLI_NULL;
	if (!x)
		return 0;
	ptr = RTCLI_CALLOC_INTERNAL (n, x);
	if (ptr) 
		return ptr;
	rtcli_error("[rtcli_calloc] Could not calloc %i bytes with %i num", x, n);
	return RTCLI_NULL;
}


RTCLI_EXTERN_C RTCLI_API 
void* rtcli_malloc(rtcli_usize x)
{
    void* ptr = RTCLI_NULL;
	if (!x)
		return 0;
	ptr = RTCLI_MALLOC_INTERNAL (x);
	if (ptr) 
		return ptr;
	rtcli_error("[rtcli_malloc] Could not allocate %i bytes", x);
	return RTCLI_NULL;
}

RTCLI_EXTERN_C RTCLI_API 
void rtcli_free (void* ptr)
{
	if (ptr != NULL)
		RTCLI_FREE_INTERNAL (ptr);
}

void* rtcli_realloc (void* obj, rtcli_usize size)
{
	void* ptr;
	if (!size) {
		rtcli_free (obj);
		return 0;
	}
	ptr = RTCLI_REALLOC_INTERNAL (obj, size);
	if (ptr)
		return ptr;
	rtcli_error ("[rtcli_realloc] Could not reallocate %i bytes", size);
	return RTCLI_NULL;
}