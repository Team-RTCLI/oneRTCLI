#include "cgpu/api.h"
#ifdef CGPU_USE_WEBGPU
#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#include "cgpu/backend/webgpu/cgpu_exts.h"
#endif
#ifdef CGPU_USE_VULKAN
#include "cgpu/backend/vulkan/cgpu_vulkan.h"
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
    assert((desc->backend == ECGPUBackEnd_WEBGPU || desc->backend == ECGPUBackEnd_VULKAN) 
        && "cgpu support only webgpu & vulkan currently!");
    const CGpuProcTable* tbl = CGPU_NULLPTR;
    if(desc->backend == ECGPUBackEnd_WEBGPU)
    {
#ifdef _MACOS 
        WGPUBackendType type = WGPUBackendType_Metal;
#else
        WGPUBackendType type = WGPUBackendType_Vulkan;
#endif
        tbl = CGPU_WebGPUProcTable(type);

    } else if (desc->backend == ECGPUBackEnd_VULKAN)
    {
        tbl = CGPU_VulkanProcTable();
    }
    CGpuInstanceId instance = tbl->create_instance(desc);
    instance->proc_table = tbl;
    return instance;
}

void cgpu_destroy_instance(CGpuInstanceId instance)
{
    assert(instance != CGPU_NULLPTR && "fatal: can't destroy NULL instance!");
    instance->proc_table->destroy_instance(instance);
}

void cgpu_enum_adapters(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num)
{
    assert(instance != CGPU_NULLPTR && "fatal: can't destroy NULL instance!");

    instance->proc_table->enum_adapters(instance, adapters, adapters_num);
}

CGpuAdapterDetail cgpu_query_adapter_detail(const CGpuAdapterId adapter)
{
    return adapter->instance->proc_table->query_adapter_detail(adapter);
}