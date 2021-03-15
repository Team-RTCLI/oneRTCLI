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
    auto surface = cgpu_surface_from_hwnd(instance, hwnd);

    EXPECT_NE(surface, CGPU_NULLPTR);
    EXPECT_NE(surface, nullptr);
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