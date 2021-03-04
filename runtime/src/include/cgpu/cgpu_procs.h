#pragma once
#include "api.h"

#ifdef __cplusplus
extern "C" {
#endif

// procs
typedef CGpuInstanceId (*CGPUProcCreateInstance)(CGpuInstanceDescriptor const* descriptor);
typedef void (*CGPUProcDestroyInstance)(CGpuInstanceId instance);
typedef void (*CGPUProcEnumAdapters)(CGpuInstanceId instance, const CGpuAdapter* adapters, size_t* adapters_num);

#ifdef __cplusplus
} // end extern "C"
#endif