#pragma once
#include "rtcli/vm/interpreter/meta.h"
#include <stdint.h>

typedef struct VMInterpreter
{
    struct VMStackFrame* sfs; 
    rtcli_u32 sf_size;
#ifdef __cplusplus
    void Exec(struct VMStackFrame* stack, struct CIL_IL il);
#endif
} VMInterpreter;

RTCLI_EXTERN_C RTCLI_API void vm_exec_ldarg(struct VMStackFrame* stack, int arg_index);
RTCLI_EXTERN_C RTCLI_API void vm_exec_ldc_i4(struct VMStackFrame* stack, rtcli_i32 value);
RTCLI_EXTERN_C RTCLI_API void vm_exec_stloc(struct VMStackFrame* stack, rtcli_i32 loc_index);
RTCLI_EXTERN_C RTCLI_API void vm_exec_ldloc(struct VMStackFrame* stack, rtcli_i32 loc_index);
RTCLI_EXTERN_C RTCLI_API void vm_exec_add(struct VMStackFrame* stack);
