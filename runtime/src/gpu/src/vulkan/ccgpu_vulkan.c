#include "cgpu/backend/vulkan/cgpu_vulkan.h"
#ifdef _WIN32
#include "windows.h"
#include "vulkan/vulkan_win32.h"
#endif
#include <assert.h>

#ifdef CGPU_USE_VULKAN

const CGpuProcTable tbl_vk = 
{
    .create_instance = &cgpu_create_instance_vulkan,
    .destroy_instance = &cgpu_destroy_instance_vulkan
};

const CGpuProcTable* CGPU_VulkanProcTable()
{
    return &tbl_vk;
}

CGpuInstanceId cgpu_create_instance_vulkan(CGpuInstanceDescriptor const* descriptor)
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
	{
		VkInstanceCreateInfo createInfo = {0};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
        const char* exts[] = {
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            VK_KHR_SURFACE_EXTENSION_NAME
        };
		createInfo.enabledExtensionCount = 2;
		createInfo.ppEnabledExtensionNames = exts;
		//if (bEnableValidationLayers && !checkValidationLayerSupport(validationLayers))
		//{
	    //    assert(0 && "validation layers requested, but not available!");
		//}

		//VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		//if (bEnableValidationLayers)
		//{
		//	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		//	createInfo.ppEnabledLayerNames = validationLayers.data();

		//	debugCreateInfo = {};
		//	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		//	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		//	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		//	debugCreateInfo.pfnUserCallback = debugCallback;

		//	createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		//}
		//else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = VK_NULL_HANDLE;
		//}
		if (vkCreateInstance(&createInfo, VK_NULL_HANDLE, &result->mVkInstance) != VK_SUCCESS)
		{
			assert(0 && "Vulkan: failed to create instance!");
		}
        return &(result->super);
	}
}

void cgpu_destroy_instance_vulkan(CGpuInstanceId instance)
{
    CGpuInstance_Vulkan* to_destroy = (CGpuInstance_Vulkan*)instance;
    vkDestroyInstance(to_destroy->mVkInstance, VK_NULL_HANDLE);
    free(to_destroy);
}


// exts

#include "cgpu/backend/vulkan/cgpu_vulkan_exts.h"



#endif