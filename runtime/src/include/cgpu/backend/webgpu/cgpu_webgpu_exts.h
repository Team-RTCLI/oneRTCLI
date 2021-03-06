#include "cgpu/api.h"

#ifdef __EMSCRIPTEN__
#else
#include "dawn/webgpu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// api extentions.
CGpuInstanceId cgpu_webgpu_create_instance(CGpuInstanceDescriptor const* descriptor);
WGPUBackendType cgpu_webgpu_query_backend(CGpuAdapterId adapter);

#ifdef __cplusplus
} // end extern "C"
#endif