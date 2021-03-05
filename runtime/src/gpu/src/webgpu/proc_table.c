#include "cgpu/backend/webgpu/cgpu_webgpu.h"

const CGpuProcTable tbl = 
{
    .create_instance = &cgpu_create_instance_webgpu,
    .destroy_instance = &cgpu_destroy_instance_webgpu,
    .enum_adapters = &cgpu_enum_adapters_webgpu,
    .drop_adapter = &cgpu_drop_adapter_webgpu,
    .get_default_adapter = &cgpu_get_default_adapter_webgpu
};

const CGpuProcTable* CGPU_WebGPUProcTable(const WGPUBackendType t)
{
    return &tbl;
}
