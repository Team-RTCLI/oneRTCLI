#define DLL_IMPLEMENTATION
#include "cgpu/backend/vulkan/cgpu_vulkan.h"
#include "cgpu/extensions/cgpu_vulkan_exts.h"
#include <assert.h>
#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#endif
#if defined(__linux__) || defined(__linux) || defined(__EMSCRIPTEN__)
#include <alloca.h>
#endif
#ifdef CGPU_USE_VULKAN

const CGpuProcTable tbl_vk = 
{
    .create_instance = &cgpu_create_instance_vulkan,
    .free_instance = &cgpu_free_instance_vulkan,
	.enum_adapters = &cgpu_enum_adapters_vulkan,
	.query_adapter_detail = &cgpu_query_adapter_detail_vulkan,
	.query_queue_count = &cgpu_query_queue_count_vulkan,
	.create_device = &cgpu_create_device_vulkan,
	.free_device = &cgpu_free_device_vulkan,
	.get_queue = &cgpu_get_queue_vulkan,
	.free_queue = &cgpu_free_queue_vulkan,
	.create_command_encoder = &cgpu_create_command_encoder_vulkan,
	.free_command_encoder = &cgpu_free_command_encoder_vulkan,
	.create_swapchain = &cgpu_create_swapchain_vulkan,
	.free_swapchain = &cgpu_free_swapchain_vulkan
};


const CGpuProcTable* CGPU_VulkanProcTable()
{
    return &tbl_vk;
}

