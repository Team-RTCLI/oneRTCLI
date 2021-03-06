#include "cgpu/api.h"
#include <iostream>
#include <vector>

CGpuInstanceId init_instance(ECGPUBackEnd backend)
{
    CGpuInstanceDescriptor desc;
    desc.backend = backend;
    CGpuInstanceId instance = cgpu_create_instance(&desc);
    return instance;
}

auto webgpu_instance = init_instance(ECGPUBackEnd_WEBGPU);
auto vulkan_instance = init_instance(ECGPUBackEnd_VULKAN);

int main()
{
    CGpuInstanceId instance = webgpu_instance;
    // enum adapters
    {
        size_t adapters_count = 0;
        cgpu_enum_adapters(instance, nullptr, &adapters_count);
        std::vector<CGpuAdapterId> adapters; adapters.resize(adapters_count);
        cgpu_enum_adapters(instance, adapters.data(), &adapters_count);
        for(auto adapter : adapters)
        {
            auto prop = cgpu_query_adapter_detail(adapter);
            std::cout << "device id: " << prop.deviceId << "  vendor id: " << prop.vendorId << "\n";
            std::cout << "    name: " << prop.name << "\n";
            std::cout << "    description: " << prop.driverDescription << "\n";
        }
    }
    cgpu_destroy_instance(instance);
    return 0;
}