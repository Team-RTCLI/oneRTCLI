#define DLL_IMPLEMENTATION

#include "cgpu/backend/vulkan/cgpu_vulkan.h"
#include "cgpu/backend/vulkan/cgpu_vulkan_exts.h"
#include <assert.h>
#include <stdlib.h>

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
	.free_queue = &cgpu_free_queue_vulkan
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
    CGpuAdapterDetail d = {};
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
	return &Q->super;
}

void cgpu_free_queue_vulkan(CGpuQueueId queue)
{
	free(queue);
}

// exts
#include "cgpu/backend/vulkan/cgpu_vulkan_exts.h"


#endif