#pragma once
#include "rtcli/base-types.h"
#include "rtcli/vm/interpreter/interpreter.h"

typedef struct VMStackOp
{
    rtcli_u64 val;
    struct VMInterpreterType type;
} VMStackOp;

// [*-local_mem-*][*--opstack--*]
// [        frame_memory        ] 
typedef struct VMStackFrame
{
    struct VMStackOp* ops;
    rtcli_usize ops_ht;

    // lss: large struct stack
    rtcli_byte* lss;
    rtcli_usize lss_ht;
    rtcli_usize lss_alloc_size;
} VMStackFrame;

RTCLI_EXTERN_C RTCLI_API 
struct VMStackFrame
create_vmstack(rtcli_byte* frame_memory, rtcli_usize lss_alloc_size);

RTCLI_EXTERN_C RTCLI_API void ldarg(int arg_index);