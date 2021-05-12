#pragma once
#include "rtcli/base-types.h"

typedef struct VMParameterInfo
{
    const char* name;
    int32_t position;
    uint32_t token;
    const struct VMClass* parameter_type;
} VMParameterInfo;