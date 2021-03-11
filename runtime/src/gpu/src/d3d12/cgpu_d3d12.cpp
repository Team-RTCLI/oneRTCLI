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
#include <vector>

#if !defined(XBOX)
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
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
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&result->pDXDebug))))
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

#include <comdef.h>

// Call this only once.
void getProperGpuCount(CGpuInstance_D3D12* instance, uint32_t* count, bool* foundSoftwareAdapter)
{
    IDXGIAdapter4* adapter = NULL;
    std::vector<IDXGIAdapter4*> adapters;
    std::vector<D3D_FEATURE_LEVEL> adapter_levels;
    // Find number of usable GPUs
    // Use DXGI6 interface which lets us specify gpu preference so we dont need to use NVOptimus or AMDPowerExpress exports
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != instance->pDXGIFactory->EnumAdapterByGpuPreference(i,
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
                        instance->mAdaptersCount++;
                        // Add ref
                        {
                            adapters.push_back(adapter);
                            adapter_levels.push_back(feature_levels[level]);
                        }
                        break;
                    }
                }
            }
        } else
        {
            *foundSoftwareAdapter = true;
            adapter->Release();
        }
    }
    *count = instance->mAdaptersCount;
    instance->pAdapters = (CGpuAdapter_D3D12*)malloc(sizeof(CGpuAdapter_D3D12) * instance->mAdaptersCount);
    for(uint32_t i = 0; i < *count; i++)
    {
        instance->pAdapters[i].pDxActiveGPU = adapters[i];
        instance->pAdapters[i].mFeatureLevel = adapter_levels[i];
        DXGI_ADAPTER_DESC3 desc = {};
        adapters[i]->GetDesc3(&desc);

        instance->pAdapters[i].mDeviceId = desc.DeviceId;
        instance->pAdapters[i].mVendorId = desc.VendorId;
        _bstr_t b(desc.Description);
        char* str = b;
        memcpy(instance->pAdapters[i].mDescription, str, b.length());
        instance->pAdapters[i].mDescription[b.length()] = '\0';

        instance->pAdapters[i].super.instance = &instance->super;
    }
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
        uint32_t gpuCount = 0;
        bool foundSoftwareAdapter = false;
        getProperGpuCount(result, &gpuCount, &foundSoftwareAdapter);
        // If the only adapter we found is a software adapter, log error message for QA
        if (!gpuCount && foundSoftwareAdapter)
        {
            assert(0 && "The only available GPU has DXGI_ADAPTER_FLAG_SOFTWARE. Early exiting");
            return CGPU_NULLPTR;
        }
    } else {
        assert("[D3D12 Fatal]: Create DXGIFactory2 Failed!");
    }
#endif
    return &result->super;
}

void cgpu_destroy_instance_d3d12(CGpuInstanceId instance)
{
    CGpuInstance_D3D12* result = (CGpuInstance_D3D12*)instance;
    if(result->mAdaptersCount > 0)
    {
        for(uint32_t i = 0; i < result->mAdaptersCount; i++)
        {
            result->pAdapters[i].pDxActiveGPU->Release();
        }
    }
    result->pDXGIFactory->Release();
    if(result->pDXDebug)
    {
        result->pDXDebug->Release();
    }
#ifdef _DEBUG
    {
        IDXGIDebug1* dxgiDebug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
        {
            dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, 
                DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
        dxgiDebug->Release();
    }
#endif
    delete result;
}

void cgpu_enum_adapters_d3d12(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num)
{
    assert(instance != nullptr && "fatal: null instance!");
    CGpuInstance_D3D12* I = (CGpuInstance_D3D12*)instance;
    *adapters_num = I->mAdaptersCount;
    if(!adapters) {
        return;
    } else {
        for(auto i = 0u; i < *adapters_num; i++)
            adapters[i] = &(I->pAdapters[i].super);
    }
}

CGpuAdapterDetail cgpu_query_adapter_detail_d3d12(const CGpuAdapterId adapter)
{
    const CGpuAdapter_D3D12* A = (CGpuAdapter_D3D12*)adapter;
    CGpuAdapterDetail d = {};
    d.deviceId = A->mDeviceId;
    d.vendorId = A->mVendorId;
    d.name = A->mDescription;
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







