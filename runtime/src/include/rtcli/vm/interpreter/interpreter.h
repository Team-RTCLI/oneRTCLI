#pragma once
#include "rtcli/vm/interpreter/meta.h"
#include <stdint.h>

typedef struct VMInterpreter
{
    struct VMStackFrame* sfs; 
    rtcli_u32 sf_size;
    rtcli_u32 sf_capacity;

    rtcli_byte opstack[4096 * 4];
    rtcli_byte* opstack_current;

#ifdef __cplusplus
public:
    VMInterpreter(rtcli_usize lss_alloc_size);
    ~VMInterpreter();
    RTCLI_API void Exec(struct VMStackFrame* stack, const struct MIL_IL il);
    RTCLI_API void Exec(struct VMStackFrame* stack, const struct CIL_IL il);
    RTCLI_API void Exec(struct VMStackFrame* stack, struct VMInterpreterMethod* method, rtcli_arg_slot* args);
    RTCLI_API void ExecNextStack(struct VMInterpreterMethod* method, rtcli_arg_slot* args);
protected:
    struct VMStackFrame* CurrentStackFrame();
    struct VMStackFrame* NextStackFrame();
    struct VMStackFrame* DiscardStackFrame();
#endif
} VMInterpreter;

RTCLI_EXTERN_C RTCLI_API void vm_exec_nop(struct VMStackFrame* stack);
RTCLI_EXTERN_C RTCLI_API void vm_exec_ldarg(struct VMStackFrame* stack, int arg_index);
RTCLI_EXTERN_C RTCLI_API void vm_exec_ldc_i4(struct VMStackFrame* stack, rtcli_i32 value);
RTCLI_EXTERN_C RTCLI_API void vm_exec_stloc(struct VMStackFrame* stack, rtcli_i32 loc_index);
RTCLI_EXTERN_C RTCLI_API void vm_exec_ldloc(struct VMStackFrame* stack, rtcli_i32 loc_index);
RTCLI_EXTERN_C RTCLI_API void vm_exec_add(struct VMStackFrame* stack);
//RTCLI_EXTERN_C RTCLI_API void vm_exec_call(struct VMStackFrame* stack, struct VMInterpreterMethod* method);

RTCLI_EXTERN_C RTCLI_API void VMInterpreter_Init(
    struct VMInterpreter* interpreter, rtcli_usize lss_alloc_size);
RTCLI_EXTERN_C RTCLI_API void VMInterpreter_Destroy(struct VMInterpreter* interpreter);
RTCLI_EXTERN_C RTCLI_API void VMInterpreter_Exec(
    struct VMInterpreter* interpreter, struct VMInterpreterMethod* method);