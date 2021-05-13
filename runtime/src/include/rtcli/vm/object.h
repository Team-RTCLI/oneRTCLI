#pragma once
#include "rtcli/base-types.h"

// [*-p_vtable-*][*--rc--*][*--flags--*][*-cpp_field-*][*-csharp_field-*]
// [                           rtcli_object                             ] 
typedef struct rtcli_object
{
    struct VMClass* p_vtable;
    rtcli_rc m_rc;
    rtcli_object_flags m_flags;
} rtcli_object;

typedef struct VMValueClass 
{

} VMValueClass;

#ifdef __cplusplus

namespace RTCLI::System
{
    using __object = ::rtcli_object;
};

#endif