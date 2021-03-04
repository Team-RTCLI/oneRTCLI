#include "cgpu/api.h"
#ifdef CGPU_USE_WEBGPU
#include "cgpu/backend/webgpu/cgpu_webgpu.h"
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
    assert(desc->backend == ECGPUBackEnd_WEBGPU && "cgpu support only webgpu currently!");
#ifdef _MACOS 
    WGPUBackendType type = WGPUBackendType_Metal;
#else
    WGPUBackendType type = WGPUBackendType_Vulkan;
#endif
    const CGpuProcTable* tbl = CGPU_WebGPUProcTable(type);
    CGpuInstanceId instance = tbl->create_instance(desc);
    instance->proc_table = tbl;
    
    return instance;
}

void cgpu_destroy_instance(CGpuInstanceId instance)
{
    assert(instance != CGPU_NULLPTR && "fatal: can't destroy NULL instance!");
    instance->proc_table->destroy_instance(instance);
}

CGpuAdapterId cgpu_request_adapter(CGpuInstanceId instance)
{

    
    return CGPU_NULLPTR;
}