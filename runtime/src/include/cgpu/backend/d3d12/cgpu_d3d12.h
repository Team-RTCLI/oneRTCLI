#pragma once
#include "cgpu/api.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#ifdef __cplusplus
extern "C" {
#endif

const CGpuProcTable* CGPU_D3D12ProcTable();

CGpuInstanceId cgpu_create_instance_d3d12(CGpuInstanceDescriptor const* descriptor);
void cgpu_destroy_instance_d3d12(CGpuInstanceId instance);
void cgpu_enum_adapters_d3d12(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);
CGpuAdapterDetail cgpu_query_adapter_detail_d3d12(const CGpuAdapterId adapter);
uint32_t cgpu_query_queue_count_d3d12(const CGpuAdapterId adapter, const ECGpuQueueType type);

typedef struct CGpuInstance_D3D12 {
    CGpuInstance super;
#if defined(XBOX)
    IDXGIFactory2*                  pDXGIFactory;
#elif defined(_WIN32)
    IDXGIFactory6*                  pDXGIFactory;
#endif
    ID3D12Debug*                    pDXDebug;

    struct CGpuAdapter_D3D12*       pAdapters;
    uint32_t                        mAdaptersCount;
#if defined(__cplusplus)

#endif

} CGpuInstance_D3D12;

typedef struct CGpuAdapter_D3D12 {
    CGpuAdapter super;
#if defined(XBOX)
	IDXGIAdapter*                   pDxActiveGPU;
#elif defined(_WIN32)
	IDXGIAdapter4*                  pDxActiveGPU;
#endif
    D3D_FEATURE_LEVEL               mFeatureLevel;
    uint32_t                        mDeviceId;
    uint32_t                        mVendorId;
    char                            mDescription[128];
} CGpuAdapter_D3D12;


#ifdef __cplusplus
} // end extern "C"
#endif