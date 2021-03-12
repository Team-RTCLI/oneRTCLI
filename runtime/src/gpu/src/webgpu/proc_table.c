#define DLL_IMPLEMENTATION
#include "cgpu/backend/webgpu/cgpu_webgpu.h"

const CGpuProcTable tbl_webgpu = 
{
    .create_instance = &cgpu_create_instance_webgpu,
    .destroy_instance = &cgpu_destroy_instance_webgpu,
    .enum_adapters = &cgpu_enum_adapters_webgpu,
    .query_adapter_detail = &cgpu_query_adapter_detail_webgpu,
    .query_queue_count = &cgpu_query_queue_count_webgpu,
    .create_device = &cgpu_create_device_webgpu,
    .destroy_device = &cgpu_destroy_device_webgpu,
    .get_queue = &cgpu_get_queue_webgpu,
    .free_queue = &cgpu_free_queue_webgpu
};

const CGpuProcTable* CGPU_WebGPUProcTable(const WGPUBackendType t)
{
    return &tbl_webgpu;
}
