#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#ifdef CGPU_USE_WEBGPU
#include "dawn_platform/dawn_platform_export.h"
#include "dawn/webgpu_cpp.h"
#include "dawn_native/DawnNative.h"
#include "dawn/dawn_proc.h"
#include "dawn/dawn_proc_table.h"
#endif
#include <cassert>

CGpuInstanceId cgpu_create_instance_webgpu(CGpuInstanceDescriptor const* descriptor)
{
    dawn_native::Instance* d_instance = new dawn_native::Instance();
    d_instance->DiscoverDefaultAdapters();
    
    auto instance = (CGpuInstance_WebGpu*)malloc(sizeof(CGpuInstance_WebGpu));
    assert(instance != nullptr && "malloc CGpuIntance_WebGpu failed!");
	instance->native_instance = (NativeInstanceId)d_instance;
#ifdef _MACOS 
    WGPUBackendType type = WGPUBackendType_Metal;
#else
    WGPUBackendType type = WGPUBackendType_Vulkan;
#endif
    instance->backend = type;
    return (CGpuInstanceId)instance;
}

void cgpu_destroy_instance_webgpu(CGpuInstanceId instance)
{
    assert(instance != nullptr && "fatal: can't destroy empty instance!");
    CGpuInstance_WebGpu* wgpuInstance = (CGpuInstance_WebGpu*)instance;
    dawn_native::Instance* nativeInstance = (dawn_native::Instance*)wgpuInstance->native_instance;
    
    delete nativeInstance;
    free(wgpuInstance);
}


void cgpu_enum_adapters_webgpu(CGpuInstanceId instance, const CGpuAdapter* adapters, size_t* adapters_num)
{
    
}