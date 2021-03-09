#include "cgpu/backend/vulkan/cgpu_vulkan.h"
#include "cgpu/backend/vulkan/cgpu_vulkan_exts.h"
#include <assert.h>
#include <stdlib.h>

#ifdef CGPU_USE_VULKAN

const CGpuProcTable tbl_vk = 
{
    .create_instance = &cgpu_create_instance_vulkan,
    .destroy_instance = &cgpu_destroy_instance_vulkan,
	.enum_adapters = &cgpu_enum_adapters_vulkan,
	.query_adapter_detail = &cgpu_query_adapter_detail_vulkan
};

const CGpuProcTable* CGPU_VulkanProcTable()
{
    return &tbl_vk;
}

CGpuInstanceId cgpu_create_instance_vulkan(CGpuInstanceDescriptor const* descriptor)
{
	return cgpu_vulkan_create_instance(CGPU_NULLPTR);
}

void cgpu_enum_adapters_vulkan(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num)
{
	CGpuInstance_Vulkan* I = (CGpuInstance_Vulkan*)instance;
	if (adapters == CGPU_NULLPTR)
	{
		*adapters_num = I->mPhysicalDeviceCount;
		return;
	} else {
		for(uint32_t i = 0; i < I->mPhysicalDeviceCount; i++)
		{
			adapters[i] = &I->pVulkanAdapters[i].super;
		}
	}
}

void cgpu_destroy_instance_vulkan(CGpuInstanceId instance)
{
    CGpuInstance_Vulkan* to_destroy = (CGpuInstance_Vulkan*)instance;
    vkDestroyInstance(to_destroy->mVkInstance, VK_NULL_HANDLE);
	free(to_destroy->pVulkanAdapters);
	free(to_destroy);
}

CGpuAdapterDetail cgpu_query_adapter_detail_vulkan(const CGpuAdapterId adapter)
{
    CGpuAdapter_Vulkan* a = (CGpuAdapter_Vulkan*)adapter;
    CGpuAdapterDetail d = {};
	d.backend = ECGPUBackEnd_VULKAN;
	d.deviceId = a->mPhysicalDeviceProps.deviceID;
	d.vendorId = a->mPhysicalDeviceProps.vendorID;
	d.name = a->mPhysicalDeviceProps.deviceName;
	d.driverDescription = "Vulkan";
    return d;
}

// exts
#include "cgpu/backend/vulkan/cgpu_vulkan_exts.h"

CGpuInstanceId cgpu_vulkan_create_instance(CGpuVulkanInstanceDescriptor const* descriptor)
{
	CGpuInstance_Vulkan* result = malloc(sizeof(CGpuInstance_Vulkan));
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "CGPU";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // cn: 创建VkInstance.
	// en: Create VkInstance.
	// jp: VkInstanceを作る.
	VkInstanceCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	const char* exts[] = {
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_KHR_SURFACE_EXTENSION_NAME
	};

	if(descriptor == CGPU_NULLPTR)
	{
		createInfo.enabledExtensionCount = 2;
		createInfo.ppEnabledExtensionNames = exts;
	} else {
		createInfo.enabledExtensionCount = descriptor->mInstanceExtensionCount;
		createInfo.ppEnabledExtensionNames = descriptor->ppInstanceExtensions;
	}

	if (descriptor != CGPU_NULLPTR)
	{
		createInfo.enabledLayerCount = descriptor->mInstanceLayerCount;
		createInfo.ppEnabledLayerNames = descriptor->ppInstanceLayers;
		createInfo.pNext = descriptor->pDebugUtilsMessenger;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = VK_NULL_HANDLE;
	}
	
	if (vkCreateInstance(&createInfo, VK_NULL_HANDLE, &result->mVkInstance) != VK_SUCCESS)
	{
		assert(0 && "Vulkan: failed to create instance!");
	}
	
	vkEnumeratePhysicalDevices(result->mVkInstance, &result->mPhysicalDeviceCount, CGPU_NULLPTR);
	if(result->mPhysicalDeviceCount != 0)
	{
		result->pVulkanAdapters = malloc(sizeof(CGpuAdapter_Vulkan) * result->mPhysicalDeviceCount);
		VkPhysicalDevice* pysicalDevices = malloc(sizeof(VkPhysicalDevice) * result->mPhysicalDeviceCount);
		vkEnumeratePhysicalDevices(result->mVkInstance, &result->mPhysicalDeviceCount, pysicalDevices);
		for(uint32_t i = 0; i < result->mPhysicalDeviceCount; i++)
		{
			result->pVulkanAdapters[i].super.instance = &result->super;
			result->pVulkanAdapters[i].mPhysicalDevice = pysicalDevices[i];
			vkGetPhysicalDeviceProperties(pysicalDevices[i], &result->pVulkanAdapters[i].mPhysicalDeviceProps);
			vkGetPhysicalDeviceFeatures(pysicalDevices[i], &result->pVulkanAdapters[i].mPhysicalDeviceFeatures);
		}
		free(pysicalDevices);
	} else {
		assert(0 && "Vulkan: 0 physical device avalable!");
	}
	return &(result->super);
}

#endif