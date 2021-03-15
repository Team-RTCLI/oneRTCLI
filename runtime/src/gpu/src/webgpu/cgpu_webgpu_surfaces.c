#include "cgpu/backend/webgpu/cgpu_webgpu.h"
#include "cgpu/backend/webgpu/cgpu_webgpu_surfaces.h"


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

void cgpu_surface_free_webgpu(CGpuInstanceId instance, CGpuSurfaceId surface)
{

}

#if defined(_WIN32) || defined(_WIN64)
CGpuSurfaceId cgpu_surface_from_hwnd_webgpu(CGpuInstanceId instance, HWND window)
{
    return CGPU_NULLPTR;
}
#endif