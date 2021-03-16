#pragma once
#include "cgpu/api.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#ifdef __cplusplus
extern "C" {
#endif

const CGpuSurfacesProcTable* CGPU_D3D12SurfacesProcTable();

void cgpu_free_surface_d3d12(CGpuDeviceId device, CGpuSurfaceId surface);

#if defined(_WIN32) || defined(_WIN64)
CGpuSurfaceId cgpu_surface_from_hwnd_d3d12(CGpuDeviceId device, HWND window);
#endif

#ifdef __cplusplus
} // end extern "C"
#endif