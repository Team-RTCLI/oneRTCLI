#pragma once
#include "cgpu/api.h"
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "volk.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CGpuVulkanInstanceDescriptor {
    const char**                 ppInstanceLayers;
	const char**                 ppInstanceExtensions;
	const char**                 ppDeviceExtensions;
	uint32_t                     mInstanceLayerCount;
	uint32_t                     mInstanceExtensionCount;
	uint32_t                     mDeviceExtensionCount;
	const VkDebugUtilsMessengerCreateInfoEXT* pDebugUtilsMessenger;
} CGpuVulkanInstanceDescriptor;

// api extentions.
CGPU_API CGpuInstanceId cgpu_vulkan_create_instance(CGpuInstanceDescriptor const* desc, CGpuVulkanInstanceDescriptor const* exts);

#ifdef __cplusplus
} // end extern "C"
#endif