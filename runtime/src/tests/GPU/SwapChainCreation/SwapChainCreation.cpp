#include "gtest/gtest.h"
#include "cgpu/api.h"

class SwapChainCreation : public::testing::TestWithParam<ECGPUBackEnd>
{
protected:
	void SetUp() override
	{
        ECGPUBackEnd backend = GetParam();
        CGpuInstanceDescriptor desc;
        desc.backend = backend;
        desc.enableDebugLayer = true;
        desc.enableGpuBasedValidation = true;
        instance = cgpu_create_instance(&desc);

        EXPECT_NE(instance, CGPU_NULLPTR);
        EXPECT_NE(instance, nullptr);
    }

    void TearDown() override
    {
        cgpu_free_instance(instance);
    }

    CGpuInstanceId instance;
};

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "windows.h"
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND createWin32Window();


TEST_P(SwapChainCreation, CreateFromHWND)
{
    auto hwnd = createWin32Window();
    size_t adapters_count = 0;
    cgpu_enum_adapters(instance, nullptr, &adapters_count);
    std::vector<CGpuAdapterId> adapters; adapters.resize(adapters_count);
    cgpu_enum_adapters(instance, adapters.data(), &adapters_count);
    for(auto adapter : adapters)
    {
        auto gQueue = cgpu_query_queue_count(adapter, ECGpuQueueType_Graphics); 
        auto cQueue = cgpu_query_queue_count(adapter, ECGpuQueueType_Compute); 
        auto tQueue = cgpu_query_queue_count(adapter, ECGpuQueueType_Transfer); 

        std::vector<CGpuQueueGroupDescriptor> queueGroup; 
        if(gQueue > 0) queueGroup.push_back(CGpuQueueGroupDescriptor{ECGpuQueueType_Graphics, 1});
        if(cQueue > 0) queueGroup.push_back(CGpuQueueGroupDescriptor{ECGpuQueueType_Compute, 1});
        if(tQueue > 0) queueGroup.push_back(CGpuQueueGroupDescriptor{ECGpuQueueType_Transfer, 1});
        CGpuDeviceDescriptor descriptor = {};
        descriptor.queueGroups = queueGroup.data();
        descriptor.queueGroupCount = queueGroup.size();

        auto device = cgpu_create_device(adapter, &descriptor);
        EXPECT_NE(device, nullptr);
        EXPECT_NE(device, CGPU_NULLPTR);

        auto surface = cgpu_surface_from_hwnd(device, hwnd);

        EXPECT_NE(surface, CGPU_NULLPTR);
        EXPECT_NE(surface, nullptr);

        cgpu_free_device(device);
    }
}
#endif

static const auto allPlatforms = testing::Values(
#ifndef TEST_WEBGPU    
    #ifdef CGPU_USE_VULKAN
        ECGPUBackEnd_VULKAN
    #endif
    #ifdef CGPU_USE_D3D12
        ,ECGPUBackEnd_D3D12
    #endif
#else
    #ifdef CGPU_USE_WEBGPU
        ECGPUBackEnd_WEBGPU   
    #endif
#endif
);

INSTANTIATE_TEST_SUITE_P(SwapChainCreation, SwapChainCreation, allPlatforms);


#if defined(_WIN32) || defined(_WIN64)
LRESULT CALLBACK WindowProcedure( HWND window, UINT msg, WPARAM wp, LPARAM lp )
{
    switch(msg)
    {
    case WM_DESTROY:
        std::cout << "\ndestroying window\n" ;
        PostQuitMessage(0) ;
        return 0L ;
    case WM_LBUTTONDOWN:
        std::cout << "\nmouse left button down at (" << LOWORD(lp) << ',' << HIWORD(lp) << ")\n" ;
    default:
        return DefWindowProc( window, msg, wp, lp ) ;
    }
}

HWND createWin32Window()
{
    // Register the window class.
    const char* const myclass = "myclass" ;
    WNDCLASSEX wndclass = 
    { 
        sizeof(WNDCLASSEX), CS_DBLCLKS,
        WindowProcedure,
        0, 0, GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
        LoadCursor(0,IDC_ARROW), HBRUSH(COLOR_WINDOW+1),
        0, myclass, LoadIcon(0,IDI_APPLICATION) 
    };
    static bool bRegistered = RegisterClassEx(&wndclass);
    if(bRegistered)
    {
        HWND window = CreateWindowEx( 0, myclass, "title",
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), 0 ) ;
        if(window)
        {
            ShowWindow( window, SW_SHOWDEFAULT ) ;
        }
        return window;
    }
    return CGPU_NULLPTR;
}
#endif