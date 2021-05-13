#include "rtcli/base-types.h"
#define DLL_IMPLEMENTATION
#include "rtcli/vm/class.h"
#include "rtcli/vm/string.h"
#include "assert.h"

RTCLI_EXTERN_C RTCLI_API 
rtcli_bool VMInnerActualType_isUnsined(enum VMInnerActualType type)
{
    switch (type)
    {
    case VM_INNER_ACTUAL_TYPE_INT:
    case VM_INNER_ACTUAL_TYPE_NATIVEUINT:
    case VM_INNER_ACTUAL_TYPE_ULONG:
    case VM_INNER_ACTUAL_TYPE_UBYTE:
    case VM_INNER_ACTUAL_TYPE_USHORT:
    case VM_INNER_ACTUAL_TYPE_CHAR:
        return true;
    default:
        return false;
    }
}

RTCLI_EXTERN_C RTCLI_API 
rtcli_u64 VMInnerActualType_ActualSize(enum VMInnerActualType type)
{
    switch (type)
    {
    case VM_INNER_ACTUAL_TYPE_VOID: return 0;
    case VM_INNER_ACTUAL_TYPE_BOOL: return 1;
    case VM_INNER_ACTUAL_TYPE_CHAR: return sizeof(rtcli_char);
    case VM_INNER_ACTUAL_TYPE_BYTE: return sizeof(rtcli_byte);
    case VM_INNER_ACTUAL_TYPE_UBYTE: return sizeof(rtcli_byte);
    case VM_INNER_ACTUAL_TYPE_SHORT: return sizeof(rtcli_i16);
    case VM_INNER_ACTUAL_TYPE_USHORT: return sizeof(rtcli_u16);
    case VM_INNER_ACTUAL_TYPE_INT: return sizeof(rtcli_i32);
    case VM_INNER_ACTUAL_TYPE_UINT: return sizeof(rtcli_u32);
    case VM_INNER_ACTUAL_TYPE_LONG: return sizeof(rtcli_i64);
    case VM_INNER_ACTUAL_TYPE_ULONG: return sizeof(rtcli_u64);
    case VM_INNER_ACTUAL_TYPE_NATIVEINT: return sizeof(rtcli_isize);
    case VM_INNER_ACTUAL_TYPE_NATIVEUINT: return sizeof(rtcli_usize);
    case VM_INNER_ACTUAL_TYPE_FLOAT: return sizeof(rtcli_f32);
    case VM_INNER_ACTUAL_TYPE_DOUBLE: return sizeof(rtcli_f32);
    case VM_INNER_ACTUAL_TYPE_STRING: return sizeof(VMString);
    case VM_INNER_ACTUAL_TYPE_PTR: return sizeof(intptr_t);
    case VM_INNER_ACTUAL_TYPE_BYREF: return sizeof(intptr_t);
    case VM_INNER_ACTUAL_TYPE_VALUECLASS: 
    case VM_INNER_ACTUAL_TYPE_CLASS:
    case VM_INNER_ACTUAL_TYPE_REFANY:
    default:
        assert(0);
    }
    return 0;
}

RTCLI_EXTERN_C RTCLI_API 
rtcli_u64 VMInnerActualType_StackSize(enum VMInnerActualType type)
{
    switch (type)
    {
    case VM_INNER_ACTUAL_TYPE_VOID: return 0;
    case VM_INNER_ACTUAL_TYPE_BOOL: return 1;
    case VM_INNER_ACTUAL_TYPE_CHAR: return sizeof(rtcli_char);
    case VM_INNER_ACTUAL_TYPE_BYTE: return sizeof(rtcli_byte);
    case VM_INNER_ACTUAL_TYPE_UBYTE: return sizeof(rtcli_byte);
    case VM_INNER_ACTUAL_TYPE_SHORT: return sizeof(rtcli_i16);
    case VM_INNER_ACTUAL_TYPE_USHORT: return sizeof(rtcli_u16);
    case VM_INNER_ACTUAL_TYPE_INT: return sizeof(rtcli_i32);
    case VM_INNER_ACTUAL_TYPE_UINT: return sizeof(rtcli_u32);
    case VM_INNER_ACTUAL_TYPE_LONG: return sizeof(rtcli_i64);
    case VM_INNER_ACTUAL_TYPE_ULONG: return sizeof(rtcli_u64);
    case VM_INNER_ACTUAL_TYPE_NATIVEINT: return sizeof(rtcli_isize);
    case VM_INNER_ACTUAL_TYPE_NATIVEUINT: return sizeof(rtcli_usize);
    case VM_INNER_ACTUAL_TYPE_FLOAT: return sizeof(rtcli_f32);
    case VM_INNER_ACTUAL_TYPE_DOUBLE: return sizeof(rtcli_f32);
    case VM_INNER_ACTUAL_TYPE_STRING: return sizeof(VMString);
    case VM_INNER_ACTUAL_TYPE_PTR: return sizeof(intptr_t);
    case VM_INNER_ACTUAL_TYPE_BYREF: return sizeof(intptr_t);
    case VM_INNER_ACTUAL_TYPE_VALUECLASS: return sizeof(intptr_t);
    case VM_INNER_ACTUAL_TYPE_CLASS: return sizeof(intptr_t);
    case VM_INNER_ACTUAL_TYPE_REFANY: return sizeof(intptr_t);
    default:
        assert(0);
    }
    return 0;
}