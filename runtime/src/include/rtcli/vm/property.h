#pragma once
#include "rtcli/base-types.h"

typedef struct VMPropertyInfo 
{
    struct VMClass* parent;
    const char* name;
    const struct VMMethodInfo *get;
    const struct VMMethodInfo *set;
    uint32_t attrs;
    uint32_t token;
} VMPropertyInfo;

#ifdef __cplusplus

#endif