#pragma once
#include "rtcli/base-types.h"
#include "rtcli/metadata/method.h"
#include "rtcli/metadata/class.h"
#include <stdint.h>

typedef struct VMInterpreterType {
#ifdef __cplusplus
public:
    RTCLI_FORCEINLINE VMInterpreterType(VMInnerActualType inner_type) 
        : vm_inner_type(inner_type), is_inner(true)
    {

    }
    RTCLI_FORCEINLINE VMInterpreterType(VMRuntimeTypeHandle rtType) 
        : vm_type(rtType), is_inner(false)
    {

    }
    RTCLI_FORCEINLINE bool isStruct() const 
    {
        return is_inner ? false : vm_type->isStruct();
    }
    RTCLI_FORCEINLINE VMInnerActualType InnerActualType(void) const
    {
        return is_inner ? vm_inner_type : vm_type->InnerActualType();
    }
    RTCLI_FORCEINLINE bool isLargeStruct() const 
    {
        return is_inner ? false : vm_type->isLargeStruct();
    }
    RTCLI_FORCEINLINE rtcli_usize ActualSize() const
    {
        return is_inner ? VMInnerActualType_ActualSize(vm_inner_type) : vm_type->actual_size;
    }
    RTCLI_FORCEINLINE rtcli_usize SizeOnStack() const
    {
        if(isStruct())
        {
            return ActualSize();
        } else {
            return is_inner ? VMInnerActualType_StackSize(vm_inner_type) : sizeof(intptr_t);
        }
    }
#endif
    union
    {
        VMRuntimeTypeHandle vm_type;
        VMInnerActualType   vm_inner_type;
    };
    rtcli_bool is_inner : 1;
} VMInterpreterType;

RTCLI_EXTERN_C RTCLI_API 
void VMInterpreterType_InitFromInnerType(VMInnerActualType inner, VMInterpreterType* type);

RTCLI_EXTERN_C RTCLI_API 
rtcli_bool VMInterpreterType_isLargeStruct(VMInterpreterType type);

RTCLI_EXTERN_C RTCLI_API 
rtcli_usize VMInterpreterType_ActualSize(VMInterpreterType type);

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
