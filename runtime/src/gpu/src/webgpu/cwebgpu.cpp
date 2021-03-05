#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#ifdef CGPU_USE_WEBGPU
#include "dawn_platform/dawn_platform_export.h"
#include "dawn/webgpu_cpp.h"
#include "dawn_native/DawnNative.h"
#include "dawn/dawn_proc.h"
#include "dawn/dawn_proc_table.h"
#endif
#include <cassert>
#include <memory>

class GpuAdapterDawn : public CGpuAdapter_WebGpu
{
public:
    dawn_native::Adapter dawn_native_adapter;
};

class GpuInstanceDawn : public CGpuInstance_WebGpu
{
public:
    GpuInstanceDawn()
    {
#ifdef _MACOS 
        WGPUBackendType type = WGPUBackendType_Metal;
#else
        WGPUBackendType type = WGPUBackendType_Vulkan;
#endif
        dawn_instance = std::make_unique<dawn_native::Instance>();
        dawn_instance->DiscoverDefaultAdapters();
        auto dawn_native_adapters = dawn_instance->GetAdapters();
        dawn_adapters.resize(dawn_native_adapters.size());
        for(auto i = 0u; i < dawn_adapters.size(); i++)
        {
            dawn_adapters[i].super.instance = &this->super;
            dawn_adapters[i].dawn_native_adapter = dawn_native_adapters[i];
            dawn_adapters[i].backend = type;
        }
        backend = type;
    }

    std::vector<GpuAdapterDawn> dawn_adapters;
    std::unique_ptr<dawn_native::Instance> dawn_instance;
};

CGpuInstanceId cgpu_create_instance_webgpu(CGpuInstanceDescriptor const* descriptor)
{
    GpuInstanceDawn* dawn_instance = new GpuInstanceDawn();
    return &dawn_instance->super;
}

void cgpu_destroy_instance_webgpu(CGpuInstanceId instance)
{
    GpuInstanceDawn* dawn_instance = (GpuInstanceDawn*)instance;
    delete instance;
}

void cgpu_enum_adapters_webgpu(CGpuInstanceId instance, const CGpuAdapterId* adapters, size_t* adapters_num)
{
    assert(instance != nullptr && "fatal: null instance!");
    GpuInstanceDawn* wgpuInstance = (GpuInstanceDawn*)instance;
    if(!adapters)
    {
        *adapters_num = wgpuInstance->dawn_adapters.size();
        return;
    } else
    {

    }

}

void cgpu_drop_adapter_webgpu(CGpuAdapterId adapter)
{

}

CGpuAdapterId cgpu_get_default_adapter_webgpu(CGpuInstanceId adapter)
{

}