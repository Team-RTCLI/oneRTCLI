#include "cgpu/backend/vulkan/cgpu_vulkan_exts.h"
#include "cgpu/backend/vulkan/cgpu_vulkan.h"
#include <cassert>
#include <stdlib.h>
#include <vector>

#ifdef CGPU_USE_VULKAN
const char* validation_layer_name = "VK_LAYER_KHRONOS_validation";

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) 
{
	switch(messageSeverity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			printf("[verbose]");break; 
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: 
			printf("[info]");break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			printf("[warning]"); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT :
			printf("[error]"); break;
		default:
			return VK_TRUE;
	}
	printf(" validation layer: %s\n", pCallbackData->pMessage); 
    return VK_FALSE;
}

CGpuInstanceId cgpu_vulkan_create_instance(CGpuInstanceDescriptor const* desc, CGpuVulkanInstanceDescriptor const* exts_desc)
{	
	static auto volkInit = volkInitialize();
	assert((volkInit == VK_SUCCESS) && "Volk Initialize Failed!");

	CGpuInstance_Vulkan* result = (CGpuInstance_Vulkan*)malloc(sizeof(CGpuInstance_Vulkan));
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "CGPU";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    // cn: 创建VkInstance.
	// en: Create VkInstance.
	// jp: VkInstanceを作る.
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	std::vector<const char*> exts = {
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_KHR_SURFACE_EXTENSION_NAME
	};
	if(desc->enableDebugLayer)
	{
		exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	if(exts_desc != CGPU_NULLPTR) // Extensions
	{
		exts.insert(exts.end(), exts_desc->ppInstanceExtensions, exts_desc->ppInstanceExtensions + exts_desc->mInstanceExtensionCount);
	}
	createInfo.enabledExtensionCount = exts.size();
	createInfo.ppEnabledExtensionNames = exts.data();
	
	if(desc->enableGpuBasedValidation)
	{
		if(!desc->enableDebugLayer)
			printf("[Vulkan Warning]: GpuBasedValidation enabled while ValidationLayer is closed, there'll be no effect.");
#if VK_HEADER_VERSION >= 108
		VkValidationFeaturesEXT validationFeaturesExt = { VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
		VkValidationFeatureEnableEXT enabledValidationFeatures[] =
		{
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
		};

		if (desc->enableGpuBasedValidation)
		{
			validationFeaturesExt.enabledValidationFeatureCount = 1;
			validationFeaturesExt.pEnabledValidationFeatures = enabledValidationFeatures;
		}
		createInfo.pNext = &validationFeaturesExt;
#else
		printf("[Vulkan Warning]: GpuBasedValidation enabled but VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT is not supported!\n");
#endif
	}

    std::vector<const char*> layers;
	if (exts_desc != CGPU_NULLPTR) // Layers
	{
        layers.insert(layers.end(), 
            exts_desc->ppInstanceLayers, exts_desc->ppInstanceLayers + exts_desc->mInstanceLayerCount);

        if(desc->enableDebugLayer) 
            layers.push_back(validation_layer_name);
		
	}
	createInfo.enabledLayerCount = layers.size();
	createInfo.ppEnabledLayerNames = layers.data();
	if (vkCreateInstance(&createInfo, VK_NULL_HANDLE, &result->mVkInstance) != VK_SUCCESS)
	{
		assert(0 && "Vulkan: failed to create instance!");
	}

#if defined(NX64)
	loadExtensionsNX(result->mVkInstance);
#else
	// Load Vulkan instance functions
	volkLoadInstance(result->mVkInstance);
#endif

	// enum physical devices & store informations.
	vkEnumeratePhysicalDevices(result->mVkInstance, &result->mPhysicalDeviceCount, CGPU_NULLPTR);
	if(result->mPhysicalDeviceCount != 0)
	{
		result->pVulkanAdapters = (CGpuAdapter_Vulkan*)malloc(sizeof(CGpuAdapter_Vulkan) * result->mPhysicalDeviceCount);
		VkPhysicalDevice* pysicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * result->mPhysicalDeviceCount);
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

    // open validation layer.
    if(desc->enableDebugLayer)
    {
        const VkDebugUtilsMessengerCreateInfoEXT* messengerInfoPtr = nullptr;
        VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {};
        if(exts_desc && exts_desc->pDebugUtilsMessenger) {
            messengerInfoPtr = exts_desc->pDebugUtilsMessenger;
        } else {
            messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			messengerInfo.pfnUserCallback = debugCallback;
			messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			messengerInfo.messageType = 
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			messengerInfo.flags = 0;
			messengerInfo.pUserData = NULL;
            messengerInfoPtr = &messengerInfo;
        }
		assert(vkCreateDebugUtilsMessengerEXT && "Load vkCreateDebugUtilsMessengerEXT failed!");
        VkResult res = vkCreateDebugUtilsMessengerEXT(result->mVkInstance,
            messengerInfoPtr, nullptr, &(result->pVkDebugUtilsMessenger));
        if (VK_SUCCESS != res)
        {
            assert(0 && "vkCreateDebugUtilsMessengerEXT failed - disabling Vulkan debug callbacks");
        }
    }

	return &(result->super);
}

void cgpu_destroy_instance_vulkan(CGpuInstanceId instance)
{
    CGpuInstance_Vulkan* to_destroy = (CGpuInstance_Vulkan*)instance;
    vkDestroyInstance(to_destroy->mVkInstance, VK_NULL_HANDLE);
	free(to_destroy->pVulkanAdapters);
	free(to_destroy);
}


#endif