#pragma once
#include "cgpu_config.h"
#include "stdbool.h"
#define CGPU_ARRAY_LEN(array) ((sizeof(array) / sizeof(array[0])))

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
    ECGPUBackEnd_WEBGPU = 0,
#endif
#ifdef CGPU_USE_VULKAN
    ECGPUBackEnd_VULKAN = 1,
#endif
#ifdef CGPU_USE_D3D12
    ECGPUBackEnd_D3D12 = 2,
#ifdef XBOX
    ECGPUBackEnd_XBOX_D3D12 = 3,
#endif
#endif
#ifdef CGPU_USE_AGC
    ECGPUBackEnd_AGC = 4,
#endif
#ifdef CGPU_USE_METAL
    ECGPUBackEnd_METAL = 5,
#endif
    ECGPUBackEnd_COUNT
} ECGPUBackEnd;

typedef struct CGpuInstanceDescriptor {
    ECGPUBackEnd backend;
    bool         enableDebugLayer;
    bool         enableGpuBasedValidation;
} CGpuInstanceDescriptor;

typedef struct CGpuAdapterDetail {
    uint32_t deviceId;
    uint32_t vendorId;
    const char* name;
} CGpuAdapterDetail;

typedef uint32_t CGpuQueueIndex;
typedef enum ECGpuQueueType{
    ECGpuQueueType_Graphics = 0,
    ECGpuQueueType_Compute = 1,
    ECGpuQueueType_Transfer = 2,
    ECGpuQueueType_Count
} ECGpuQueueType;

typedef struct CGpuQueueGroupDescriptor {
    ECGpuQueueType queueType;
    uint32_t queueCount;
} CGpuQueueGroupDescriptor;

typedef struct CGpuDeviceDescriptor {
    CGpuQueueGroupDescriptor* queueGroups;
    uint32_t                  queueGroupCount;
} CGpuDeviceDescriptor;

typedef struct CGpuInstance* CGpuInstanceId;
typedef struct CGpuAdapter {
    const struct CGpuInstance* instance;
} CGpuAdapter;
typedef CGpuAdapter* CGpuAdapterId;

typedef struct CGpuDevice {
    const CGpuAdapterId adapter;
#ifdef __cplusplus
    CGpuDevice() :adapter(CGPU_NULLPTR) {}
#endif
} CGpuDevice;
typedef CGpuDevice* CGpuDeviceId;

typedef struct CGpuQueue {
    CGpuDeviceId   device;    
    ECGpuQueueType type; 
    CGpuQueueIndex index;
} CGpuQueue;
typedef CGpuQueue* CGpuQueueId;

typedef struct CGpuCommandPool {
    CGpuQueueId queue;
} CGpuCommandPool;
typedef CGpuCommandPool* CGpuCommandPoolId;

typedef struct CGpuCommandPoolDescriptor {
    bool transient;
} CGpuCommandPoolDescriptor;

CGPU_API CGpuInstanceId cgpu_create_instance(const CGpuInstanceDescriptor* desc);
typedef CGpuInstanceId (*CGPUProcCreateInstance)(const CGpuInstanceDescriptor * descriptor);
CGPU_API void cgpu_free_instance(CGpuInstanceId instance);
typedef void (*CGPUProcFreeInstance)(CGpuInstanceId instance);

CGPU_API void cgpu_enum_adapters(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);
typedef void (*CGPUProcEnumAdapters)(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);

CGPU_API CGpuAdapterDetail cgpu_query_adapter_detail(const CGpuAdapterId adapter);
typedef CGpuAdapterDetail (*CGPUProcQueryAdapterDetail)(const CGpuAdapterId instance);
CGPU_API uint32_t cgpu_query_queue_count(const CGpuAdapterId adapter, const ECGpuQueueType type);
typedef uint32_t (*CGPUProcQueryQueueCount)(const CGpuAdapterId adapter, const ECGpuQueueType type);

CGPU_API CGpuDeviceId cgpu_create_device(CGpuAdapterId adapter, const CGpuDeviceDescriptor* desc);
typedef CGpuDeviceId (*CGPUProcCreateDevice)(CGpuAdapterId adapter, const CGpuDeviceDescriptor* desc);
CGPU_API void cgpu_free_device(CGpuDeviceId device);
typedef void (*CGPUProcFreeDevice)(CGpuDeviceId device);

CGPU_API CGpuQueueId cgpu_get_queue(CGpuDeviceId device, ECGpuQueueType type, uint32_t index);
typedef CGpuQueueId (*CGPUProcGetQueue)(CGpuDeviceId device, ECGpuQueueType type, uint32_t index);
CGPU_API void cgpu_free_queue(CGpuQueueId queue);
typedef void (*CGPUProcFreeQueue)(CGpuQueueId queue);

CGPU_API CGpuCommandPoolId cgpu_create_command_pool(CGpuQueueId queue, const CGpuCommandPoolDescriptor* desc);
typedef void (*CGPUProcCreateCommandPool)(CGpuQueueId queue, const CGpuCommandPoolDescriptor* desc);
CGPU_API void cgpu_free_command_pool(CGpuCommandPoolId pool);
typedef void (*CGPUProcFreeCommandPool)(CGpuCommandPoolId pool);



typedef struct CGpuBuffer {const CGpuDeviceId device;} CGpuBuffer;
typedef CGpuBuffer* CGpuBufferId;


typedef struct CGpuProcTable {
    CGPUProcCreateInstance     create_instance;
    CGPUProcFreeInstance    free_instance;
    CGPUProcEnumAdapters       enum_adapters;
    CGPUProcQueryAdapterDetail query_adapter_detail;
    CGPUProcQueryQueueCount    query_queue_count;
    CGPUProcCreateDevice       create_device;
    CGPUProcFreeDevice      free_device;
    CGPUProcGetQueue           get_queue;
    CGPUProcFreeQueue          free_queue;
} CGpuProcTable;

typedef struct CGpuInstance {
    const CGpuProcTable* proc_table;
} CGpuInstance;

#ifdef __cplusplus
} // end extern "C"
#endif
