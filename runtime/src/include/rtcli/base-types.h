#pragma once
#include "config.h"
#ifdef RTCLI_COMPILER_MSVC
#include <corecrt_wstring.h>
#else
#endif
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char rtcli_byte;
typedef uint8_t rtcli_u8;
typedef int8_t rtcli_i8;
typedef uint16_t rtcli_u16;
typedef int16_t rtcli_i16;
typedef uint32_t rtcli_u32;
typedef int32_t rtcli_i32;
typedef uint64_t rtcli_u64;
typedef int64_t rtcli_i64;
typedef float rtcli_f32;
typedef double rtcli_f64;
typedef intptr_t rtcli_intptr;
typedef uintptr_t rtcli_uintptr;
typedef size_t rtcli_usize;
typedef ptrdiff_t rtcli_isize;
typedef void rtcli_void;

typedef rtcli_i64 rtcli_arg_slot;

#include <stdbool.h>
typedef bool rtcli_bool;

typedef rtcli_u32 rtcli_rc;
typedef rtcli_u32 rtcli_object_flags;

typedef struct rtcli_guid
{
    rtcli_u32 data1;
    rtcli_u16 data2;
    rtcli_u16 data3;
    rtcli_u8  data4[8];
} rtcli_guid;

/* Alignment for VMArray.vector */
#if defined(_AIX)
/*
 * HACK: doubles in structs always align to 4 on AIX... even on 64-bit,
 * which is bad for aligned usage like what System.Array.FastCopy does
 */
typedef rtcli_u64 rtcli_64bit_aligned;
#else
typedef double rtcli_64bit_aligned;
#endif

#ifdef RTCLI_COMPILER_MSVC
    typedef wchar_t rtcli_char;
#elif __has_feature(cxx_unicode_literals)
    typedef char16_t rtcli_char;
#else
    typedef uint16_t rtcli_char;
#endif

#define RTCLI_STRING(str) L##str
#define RTCLI_STRING_LENGTH(str) wcslen((str))

#ifdef RTCLI_COMPILER_MSVC
    typedef wchar_t rtcli_native_char;
    
    #define RTCLI_NATIVE_STRING(str) L##str
    #define RTCLI_NATIVE_STRING_LENGTH(str) wcslen((str))

    #define RTCLI_STRING_LENGTH(str) wcslen((str))
#else
    typedef char rtcli_native_char;

    //#define RTCLI_NATIVE_STRING(str) str
    //#define RTCLI_NATIVE_STRING_LENGTH(str) strlen((str))
#endif

struct VMStackFrame;
typedef void (*VMMethodPointer)(struct VMStackFrame* stackframe);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace RTCLI
{
    using u8 = ::rtcli_u8;
    using i8 = ::rtcli_i8;
    using u16 = ::rtcli_u16;
    using i16 = ::rtcli_i16;
    using u32 = ::rtcli_u32;
    using i32 = ::rtcli_i32;
    using u64 = ::rtcli_u64;
    using i64 = ::rtcli_i64;
    using f32 = ::rtcli_f32;
    using f64 = ::rtcli_f64;
    using intptr = ::rtcli_intptr;
    using usize = ::rtcli_usize;
    using isize = ::rtcli_isize;
    using Void = ::rtcli_void;
    using Byte = ::rtcli_byte;
#ifdef RTCLI_COMPILER_CPP20
    using c8 = char8_t;
#else
	using c8 = char;
	static_assert(sizeof(c8) == 1, "Incorrect c8 type size.");
#endif
    using c16 = char16_t;
    using c32 = char32_t;

    using Char = ::rtcli_char;
    using NativeChar = ::rtcli_native_char;

    template<typename T, typename V>
    RTCLI_FORCEINLINE T StaticCast(V&& v)
    {
        return static_cast<T>(forward<V>(v));
    }

    namespace System
    {
        using Void = RTCLI::Void;
    }
}
#endif