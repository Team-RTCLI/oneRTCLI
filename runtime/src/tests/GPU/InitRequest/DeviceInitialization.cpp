#include "gtest/gtest.h"
#include "cgpu/api.h"

CGpuInstanceId init_instance(ECGPUBackEnd backend)
{
    CGpuInstanceDescriptor desc;
    desc.backend = backend;
    CGpuInstanceId instance = cgpu_create_instance(&desc);
    return instance;
}

int enum_adapters(CGpuInstanceId instance)
{
    // enum adapters
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
    //cgpu_destroy_instance(instance);
    return adapters_count;
}

class CGpuTest : public::testing::TestWithParam<ECGPUBackEnd>
{
protected:
	void SetUp() override
	{}
	CGpuInstanceId instance;
};

TEST_P(CGpuTest, InstanceCreation)
{
    ECGPUBackEnd backend =  GetParam();
    EXPECT_TRUE(init_instance(backend) != CGPU_NULLPTR);
}
TEST_P(CGpuTest, AdapterEnum)
{
    ECGPUBackEnd backend = GetParam();
    instance = init_instance(backend);
    EXPECT_TRUE(enum_adapters(instance) > 0);
}
INSTANTIATE_TEST_SUITE_P(InstanceCreation, CGpuTest, testing::Values(ECGPUBackEnd_WEBGPU, ECGPUBackEnd_VULKAN));
INSTANTIATE_TEST_SUITE_P(AdapterEnum, CGpuTest, testing::Values(ECGPUBackEnd_WEBGPU, ECGPUBackEnd_VULKAN));
