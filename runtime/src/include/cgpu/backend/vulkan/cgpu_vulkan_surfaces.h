#pragma once
#include "cgpu/backend/vulkan/cgpu_vulkan.h"

#ifdef __cplusplus
extern "C" {
#endif

const CGpuSurfacesProcTable* CGPU_VulkanSurfacesProcTable();

void cgpu_surface_free_vulkan(CGpuInstanceId instance, CGpuSurfaceId surface);

#if defined(_WIN32) || defined(_WIN64)
CGpuSurfaceId cgpu_surface_from_hwnd_vulkan(CGpuInstanceId instance, HWND window);
#endif

#ifdef __cplusplus
} // end extern "C"
#endif