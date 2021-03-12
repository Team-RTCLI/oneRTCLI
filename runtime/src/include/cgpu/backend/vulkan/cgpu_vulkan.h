#pragma once
#include "cgpu/api.h"
#include "volk.h"

#ifdef __cplusplus
extern "C" {
#endif

const CGpuProcTable* CGPU_VulkanProcTable();

CGpuInstanceId cgpu_create_instance_vulkan(CGpuInstanceDescriptor const* descriptor);
void cgpu_destroy_instance_vulkan(CGpuInstanceId instance);
void cgpu_enum_adapters_vulkan(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);
CGpuAdapterDetail cgpu_query_adapter_detail_vulkan(const CGpuAdapterId adapter);
uint32_t cgpu_query_queue_count_vulkan(const CGpuAdapterId adapter, const ECGpuQueueType type);
CGpuDeviceId cgpu_create_device_vulkan(CGpuAdapterId adapter, const CGpuDeviceDescriptor* desc);
void cgpu_destroy_device_vulkan(CGpuDeviceId device);

typedef struct CGpuInstance_Vulkan {
    CGpuInstance super;
    VkInstance mVkInstance;
    VkDebugUtilsMessengerEXT pVkDebugUtilsMessenger;
    struct CGpuAdapter_Vulkan* pVulkanAdapters;
    uint32_t mPhysicalDeviceCount;
} CGpuInstance_Vulkan;

typedef struct CGpuAdapter_Vulkan {
    CGpuAdapter super;
    VkPhysicalDevice mPhysicalDevice;
    VkPhysicalDeviceProperties mPhysicalDeviceProps;
    VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;
    struct VkQueueFamilyProperties* pQueueFamilyProperties;
    uint32_t mQueueFamilyPropertiesCount;
    int64_t mQueueFamilyIndices[ECGpuQueueType_Count];
} CGpuAdapter_Vulkan;

typedef struct CGpuDevice_Vulkan {
    CGpuDevice super;
    VkDevice pVkDevice;
    struct VolkDeviceTable mVkDeviceTable;
} CGpuDevice_Vulkan;

#ifdef __cplusplus
} // end extern "C"
#endif