#pragma once
#include "cgpu/api.h"

#ifdef __cplusplus
extern "C" {
#endif

// procs
typedef CGpuInstanceId (*CGPUProcCreateInstance)(CGpuInstanceDescriptor const* descriptor);
typedef void (*CGPUProcDestroyInstance)(CGpuInstanceId instance);
typedef void (*CGPUProcEnumAdapters)(CGpuInstanceId instance, const CGpuAdapter* adapters, size_t* adapters_num);
typedef void (*CGPUProcDropAdapter)(CGpuAdapterId adapter);
typedef CGpuAdapterId (*CGPUProcGetDefaultAdapter)(CGpuInstanceId instance);

typedef struct CGpuProcTable 
{
    CGPUProcCreateInstance create_instance;
    CGPUProcDestroyInstance destroy_instance;
    CGPUProcEnumAdapters enum_adapters;
    CGPUProcDropAdapter drop_adapter;
    CGPUProcGetDefaultAdapter get_default_adapter;
} CGpuProcTable;

typedef struct CGpuInstance
{
    const CGpuProcTable* proc_table;
} CGpuInstance;

#ifdef __cplusplus
} // end extern "C"
#endif