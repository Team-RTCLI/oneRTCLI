#include "cgpu/api.h"

#ifdef __EMSCRIPTEN__
#else
#include "dawn/webgpu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

const CGpuProcTable* CGPU_WebGPUProcTable(const enum WGPUBackendType t);

typedef struct CGpuInstance_WebGpu {
    CGpuInstance super;
    WGPUBackendType backend;
} CGpuInstance_WebGpu;

typedef struct CGpuAdapter_WebGpu {
    CGpuAdapter super;
} CGpuAdapter_WebGpu;

#ifdef __cplusplus
} // end extern "C"
#endif