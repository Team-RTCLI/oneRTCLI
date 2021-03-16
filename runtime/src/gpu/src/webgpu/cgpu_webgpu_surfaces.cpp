#include "cgpu/api.h"
#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#include "cgpu/backend/webgpu/cgpu_webgpu_surfaces.h"
#include "dawn/webgpu.h"

const CGpuSurfacesProcTable s_tbl_webgpu = 
{
    .cgpu_surface_free = cgpu_surface_free_webgpu,
#if defined(_WIN32) || defined(_WIN64)
    .from_hwnd = cgpu_surface_from_hwnd_webgpu
#endif
};

const CGpuSurfacesProcTable* CGPU_WebGPUSurfacesProcTable()
{
	return &s_tbl_webgpu;
}

void cgpu_surface_free_webgpu(CGpuDeviceId device, CGpuSurfaceId surface)
{
    
}

#if defined(_WIN32) || defined(_WIN64)

#ifdef DAWN_ENABLE_BACKEND_VULKAN

static VkSurfaceKHR createVkSurface(WGPUDevice device, HWND window) {
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkWin32SurfaceCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	info.hinstance = GetModuleHandle(NULL);
	info.hwnd = window;
	vkCreateWin32SurfaceKHR(
		dawn_native::vulkan::GetInstance(device),
		&info, nullptr, &surface);
	return surface;
}
#endif //DAWN_ENABLE_BACKEND_VULKAN

CGpuSurfaceId cgpu_surface_from_hwnd_webgpu(CGpuDeviceId device, HWND window)
{
    CGpuInstance_WebGpu* I = (CGpuInstance_WebGpu*)device->adapter->instance;
    switch (I->backend)
    {
        case WGPUBackendType_D3D12: 
            return (CGpuSurfaceId)window;
        case WGPUBackendType_Vulkan:
        {
            //createVkSurface(I->, window.handle())
        }
        default: return CGPU_NULLPTR;
    }
    return CGPU_NULLPTR;
}

#endif