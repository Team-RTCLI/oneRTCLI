#define DLL_IMPLEMENTATION
#include "cgpu/api.h"
#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#include "cgpu/backend/webgpu/cgpu_webgpu_surfaces.h"
#include <cassert>

#ifdef __EMSCRIPTEN__

#else
    #include "dawn/webgpu.h"
    #include "dawn_native/DawnNative.h"

    #if defined(_WIN32) || defined(_WIN64)
        #define VK_USE_PLATFORM_WIN32_KHR
        #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
        #endif
        #include "windows.h"
    #endif
    #ifdef DAWN_ENABLE_BACKEND_VULKAN
        #include "volk.h"
        #include <dawn_native/VulkanBackend.h>
    #endif
    
    void cgpu_free_surface_webgpu(CGpuDeviceId device, CGpuSurfaceId surface)
    {
        CGpuDevice_WebGpu* D = (CGpuDevice_WebGpu*)device;
    #ifdef DAWN_ENABLE_BACKEND_VULKAN
        auto vkInstance = dawn_native::vulkan::GetInstance(D->pWGPUDevice);
        vkDestroySurfaceKHR(vkInstance, (VkSurfaceKHR)surface, nullptr);
    #endif
    }

    #if defined(_WIN32) || defined(_WIN64)
        #ifdef DAWN_ENABLE_BACKEND_VULKAN

            static VkSurfaceKHR createVkSurface(WGPUDevice device, HWND window) 
            {
                VkSurfaceKHR surface = VK_NULL_HANDLE;
                VkWin32SurfaceCreateInfoKHR info = {};
                info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                info.hinstance = GetModuleHandle(NULL);
                info.hwnd = window;
                auto vkInstance = dawn_native::vulkan::GetInstance(device);

                vkCreateWin32SurfaceKHR(vkInstance, &info, nullptr, &surface);
                return surface;
            }
        #endif //DAWN_ENABLE_BACKEND_VULKAN

        CGpuSurfaceId cgpu_surface_from_hwnd_webgpu(CGpuDeviceId device, HWND window)
        {
            CGpuDevice_WebGpu* D = (CGpuDevice_WebGpu*)device;
            CGpuInstance_WebGpu* I = (CGpuInstance_WebGpu*)device->adapter->instance;
            switch (I->backend)
            {
                case WGPUBackendType_D3D12: 
                    return (CGpuSurfaceId)window;
                case WGPUBackendType_Vulkan:
                    return (CGpuSurfaceId)createVkSurface(D->pWGPUDevice, window);
                default:
                    return CGPU_NULLPTR;
            }
            return CGPU_NULLPTR;
        }
    #endif // _WIN32 || _WIN64

#endif //__EMSCRIPTEN__