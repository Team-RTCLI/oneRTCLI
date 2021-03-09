#pragma once
#include "cgpu/api.h"
#include "stdbool.h"
#include "vulkan/vulkan_core.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "vulkan/vulkan_win32.h"
#endif

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
	/// Flag to specify whether to request all queues from the gpu or just one of each type
	/// This will affect memory usage - Around 200 MB more used if all queues are requested
	bool                         mRequestAllAvailableQueues;
	const VkDebugUtilsMessengerCreateInfoEXT* pDebugUtilsMessenger;
} CGpuVulkanInstanceDescriptor;

// api extentions.
CGpuInstanceId cgpu_vulkan_create_instance(CGpuVulkanInstanceDescriptor const* descriptor);

#ifdef __cplusplus
} // end extern "C"
#endif