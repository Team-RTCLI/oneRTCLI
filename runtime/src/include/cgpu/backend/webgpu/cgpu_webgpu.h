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
void cgpu_enum_adapters_webgpu(CGpuInstanceId instance, const CGpuAdapter* adapters, size_t* adapters_num);

typedef struct NativeInstanceDummy
{

} NativeInstanceDummy;
typedef NativeInstanceDummy* NativeInstanceId;
typedef struct CGpuInstance_WebGpu {
    CGpuInstance super;
    WGPUBackendType backend;
    NativeInstanceId native_instance;
} CGpuInstance_WebGpu;

#ifdef __cplusplus
} // end extern "C"
#endif