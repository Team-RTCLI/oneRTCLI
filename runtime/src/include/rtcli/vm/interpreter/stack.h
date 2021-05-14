#pragma once
#include "rtcli/vm/interpreter/meta.h"
#include "rtcli/vm/interpreter/interpreter.h"
#ifdef __cplusplus
#include <cassert>
#endif

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

    rtcli_usize ip; // ECMA-355-MethodState-IP

    rtcli_u64 locals_size;
    struct VMStackOp* ops;
    rtcli_usize ops_ht;
    
    // local mem ==> |int64|int64|int64|int64|
    rtcli_byte* local_var_memory;
#ifdef __cplusplus
    RTCLI_FORCEINLINE rtcli_i64* FixedSizeLocalSlot(rtcli_usize loc_index) {
        const auto casted = reinterpret_cast<rtcli_i64*>(local_var_memory);
        return casted + loc_index;
    }
#endif //__cplusplus

    // lss: large struct stack
    rtcli_byte* lss;
    rtcli_usize lss_ht;
    rtcli_usize lss_alloc_size;
#ifdef __cplusplus
    RTCLI_API void LdFromMemAddr(void* addr, struct VMInterpreterType type);
    RTCLI_API void StToMemAddr(void* addr, struct VMInterpreterType type);

    RTCLI_FORCEINLINE void* GetArgAddr(rtcli_usize index) {
        return &args[method->arguments[index].offset];
    }
    RTCLI_FORCEINLINE VMInterpreterType GetArgType(rtcli_usize index) {
        return method->arguments[index].type;
    }
    RTCLI_FORCEINLINE void OpStackSetType(rtcli_usize index, VMInterpreterType type) {
        ops[index].type = type;
    }
    RTCLI_FORCEINLINE VMInterpreterType OpStackGetType(rtcli_usize index) {
        return ops[index].type;
    }
    template<typename T>
    RTCLI_FORCEINLINE T OpStackGetValue(rtcli_usize index) {
        auto addr = OpStackGetValueAddr<T>(index);
        return *addr;
    }
    template<typename T>
    RTCLI_FORCEINLINE void OpStackSetValue(rtcli_usize index, T value) {
        *OpStackGetValueAddr<T>(index) = value;
    }
    template<typename T = void>
    RTCLI_FORCEINLINE T* OpStackGetValueAddr(unsigned index) {
        return reinterpret_cast<T*>(&ops[index].val);
    }
    RTCLI_FORCEINLINE rtcli_i64 GetSmallStructValue(void* src, size_t sz) {
        assert(sz <= sizeof(rtcli_i64));
        rtcli_i64 ret = 0;
        memcpy(&ret, src, sz);
        return ret;
    }
    RTCLI_FORCEINLINE float RemFunc(float v1, float v2) {
        assert(0 && "not implemented!");
        return 0.f;
    }
    template<enum VMInterpreterArithOp op, typename T, bool IsIntType, enum VMInnerActualType cit, bool TypeIsUnchanged>
    void DoOpCalc(T val1, T val2);
    template<enum VMInterpreterArithOp op, bool bInterpreterLooseRules = false>
    void DoOp();
protected:
    void LargeStructStackCanPush(rtcli_usize sz);
    void* LargeStructStackPush(rtcli_usize sz);

    RTCLI_FORCEINLINE void LargeStructOperandStackPop(rtcli_usize sz, void* from_addr) {
        // TODO: check
        lss_ht -= sz;
    }
#endif
} VMStackFrame;

RTCLI_EXTERN_C RTCLI_API 
struct VMStackFrame create_vm_stackframe(struct VMInterpreterMethod* method_info,
    rtcli_byte* frame_memory, rtcli_usize lss_alloc_size);

#ifdef __cplusplus
#include "rtcli/vm/interpreter/stack.inl"
#endif