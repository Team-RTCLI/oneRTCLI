#include "cgpu/api.h"
#ifdef CGPU_USE_WEBGPU

#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#include <cassert>
#include <memory>
#include "cgpu/backend/webgpu/bridge.h"

// implementations
CGpuInstanceId cgpu_create_instance_webgpu(CGpuInstanceDescriptor const* descriptor);
void cgpu_destroy_instance_webgpu(CGpuInstanceId instance);
void cgpu_enum_adapters_webgpu(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);
void cgpu_drop_adapter_webgpu(CGpuAdapterId adapter);
CGpuAdapterId cgpu_get_default_adapter_webgpu(CGpuInstanceId adapter);
CGpuAdapterDetail cgpu_query_adapter_detail_webgpu(const CGpuAdapterId adapter);

const CGpuProcTable tbl = 
{
    .create_instance = &cgpu_create_instance_webgpu,
    .destroy_instance = &cgpu_destroy_instance_webgpu,
    .enum_adapters = &cgpu_enum_adapters_webgpu,
    .get_default_adapter = &cgpu_get_default_adapter_webgpu,
    .query_adapter_detail = &cgpu_query_adapter_detail_webgpu
};

const CGpuProcTable* CGPU_WebGPUProcTable(const WGPUBackendType t)
{
    return &tbl;
}

#ifdef __EMSCRIPTEN__

#else 
// dawn implementation
#include "dawn_platform/dawn_platform_export.h"
#include "dawn/webgpu_cpp.h"
#include "dawn_native/DawnNative.h"
#include "dawn/dawn_proc.h"
#include "dawn/dawn_proc_table.h"
class GpuAdapterDawn : public CGpuAdapter_WebGpu
{
public:
    dawn_native::Adapter dawn_native_adapter;
    wgpu::AdapterProperties properties;
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

            dawn_native_adapters[i].GetProperties(&dawn_adapters[i].properties);
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
    delete dawn_instance;
}

void cgpu_enum_adapters_webgpu(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num)
{
    assert(instance != nullptr && "fatal: null instance!");
    GpuInstanceDawn* wgpuInstance = (GpuInstanceDawn*)instance;
    *adapters_num = wgpuInstance->dawn_adapters.size();
    if(!adapters)
    {
        return;
    } else {
        for(auto i = 0u; i < *adapters_num; i++)
            adapters[i] = &(wgpuInstance->dawn_adapters[i].super);
    }
}

CGpuAdapterId cgpu_get_default_adapter_webgpu(CGpuInstanceId adapter)
{
    return CGPU_NULLPTR;
}

CGpuAdapterDetail cgpu_query_adapter_detail_webgpu(const CGpuAdapterId adapter)
{
    auto a = (GpuAdapterDawn*)adapter;
    CGpuAdapterDetail d = {};
    d.deviceId = a->properties.deviceID;
    d.vendorId = a->properties.vendorID;
    d.driverDescription = a->properties.driverDescription;
    d.name = a->properties.name;
    d.backend = ECGPUBackEnd_WEBGPU;
    return d;
}

WGPUBackendType cgpu_webgpu_query_backend(CGpuAdapterId adapter)
{
    auto a = (GpuAdapterDawn*)adapter;
    switch (a->properties.backendType)
    {
        case wgpu::BackendType::D3D11: return WGPUBackendType_D3D11;
        case wgpu::BackendType::D3D12: return WGPUBackendType_D3D12;
        case wgpu::BackendType::Metal: return WGPUBackendType_Metal;
        case wgpu::BackendType::Vulkan: return WGPUBackendType_Vulkan;
        case wgpu::BackendType::OpenGL: return WGPUBackendType_OpenGL;
        case wgpu::BackendType::OpenGLES: return WGPUBackendType_OpenGLES;
        default: assert(0 && "WGPU ERROR Backend!");
    }
    return WGPUBackendType_Null;
}
#endif // end #ifdef __EMSCRIPTEN__


// common implementations






#endif // end #ifdef CGPU_USE_WEBGPU
