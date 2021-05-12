#pragma once
#include "rtcli/base-types.h"

typedef struct VMFieldInfo 
{
    const char* name;
    const struct VMClass* type;
    struct VMClass* parent;
    int32_t offset; // If offset is -1, then it's thread static
    uint32_t token;
} VMFieldInfo;

#ifdef __cplusplus

#endif