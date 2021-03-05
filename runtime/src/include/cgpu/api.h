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

typedef struct CGpuAdapter 
{
    const char* label;
    const struct CGpuInstance* instance;
} CGpuAdapter;
typedef CGpuAdapter* CGpuAdapterId;
typedef struct CGpuDevice {const char* label;} CGpuDevice;
typedef CGpuDevice* CGpuDeviceId;
typedef struct CGpuBuffer {const char* label;} CGpuBuffer;
typedef CGpuBuffer* CGpuBufferId;

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

typedef struct CGpuInstanceDescriptor
{
    ECGPUBackEnd backend;
} CGpuInstanceDescriptor;
// CGpuInstance is defined in cgpu_instnace.h
typedef struct CGpuInstance* CGpuInstanceId;

CGpuInstanceId cgpu_create_instance(const CGpuInstanceDescriptor* desc);
void cgpu_destroy_instance(CGpuInstanceId instance);
void cgpu_enum_adapters(CGpuInstanceId instance, const CGpuAdapterId* adapters, size_t* adapters_num);
void cgpu_drop_adapter(CGpuAdapterId adapter);
CGpuAdapterId cgpu_get_default_adapter(CGpuInstanceId instance);

#ifdef __cplusplus
} // end extern "C"
#endif
