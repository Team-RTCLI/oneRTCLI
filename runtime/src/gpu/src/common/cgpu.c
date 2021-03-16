#include "cgpu/api.h"
#ifdef CGPU_USE_WEBGPU
#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#include "cgpu/backend/webgpu/cgpu_webgpu_surfaces.h"
#include "cgpu/extensions/cgpu_webgpu_exts.h"
#endif
#ifdef CGPU_USE_VULKAN
#include "cgpu/backend/vulkan/cgpu_vulkan.h"
#include "cgpu/backend/vulkan/cgpu_vulkan_surfaces.h"
#endif
#ifdef CGPU_USE_D3D12
#include "cgpu/backend/d3d12/cgpu_d3d12.h"
#include "cgpu/backend/d3d12/cgpu_d3d12_surfaces.h"
#endif

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #ifdef TARGET_OS_MAC
        #define _MACOS
    #endif
#elif defined _WIN32 || defined _WIN64
#endif 
#include "assert.h"

CGpuInstanceId cgpu_create_instance(const CGpuInstanceDescriptor* desc)
{
    // case webgpu
    assert((desc->backend == ECGPUBackEnd_WEBGPU || desc->backend == ECGPUBackEnd_VULKAN
          || desc->backend == ECGPUBackEnd_D3D12) 
        && "cgpu support only webgpu & vulkan & d3d12 currently!");
    const CGpuProcTable* tbl = CGPU_NULLPTR;
    const CGpuSurfacesProcTable* s_tbl = CGPU_NULLPTR;

    if (desc->backend == ECGPUBackEnd_COUNT)
    {

    }
#ifdef CGPU_USE_WEBGPU
    else if(desc->backend == ECGPUBackEnd_WEBGPU)
    {
#ifdef DAWN_ENABLE_BACKEND_D3D12 
        WGPUBackendType type = WGPUBackendType_D3D12;
#elif defined(DAWN_ENABLE_BACKEND_VULKAN)
        WGPUBackendType type = WGPUBackendType_Vulkan;
#elif defined(DAWN_ENABLE_BACKEND_METAL)
        WGPUBackendType type = WGPUBackendType_Metal;
#endif
        tbl = CGPU_WebGPUProcTable(type);
        s_tbl = CGPU_WebGPUSurfacesProcTable();
    } 
#endif
#ifdef CGPU_USE_VULKAN
    else if (desc->backend == ECGPUBackEnd_VULKAN) {
        tbl = CGPU_VulkanProcTable();
        s_tbl = CGPU_VulkanSurfacesProcTable();
    } 
#endif
#ifdef CGPU_USE_D3D12
    else if (desc->backend == ECGPUBackEnd_D3D12) {
        tbl = CGPU_D3D12ProcTable();
        s_tbl = CGPU_D3D12SurfacesProcTable();
    }
#endif
    CGpuInstanceId instance = tbl->create_instance(desc);
    instance->proc_table = tbl;
    instance->surfaces_table = s_tbl;
    return instance;
}

void cgpu_free_instance(CGpuInstanceId instance)
{
    assert(instance != CGPU_NULLPTR && "fatal: can't destroy NULL instance!");
    assert(instance->proc_table->enum_adapters && "free_instance Proc Missing!");

    instance->proc_table->free_instance(instance);
}

void cgpu_enum_adapters(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num)
{
    assert(instance != CGPU_NULLPTR && "fatal: can't destroy NULL instance!");
    assert(instance->proc_table->enum_adapters && "enum_adapters Proc Missing!");

    instance->proc_table->enum_adapters(instance, adapters, adapters_num);
}

