#pragma once
#include "cgpu/api.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

const CGpuProcTable* CGPU_VulkanProcTable();

CGpuInstanceId cgpu_create_instance_vulkan(CGpuInstanceDescriptor const* descriptor);
void cgpu_destroy_instance_vulkan(CGpuInstanceId instance);
void cgpu_enum_adapters_vulkan(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);
CGpuAdapterDetail cgpu_query_adapter_detail_vulkan(const CGpuAdapterId adapter);

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