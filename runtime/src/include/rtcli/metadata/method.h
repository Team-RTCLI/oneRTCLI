#pragma once
#include "rtcli/base-types.h"
#include "rtcli/cil/opcode.h"

#define METHOD_FLAG_NATIVE 0x00000000000000001
#define METHOD_FLAG_DYNAMIC 0x00000000000000002

struct VMClass;

typedef struct VMCILMethodBody
{
    struct CIL_IL* ILs;
    rtcli_usize ILs_count;
} VMCILMethodBody;

typedef struct VMMethodInfo 
{
    union
    {
        VMMethodPointer method_pointer;
        const struct VMCILMethodBody* dynamic_method;
    };
    const char* name;
    struct VMClass* klass;
    const struct VMClass* return_type;

    const struct VMParameterInfo* parameters;
    rtcli_u8 parameters_count;
    
    rtcli_u32 max_stack;

    rtcli_u32 flags;
} VMMethodInfo;

#ifdef __cplusplus

#endif