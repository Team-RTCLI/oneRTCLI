#pragma once
#include "rtcli/base-types.h"
#include "rtcli/vm/class.h"

typedef struct VMInterpreterType {
    union
    {
        VMRuntimeTypeHandle type;
        // 0x0 class 
        // 0x1 INT64 struct
        // 0x2 native sturct type (maybe large)
        // 0x3 large struct
        rtcli_u32 flags : 2;
    };
} VMInterpreterType;

RTCLI_EXTERN_C RTCLI_API 
VMInterpreterType VMInterpreterType_from_large_struct(VMInterpreterType interpreter_type);
RTCLI_EXTERN_C RTCLI_API 
rtcli_bool VMInterpreterType_is_large_struct(VMInterpreterType interpreter_type);