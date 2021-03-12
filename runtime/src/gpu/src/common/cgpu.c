#include "cgpu/api.h"
#ifdef CGPU_USE_WEBGPU
#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#include "cgpu/backend/webgpu/cgpu_webgpu_exts.h"
#endif
#ifdef CGPU_USE_VULKAN
#include "cgpu/backend/vulkan/cgpu_vulkan.h"
#endif
#ifdef CGPU_USE_D3D12
#include "cgpu/backend/d3d12/cgpu_d3d12.h"
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
    if(desc->backend == ECGPUBackEnd_WEBGPU)
    {
#ifdef _MACOS 
        WGPUBackendType type = WGPUBackendType_Metal;
#else
        WGPUBackendType type = WGPUBackendType_Vulkan;
#endif
        tbl = CGPU_WebGPUProcTable(type);

    } else if (desc->backend == ECGPUBackEnd_VULKAN) {
        tbl = CGPU_VulkanProcTable();
    } else if (desc->backend == ECGPUBackEnd_D3D12) {
        tbl = CGPU_D3D12ProcTable();
    }
    CGpuInstanceId instance = tbl->create_instance(desc);
    instance->proc_table = tbl;
    return instance;
}

void cgpu_destroy_instance(CGpuInstanceId instance)
{
    assert(instance != CGPU_NULLPTR && "fatal: can't destroy NULL instance!");
    assert(instance->proc_table->enum_adapters && "destroy_instance Proc Missing!");

    instance->proc_table->destroy_instance(instance);
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

void cgpu_destroy_device(CGpuDeviceId device)
{
    assert(device != CGPU_NULLPTR && "fatal: call on NULL device!");
    assert(device->adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(device->adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(device->adapter->instance->proc_table->destroy_device && "destroy_device Proc Missing!");

    return device->adapter->instance->proc_table->destroy_device(device);
}

CGpuQueueId cgpu_get_queue(CGpuDeviceId device, ECGpuQueueType type, uint32_t index)
{
    assert(device != CGPU_NULLPTR && "fatal: call on NULL device!");
    assert(device->adapter != CGPU_NULLPTR && "fatal: call on NULL adapter!");
    assert(device->adapter->instance != CGPU_NULLPTR && "fatal: Missing instance of adapter!");
    assert(device->adapter->instance->proc_table->destroy_device && "destroy_device Proc Missing!");

    return device->adapter->instance->proc_table->get_queue(device, type, index);
}