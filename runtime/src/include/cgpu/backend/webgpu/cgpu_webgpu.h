#include "cgpu/api.h"
#include "cgpu/cgpu_instance.h"

#ifdef CGPU_USE_WEBGPU
#include "dawn/webgpu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

const CGpuProcTable* CGPU_WebGPUProcTable(const WGPUBackendType t);

// implementations
CGpuInstanceId cgpu_create_instance_webgpu(CGpuInstanceDescriptor const* descriptor);
void cgpu_destroy_instance_webgpu(CGpuInstanceId instance);
void cgpu_enum_adapters_webgpu(CGpuInstanceId instance, const CGpuAdapterId* adapters, size_t* adapters_num);
void cgpu_drop_adapter_webgpu(CGpuAdapterId adapter);
CGpuAdapterId cgpu_get_default_adapter_webgpu(CGpuInstanceId adapter);

typedef struct CGpuInstance_WebGpu {
    CGpuInstance super;
    WGPUBackendType backend;
} CGpuInstance_WebGpu;

typedef struct CGpuAdapter_WebGpu {
    CGpuAdapter super;
    WGPUBackendType backend;
} CGpuAdapter_WebGpu;

#ifdef __cplusplus
} // end extern "C"
#endif