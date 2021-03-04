#pragma once
#include "cgpu_procs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CGpuProcTable 
{
    CGPUProcCreateInstance create_instance;
    CGPUProcDestroyInstance destroy_instance;
    CGPUProcEnumAdapters enum_adapters;
} CGpuProcTable;

typedef struct CGpuInstance
{
    const CGpuProcTable* proc_table;
} CGpuInstance;

#ifdef __cplusplus
} // end extern "C"
#endif