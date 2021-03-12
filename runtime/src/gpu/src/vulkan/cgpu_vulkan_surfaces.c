#define DLL_IMPLEMENTATION
#include "cgpu/backend/vulkan/cgpu_vulkan_surfaces.h"

const CGpuSurfacesProcTable s_tbl_vk = 
{
    .cgpu_surface_free = cgpu_surface_free_vulkan,
#if defined(_WIN32) || defined(_WIN64)
    .from_hwnd = cgpu_surface_from_hwnd_vulkan
#endif
};

const CGpuSurfacesProcTable* CGPU_VulkanSurfacesProcTable()
{
	return &s_tbl_vk;
}

void cgpu_surface_free_vulkan(CGpuSurfaceId surface)
{
    
}

#if defined(_WIN32) || defined(_WIN64)
CGpuSurfaceId cgpu_surface_from_hwnd_vulkan(HWND window)
{
    VkWin32SurfaceCreateInfoKHR create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.pNext = NULL;
	create_info.flags = 0;
	create_info.hinstance = ::GetModuleHandle(NULL);
	create_info.hwnd = window;
    return CGPU_NULLPTR;
	//CHECK_VKRESULT(vkCreateWin32SurfaceKHR(pRenderer->pVkInstance, &add_info, &gVkAllocationCallbacks, &vkSurface));
}
#endif