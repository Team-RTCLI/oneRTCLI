#pragma once
#include "rtcli/base-types.h"

#define METHOD_FLAG_NATIVE 0x00000000000000001
#define METHOD_FLAG_DYNAMIC 0x00000000000000002

struct VMClass;

typedef struct VMMethodInfo 
{
    union
    {
        VMMethodPointer method_pointer;
        const struct DynamicMethod* dynamic_method;
    };
    const char* name;
    struct VMClass* klass;
    const struct VMClass* return_type;

    const struct VMParameterInfo* parameters;
    rtcli_u8 parameters_count;

    rtcli_u32 flags;
} VMMethodInfo;

#ifdef __cplusplus

#endif