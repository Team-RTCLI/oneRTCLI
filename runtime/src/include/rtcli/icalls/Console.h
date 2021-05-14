#pragma once
#include "rtcli/vm/string.h"

INTERNAL_CALL
RTCLI_EXTERN_C RTCLI_API void VMInternal_Console_Write(VMString* s);
INTERNAL_CALL
RTCLI_EXTERN_C RTCLI_API void VMInternal_Console_WriteLine(VMString* s);

#ifdef __cplusplus
namespace RTCLI
{
namespace Internal
{
class Console
{
public:
    INTERNAL_CALL RTCLI_API static void Write(VMString* s);
    INTERNAL_CALL RTCLI_API static void Write(VMString& s);
    INTERNAL_CALL RTCLI_API static void WriteLine(VMString* s = nullptr);
    INTERNAL_CALL RTCLI_API static void WriteLine(VMString& s);
};
}
}
#endif