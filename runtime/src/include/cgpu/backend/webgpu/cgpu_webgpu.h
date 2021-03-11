#include "cgpu/api.h"

#ifdef __EMSCRIPTEN__
#else
#include "dawn/webgpu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

const CGpuProcTable* CGPU_WebGPUProcTable(const enum WGPUBackendType t);

// implementations
CGpuInstanceId cgpu_create_instance_webgpu(CGpuInstanceDescriptor const* descriptor);
void cgpu_destroy_instance_webgpu(CGpuInstanceId instance);
void cgpu_enum_adapters_webgpu(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);
CGpuAdapterDetail cgpu_query_adapter_detail_webgpu(const CGpuAdapterId adapter);
uint32_t cgpu_query_queue_count_webgpu(const CGpuAdapterId adapter, const ECGpuQueueType type);
CGpuDeviceId cgpu_create_device_webgpu(CGpuAdapterId adapter, const CGpuDeviceDescriptor* desc);
void cgpu_destroy_device_webgpu(CGpuDeviceId device);

typedef struct CGpuInstance_WebGpu {
    CGpuInstance super;
    WGPUBackendType backend;
} CGpuInstance_WebGpu;

typedef struct CGpuAdapter_WebGpu {
    CGpuAdapter super;
} CGpuAdapter_WebGpu;

typedef struct CGpuDevice_WebGpu {
    CGpuDevice super;
    WGPUDevice pWGPUDevice;
#ifdef __cplusplus
    virtual ~CGpuDevice_WebGpu() {}
#endif
} CGpuDevice_WebGpu;

#ifdef __cplusplus
} // end extern "C"
#endif