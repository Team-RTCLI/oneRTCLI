#include "cgpu/api.h"

#ifdef __EMSCRIPTEN__
#else
#include "dawn/webgpu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// api extentions.
WGPUBackendType cgpu_webgpu_query_backend(CGpuAdapterId adapter);

#ifdef __cplusplus
} // end extern "C"
#endif