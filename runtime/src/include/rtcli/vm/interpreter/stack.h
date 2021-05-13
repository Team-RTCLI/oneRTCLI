#pragma once
#include "rtcli/base-types.h"
#include "rtcli/metadata/method.h"
#include "rtcli/vm/interpreter/interpreter.h"

typedef struct VMInterpreterLocal 
{
    VMInterpreterType type;
    rtcli_u64 offset;
} VMInterpreterLocal;

typedef struct VMInterpreterArgument
{
    VMInterpreterType type;
    rtcli_u64 offset;
} VMInterpreterArgument;

typedef struct VMInterpreterMethod
{
    VMMethodInfo method;
    struct VMInterpreterArgument* arguments;
    struct VMInterpreterLocal* locals;
    rtcli_usize locals_count;
#ifdef __cplusplus
    RTCLI_FORCEINLINE rtcli_usize LocalsMemorySize() const
    {
        rtcli_usize latest_index = 0;
        rtcli_usize max_offset = 0;
        for(rtcli_usize i = 0; i < locals_count; i++)
        {
            VMInterpreterLocal& current_local = locals[i];
            if(max_offset < current_local.offset)
            {
                max_offset = current_local.offset;
                latest_index = i;
            }
        }
        return max_offset + VMInnerActualType_StackSize(
           locals[latest_index].type.InnerActualType()
        );
    }
#endif
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

    rtcli_u64 locals_size;
    struct VMStackOp* ops;
    rtcli_usize ops_ht;
    
    // local mem ==> |int64|int64|int64|int64|
    rtcli_byte* local_var_memory;
#ifdef __cplusplus
    RTCLI_FORCEINLINE rtcli_i64* FixedSizeLocalSlot(rtcli_usize loc_index)
    {
        return reinterpret_cast<rtcli_i64*>(local_var_memory) + loc_index;
    }
#endif //__cplusplus

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

    RTCLI_FORCEINLINE void LargeStructOperandStackPop(rtcli_usize sz, void* from_addr)
    {
        // TODO: check
        lss_ht -= sz;
    }
#endif
} VMStackFrame;

RTCLI_EXTERN_C RTCLI_API 
struct VMStackFrame create_vmstack(struct VMInterpreterMethod* method_info, rtcli_byte* args,
    rtcli_byte* frame_memory, rtcli_usize lss_alloc_size);

