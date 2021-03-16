#define DLL_IMPLEMENTATION

#include "cgpu/api.h"
#ifdef CGPU_USE_WEBGPU

#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#include <cassert>
#include <memory>
#include "cgpu/backend/webgpu/bridge.h"

namespace impl
{
	/**
	 * Dawn error handling callback (adheres to \c WGPUErrorCallback).
	 *
	 * \param[in] message error string
	 */
	static void printError(WGPUErrorType type, const char* message, void*)
    {
		printf("CGPU Dawn ERROR: %s\n", message);
	}
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

struct GpuDeviceDawn : public CGpuDevice_WebGpu
{

};

class GpuInstanceDawn : public CGpuInstance_WebGpu
{
public:
    GpuInstanceDawn()
    {
#ifdef DAWN_ENABLE_BACKEND_D3D12 
        WGPUBackendType type = WGPUBackendType_D3D12;
#elif defined(DAWN_ENABLE_BACKEND_VULKAN)
        WGPUBackendType type = WGPUBackendType_Vulkan;
#elif defined(DAWN_ENABLE_BACKEND_METAL)
        WGPUBackendType type = WGPUBackendType_Metal;
#endif
        mDawnInstance = std::make_unique<dawn_native::Instance>();
        mDawnInstance->DiscoverDefaultAdapters();
        auto dawn_native_adapters = mDawnInstance->GetAdapters();
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
    std::unique_ptr<dawn_native::Instance> mDawnInstance;
};

CGpuInstanceId cgpu_create_instance_webgpu(CGpuInstanceDescriptor const* descriptor)
{
    GpuInstanceDawn* dawn_instance = new GpuInstanceDawn();
    return &dawn_instance->super;
}
 
void cgpu_free_instance_webgpu(CGpuInstanceId instance)
{
    GpuInstanceDawn* dawn_instance = (GpuInstanceDawn*)instance;
    delete dawn_instance;
}

void cgpu_enum_adapters_webgpu(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num)
{
    assert(instance != nullptr && "fatal: null instance!");
    GpuInstanceDawn* wgpuInstance = (GpuInstanceDawn*)instance;
    *adapters_num = wgpuInstance->dawn_adapters.size();
    if(!adapters) {
        return;
    } else {
        for(auto i = 0u; i < *adapters_num; i++)
            adapters[i] = &(wgpuInstance->dawn_adapters[i].super);
    }
}

CGpuAdapterDetail cgpu_query_adapter_detail_webgpu(const CGpuAdapterId adapter)
{
    auto a = (GpuAdapterDawn*)adapter;
    CGpuAdapterDetail d = {};
    d.deviceId = a->properties.deviceID;
    d.vendorId = a->properties.vendorID;
    d.name = a->properties.name;
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


CGpuDeviceId cgpu_create_device_webgpu(CGpuAdapterId adapter, const CGpuDeviceDescriptor* desc)
{
    auto a = (GpuAdapterDawn*)adapter;
    GpuDeviceDawn* created = new GpuDeviceDawn();
    if (a->dawn_native_adapter)
    {
        created->pWGPUDevice = a->dawn_native_adapter.CreateDevice();
        *const_cast<CGpuAdapterId*>(&created->super.adapter) = adapter;

        DawnProcTable procs(dawn_native::GetProcs());
		procs.deviceSetUncapturedErrorCallback(created->pWGPUDevice, impl::printError, nullptr);
		dawnProcSetProcs(&procs);
    }
    return &created->super;
}

void cgpu_free_device_webgpu(CGpuDeviceId device)
{
    CGpuDevice_WebGpu* toDestroy = (CGpuDevice_WebGpu*)device;
    delete toDestroy;
}

#endif // end #ifdef __EMSCRIPTEN__




#endif // end #ifdef CGPU_USE_WEBGPU
