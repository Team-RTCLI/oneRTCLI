#include "cgpu/api.h"

#ifdef __EMSCRIPTEN__
#else
#include "dawn/webgpu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


void cgpu_free_surface_webgpu(CGpuDeviceId device, CGpuSurfaceId surface);

#if defined(_WIN32) || defined(_WIN64)
CGpuSurfaceId cgpu_surface_from_hwnd_webgpu(CGpuDeviceId device, HWND window);
#endif

#ifdef __cplusplus
} // end extern "C"
#endif