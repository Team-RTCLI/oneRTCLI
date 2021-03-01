#define DLL_IMPLEMENTATION
#include "rtcli/interop/kernel32.h"
#include "rtcli/detail/log.h"
#include <windows.h>


INTERNAL_CALL
RTCLI_EXTERN_C RTCLI_API void rtcli_interop_beep(rtcli_i32 frequency, rtcli_i32 duration)
{
    const int MinBeepFrequency = 37;
    const int MaxBeepFrequency = 32767;

    if (frequency < MinBeepFrequency || frequency > MaxBeepFrequency)
    {
        rtcli_error("frequecy must be between %i and %i", MinBeepFrequency, MaxBeepFrequency);
        //throw new ArgumentOutOfRangeException(nameof(frequency), frequency, SR.Format(SR.ArgumentOutOfRange_BeepFrequency, MinBeepFrequency, MaxBeepFrequency));
        return;
    }
    if (duration <= 0)
    {
        rtcli_error("duration %i needs to be a positive number", duration);
        //throw new ArgumentOutOfRangeException(nameof(duration), duration, SR.ArgumentOutOfRange_NeedPosNum);
        return;
    }
    Beep(frequency, duration);
}