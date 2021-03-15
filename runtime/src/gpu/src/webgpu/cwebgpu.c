#define DLL_IMPLEMENTATION
#include <stdlib.h>
#include "cgpu/api.h"
#ifdef CGPU_USE_WEBGPU
#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#include <assert.h>
#include "cgpu/backend/webgpu/bridge.h"

const CGpuProcTable tbl_webgpu = 
{
    .create_instance = &cgpu_create_instance_webgpu,
    .free_instance = &cgpu_free_instance_webgpu,
    .enum_adapters = &cgpu_enum_adapters_webgpu,
    .query_adapter_detail = &cgpu_query_adapter_detail_webgpu,
    .query_queue_count = &cgpu_query_queue_count_webgpu,
    .create_device = &cgpu_create_device_webgpu,
    .free_device = &cgpu_free_device_webgpu,
    .get_queue = &cgpu_get_queue_webgpu,
    .free_queue = &cgpu_free_queue_webgpu,
    .create_command_encoder = &cgpu_create_command_encoder_webgpu,
    .free_command_encoder = &cgpu_free_command_encoder_webgpu
};

const CGpuProcTable* CGPU_WebGPUProcTable(const WGPUBackendType t)
{
    return &tbl_webgpu;
}


// common implementations

uint32_t cgpu_query_queue_count_webgpu(const CGpuAdapterId adapter, const ECGpuQueueType type)
{
    switch (type)
    {
        // WGPU Supports only main-queue now.
        case ECGpuQueueType_Graphics: return 1;
        case ECGpuQueueType_Compute:  return 0;
        case ECGpuQueueType_Transfer: return 0;
        default: assert(0 && "WGPU ERROR QueueType!");
    }
    return UINT32_MAX;
}

CGpuQueueId cgpu_get_queue_webgpu(CGpuDeviceId device, ECGpuQueueType type, uint32_t index)
{
    CGpuDevice_WebGpu* wgpuDevice = (CGpuDevice_WebGpu*)device;
    CGpuQueue_WebGpu* wgpuQueue = (CGpuQueue_WebGpu*)malloc(sizeof(CGpuQueue_WebGpu));
    wgpuQueue->pWGPUQueue = wgpuDeviceGetDefaultQueue(wgpuDevice->pWGPUDevice);
    return &wgpuQueue->super;
}

void cgpu_free_queue_webgpu(CGpuQueueId queue)
{
    free(queue);
}

CGpuCommandEncoderId cgpu_create_command_encoder_webgpu(CGpuQueueId queue, const CGpuCommandEncoderDescriptor* desc)
{
    assert(queue && "WGPU ERROR: NULL QUEUE!");
    
    CGpuCommandEncoder_WebGpu* E = (CGpuCommandEncoder_WebGpu*)malloc(sizeof(CGpuCommandEncoder_WebGpu));
    //CGpuQueue_WebGpu* Q = (CGpuQueue_WebGpu*)queue;
    CGpuDevice_WebGpu* D = (CGpuDevice_WebGpu*)queue->device;
    E->pWGPUEncoder = wgpuDeviceCreateCommandEncoder(D->pWGPUDevice, CGPU_NULLPTR);
    return &E->super;
}

void cgpu_free_command_encoder_webgpu(CGpuCommandEncoderId encoder)
{
    CGpuCommandEncoder_WebGpu* E = (CGpuCommandEncoder_WebGpu*)encoder;
    assert(encoder && "WGPU ERROR: NULL ENCODER!");
    assert(E->pWGPUEncoder && "WGPU ERROR: NULL WGPU ENCODER!");

    wgpuCommandEncoderRelease(E->pWGPUEncoder);
}


#endif // end #ifdef CGPU_USE_WEBGPU