#pragma once
#include "cgpu/api.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

const CGpuProcTable* CGPU_VulkanProcTable();

CGpuInstanceId cgpu_create_instance_vulkan(CGpuInstanceDescriptor const* descriptor);
void cgpu_destroy_instance_vulkan(CGpuInstanceId instance);

typedef struct CGpuInstance_Vulkan {
    CGpuInstance super;
    VkInstance mVkInstance;
} CGpuInstance_Vulkan;

typedef struct CGpuAdapter_Vulkan {
    CGpuAdapter super;
    VkPhysicalDevice mPhysicalDevice;
} CGpuAdapter_Vulkan;


#ifdef __cplusplus
} // end extern "C"
#endif