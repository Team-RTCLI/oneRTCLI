#pragma once
#include "rtcli/base-types.h"
#include "rtcli/vm/object.h"

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

    const struct VMMethodInfo** methods;
    uint16_t method_count;

    uint32_t actual_size;
} VMClass;

#ifdef __cplusplus

#endif

#define VM_OBJECT_FIELDS(STRUCT, BASETYPE)
// Here Starts C++ Fields
// Here Starts Dynamic Fields