CGpuInstanceId cgpu_create_instance_vulkan(CGpuInstanceDescriptor const* descriptor)
{
	return cgpu_vulkan_create_instance(descriptor, CGPU_NULLPTR);
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

CGpuAdapterDetail cgpu_query_adapter_detail_vulkan(const CGpuAdapterId adapter)
{
    CGpuAdapter_Vulkan* a = (CGpuAdapter_Vulkan*)adapter;
    CGpuAdapterDetail d = {0};
	d.deviceId = a->mPhysicalDeviceProps.deviceID;
	d.vendorId = a->mPhysicalDeviceProps.vendorID;
	d.name = a->mPhysicalDeviceProps.deviceName;
    return d;
}

uint32_t cgpu_query_queue_count_vulkan(const CGpuAdapterId adapter, const ECGpuQueueType type)
{
	CGpuAdapter_Vulkan* a = (CGpuAdapter_Vulkan*)adapter;
	uint32_t count = 0;
	switch (type)
	{
		case ECGpuQueueType_Graphics:
		{
			for(uint32_t i = 0; i < a->mQueueFamilyPropertiesCount; i++)
			{
				const VkQueueFamilyProperties* prop =  &a->pQueueFamilyProperties[i];
				if(prop->queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					count += prop->queueCount;
				}
			}
		} break;
		case ECGpuQueueType_Compute:
		{
			for(uint32_t i = 0; i < a->mQueueFamilyPropertiesCount; i++)
			{
				const VkQueueFamilyProperties* prop =  &a->pQueueFamilyProperties[i];
				if(prop->queueFlags & VK_QUEUE_COMPUTE_BIT)
					if(!(prop->queueFlags & VK_QUEUE_GRAPHICS_BIT))
					{
						count += prop->queueCount;
					}
			}
		} break;
		case ECGpuQueueType_Transfer:
		{
			for(uint32_t i = 0; i < a->mQueueFamilyPropertiesCount; i++)
			{
				const VkQueueFamilyProperties* prop =  &a->pQueueFamilyProperties[i];
				if(prop->queueFlags & VK_QUEUE_TRANSFER_BIT)
					if(!(prop->queueFlags & VK_QUEUE_COMPUTE_BIT))
						if(!(prop->queueFlags & VK_QUEUE_GRAPHICS_BIT))
						{
							count += prop->queueCount;
						}
			}
		} break;
		default: assert(0 && "CGPU VULKAN: ERROR Queue Type!");
	}
	return count;
}

CGpuQueueId cgpu_get_queue_vulkan(CGpuDeviceId device, ECGpuQueueType type, uint32_t index)
{
	assert(device && "CGPU VULKAN: NULL DEVICE!");
	CGpuDevice_Vulkan* D = (CGpuDevice_Vulkan*)device;
	CGpuAdapter_Vulkan* A = (CGpuAdapter_Vulkan*)device->adapter;
	CGpuQueue_Vulkan* Q = (CGpuQueue_Vulkan*)malloc(sizeof(CGpuQueue_Vulkan));

	D->mVkDeviceTable.vkGetDeviceQueue(D->pVkDevice, A->mQueueFamilyIndices[type], index, &Q->pVkQueue);
	Q->mVkQueueFamilyIndex = A->mQueueFamilyIndices[type];
	return &Q->super;
}

void cgpu_free_queue_vulkan(CGpuQueueId queue)
{
	free(queue);
}

VkCommandPool allocate_transient_command_pool(CGpuDevice_Vulkan* D, CGpuQueueId queue)
{
	VkCommandPool P = VK_NULL_HANDLE;
	//CGpuQueue_Vulkan* Q = (CGpuQueue_Vulkan*)queue;
	CGpuAdapter_Vulkan* A = (CGpuAdapter_Vulkan*)queue->device->adapter;

	VkCommandPoolCreateInfo create_info = {0};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create_info.pNext = NULL;
	// transient.
	create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	create_info.queueFamilyIndex = A->mQueueFamilyIndices[queue->type];

	if( VK_SUCCESS != 
#ifdef VK_USE_VOLK_DEVICE_TABLE 
	D->mVkDeviceTable.
#endif
		vkCreateCommandPool(D->pVkDevice, &create_info, GLOBAL_VkAllocationCallbacks, &P) )
	{
		assert(0 && "CGPU VULKAN: CREATE COMMAND POOL FAILED!");
	}
	return P;
}

void free_transient_command_pool(CGpuDevice_Vulkan* D, VkCommandPool pool)
{
#ifdef VK_USE_VOLK_DEVICE_TABLE 
	D->mVkDeviceTable.
#endif
		vkDestroyCommandPool(D->pVkDevice, pool, GLOBAL_VkAllocationCallbacks);
}

CGpuCommandEncoderId cgpu_create_command_encoder_vulkan(CGpuQueueId queue, const CGpuCommandEncoderDescriptor* desc)
{
	CGpuDevice_Vulkan* D = (CGpuDevice_Vulkan*)queue->device;
	CGpuCommandEncoder_Vulkan* E = (CGpuCommandEncoder_Vulkan*)malloc(sizeof(CGpuCommandEncoder_Vulkan));
	E->pVkCmdPool = allocate_transient_command_pool(D, queue);
	return &E->super;
}

void cgpu_free_command_encoder_vulkan(CGpuCommandEncoderId encoder)
{
	CGpuDevice_Vulkan* D = (CGpuDevice_Vulkan*)encoder->queue->device;
	CGpuCommandEncoder_Vulkan* E  = (CGpuCommandEncoder_Vulkan*)encoder;
	free_transient_command_pool(D, E->pVkCmdPool);
	free(encoder);
}

#define clamp(x, min, max) (x) < (min) ? (min) : ((x) > (max) ? (max) : (x));

CGpuSwapChainId cgpu_create_swapchain_vulkan(CGpuDeviceId device, const CGpuSwapChainDescriptor* desc)
{
	CGpuDevice_Vulkan* D = (CGpuDevice_Vulkan*)device;
	CGpuAdapter_Vulkan* A = (CGpuAdapter_Vulkan*)device->adapter;
	//CGpuInstance_Vulkan* I = (CGpuInstance_Vulkan*)device->adapter->instance;
	VkSurfaceKHR vkSurface = (VkSurfaceKHR)desc->surface;

	VkSurfaceCapabilitiesKHR caps = {0};
	if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceCapabilitiesKHR(A->pPhysicalDevice, vkSurface, &caps))
	{
		assert(0 && "vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed!");
	}
	if ((caps.maxImageCount > 0) && (desc->imageCount > caps.maxImageCount))
	{
		//LOGF(LogLevel::eWARNING, "Changed requested SwapChain images {%u} to maximum allowed SwapChain images {%u}", pDesc->mImageCount, caps.maxImageCount);
		((CGpuSwapChainDescriptor*)desc)->imageCount = caps.maxImageCount;
	} else if (desc->imageCount < caps.minImageCount) {
		//LOGF(LogLevel::eWARNING, "Changed requested SwapChain images {%u} to minimum required SwapChain images {%u}", pDesc->mImageCount, caps.minImageCount);
		((CGpuSwapChainDescriptor*)desc)->imageCount = caps.minImageCount;
	}

	// Surface format
	// Select a surface format, depending on whether HDR is available.
	VkSurfaceFormatKHR surface_format = {0};
	surface_format.format = VK_FORMAT_UNDEFINED;
	uint32_t            surfaceFormatCount = 0;
	if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceFormatsKHR(
		A->pPhysicalDevice, vkSurface, &surfaceFormatCount, CGPU_NULLPTR))
	{
		assert(0 && "fatal: vkGetPhysicalDeviceSurfaceFormatsKHR failed!");
	}
	// Allocate and get surface formats
	VkSurfaceFormatKHR* formats = NULL;
	formats = (VkSurfaceFormatKHR*)calloc(surfaceFormatCount, sizeof(VkSurfaceFormatKHR));
	if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceFormatsKHR(
		A->pPhysicalDevice, vkSurface, &surfaceFormatCount, formats))
	{
		assert(0 && "fatal: vkGetPhysicalDeviceSurfaceFormatsKHR failed!");
	}

	const VkSurfaceFormatKHR hdrSurfaceFormat =
	{ VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_COLOR_SPACE_HDR10_ST2084_EXT };
	if ((1 == surfaceFormatCount) && (VK_FORMAT_UNDEFINED == formats[0].format))
	{
		surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
		surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	} else {
		VkFormat requested_format = pf_translate_to_vulkan(desc->format);
		VkColorSpaceKHR requested_color_space = requested_format == hdrSurfaceFormat.format ? hdrSurfaceFormat.colorSpace : VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		for (uint32_t i = 0; i < surfaceFormatCount; ++i)
		{
			if ((requested_format == formats[i].format) && (requested_color_space == formats[i].colorSpace))
			{
				surface_format.format = requested_format;
				surface_format.colorSpace = requested_color_space;
				break;
			}
		}
		// Default to VK_FORMAT_B8G8R8A8_UNORM if requested format isn't found
		if (VK_FORMAT_UNDEFINED == surface_format.format)
		{
			surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
			surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		}
	}
	assert(VK_FORMAT_UNDEFINED != surface_format.format);
	free(formats);

	// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
	// This mode waits for the vertical blank ("v-sync")
	VkPresentModeKHR  present_mode = VK_PRESENT_MODE_FIFO_KHR;
	uint32_t          swapChainImageCount = 0;
	VkPresentModeKHR* modes = NULL;
	// Get present mode count
	if (VK_SUCCESS != vkGetPhysicalDeviceSurfacePresentModesKHR(
		A->pPhysicalDevice, vkSurface, &swapChainImageCount, NULL))
	{
		assert(0 && "fatal: vkGetPhysicalDeviceSurfacePresentModesKHR failed!");
	}

	// Allocate and get present modes
	modes = (VkPresentModeKHR*)alloca(swapChainImageCount * sizeof(*modes));
	if (VK_SUCCESS != vkGetPhysicalDeviceSurfacePresentModesKHR(
		A->pPhysicalDevice, vkSurface, &swapChainImageCount, modes))
	{
		assert(0 && "fatal: vkGetPhysicalDeviceSurfacePresentModesKHR failed!");
	}
	VkPresentModeKHR preferredModeList[] = {
		VK_PRESENT_MODE_IMMEDIATE_KHR,
		VK_PRESENT_MODE_MAILBOX_KHR,
		VK_PRESENT_MODE_FIFO_RELAXED_KHR,
		VK_PRESENT_MODE_FIFO_KHR
	};
	const uint32_t preferredModeCount = CGPU_ARRAY_LEN(preferredModeList);

	uint32_t preferredModeStartIndex = desc->enableVsync ? 2 : 0;
	for (uint32_t j = preferredModeStartIndex; j < preferredModeCount; ++j)
	{
		VkPresentModeKHR mode = preferredModeList[j];
		uint32_t         i = 0;
		for (i = 0; i < swapChainImageCount; ++i)
		{
			if (modes[i] == mode)
			{
				break;
			}
		}
		if (i < swapChainImageCount)
		{
			present_mode = mode;
			break;
		}
	}

	// Swapchain
	VkExtent2D extent = { 0 };
	extent.width = clamp(desc->width, caps.minImageExtent.width, caps.maxImageExtent.width);
	extent.height = clamp(desc->height, caps.minImageExtent.height, caps.maxImageExtent.height);

	CGpuQueue_Vulkan* Q = (CGpuQueue_Vulkan*)desc->presentQueues[0];
	VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
	uint32_t      presentQueueFamilyIndex = -1;
	// Check Queue Present Support.
	{
		VkBool32 sup = VK_FALSE;
		VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(
			A->pPhysicalDevice, Q->mVkQueueFamilyIndex, vkSurface, &sup);
		if ((VK_SUCCESS == res) && (VK_TRUE == sup))
		{
			presentQueueFamilyIndex = Q->mVkQueueFamilyIndex;
		} else {
			// Get queue family properties
			uint32_t queueFamilyPropertyCount = 0;
			VkQueueFamilyProperties* queueFamilyProperties = NULL;
			vkGetPhysicalDeviceQueueFamilyProperties(A->pPhysicalDevice, &queueFamilyPropertyCount, NULL);
			queueFamilyProperties = (VkQueueFamilyProperties*)alloca(queueFamilyPropertyCount * sizeof(VkQueueFamilyProperties));
			vkGetPhysicalDeviceQueueFamilyProperties(A->pPhysicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);

			// Check if hardware provides dedicated present queue
			if (queueFamilyPropertyCount)
			{
				for (uint32_t index = 0; index < queueFamilyPropertyCount; ++index)
				{
					VkBool32 supports_present = VK_FALSE;
					VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(A->pPhysicalDevice, index, vkSurface, &supports_present);
					if ((VK_SUCCESS == res) && (VK_TRUE == supports_present) && Q->mVkQueueFamilyIndex != index)
					{
						presentQueueFamilyIndex = index;
						break;
					}
				}
				// If there is no dedicated present queue, just find the first available queue which supports present
				if (presentQueueFamilyIndex == -1)
				{
					for (uint32_t index = 0; index < queueFamilyPropertyCount; ++index)
					{
						VkBool32 supports_present = VK_FALSE;
						VkResult res =
							vkGetPhysicalDeviceSurfaceSupportKHR(A->pPhysicalDevice, index, vkSurface, &supports_present);
						if ((VK_SUCCESS == res) && (VK_TRUE == supports_present))
						{
							presentQueueFamilyIndex = index;
							break;
						}
						else
						{
							// No present queue family available. Something goes wrong.
							assert(0);
						}
					}
				}
			}
		}
	}

	VkSurfaceTransformFlagBitsKHR pre_transform;
	// #TODO: Add more if necessary but identity should be enough for now
	if (caps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	} else {
		pre_transform = caps.currentTransform;
	}

	VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[] =
	{
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
	};
	VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
	for (uint32_t _i = 0; _i < CGPU_ARRAY_LEN(compositeAlphaFlags); _i++)
	{
		if (caps.supportedCompositeAlpha & compositeAlphaFlags[_i])
		{
			composite_alpha = compositeAlphaFlags[_i];
			break;
		}
	}
	assert(composite_alpha != VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR);

	CGpuSwapChain_Vulkan* S = (CGpuSwapChain_Vulkan*)malloc(sizeof(CGpuSwapChain_Vulkan));
	S->super.device = device;
	VkSwapchainCreateInfoKHR swapChainCreateInfo = { 0 };
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.pNext = NULL;
	swapChainCreateInfo.flags = 0;
	swapChainCreateInfo.surface = vkSurface;
	swapChainCreateInfo.minImageCount = desc->imageCount;
	swapChainCreateInfo.imageFormat = surface_format.format;
	swapChainCreateInfo.imageColorSpace = surface_format.colorSpace;
	swapChainCreateInfo.imageExtent = extent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.imageSharingMode = sharing_mode;
	swapChainCreateInfo.queueFamilyIndexCount = 1;
	swapChainCreateInfo.pQueueFamilyIndices = &presentQueueFamilyIndex;
	swapChainCreateInfo.preTransform = pre_transform;
	swapChainCreateInfo.compositeAlpha = composite_alpha;
	swapChainCreateInfo.presentMode = present_mode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	VkResult res = D->mVkDeviceTable.vkCreateSwapchainKHR(
		D->pVkDevice, &swapChainCreateInfo, GLOBAL_VkAllocationCallbacks, &S->pVkSwapChain);
	if (VK_SUCCESS != res)
	{
		assert(0 && "fatal: vkCreateSwapchainKHR failed!");
	}
	S->pVkSurface = vkSurface;
	return &S->super;
}

void cgpu_free_swapchain_vulkan(CGpuSwapChainId swapchain)
{
	CGpuSwapChain_Vulkan* S = (CGpuSwapChain_Vulkan*)swapchain;
	CGpuDevice_Vulkan* D = (CGpuDevice_Vulkan*)swapchain->device;

	D->mVkDeviceTable.vkDestroySwapchainKHR(D->pVkDevice, S->pVkSwapChain, GLOBAL_VkAllocationCallbacks);

	free(swapchain);
}

// exts
#include "cgpu/extensions/cgpu_vulkan_exts.h"


#endif