CGpuAdapterDetail cgpu_query_adapter_detail(const CGpuAdapterId adapter)
{
    assert(adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(adapter->instance->proc_table->query_adapter_detail && "query_adapter_detail Proc Missing!");

    return adapter->instance->proc_table->query_adapter_detail(adapter);
}

uint32_t cgpu_query_queue_count(const CGpuAdapterId adapter, const ECGpuQueueType type)
{
    assert(adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(adapter->instance->proc_table->query_queue_count && "query_queue_count Proc Missing!");
    
    return adapter->instance->proc_table->query_queue_count(adapter, type);
}

CGpuDeviceId cgpu_create_device(CGpuAdapterId adapter, const CGpuDeviceDescriptor* desc)
{
    assert(adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(adapter->instance->proc_table->create_device && "create_device Proc Missing!");

    return adapter->instance->proc_table->create_device(adapter, desc);
}

void cgpu_free_device(CGpuDeviceId device)
{
    assert(device != CGPU_NULLPTR && "fatal: call on NULL device!");
    assert(device->adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(device->adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(device->adapter->instance->proc_table->free_device && "free_device Proc Missing!");

    return device->adapter->instance->proc_table->free_device(device);
}

CGpuQueueId cgpu_get_queue(CGpuDeviceId device, ECGpuQueueType type, uint32_t index)
{
    assert(device != CGPU_NULLPTR && "fatal: call on NULL device!");
    assert(device->adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(device->adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(device->adapter->instance->proc_table->free_device && "free_device Proc Missing!");

    CGpuQueueId queue = device->adapter->instance->proc_table->get_queue(device, type, index);
    queue->index = index; queue->type = type; queue->device = device;
    return queue;
}

void cgpu_free_queue(CGpuQueueId queue)
{
    assert(queue != CGPU_NULLPTR && "fatal: call on NULL queue!");
    assert(queue->device != CGPU_NULLPTR && "fatal: call on NULL device!");
    assert(queue->device->adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(queue->device->adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(queue->device->adapter->instance->proc_table->free_device && "free_device Proc Missing!");

    return queue->device->adapter->instance->proc_table->free_queue(queue);
}

CGPU_API CGpuCommandEncoderId cgpu_create_command_encoder(CGpuQueueId queue,
    const CGpuCommandEncoderDescriptor* desc)
{
    assert(queue != CGPU_NULLPTR && "fatal: call on NULL queue!");
    assert(queue->device != CGPU_NULLPTR && "fatal: call on NULL device!");
    assert(queue->device->adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(queue->device->adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(queue->device->adapter->instance->proc_table->free_device && "free_device Proc Missing!");

    CGpuCommandEncoderId encoder = queue->device->adapter->instance->proc_table->create_command_encoder(queue, desc);
    encoder->queue = queue;
    return encoder;
}

CGPU_API void cgpu_free_command_encoder(CGpuCommandEncoderId encoder)
{
    assert(encoder != CGPU_NULLPTR && "fatal: call on NULL encoder!");
    assert(encoder->queue != CGPU_NULLPTR && "fatal: call on NULL queue!");
    assert(encoder->queue->device != CGPU_NULLPTR && "fatal: call on NULL device!");
    assert(encoder->queue->device->adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(encoder->queue->device->adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(encoder->queue->device->adapter->instance->proc_table->free_device && "free_device Proc Missing!");

    return encoder->queue->device->adapter->instance->proc_table->free_command_encoder(encoder);
}


// surfaces
CGpuSurfaceId cgpu_surface_from_hwnd(CGpuDeviceId device, HWND window)
{
    assert(device != CGPU_NULLPTR && "fatal: call on NULL device!");
    assert(device->adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(device->adapter->instance != CGPU_NULLPTR && "fatal: call on NULL instnace!");
    assert(device->adapter->instance->surfaces_table != CGPU_NULLPTR && "surfaces_table Missing!");
    assert(device->adapter->instance->surfaces_table->from_hwnd != CGPU_NULLPTR && "free_instance Proc Missing!");

    return device->adapter->instance->surfaces_table->from_hwnd(device, window);
}

void cgpu_free_surface(CGpuDeviceId device, CGpuSurfaceId surface)
{
    assert(device != CGPU_NULLPTR && "fatal: call on NULL device!");
    assert(device->adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(device->adapter->instance != CGPU_NULLPTR && "fatal: call on NULL instnace!");
    assert(device->adapter->instance->surfaces_table != CGPU_NULLPTR && "surfaces_table Missing!");
    assert(device->adapter->instance->surfaces_table->from_hwnd != CGPU_NULLPTR && "free_instance Proc Missing!");

    return device->adapter->instance->surfaces_table->cgpu_free_surface(device, surface);
}
//