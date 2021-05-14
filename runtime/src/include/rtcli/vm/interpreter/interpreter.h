#pragma once
#include "rtcli/vm/interpreter/meta.h"
#include <stdint.h>

typedef struct VMInterpreter
{
    struct VMStackFrame* sfs; 
    rtcli_u32 sf_size;
    rtcli_u32 sf_capacity;

    rtcli_arg_slot* args;
#ifdef __cplusplus
public:
    RTCLI_API void Exec(struct VMStackFrame* stack, const struct MIL_IL il);
    RTCLI_API void Exec(struct VMStackFrame* stack, const struct CIL_IL il);
    RTCLI_API void Exec(struct VMStackFrame* stack, struct VMInterpreterMethod* method, rtcli_byte* args);
#endif
} VMInterpreter;

RTCLI_EXTERN_C RTCLI_API void vm_exec_nop(struct VMStackFrame* stack);
RTCLI_EXTERN_C RTCLI_API void vm_exec_ldarg(struct VMStackFrame* stack, int arg_index);
RTCLI_EXTERN_C RTCLI_API void vm_exec_ldc_i4(struct VMStackFrame* stack, rtcli_i32 value);
RTCLI_EXTERN_C RTCLI_API void vm_exec_stloc(struct VMStackFrame* stack, rtcli_i32 loc_index);
RTCLI_EXTERN_C RTCLI_API void vm_exec_ldloc(struct VMStackFrame* stack, rtcli_i32 loc_index);
RTCLI_EXTERN_C RTCLI_API void vm_exec_add(struct VMStackFrame* stack);
//RTCLI_EXTERN_C RTCLI_API void vm_exec_call(struct VMStackFrame* stack, struct VMInterpreterMethod* method);


RTCLI_EXTERN_C RTCLI_API void interpreter_exec(
    struct VMInterpreter* interpreter, struct VMInterpreterMethod* method,
    rtcli_byte* args);
RTCLI_EXTERN_C RTCLI_API void interpreter_exec_at_stackframe(
    struct VMInterpreter* interpreter, struct VMInterpreterMethod* method, 
    rtcli_byte* args, struct VMStackFrame* stackframe);