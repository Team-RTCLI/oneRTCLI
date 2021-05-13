#pragma once
#include "rtcli/base-types.h"
#include "rtcli/vm/method.h"
#include "rtcli/vm/interpreter/interpreter.h"

typedef struct VMInterpreterArgument
{
    VMInterpreterType type;
    rtcli_u64 offset;
} VMInterpreterArgument;

typedef struct VMInterpreterMethod
{
    VMMethodInfo method;
    struct VMInterpreterArgument* arguments;
} VMInterpreterMethod;

typedef struct VMStackOp
{
    rtcli_u64 val;
    struct VMInterpreterType type;
} VMStackOp;

// [*-local_mem-*][*--opstack--*]
// [        frame_memory        ] 
typedef struct VMStackFrame
{
    struct VMInterpreterMethod* method;
    rtcli_byte* args;

    struct VMStackOp* ops;
    rtcli_usize ops_ht;

    // lss: large struct stack
    rtcli_byte* lss;
    rtcli_usize lss_ht;
    rtcli_usize lss_alloc_size;

#ifdef __cplusplus
    RTCLI_API void LdFromMemAddr(void* addr, struct VMInterpreterType type);
    RTCLI_API void StToMemAddr(void* addr, struct VMInterpreterType type);

    RTCLI_FORCEINLINE void* GetArgAddr(rtcli_usize index)
    {
        return &args[method->arguments[index].offset];
    }

    RTCLI_FORCEINLINE VMInterpreterType GetArgType(rtcli_usize index)
    {
        return method->arguments[index].type;
    }

    RTCLI_FORCEINLINE void OpStackSetType(rtcli_usize index, VMInterpreterType type)
    {
        ops[index].type = type;
    }
    template<typename T>
    RTCLI_FORCEINLINE T OpStackGetValue(rtcli_usize index)
    {
        return *OpStackGetValueAddr<T>(index);   
    }
    template<typename T>
    RTCLI_FORCEINLINE void OpStackSetValue(rtcli_usize index, T value)
    {
        *OpStackGetValueAddr<T>(index) = value;
    }
    template<typename T = void>
    RTCLI_FORCEINLINE T* OpStackGetValueAddr(unsigned index)
    {
        return reinterpret_cast<T*>(&ops[index].val);
    }
    RTCLI_FORCEINLINE rtcli_i64 GetSmallStructValue(void* src, size_t sz)
    {
        //assert(sz <= sizeof(rtcli_i64));
        rtcli_i64 ret = 0;
        memcpy(&ret, src, sz);
        return ret;
    }
protected:
    void LargeStructStackCanPush(rtcli_usize sz);
    void* LargeStructStackPush(rtcli_usize sz);
#endif
} VMStackFrame;

RTCLI_EXTERN_C RTCLI_API 
struct VMStackFrame create_vmstack(struct VMInterpreterMethod* method_info, rtcli_byte* args,
    rtcli_byte* frame_memory, rtcli_usize lss_alloc_size);

