#pragma once
#include "rtcli/base-types.h"
#include "rtcli/metadata/class.h"

typedef struct VMClassHashTable* VMClassTablePtr;

typedef struct VMModule 
{
    // hash_map<string, VMClass*>
    VMClassTablePtr class_table;
#ifdef __cplusplus
    VMModule();
    ~VMModule();
    VMRuntimeTypeHandle GetClass(const char* key);
    rtcli_bool InsertClass(const VMRuntimeTypeHandle klass);
#endif
} VMModule;

RTCLI_EXTERN_C RTCLI_API void VMModule_Init(struct VMModule* vm_module);
RTCLI_EXTERN_C RTCLI_API void VMModule_Destroy(struct VMModule* vm_module);
RTCLI_EXTERN_C RTCLI_API rtcli_bool VMModule_InsertClass(struct VMModule* vm_module, const VMRuntimeTypeHandle klass);
RTCLI_EXTERN_C RTCLI_API VMRuntimeTypeHandle VMModule_GetClass(struct VMModule* vm_module, const char* key);