#include "cgpu/api.h"
#include <iostream>
#include <vector>

int main()
{
    CGpuInstanceDescriptor desc;
    desc.backend = ECGPUBackEnd_WEBGPU;
    CGpuInstanceId instance = cgpu_create_instance(&desc);

    size_t adapters_count = 0;
    cgpu_enum_adapters(instance, nullptr, &adapters_count);
    std::vector<CGpuAdapterId> adapters; adapters.resize(adapters_count);
    cgpu_enum_adapters(instance, adapters.data(), &adapters_count);
    for(auto adapter : adapters)
    {

    }
    cgpu_destroy_instance(instance);

    return 0;
}