#pragma once
#include "base-types.h"

/*--------------------------------------mem apis-------------------------------------*/

RTCLI_EXTERN_C RTCLI_API void* rtcli_malloc(rtcli_usize x);

RTCLI_EXTERN_C RTCLI_API void* rtcli_calloc(rtcli_usize n, rtcli_usize x);

RTCLI_EXTERN_C RTCLI_API void rtcli_free(void* ptr);

RTCLI_EXTERN_C RTCLI_API void* rtcli_realloc(void* ptr, rtcli_usize size);

