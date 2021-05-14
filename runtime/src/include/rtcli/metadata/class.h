#pragma once
#include "rtcli/base-types.h"
#include "rtcli/vm/object.h"

struct VMFieldInfo;
struct VMEventInfo;
struct VMPropertyInfo;
struct VMMethodInfo;

typedef enum VMInnerActualType
{
    VM_INNER_ACTUAL_TYPE_UNDEF           = 0x0,
    VM_INNER_ACTUAL_TYPE_VOID            = 0x1,
    VM_INNER_ACTUAL_TYPE_BOOL            = 0x2,
    VM_INNER_ACTUAL_TYPE_CHAR            = 0x3,
    VM_INNER_ACTUAL_TYPE_BYTE            = 0x4,
    VM_INNER_ACTUAL_TYPE_UBYTE           = 0x5,
    VM_INNER_ACTUAL_TYPE_SHORT           = 0x6,
    VM_INNER_ACTUAL_TYPE_USHORT          = 0x7,
    VM_INNER_ACTUAL_TYPE_INT             = 0x8,
    VM_INNER_ACTUAL_TYPE_UINT            = 0x9,
    VM_INNER_ACTUAL_TYPE_LONG            = 0xa,
    VM_INNER_ACTUAL_TYPE_ULONG           = 0xb,
    VM_INNER_ACTUAL_TYPE_NATIVEINT       = 0xc,
    VM_INNER_ACTUAL_TYPE_NATIVEUINT      = 0xd,
    VM_INNER_ACTUAL_TYPE_FLOAT           = 0xe,
    VM_INNER_ACTUAL_TYPE_DOUBLE          = 0xf,
    VM_INNER_ACTUAL_TYPE_STRING          = 0x10,         // Not used, should remove
    VM_INNER_ACTUAL_TYPE_PTR             = 0x11,
    VM_INNER_ACTUAL_TYPE_BYREF           = 0x12,
    VM_INNER_ACTUAL_TYPE_VALUECLASS      = 0x13,
    VM_INNER_ACTUAL_TYPE_CLASS           = 0x14,
    VM_INNER_ACTUAL_TYPE_REFANY          = 0x15,

    // VM_INNER_ACTUAL_TYPE_VAR is for a generic type variable.
    // Generic type variables only appear when the JIT is doing
    // verification (not NOT compilation) of generic code
    // for the EE, in which case we're running
    // the JIT in "import only" mode.

    VM_INNER_ACTUAL_TYPE_VAR             = 0x16,
    VM_INNER_ACTUAL_TYPE_COUNT,                         // number of jit types
} VMInnerActualType;

