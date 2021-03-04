#pragma once
#include "rtcli/base-types.h"

INTERNAL_CALL
RTCLI_EXTERN_C RTCLI_API void rtcli_interop_beep(rtcli_i32 frequency/*=800*/, rtcli_i32 duration/*=200*/);

#ifdef __cplusplus
namespace RTCLI
{
namespace Interop
{
    INTERNAL_CALL 
    RTCLI_FORCEINLINE static void Beep(rtcli_i32 frequency = 800, rtcli_i32 duration = 200)
    {
        return ::rtcli_interop_beep(frequency, duration);
    }
}
}
#endif