#pragma once
#include "cgpu_config.h"

#ifdef __cplusplus
extern "C" {
#ifndef CGPU_NULLPTR
#define CGPU_NULLPTR nullptr
#endif
#else
#ifndef CGPU_NULLPTR
#define CGPU_NULLPTR NULL
#endif
#endif
#include <stdint.h>

typedef int CGpuVersion;
typedef enum ECGPUBackEnd
{
#ifdef CGPU_USE_WEBGPU
    ECGPUBackEnd_WEBGPU,
#endif
#ifdef CGPU_USE_VULKAN
    ECGPUBackEnd_VULKAN,
#endif
#ifdef CGPU_USE_D3D12
    ECGPUBackEnd_D3D12,
#endif
    ECGPUBackEnd_COUNT
} ECGPUBackEnd;

typedef struct CGpuAdapterDetail
{
    uint32_t deviceId;
    uint32_t vendorId;
    const char* name;
    const char* driverDescription;
    ECGPUBackEnd backend;
} CGpuAdapterDetail;

typedef struct CGpuAdapter 
{
    const struct CGpuInstance* instance;
} CGpuAdapter;
typedef CGpuAdapter* CGpuAdapterId;
typedef struct CGpuDevice {const char* label;} CGpuDevice;
typedef CGpuDevice* CGpuDeviceId;
typedef struct CGpuBuffer {const char* label;} CGpuBuffer;
typedef CGpuBuffer* CGpuBufferId;

typedef struct CGpuInstanceDescriptor
{
    ECGPUBackEnd backend;
} CGpuInstanceDescriptor;
// CGpuInstance is defined in cgpu_instnace.h
typedef struct CGpuInstance* CGpuInstanceId;

CGpuInstanceId cgpu_create_instance(const CGpuInstanceDescriptor* desc);
typedef CGpuInstanceId (*CGPUProcCreateInstance)(CGpuInstanceDescriptor const* descriptor);

void cgpu_destroy_instance(CGpuInstanceId instance);
typedef void (*CGPUProcDestroyInstance)(CGpuInstanceId instance);

void cgpu_enum_adapters(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);
typedef void (*CGPUProcEnumAdapters)(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);

CGpuAdapterId cgpu_get_default_adapter(CGpuInstanceId instance);
typedef CGpuAdapterId (*CGPUProcGetDefaultAdapter)(CGpuInstanceId instance);

CGpuAdapterDetail cgpu_query_adapter_detail(const CGpuAdapterId adapter);
typedef CGpuAdapterDetail (*CGPUProcQueryAdapterDetail)(const CGpuAdapterId instance);

typedef struct CGpuProcTable 
{
    CGPUProcCreateInstance create_instance;
    CGPUProcDestroyInstance destroy_instance;
    CGPUProcEnumAdapters enum_adapters;
    CGPUProcGetDefaultAdapter get_default_adapter;
    CGPUProcQueryAdapterDetail query_adapter_detail;
} CGpuProcTable;

typedef struct CGpuInstance
{
    const CGpuProcTable* proc_table;
} CGpuInstance;


#ifdef __cplusplus
} // end extern "C"
#endif
