#include "cgpu/backend/d3d12/cgpu_d3d12.h"
#include <assert.h>
#include <stdlib.h>
#ifdef CGPU_USE_D3D12
//
// C++ is the only language supported by D3D12:
//   https://msdn.microsoft.com/en-us/library/windows/desktop/dn899120(v=vs.85).aspx
//
#if !defined(__cplusplus)
#error "D3D12 requires C++! Sorry!"
#endif
#include <stdio.h>

#if !defined(XBOX)
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p_var) \
	if (p_var)              \
	{                       \
		p_var->Release();   \
		p_var = NULL;       \
	}
#endif

D3D_FEATURE_LEVEL feature_levels[] =
{
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0
};

void optionalEnableDebugLayer(CGpuInstance_D3D12* result, CGpuInstanceDescriptor const* descriptor)
{
    if(descriptor->enableDebugLayer)
    {
        if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(result->pDXDebug), (void**)&(result->pDXDebug))))
        {
            result->pDXDebug->EnableDebugLayer();
            if(descriptor->enableGpuBasedValidation)
            {
                ID3D12Debug1* pDebug1 = NULL;
                if (SUCCEEDED(result->pDXDebug->QueryInterface(IID_PPV_ARGS(&pDebug1))))
                {
                    pDebug1->SetEnableGPUBasedValidation(descriptor->enableGpuBasedValidation);
                    pDebug1->Release();
                }
            }
        }
    } else if(descriptor->enableGpuBasedValidation) {
        printf("[D3D12 Warning]: GpuBasedValidation enabled while DebugLayer is closed, there'll be no effect.");
    }
}

// Call this only once.
uint32_t getProperGpuCount(IDXGIFactory6* pDXGIFactory)
{
    uint32_t gpuCount = 0;
    IDXGIAdapter4* adapter = NULL;
    bool foundSoftwareAdapter = false;
    // Find number of usable GPUs
    // Use DXGI6 interface which lets us specify gpu preference so we dont need to use NVOptimus or AMDPowerExpress exports
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != pDXGIFactory->EnumAdapterByGpuPreference(i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
        IID_PPV_ARGS(&adapter)); ++i)
    {
        DXGI_ADAPTER_DESC3 desc = {};
        adapter->GetDesc3(&desc);
        // Ignore Microsoft Driver
        if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
        {
            uint32_t level_c = CGPU_ARRAY_LEN(feature_levels);
            for (uint32_t level = 0; level < level_c; ++level)
            {
                // Make sure the adapter can support a D3D12 device
                if (SUCCEEDED(D3D12CreateDevice(adapter, feature_levels[level], __uuidof(ID3D12Device), NULL)))
                {
                    CGpuAdapter_D3D12 cgpuAdapter = {};
                    HRESULT hres = adapter->QueryInterface(IID_PPV_ARGS(&cgpuAdapter.pDxActiveGPU));
                    if (SUCCEEDED(hres))
                    {
                        SAFE_RELEASE(cgpuAdapter.pDxActiveGPU);
                        ++gpuCount;
                        break;
                    }
                }
            }
        } else
        {
            foundSoftwareAdapter = true;
        }
        adapter->Release();
    }
    return gpuCount;
} 

CGpuInstanceId cgpu_create_instance_d3d12(CGpuInstanceDescriptor const* descriptor)
{
    CGpuInstance_D3D12* result = new CGpuInstance_D3D12();
    optionalEnableDebugLayer(result, descriptor);

    UINT flags = 0;
    if(descriptor->enableDebugLayer)
        flags = DXGI_CREATE_FACTORY_DEBUG;
#if defined(XBOX)
#else 
    if(SUCCEEDED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&result->pDXGIFactory))))
    {
        uint32_t gpuCount = getProperGpuCount(result->pDXGIFactory);
        printf("GPU Count %i\n", gpuCount);
    } else {
        assert("[D3D12 Fatal]: Create DXGIFactory2 Failed!");
    }
#endif

    return &result->super;
}

void cgpu_destroy_instance_d3d12(CGpuInstanceId instance)
{
    CGpuInstance_D3D12* result = (CGpuInstance_D3D12*)instance;
    if(result->pDXDebug) result->pDXDebug->Release();
    result->pDXGIFactory->Release();
    delete result;
}

void cgpu_enum_adapters_d3d12(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num)
{

}

CGpuAdapterDetail cgpu_query_adapter_detail_d3d12(const CGpuAdapterId adapter)
{
    CGpuAdapterDetail d = {};
    return d;
}

#include "cgpu/backend/d3d12/cgpu_d3d12_exts.h"
// extentions
CGpuDREDSettingsId cgpu_d3d12_enable_DRED()
{
    CGpuDREDSettingsId settings = new CGpuDREDSettings();
    SUCCEEDED(D3D12GetDebugInterface(__uuidof(settings->pDredSettings), (void**)&(settings->pDredSettings)));
    // Turn on auto-breadcrumbs and page fault reporting.
    settings->pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
    settings->pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
    return settings;
}

void cgpu_d3d12_disable_DRED(CGpuDREDSettingsId settings)
{
    settings->pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_OFF);
    settings->pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_OFF);
    settings->pDredSettings->Release();
    delete settings;
}

#endif







