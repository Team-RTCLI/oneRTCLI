#pragma once
#include "rtcli/base-types.h"
#include "rtcli/metadata/method.h"
#include "rtcli/metadata/class.h"

typedef struct VMInterpreterType {
#ifdef __cplusplus
public:
    RTCLI_FORCEINLINE VMInterpreterType(nullptr_t ptr)
    {
        rtcli_error("Can't Create InterpreterType With nullptr");
    } 
    RTCLI_FORCEINLINE VMInterpreterType(VMInnerActualType inner_type) 
        : vm_inner_type((VMInnerActualType)(0x1 | (inner_type << 2)))
    {

    }
    RTCLI_FORCEINLINE VMInterpreterType(VMRuntimeTypeHandle rtType) 
        : vm_type((VMRuntimeTypeHandle)(0x0 | ((rtcli_isize)rtType << 2)))
    {

    }
    RTCLI_FORCEINLINE bool isStruct() const 
    {
        return is_inner() ? false : TypeHandle()->isStruct();
    }
    RTCLI_FORCEINLINE VMInnerActualType InnerActualType(void) const
    {
        return is_inner() ? InnerType() : TypeHandle()->InnerActualType();
    }
    RTCLI_FORCEINLINE bool isLargeStruct() const 
    {
        return is_inner() ? false : TypeHandle()->isLargeStruct();
    }
    RTCLI_FORCEINLINE rtcli_usize ActualSize() const
    {
        return is_inner() ? 
            VMInnerActualType_ActualSize(InnerType()) :
            TypeHandle()->actual_size;
    }
    RTCLI_FORCEINLINE VMRuntimeTypeHandle TypeHandle() const
    {
        return (VMRuntimeTypeHandle)((rtcli_isize)vm_type >> 2);
    }
    RTCLI_FORCEINLINE VMInnerActualType InnerType() const
    {
        return (VMInnerActualType)(vm_inner_type >> 2);
    }
    RTCLI_FORCEINLINE rtcli_bool is_inner() const
    {
        return (vm_type && 0x1) == 0x1;
    }
    RTCLI_FORCEINLINE rtcli_usize SizeOnStack() const
    {
        if(isStruct())
        {
            return ActualSize();
        } else {
            return is_inner() ? VMInnerActualType_StackSize(InnerType()) : sizeof(intptr_t);
        }
    }
#endif
    union
    {
        VMRuntimeTypeHandle vm_type;
        VMInnerActualType   vm_inner_type;
    };
} VMInterpreterType;

RTCLI_EXTERN_C RTCLI_API 
void VMInterpreterType_InitFromInnerType(VMInnerActualType inner, VMInterpreterType* type);

RTCLI_EXTERN_C RTCLI_API 
rtcli_bool VMInterpreterType_isLargeStruct(VMInterpreterType type);

RTCLI_EXTERN_C RTCLI_API 
rtcli_usize VMInterpreterType_ActualSize(VMInterpreterType type);

typedef enum VMInterpreterArithOp
{
    AOp_Add, AOp_Sub, AOp_Mul, AOp_Div, AOp_Rem
} VMInterpreterArithOp;

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


typedef struct VMMILMethodBody
{
    struct MIL_IL* ILs;
    rtcli_usize ILs_count;
} VMMILMethodBody;

typedef struct VMInterpreterMethod
{
    VMMethodInfo method;
    struct VMMILMethodBody* optimized_dynamic_method;

    struct VMInterpreterArgument* arguments;
    struct VMInterpreterLocal* locals;
    rtcli_usize locals_count;
#ifdef __cplusplus
    RTCLI_FORCEINLINE rtcli_usize LocalsMemorySize() const {
        return locals_count * sizeof(rtcli_i64);
    }
#endif
} VMInterpreterMethod;

RTCLI_EXTERN_C RTCLI_API 
void optimize_method_body(const struct VMCILMethodBody* raw, struct VMMILMethodBody* optimized);