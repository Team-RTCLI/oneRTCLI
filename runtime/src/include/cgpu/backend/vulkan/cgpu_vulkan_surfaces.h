#pragma once
#include "cgpu/api.h"
#include "volk.h"

#ifdef __cplusplus
extern "C" {
#endif

const CGpuSurfacesProcTable* CGPU_VulkanSurfacesProcTable();


void cgpu_surface_free_vulkan(CGpuSurfaceId surface);

#if defined(_WIN32) || defined(_WIN64)
CGpuSurfaceId cgpu_surface_from_hwnd_vulkan(HWND window);
#endif

#ifdef __cplusplus
} // end extern "C"
#endif