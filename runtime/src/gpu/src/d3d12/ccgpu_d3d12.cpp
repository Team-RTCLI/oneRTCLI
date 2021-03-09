#include "cgpu/backend/d3d12/cgpu_d3d12.h"
#include "cgpu/backend/d3d12/cgpu_d3d12_exts.h"
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

CGpuInstanceId cgpu_create_instance_d3d12(CGpuInstanceDescriptor const* descriptor)
{
    CGpuInstance_D3D12* result = new CGpuInstance_D3D12();
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

    return CGPU_NULLPTR;
}

void cgpu_destroy_instance_d3d12(CGpuInstanceId instance)
{
    CGpuInstance_D3D12* result = (CGpuInstance_D3D12*)instance;
    
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

#endif