typedef enum VMInnerActualTypeShifted
{
    VM_INNER_ACTUAL_TYPE_UNDEF_SHIFTED           = (VM_INNER_ACTUAL_TYPE_UNDEF << 2),
    VM_INNER_ACTUAL_TYPE_VOID_SHIFTED            = (VM_INNER_ACTUAL_TYPE_VOID << 2),
    VM_INNER_ACTUAL_TYPE_BOOL_SHIFTED            = (VM_INNER_ACTUAL_TYPE_BOOL << 2),
    VM_INNER_ACTUAL_TYPE_CHAR_SHIFTED            = (VM_INNER_ACTUAL_TYPE_CHAR << 2),
    VM_INNER_ACTUAL_TYPE_BYTE_SHIFTED            = (VM_INNER_ACTUAL_TYPE_BYTE << 2),
    VM_INNER_ACTUAL_TYPE_UBYTE_SHIFTED           = (VM_INNER_ACTUAL_TYPE_UBYTE << 2),
    VM_INNER_ACTUAL_TYPE_SHORT_SHIFTED           = (VM_INNER_ACTUAL_TYPE_SHORT << 2),
    VM_INNER_ACTUAL_TYPE_USHORT_SHIFTED          = (VM_INNER_ACTUAL_TYPE_USHORT << 2),
    VM_INNER_ACTUAL_TYPE_INT_SHIFTED             = (VM_INNER_ACTUAL_TYPE_INT << 2),
    VM_INNER_ACTUAL_TYPE_UINT_SHIFTED            = (VM_INNER_ACTUAL_TYPE_UINT << 2),
    VM_INNER_ACTUAL_TYPE_LONG_SHIFTED            = (VM_INNER_ACTUAL_TYPE_LONG << 2),
    VM_INNER_ACTUAL_TYPE_ULONG_SHIFTED           = (VM_INNER_ACTUAL_TYPE_ULONG << 2),
    VM_INNER_ACTUAL_TYPE_NATIVEINT_SHIFTED       = (VM_INNER_ACTUAL_TYPE_NATIVEINT << 2),
    VM_INNER_ACTUAL_TYPE_NATIVEUINT_SHIFTED      = (VM_INNER_ACTUAL_TYPE_NATIVEUINT << 2),
    VM_INNER_ACTUAL_TYPE_FLOAT_SHIFTED           = (VM_INNER_ACTUAL_TYPE_FLOAT << 2),
    VM_INNER_ACTUAL_TYPE_DOUBLE_SHIFTED          = (VM_INNER_ACTUAL_TYPE_DOUBLE << 2),
    VM_INNER_ACTUAL_TYPE_STRING_SHIFTED          = (VM_INNER_ACTUAL_TYPE_STRING << 2),         // Not used, should remove
    VM_INNER_ACTUAL_TYPE_PTR_SHIFTED             = (VM_INNER_ACTUAL_TYPE_PTR << 2),
    VM_INNER_ACTUAL_TYPE_BYREF_SHIFTED           = (VM_INNER_ACTUAL_TYPE_BYREF << 2),
    VM_INNER_ACTUAL_TYPE_VALUECLASS_SHIFTED      = (VM_INNER_ACTUAL_TYPE_VALUECLASS << 2),
    VM_INNER_ACTUAL_TYPE_CLASS_SHIFTED           = (VM_INNER_ACTUAL_TYPE_CLASS << 2),
    VM_INNER_ACTUAL_TYPE_REFANY_SHIFTED          = (VM_INNER_ACTUAL_TYPE_REFANY << 2),
    VM_INNER_ACTUAL_TYPE_VAR_SHIFTED             = (VM_INNER_ACTUAL_TYPE_VAR << 2),
    VM_INNER_ACTUAL_TYPE_COUNT_SHIFTED,                         // number of jit types
} VMInnerActualTypeShifted;

RTCLI_EXTERN_C RTCLI_API 
rtcli_bool VMInnerActualType_isUnsined(enum VMInnerActualType type);

RTCLI_EXTERN_C RTCLI_API 
rtcli_u64 VMInnerActualType_ActualSize(enum VMInnerActualType type);

RTCLI_EXTERN_C RTCLI_API 
rtcli_u64 VMInnerActualType_StackSize(enum VMInnerActualType type);

typedef struct VMClassType {
#ifdef __cplusplus
    static constexpr rtcli_u32 Class = 0x0;
    static constexpr rtcli_u32 Int64Struct = 0x1;
    static constexpr rtcli_u32 NativeStruct = 0x2;
    static constexpr rtcli_u32 LargeStruct = 0x3;
#endif
    // 0x0 class 
    // 0x1 INT64 struct
    // 0x2 native sturct type (maybe large)
    // 0x3 large struct
    rtcli_u32 value : 2;
    VMInnerActualType actual_value;
} VMClassType;

typedef struct VMClass
{
    const char* name;
    const char* namespaze;

    struct VMFieldInfo* fields;
    uint16_t field_count;

    const struct VMEventInfo* events; 
    uint16_t event_count;
    
    const struct VMPropertyInfo* properties; 
    uint16_t property_count;

    const struct VMMethodInfo* methods;
    uint16_t method_count;

    struct VMClassType class_type;
    uint32_t actual_size;
    
    // 0x0 class 
    // 0x1 INT64 struct
    // 0x2 native sturct type (maybe large)
    // 0x3 large struct
#ifdef __cplusplus
    RTCLI_FORCEINLINE rtcli_bool isStruct(void) const
    {
        return (class_type.value & 0x1) == 0x1;
    }

    RTCLI_FORCEINLINE VMInnerActualType InnerActualType(void) const
    {
        return class_type.actual_value;
    }

    RTCLI_FORCEINLINE rtcli_bool isLargeStruct(void) const
    {
        return (class_type.value & 0x3) == 0x3
            || ((class_type.value & 0x3) == 0x2 && actual_size > sizeof(rtcli_i64));
    }
#endif
} VMClass;
typedef struct VMClass* VMRuntimeTypeHandle;

typedef struct VMValueClass 
{

} VMValueClass;

#ifdef __cplusplus

#endif

#define VM_OBJECT_FIELDS(STRUCT, BASETYPE)
// Here Starts C++ Fields
// Here Starts Dynamic Fields
