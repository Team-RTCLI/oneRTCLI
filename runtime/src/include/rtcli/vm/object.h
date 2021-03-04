#pragma once
#include "rtcli/base-types.h"

struct rtcli_vtable;
// [*-p_vtable-*][*--rc--*][*--flags--*][*-cpp_field-*][*-csharp_field-*]
// [                           rtcli_object                             ] 
typedef struct rtcli_object
{
    struct rtcli_vtable* p_vtable;
    rtcli_rc m_rc;
    rtcli_object_flags m_flags;
} rtcli_object;

#ifdef __cplusplus

namespace RTCLI::System
{
    using __object = ::rtcli_object;
};

#endif