#include "cgpu/backend/d3d12/cgpu_d3d12.h"
#ifdef CGPU_USE_D3D12

const CGpuProcTable tbl_d3d12 = 
{
    .create_instance = &cgpu_create_instance_d3d12,
    .destroy_instance = &cgpu_destroy_instance_d3d12,
	.enum_adapters = &cgpu_enum_adapters_d3d12,
	.query_adapter_detail = &cgpu_query_adapter_detail_d3d12,
    .query_queue_count = &cgpu_query_queue_count_d3d12
};

const CGpuProcTable* CGPU_D3D12ProcTable()
{
    return &tbl_d3d12;
}

#endif