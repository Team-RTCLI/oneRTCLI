#pragma once
#include "rtcli/base-types.h"
#include "rtcli/vm/field.h"

#ifdef __cplusplus
typedef struct VMType : public RTCLI::System::__object
{
#else
typedef struct VMType
{
    rtcli_object object;
#endif//__cplusplus
    const char* name;
    const char* namespaze;

    uint32_t actualSize;
} VMType;

#ifdef __cplusplus

#endif