#pragma once
//#include "rtcli/cpp-string.h"
#include "rtcli/base-types.h"
#include "rtcli/vm/object.h"

typedef struct VMString
{
    rtcli_object object;
    rtcli_i32 length;
    rtcli_char chars[RTCLI_ZERO_LEN_ARRAY];
} VMString;

// RTCLI IL2CPP InternalCall API.
#ifdef __cplusplus

namespace RTCLI
{
namespace InternalCall
{
    struct RTCLI_API String
    {
        // void String::.ctor(char[] 'value') cli managed internalcall
        static void Constructor(VMString& caller, rtcli_object& chars);
    };
}    
}

#endif